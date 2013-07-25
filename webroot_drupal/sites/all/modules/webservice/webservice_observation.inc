<?php
/**
 * @file webservice_observation.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */
module_load_include('inc', 'observation', 'observation.ajax');
module_load_include('inc', 'observation', 'observation.validate');
module_load_include('inc', 'organism', 'organism.artgroup');

define('WS_API_LOG_TYPE', 'webservice api');

/**
 * Serverside script for receiving the data from the iPhone/Android Application.
 * It receives POST requests and stores it directly in the database. Normal drupal
 * authentication is used.
 * 
 * This method is for compatibility reasons with old versions of the iphone app.
 *  
 * @author Ramon Gamma, 2012
 */
function webservice_parse_import_observations_lightversion() {
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


	/**
	 * parse not useable post vars to be compatible with th eobservation module
	 * @var unknown_type
	*/

	$data = area_get_geocode_info(filter_xss($_POST['longitude']), filter_xss($_POST['latitude']));
	$data['organism_id'] = $_POST['organismn_id'];
	$data['organism_artgroup_id'] = $_POST['organism_artgroup_id'];
	$data['count'] = $_POST['count'];
	$data['date'] = $_POST['date'];
	$data['observer'] = $_POST['observer'];

	$accArray = organism_artgroup_get_attribute_by_name(t('Accuracy'));
	if (count($accArray) > 0)
		$data['attributes'][$accArray[0]] = $_POST['accuracy'] . 'm';
	$comArray = organism_artgroup_get_attribute_by_name(t('Comment'));
	if (count($comArray) > 0)
		$data['attributes'][$comArray[0]] = $_POST['comment'];

	$data['meta_description'][] = $_POST['comment'];
	$data['set_user_uid'] = $user->uid;


	$observation_status = observation_save(false, $data, true);
	// get the data
	if ($observation_status['success'] == true) {
		print('SUCCESS');
	} else {
		watchdog(
		WS_API_LOG_TYPE,
		'Something went wrong... see following error messages.',
		array(),
		WATCHDOG_ERROR);
		foreach ($observation_status['message'] as $key => $message) {
			watchdog(WS_API_LOG_TYPE, $message, array(), WATCHDOG_ERROR);
		}
		print('ERROR: Some went wrong.. check the log file for details');
	}
}
/**
 * Serverside script for receiving the data from the iPhone/Android Application.
 * It receives POST requests and stores it directly in the database. Normal drupal
 * authentication is used.
 *
 *  The following keys will be processed
 *  
 *	$_POST['guid'] => Global id of the observation
 *	$_POST['organism_id'] => id of organism
 *	$_POST['organism_artgroup_id'] => id of artgroup
 *	$_POST['longitude'] => longitude of the observation
 *	$_POST['latitude'] => latitude of the observation
 *	$_POST['count'] => the count of observed species
 *	$_POST['date'] => the date of creation
 *	$_POST['time'] => the time of creation
 *  $_POST['comment'] => Commen of the observation
 *  $_POST['observer'] => observer of the observation
 *  $_POST['inventoryguid'] => the global id of the invetory this observation belongs to.
 *  
 * @author Ramon Gamma, 2012
 * @author André Kälin, 2013
 */
function webservice_parse_import_observations() {
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

	/**
	 * parse not useable post vars to be compatible with th observation module
	 * @var unknown_type
	 */
	$data = area_get_geocode_info(filter_xss($_POST['longitude']), filter_xss($_POST['latitude']));
	$data['observation_id'] = $_POST['guid'];
	$data['organism_id'] = $_POST['organism_id'];
	$data['organism_artgroup_id'] = $_POST['organism_artgroup_id'];
	$data['count'] = $_POST['count'];
	$data['date'] = $_POST['date'];
	$data['time'] = $_POST['time'];
	$data['observer'] = $_POST['observer'];
	$data['inventory_id'] = $_POST['inventory_guid'];
	
	debug($_POST);
	
	$accArray = organism_artgroup_get_attribute_by_name(t('Accuracy'));
	if (count($accArray) > 0)
		$data['attributes'][$accArray[0]] = $_POST['accuracy'] . 'm';
	$comArray = organism_artgroup_get_attribute_by_name(t('Comment'));
 	if (count($comArray) > 0)
 		$data['attributes'][$comArray[0]] = $_POST['comment'];
 	
	$data['meta_description'][] = $_POST['comment'];
	$data['set_user_uid'] = $user->uid;
	
	$validationerrors =  webservice_observation_validate($data);
	if(!empty($validationerrors)) {
		header('HTTP/1.0 400 Bad Request');
		print('ERROR: Validation errors detected.<br>');
		foreach ($validationerrors as $message) {
			print($message.'<br>');
		}
		
	}
	else {
		$observation_status = observation_save(false, $data, true);
		// get the data
		if ($observation_status['success'] == true) {
			header('HTTP/1.0 200 OK');
			$i=0;
			//pass return values in "key=value&key2=value2" notation
			foreach($observation_status as $key => $message) {
				if($i > 0 && $i < sizeof($observation_status)) print('&');
				print($key.'='.$message);
				$i++;
			}
		} else {
			header('HTTP/1.0 400 Bad Request');
			print('ERROR: Something went wrong.. Could not persist the observation. <br>');
			foreach ($observation_status['message'] as $key => $message) {
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

function webservice_observation_validate($data) {
	$validationerrors = array();

	//Validate Observation id and permission
	if(!isset($data['observation_id'])) $validationerrors[] = t("Please pass key 'guid'.");
	else if(!observation_validate_observation_id($data['observation_id'])) t("Please pass a valid observation id.");
	else if(!observation_validate_permission($data['observation_id']))  t("Permission denied.");
	
	//Validate Artgroup id
	if(!isset($data['organism_artgroup_id'])) $validationerrors[] = t("Please pass key 'organism_artgroup_id'.");
	else if(!observation_validate_artgroup_id($data['organism_artgroup_id'])) t("Please pass an existing artgroup.");
	
	//Validate Organism id
	if(!isset($data['organism_id'])) $validationerrors[] = t("Please pass key 'organism_id'.");
	else if(!observation_validate_organism_id($data['organism_id']))  $validationerrors[] = t('Please enter an existing organism.');
	
	//Check if inventory guid >= 0 and exists
	if(!isset($data['inventory_id'])) $validationerrors[] = t("Please pass key 'inventory_guid'.");
	else if(intval($data['inventory_id']) != 0 && !observation_validate_inventory($data['inventory_id'])) t('Invalid inventory / permission denied.');
	
	if(!isset($data['count'])) $validationerrors[] = t("Please pass key 'count'.");
	else if(!observation_validate_count($data['count']))  $validationerrors[] = t('The count has to be a number and bigger than zero');
	
	if(!isset($data['date'])) $validationerrors[] = t("Please pass key 'date'.");
	else if(!observation_validate_date($data['date']))  $validationerrors[] = t('Please pass a valid date (dd.mm.yyyy)');
	
	if(!isset($data['time'])) $validationerrors[] = t("Please pass key 'time'.");
	else if(!observation_validate_time($data['time']))  $validationerrors[] = t('Please pass a valid time (HH:mm)');
	
	if(!isset($data['observer'])) $validationerrors[] = t("Please pass key 'observer'.");
	
	if(!isset($data['coordinate'])) $validationerrors[] = t("Please pass keys 'longitude' and 'latitude'.");
	else if(!observation_validate_coordinate($data['coordinate'])) $validationerrors[] = t("Please pass a correct location.");
	
	if(isset($data['details']['detmethod']) && isset($data['organism_artgroup_id'])) {
		if(!observation_validate_determination_method($data['details']['detmethod'], $data['organism_artgroup_id'])) {
			$validationerrors[] = t("Determinationmethod is not allowed for this argroup.");
		}
	}
// 	if(isset($data['details']['attributes']) && is_array($data['details']['attributes'])) {
// 		if(!observation_validate_attributes($data['details']['attributes'])) {
			
// 		}
// 	}

	if(isset($_FILES['files'])) {
		foreach($_FILES['files']['name'] as $fkey => $filename) {
			if($_FILES['files']['name'][$fkey] != '') {
				$fileextension = array_pop(explode('.', $_FILES['files']['name'][$fkey]));
				if(!observation_validate_fileextension($_FILES['files']['name'][$fkey])) {
					$validationerrors[] = t('.@extension is an unallowed fileextension!', array('@extension' => $fileextension));
				}
			}
		}
	}
	
	return $validationerrors;
}
