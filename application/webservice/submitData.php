<?php

/**
 * Serverside script for receiving the data from the iPhone/Android Application.
 * It receives POST requests and stores it directly in the database. Normal drupal
 * authentication is used. 
 * 
 * @author Robin Oster
 * 
 */

define('DRUPAL_ROOT', dirname(__FILE__) . '/..');

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

function auth() {

    header('WWW-Authenticate: Basic realm="Naturvielfalt"');
    header('HTTP/1.0 401 Unauthorized');

    echo 'Beobachtung wurde nicht gespeichert. Bitte auf naturvielfalt.ch registrieren.';
    exit;
}

if (!isset($_SERVER['PHP_AUTH_USER'])) {

    auth();

} else {

    $uid = user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']);
    $user = user_load($uid);

    if ($uid) {	
        $organism = @$_POST['organism'];
        $count = @$_POST['count'];
        $date = @$_POST['date'];
        $location = @$_POST['location'];
        $accuracy = @$_POST['accuracy'];
        $type = @$_POST['type'];
		$author = @$_POST['author'];
		$longitude = @$_POST['longitude'];
		$latitude = @$_POST['latitude'];
		$comment = @$_POST['comment'];
		
		// Reverse geocode from longitude and latitude coordinates get city, canton, etc...
		$jsondata = reverseGeocode($longitude, $latitude);
		
		// print_r($_POST);

        // get head_inventory_id
        $head = _inventory_single_get_id($user);

        // check for existing inventory
        $inventory = db_select('inventory', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('head_inventory_id', $head)->execute()->fetchField();

        if (!$inventory) {

            // inventory doesn't exist, create it
            $inventory = db_insert('inventory')->fields(array('inventory_type_id' => $type, 'head_inventory_id' => $head))->execute();
        }

		// Get location based information
		$zip = $jsondata['Placemark'][0]['AddressDetails']['Country']['AdministrativeArea']['SubAdministrativeArea']['Locality']['PostalCode']['PostalCodeNumber'];
		$country = $jsondata['Placemark'][0]['AddressDetails']['Country']['CountryName'];
		$city = $jsondata['Placemark'][0]['AddressDetails']['Country']['AdministrativeArea']['SubAdministrativeArea']['Locality']['LocalityName'];
		$canton = $jsondata['Placemark'][0]['AddressDetails']['Country']['AdministrativeArea']['AdministrativeAreaName'];

        // Create the entry insert
        $entry = db_insert('inventory_entry')->fields(array('organism_id' => $organism, 'inventory_id' => $inventory, 'position' => 0, 'accuracy' => $accuracy, 'zip' => $zip, 'township' => $city, 'canton' => $canton, 'country' => $country))->execute();

		// Add Funddatum, Beobachter und Amount
		// 7 = Funddatum
		// 44 = Beobachter
		// 20 = Anzahl
		if($entry) {
			$attributeFunddatum = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => 7, 'value' => $date))->execute();
			$attributesAnzahl = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => 20, 'value' => $count))->execute();
			$attributesBeobachter = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => 44, 'value' => $author))->execute();
			$attributesComment = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => 85, 'value' => $comment))->execute();
		}	
		
		// Add POINT to database as geometry information
		$points[] = $longitude . ' ' . $latitude;
		$geom = 'ST_GeomFromText(\'POINT(' . reset($points) . ')\', 4326)';
        
		db_update('inventory_entry')->expression('geom', $geom)->condition('id', $entry)->execute();

        echo 'Die Beobachtung wurde erfolgreich gespeichert, vielen Dank!';

    } else {
        auth();
    }
}

function reverseGeocode($longitude, $latitude) {
		
	// STATIC NATURWERK COORDINATE for testing..
	// $latitude = '47.480988';
	// $longitude = '8.209748';
	
	// set your API key here
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
	      print_r($jsondata);
	}
	
	return $jsondata;
}
