<?php

/**
 * Serverside script for receiving the data from the iPhone/Android Application.
 * It receives POST requests and stores it directly in the database. Normal drupal
 * authentication is used. 
 * 
 * @author Robin Oster
 * 
 */

$successful = true;

define('DRUPAL_ROOT', dirname(__FILE__) . '/..');

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

function auth() {
    header('WWW-Authenticate: Basic realm="Naturvielfalt"');
    header('HTTP/1.0 401 Unauthorized');

	echo 'FAIL: Please authorize';

    exit;
}

if (!isset($_SERVER['PHP_AUTH_USER'])) {
    auth();

} else {

	// Set content-length
	// header('Content-Length: ' . ob_get_length()); <---- MADE THE APP CRASH.... Don't really understand why

	// Authenticate the user (Drupal Authentification)
    $uid = user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']);
    $user = user_load($uid);

    if ($uid) {	
	
		$organism = @$_POST['organism'];
		$type = @$_POST['type'];
        $count = @$_POST['count'];
        $date = @$_POST['date'];
        $accuracy = @$_POST['accuracy'];
		$author = @$_POST['author'];
		$longitude = @$_POST['longitude'];
		$latitude = @$_POST['latitude'];
		// $comment = @$_POST['comment']; // noch nicht komplett implementiert

		// Reverse geocode from longitude and latitude coordinates get city, canton, etc...
		$jsondata = reverseGeocode($longitude, $latitude);

        // get head_inventory_id
        $head = _inventory_single_get_id($user);

        // check for existing inventory
        $inventory = db_select('inventory', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('head_inventory_id', $head)->execute()->fetchField();

        if (!$inventory) {
            // inventory doesn't exist, create it
            $inventory = db_insert('inventory')->fields(array('inventory_type_id' => $type, 'head_inventory_id' => $head))->execute();
        }

		// Get location based information
		$zip = $jsondata['Placemark'][0]['AddressDetails']['Country']['AdministrativeArea']['Locality']['PostalCode']['PostalCodeNumber'];
		if(!$zip) {
			$zip = $jsondata['Placemark'][0]['AddressDetails']['Country']['AdministrativeArea']['SubAdministrativeArea']['Locality']['PostalCode']['PostalCodeNumber'];
		}
			
		$city = $jsondata['Placemark'][0]['AddressDetails']['Country']['AdministrativeArea']['Locality']['LocalityName'];
		if(!$city) {
			$city = $jsondata['Placemark'][0]['AddressDetails']['Country']['AdministrativeArea']['SubAdministrativeArea']['Locality']['LocalityName'];
		}
		
		$country = $jsondata['Placemark'][0]['AddressDetails']['Country']['CountryName'];
		$canton = $jsondata['Placemark'][0]['AddressDetails']['Country']['AdministrativeArea']['AdministrativeAreaName'];


        // Create the entry insert
        $entry = db_insert('inventory_entry')->fields(array('organism_id' => $organism, 'inventory_id' => $inventory, 'position' => 0, 'accuracy' => $accuracy, 'zip' => $zip, 'township' => $city, 'canton' => $canton, 'country' => $country))->execute();

		// Add Funddatum, Beobachter und Amount and other dynamic attributes
		if($entry) {			
			$funddatumId = db_select('inventory_type_attribute', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('name', "Funddatum")->execute()->fetchField();
			$attributeFunddatum = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => $funddatumId, 'value' => $date))->execute();
			
			// Flowers don't have any amount..
			if($type != 16) {
				$anzahlId = db_select('inventory_type_attribute', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('name', "Anzahl")->execute()->fetchField();
				$attributesAnzahl = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => $anzahlId, 'value' => $count))->execute();
			}
			
			$beobachterId = db_select('inventory_type_attribute', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('name', "Beobachter")->execute()->fetchField();
			$attributesBeobachter = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => $beobachterId, 'value' => $author))->execute();

			// noch nicht komplett implementiert
			// $commentId = db_select('inventory_type_attribute', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('name', "Comment")->execute();
			// $attributesComment = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => 85, 'value' => $comment))->execute();

		}	
		
		// Add POINT to database as geometry information
		$points[] = $longitude . ' ' . $latitude;
		$geom = 'ST_GeomFromText(\'POINT(' . reset($points) . ')\', 4326)';
        
		db_update('inventory_entry')->expression('geom', $geom)->condition('id', $entry)->execute();


		// Create image and store information in the database
		if(isset($_FILES['file'])) {
			storeImage($entry, $uid);
		}

        if($successful) {
			echo 'SUCCESS';
		} else {
			echo 'Some went wrong.. Please check the submitData.php file on the Swissmon Webserver.';
		}
 
    } else {
        auth();
    }
}

/**
* Create a copy of the uploaded image at the right path and create all necesarry
* db entries to make the image occur at the single observation of the user.
*/
function storeImage($entry, $uid) {
	$filename = "iphoneprovepicture.png";
	$folder = "/srv/www/htdocs/drupal/application/sites/default/files/swissmon/gallery/inventory_entry/" . $entry . '/';
	// $folder = "/Applications/XAMPP/xamppfiles/htdocs/swissmon/application/sites/default/files/swissmon/gallery/inventory_entry/" . $entry . '/';
	$target_path = $folder . $filename;
	
	// echo 'Folder: ' . $folder;
	// echo 'Target path: ' . $target_path;
	
	if (!file_exists($folder)) {
		mkdir($folder, 0777);
	}

	if(move_uploaded_file($_FILES['file']['tmp_name'], $target_path)) {
		$uri = 'public://swissmon/gallery/inventory_entry/' . $entry . '/' . $filename;
		$filesize = filesize($target_path);
		$timestamp = time();

		// CREATE file_managed entry
		$file_managed_entry = db_insert('file_managed')->fields(array('uid' => $uid, 'uri' => $uri, 'filename' => $filename, 'filemime' => 'image/png', 'status' => 1, 'filesize' => $filesize, 'timestamp' => $timestamp))->execute();	

		// CREATE gallery_image entry
		if($file_managed_entry) {
			$gallery_image_entry = db_insert('gallery_image')->fields(array('item_type' => 'inventory_entry', 'item_id' => $entry, 'fid' => $file_managed_entry, 'title' => 'IPhone Belegfoto', 'description' => '', 'author' => $author, 'visible' => 1, 'owner_id' => $uid, 'created_date' => '2011-09-19 10:04:52.730903+02', 'modified_date' => '2011-09-19 10:04:52.730903+02'))->execute();
		} else {
			$successful = false;
		}
	} else {
		$successful = false;
	}
}

/**
* This method is responsible for using the Google Reverse Geocode API
* to receive the canton, city, zip, etc. to store it later on in the database
* as additional information to the longitude, latitude values.
*/
function reverseGeocode($longitude, $latitude) {
	
	// STATIC NATURWERK COORDINATE for testing..
	// $latitude = '47.480988';
	// $longitude = '8.209748';
	
	// Naturwerk API KEY
	$api_key = "ABQIAAAAwcMkDr6mXdEANl2JDXxaVRQfE9mbrKsN7lT6vEoUZH5uGIZnyxQPomqpos48lV9muY5UBtJAwRzeFQ";
	
	// format this string with the appropriate latitude longitude
	$url = 'http://maps.google.com/maps/geo?q=' . $latitude . ',' . $longitude . '&sensor=false&output=json&key=' . $api_key;
	
	// make the HTTP request
	$data = @file_get_contents($url);
	
	// parse the json response
	$jsondata = json_decode($data,true);
	
	// if we get a placemark array and the status was good, get the addres
	if(is_array($jsondata )&& $jsondata ['Status']['code']== 200)
	{
		  // For debugging. Display all returned information from the google api
	      // print_r($jsondata);
	} else {
		$successful = false;
	}
	
	return $jsondata;
}
