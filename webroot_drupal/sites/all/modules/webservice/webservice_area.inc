<?php
/**
 * @file webservice_area.inc
 * @author André Kälin, 2013
 * @copyright 2012 Naturwerk, Brugg
 */
module_load_include('inc', 'area', 'area.create');
module_load_include('inc', 'area', 'area.validate');

define('WS_API_LOG_TYPE', 'webservice api');

/**
 * Serverside script for receiving the data from the iPhone/Android Application.
 * It receives POST requests and stores it directly in the database. Normal drupal
 * authentication is used.
 * 
 *  The following keys will be processed
 *  
 *	$_POST['guid'] => Global id of the area
 *	$_POST['type'] => 'polyogon', 'line' or 'marker'
 *	$_POST['coordinates'] => json string with coordinates tuples [[longitude, latitude],[longitude, latitude]]
 *	$_POST['name'] => the name of the area
 *	$_POST['date'] => the date of creation
 *	$_POST['time'] => the time of creation
 *  $_POST['description'] => description of the area
 *
 * @author André Kälin, 2013
 */
function webservice_parse_import_areas() {
	global $user;
	watchdog(WS_API_LOG_TYPE, 'invoked', array(), WATCHDOG_DEBUG);
	if (!isset($_SERVER['PHP_AUTH_USER'])) {
		return webservice_auth();
	}

	// Authenticate the user (Drupal Authentification)
	$uid = user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']);
	if (!$uid) {
		return webservice_auth();
	}
	$user = user_load($uid);

	
	//set location geocodeinfo with first point of coordinates
	$coordinates = json_decode($_POST['coordinates'], true);
	$data = area_get_geocode_info(doubleval(filter_xss($coordinates[0][0])), doubleval(filter_xss($coordinates[0][1])));
	//remove not needed keys of geocodeinfo
	unset($data['coordinate']);
	unset($data['marker']);
	
	/**
	 * parse not useable post vars to be compatible with the area module
	 * @var unknown_type
	 */
	$data['area_id'] = $_POST['guid'];
	$data['area_type'] = $_POST['type'];
	$data['area-coords'] = $_POST['coordinates'];
	$data['fieldname'] = $_POST['name'];
	$data['date'] = $_POST['date'];
	$data['time'] = $_POST['time'];
	$data['set_user_uid'] = $user->uid;
	$data['comment'] = $_POST['description'];
	debug($data);
	debug('area webservice');
	$validationerrors =  webservice_area_validate($data);
	if(!empty($validationerrors)) {
		header('HTTP/1.0 400 Bad Request');
		print('ERROR: Validation errors detected.<br>');
		foreach ($validationerrors as $message) {
			print($message.'<br>');
		}
		
	}
	else {
		debug($data);
		$area_status = area_save(false, $data, true);
		debug($area_status);
		// get the data
		if ($area_status['success'] == true) {
			header('HTTP/1.0 200 OK');
			$i=0;
			//pass return values in "key=value&key2=value2" notation
			foreach($area_status as $key => $message) {
				if($i > 0 && $i < sizeof($area_status)) print('&');
				print($key.'='.$message);
				$i++;
			}
		} else {
			header('HTTP/1.0 400 Bad Request');
			print('ERROR: Something went wrong.. Could not persist the area. <br>');
			foreach ($area_status['message'] as $key => $message) {
				print($message.'<br>');
			}
		}
	}
}

/**
 * Sends the http header with an authentification request to the client.
 * (Opens authentification form in browser)
 *
 * @author André Kälin, 2013
 */
function webservice_auth() {
	header('WWW-Authenticate: Basic realm="Naturvielfalt"');
	header('HTTP/1.0 401 Unauthorized');

	watchdog(WS_API_LOG_TYPE, 'Authorization failed.', array(), WATCHDOG_INFO);
	die('ERROR: Please authorize');
}

/**
 * Validates incoming area data 
 * 
 * @param $data a key/value array
 * @author André Kälin, 2013
 */
function webservice_area_validate($data) {
	$validationerrors = array();
	
	if(!isset($data['area_id'])) $validationerrors[] = t("Please pass key 'guid'.");
	else if(!area_validate_area_id($data['area_id']))  $validationerrors[] = t('Please pass a valid area id.');
	else if(!area_validate_permission($data['area_id'])) $validationerrors[] = t("Permission denied.");
	
	if(!isset($data['fieldname'])) $validationerrors[] = t("Please pass key 'name'.");
	else if(!area_validate_area_name($data['fieldname']))  $validationerrors[] = t('Please pass a valid area name.');
	
	if(!isset($data['date'])) $validationerrors[] = t("Please pass key 'date'.");
	else if(!area_validate_date($data['date']))  $validationerrors[] = t('Please pass a valid date (dd.mm.yyyy)');
	
	if(!isset($data['time'])) $validationerrors[] = t("Please pass key 'time'.");
	else if(!area_validate_time($data['time']))  $validationerrors[] = t('Please pass a valid time (HH:mm)');
	
	if(!isset($data['area-coords'])) $validationerrors[] = t("Please pass key 'coordinates'");
	else if(!area_validate_coordinates(json_decode($data['area-coords'],true))) $validationerrors[] = t("Please pass a correct location.");

	if(isset($_FILES['files'])) {
		foreach($_FILES['files']['name'] as $fkey => $filename) {
			if($_FILES['files']['name'][$fkey] != '') {
				$fileextension = array_pop(explode('.', $_FILES['files']['name'][$fkey]));
				if(!area_validate_fileextension($_FILES['files']['name'][$fkey])) {
					$validationerrors[] = t('.@extension is an unallowed fileextension!', array('@extension' => $fileextension));
				}
			}
		}
	}
	
	return $validationerrors;
}
