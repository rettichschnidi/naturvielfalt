<?php
/**
 * @file  webservice_inventory.inc
 * @author André Kälin, 2013
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
 * @author André Kälin, 2013
 */
function  webservice_parse_import_observations() {
	watchdog(WS_API_LOG_TYPE, 'invoked', array(), WATCHDOG_DEBUG);

	if (!isset($_SERVER['PHP_AUTH_USER'])) {
		return  webservice_auth();
	}

	// Authenticate the user (Drupal Authentification)
	$uid = user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']);
	if (!$uid) {
		return  webservice_auth();
	}
	/*NOT YET IMPLEMENTED!!!!!!*/
	header('HTTP/1.0 200 OK');
	print('not yet implemented');
	return;
	
	
	$user = user_load($uid);

	/**
	 * parse not useable post vars to be compatible with th observation module
	 * @var unknown_type
	 */
	$data = area_get_geocode_info(filter_xss($_POST['longitude']), filter_xss($_POST['latitude']));
	$data['organism_id'] = $_POST['organism_id'];
	$data['organism_artgroup_id'] = $_POST['organism_artgroup_id'];
	$data['count'] = $_POST['count'];
	$data['date'] = $_POST['date'];
	$data['time'] = $_POST['time'];
	$data['observer'] = $_POST['observer'];
	
	
	$accArray = organism_artgroup_get_attribute_by_name(t('Accuracy'));
	if (count($accArray) > 0)
		$data['attributes'][$accArray[0]] = $_POST['accuracy'] . 'm';
	$comArray = organism_artgroup_get_attribute_by_name(t('Comment'));
 	if (count($comArray) > 0)
 		$data['attributes'][$comArray[0]] = $_POST['comment'];
 	
	$data['meta_description'][] = $_POST['comment'];
	$data['set_user_uid'] = $user->uid;
	
	$validationerrors =  webservice_validate($data);
	if(!empty($validationerrors)) {
		header('HTTP/1.0 400 Bad Request');
		print('ERROR: Validation errors detected. Check the log file for details <br>');
		foreach ($validationerrors as $message) {
			print($message.'<br>');
		}
		
	}
	else {
		header('HTTP/1.0 200 OK');
		$observation_status = observation_save(false, $data, true);
		// get the data
		if ($observation_status['success'] == true) {
			print('SUCCESS');
		} else {
			header('HTTP/1.0 400 Bad Request');
			print('ERROR: Something went wrong.. check the log file for details <br>');
			foreach ($observation_status['message'] as $key => $message) {
				print($message.'<br>');
			}
		}
	}
}

function  webservice_auth() {
	header('WWW-Authenticate: Basic realm="Naturvielfalt"');
	header('HTTP/1.0 401 Unauthorized');

	watchdog(WS_API_LOG_TYPE, 'Authorization failed.', array(), WATCHDOG_INFO);
	die('ERROR: Please authorize');
}

function  webservice_validate($data) {
	$validationerrors = array();
	if(!isset($data['organism_id'])) $validationerrors[] = t("Please pass key 'organism_id'.");
	else if(!observation_validate_organism_id($data['organism_id']))  $validationerrors[] = t('Please enter an existing organism.');

	if(!isset($data['organism_artgroup_id']) || $data['organism_artgroup_id'] < 1) $validationerrors[] = t("Please pass key 'organism_artgroup_id'.");
	
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
	return $validationerrors;
}
