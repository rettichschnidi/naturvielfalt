<?php
/**
 * @file naturvielfalt_webservice.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */
// module_load_include('inc', 'observation');
module_load_include('inc', 'observation', 'observation.ajax');
// module_load_include('inc', 'gallery', 'gallery.implementations');




/**
 * Serverside script for receiving the data from the iPhone/Android Application.
 * It receives POST requests and stores it directly in the database. Normal drupal
 * authentication is used.
 *
 * @author Ramon Gamma, 2012
 */
function naturvielfalt_webservice_parse_import_observations(){
	global $webservice_debug;
	$webservice_debug = true;

// 	error_reporting(E_ALL ^ E_NOTICE);
// 	ini_set('display_errors', 1);
// 	ini_set('error_log', $_SERVER["DOCUMENT_ROOT"].'/webservice/php.log');

// 	phpinfo();
// 	die('yeah');

	if($webservice_debug) naturvielfalt_webservice_log();
	if($webservice_debug) naturvielfalt_webservice_log('Start add a Observation');


	// setup the state for response to the client
	$successful = true;

// die(DRUPAL_ROOT);
//	should be inserted already
// 	require_once(DRUPAL_ROOT . '/includes/bootstrap.inc');
// 	drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);



	if (!isset($_SERVER['PHP_AUTH_USER'])) return naturvielfalt_webservice_auth();

		// Set content-length
		// header('Content-Length: ' . ob_get_length()); <---- MADE THE APP CRASH.... Don't really understand why

		// Authenticate the user (Drupal Authentification)
		$uid = user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']);
		if(!$uid) return naturvielfalt_webservice_auth();
		$user = user_load($uid);

		if($webservice_debug) naturvielfalt_webservice_log('REQUEST :'.var_export($_POST, true) );
		if($webservice_debug) naturvielfalt_webservice_log('FILES :'.var_export($_FILES, true) );

		/**
		 * parse not useable post vars to be compatible with th eobservation module
		 * @var unknown_type
		 */

		$data = area_get_geocodeInfo(filter_xss($_POST['longitude']), filter_xss($_POST['latitude']));
		$data['organismn_id'] = $_POST['organismn_id'];
		$data['organism_artgroup_id'] = $_POST['organism_artgroup_id'];
		$data['count'] = $_POST['count'];
		$data['date'] = $_POST['date'];
		$data['observer'] = $_POST['observer'];
		$data['attributes_custom_names'][0] = t('Accuracy');
		$data['attributes_custom_values'][0] = $_POST['accuracy'].'m';
		$data['meta_description'][0] = $_POST['comment'];

		if($webservice_debug) naturvielfalt_webservice_log('Rewrited Request :'.var_export($data, true) );

		$observation_status = observation_save(false, $data, true);
		naturvielfalt_webservice_log('tttttttttttt');
		// get the data
		if($observation_status['success'] == true) {
			if($webservice_debug) naturvielfalt_webservice_log('SUCCESS');
			print('SUCCESS');
		} else {
			if($webservice_debug) naturvielfalt_webservice_log('ERROR: Some went wrong.. check the following messages');
			if($webservice_debug) foreach ($observation_status['message'] as $key => $message) naturvielfalt_webservice_log($message);
			print('ERROR: Some went wrong.. check the log file for details');
		}

		if($webservice_debug) naturvielfalt_webservice_log('END');
		if($webservice_debug) naturvielfalt_webservice_log();

exit();

/**
 *
 * New handling end
 *
 * after testing test rest of this function can be deleted
 *
 *
 *
 *
 *
 */

// 		observation_save(false, true);
		// get the data
		$request = $_POST;

		if(!$request) naturvielfalt_webservice_error_exit('no request to processing...');


		$organism = @$request['organism'] ? $request['organism'] : "";
		$type = @$request['type'] ? $request['type'] : "";
		$count = @$request['count'] ? $request['count'] : 1;
		$date = @$request['date'] ? $request['date'] : "";
		$accuracy = @$request['accuracy'] ? $request['accuracy'] : 0;
		$author = @$request['author'] ? $request['author'] : "";
		$longitude = @$request['longitude'] ? $request['longitude'] : "";
		$latitude = @$request['latitude'] ? $request['latitude'] : "";
		// $comment = @$_POST['comment']; // noch nicht komplett implementiert

		if($webservice_debug) naturvielfalt_webservice_log('REQUEST :'.var_export($request, true) );
		if($webservice_debug) naturvielfalt_webservice_log('FILES :'.var_export($_FILES, true) );

		// Check the variables
		if($organism < 1) 	naturvielfalt_webservice_error_exit('organism should an int and not smaller than 1 || variable value:'.$organism);
		if($type < 1) 		naturvielfalt_webservice_error_exit('type should an int and not smaller than 1 || variable value:'.$type);
		if($count < 1) 		naturvielfalt_webservice_error_exit('count should an int and not smaller than 1 || variable value:'.$count);
		if($date == "") 	naturvielfalt_webservice_error_exit('date should not be empty || variable value:'.$date);
		if($accuracy < 0) 		naturvielfalt_webservice_error_exit('accuracy should an int and not smaller than 0 || variable value:'.$accuracy);
		if($author == "") 		naturvielfalt_webservice_error_exit('autor should not be empty || variable value:'.$author);
		if($longitude == "") 		naturvielfalt_webservice_error_exit('longitude should an double || variable value:'.$longitude);
		if($latitude == "") 		naturvielfalt_webservice_error_exit('latitude should an double || variable value:'.$latitude);

		// Check the vars with the db
		if(!organism_get($organism)) naturvielfalt_webservice_error_exit('organism validaten with db failed || variable value:'.$organism);
		$artgroup = organism_artgroup_get_organsim_in_artgroup($organism_id, true);


		// FIXME Dieses Daten m√ºssen allesammt validiert werden BEVOR diese auf die DB aufschlagen...


		/**
		 * inventory not used, it's just a singleobservation
		 */
		// get head_inventory_id
// 		$head = _inventory_single_get_id($user);
// 		if($head < 1) naturvielfalt_webservice_error_exit('get head inventory failed!');
// 		if($webservice_debug) naturvielfalt_webservice_log('head_inventory_id: '.$head);

		// check for existing inventory
// 		$inventory = db_select('inventory', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('head_inventory_id', $head)->execute()->fetchField();
// 		if (!$inventory) {
// 			// inventory doesn't exist, create it
// 			$inventory = db_insert('inventory')->fields(array('inventory_type_id' => $type, 'head_inventory_id' => $head))->execute();
// 			if($webservice_debug) naturvielfalt_webservice_log('create inventory');
// 		}else{
// 			if($webservice_debug) naturvielfalt_webservice_log('use existing inventory');
// 		}
// 		if($webservice_debug) naturvielfalt_webservice_log('inventory_id: '.$inventory);


		// Reverse geocode from longitude and latitude coordinates get city, canton, etc...
		$jsondata = reverseGeocode($longitude, $latitude);
		if($webservice_debug) naturvielfalt_webservice_log('reverseGeocode: '.var_export($jsondata, true));
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

		if(!$zip) $zip = "0";
		if(!$city) $city = '';
		if(!$country) $country = '';
		if(!$canton) $canton = '';

		// Create the entry insert
		$entry = db_insert('inventory_entry')->fields(array(
				'organism_id' => $organism,
				'inventory_id' => $inventory,
				'position' => 0,
				'accuracy' => $accuracy,
				'zip' => $zip,
				'township' => $city,
				'canton' => $canton,
				'country' => $country
		))->execute();

		//if($webservice_debug) naturvielfalt_webservice_log('Inventory entry id: '.$entry);

		// Add Funddatum, Beobachter und Amount and other dynamic attributes
		if($entry) {
			$funddatumId = db_select('inventory_type_attribute', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('name', "Funddatum")->execute()->fetchField();
			if($funddatumId < 0) naturvielfalt_webservice_error_exit('find date id could not be found in the db');

			$attributeFunddatum = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => $funddatumId, 'value' => $date))->execute();
			if(!$attributeFunddatum) naturvielfalt_webservice_error_exit("the find date attribute can't inserted in the db");

			// Flowers don't have any amount..
			if($type != 16) {
				$anzahlId = db_select('inventory_type_attribute', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('name', "Anzahl")->execute()->fetchField();
				if($anzahlId < 0) naturvielfalt_webservice_error_exit('anzahlId could not be found in the db');
				$attributesAnzahl = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => $anzahlId, 'value' => $count))->execute();
				if(!$attributesAnzahl) naturvielfalt_webservice_error_exit("the count attribute can't inserted in the db");
			}else{
				if($webservice_debug) naturvielfalt_webservice_log("Info: Flowers don't have any amount..");
			}

			$beobachterId = db_select('inventory_type_attribute', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('name', "Beobachter")->execute()->fetchField();
			if($anzahlId < 0) naturvielfalt_webservice_error_exit('beobachterId could not be found in the db');
			$attributesBeobachter = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => $beobachterId, 'value' => $author))->execute();
			if(!$attributesBeobachter) naturvielfalt_webservice_error_exit("the beobachter attribute can't inserted in the db");

			// TODO: Implementieren
			// noch nicht komplett implementiert
			// $commentId = db_select('inventory_type_attribute', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('name', "Comment")->execute();
			// $attributesComment = db_insert('inventory_type_attribute_inventory_entry')->fields(array('inventory_entry_id' => $entry, 'inventory_type_attribute_id' => 85, 'value' => $comment))->execute();
		}else{
			naturvielfalt_webservice_error_exit('insert inventory entry failed!');
		}

		// Add POINT to database as geometry information
		$points[] = $longitude . ' ' . $latitude;
		$geom = 'ST_GeomFromText(\'POINT(' . reset($points) . ')\', 4326)';

		db_update('inventory_entry')->expression('geom', $geom)->condition('id', $entry)->execute();

		// Create image and store information in the database
		if(isset($_FILES['file'])) {
			storeImage($entry, $uid, $author);
		}else {
			if($webservice_debug) naturvielfalt_webservice_log('INFO: no image to store');
		}

		if($successful) {
			echo 'SUCCESS';
			if($webservice_debug) naturvielfalt_webservice_log('SUCCESS');
		} else {
			echo 'Some went wrong.. Please check the submitData.php file on the Swissmon Webserver.';
			if($webservice_debug) naturvielfalt_webservice_log('ERROR: Some went wrong.. Please check the submitData.php file on the Swissmon Webserver.');
		}



	if($webservice_debug) naturvielfalt_webservice_log('END');
	if($webservice_debug) naturvielfalt_webservice_log('------------------------------------------------------------------');







}


/**
 * Create a copy of the uploaded image at the right path and create all necesarry
 * db entries to make the image occur at the single observation of the user.
 */
// function naturvielfalt_webservice_storeImage($entry, $uid, $author) {
// 	global $webservice_debug;
// 	$filename = "iphoneprovepicture.png";
// 	$folder = $_SERVER["DOCUMENT_ROOT"] . "/sites/default/files/swissmon/gallery/inventory_entry/" . $entry . '/'; // FIXME: Dateipfad soll aus der DB stammen
// 	// $folder = "/Applications/XAMPP/xamppfiles/htdocs/swissmon/application/sites/default/files/swissmon/gallery/inventory_entry/" . $entry . '/';
// 	$target_path = $folder . $filename;

// 	if($webservice_debug) naturvielfalt_webservice_log('INFO: try to store image: '. $target_path);
// 	// echo 'Folder: ' . $folder;
// 	// echo 'Target path: ' . $target_path;

// 	if (!file_exists($folder)) {
// 		if(mkdir($folder, 0777)){
// 			if($webservice_debug) naturvielfalt_webservice_log('INFO: folder created');
// 		}else{
// 			if($webservice_debug) naturvielfalt_webservice_log('ERROR: create folder failed');
// 		}
// 	}

// 	if(move_uploaded_file($_FILES['file']['tmp_name'], $target_path)) {
// 		if($webservice_debug) naturvielfalt_webservice_log('INFO: move file success');
// 		$uri = 'public://swissmon/gallery/inventory_entry/' . $entry . '/' . $filename;
// 		$filesize = filesize($target_path);
// 		$timestamp = time();

// 		// CREATE file_managed entry
// 		$file_managed_entry = db_insert('file_managed')->fields(array('uid' => $uid, 'uri' => $uri, 'filename' => $filename, 'filemime' => 'image/png', 'status' => 1, 'filesize' => $filesize, 'timestamp' => $timestamp))->execute();

// 		// CREATE gallery_image entry
// 		if($file_managed_entry) {
// 			$gallery_image_entry = db_insert('gallery_image')->fields(array('item_type' => 'inventory_entry', 'item_id' => $entry, 'fid' => $file_managed_entry, 'title' => 'IPhone Belegfoto', 'description' => '', 'author' => $author, 'visible' => 1, 'owner_id' => $uid, 'created_date' => '2011-09-19 10:04:52.730903+02', 'modified_date' => '2011-09-19 10:04:52.730903+02'))->execute();
// 		} else {
// 			$successful = false;
// 			if($webservice_debug) naturvielfalt_webservice_log('ERROR: db insert fail (file_managed)');
// 		}
// 	} else {
// 		$successful = false;
// 		if($webservice_debug) naturvielfalt_webservice_log("ERROR: can't move file");
// 	}
// }

function naturvielfalt_webservice_log($message = NULL, $type = NULL, $ip= NULL){
	if(empty($ip)){
		$ip=$_SERVER['REMOTE_ADDR'];
		$ipr=gethostbyaddr($_SERVER['REMOTE_ADDR']);
	}
	$logline = @date("d.m.Y") ." ". @date("H:i:s") ." ". $_SERVER['PHP_AUTH_USER'] ." ". $type ." ". $ip ." ". $ipr." ". $message ."\n";
	if($message == NULL) $logline = '';
	$newsletterlog = fopen($_SERVER["DOCUMENT_ROOT"].base_path().'/sites/all/modules/webservice/data.log','a');
	fwrite($newsletterlog,$logline);
	fclose($newsletterlog);
}

function naturvielfalt_webservice_error_exit($message){
	global $webservice_debug;
	if(!$message) $message = 'ERROR, exiting... (no message given)';
	if($webservice_debug) naturvielfalt_webservice_log($message);
	// clean up db inserts
	//TODO
	die($message);
}

function naturvielfalt_webservice_auth() {
	global $webservice_debug;
	header('WWW-Authenticate: Basic realm="Naturvielfalt"'); // FIXME: Use auth-digest instead of auth basic
	header('HTTP/1.0 401 Unauthorized');


	if($webservice_debug) naturvielfalt_webservice_log('FAIL: Please authorize');
	die('ERROR: Please authorize');
}