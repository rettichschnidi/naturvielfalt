<?php
/**
 * @file naturvielfalt_webservice.inc
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
 * @author Ramon Gamma, 2012
 */
function naturvielfalt_webservice_parse_import_observations() {
	watchdog(WS_API_LOG_TYPE, 'invoked', array(), WATCHDOG_DEBUG);

	if (!isset($_SERVER['PHP_AUTH_USER'])) {
		return naturvielfalt_webservice_auth();
	}

	// Authenticate the user (Drupal Authentification)
	$uid = user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']);
	if (!$uid) {
		return naturvielfalt_webservice_auth();
	}
	$user = user_load($uid);

	watchdog(WS_API_LOG_TYPE, 'REQUEST :' . var_export($_POST, true), array(), WATCHDOG_DEBUG);
	watchdog(WS_API_LOG_TYPE, 'FILES :' . var_export($_FILES, true), array(), WATCHDOG_DEBUG);

	/**
	 * parse not useable post vars to be compatible with th observation module
	 * @var unknown_type
	 */

	$data = area_get_geocode_info(filter_xss($_POST['longitude']), filter_xss($_POST['latitude']));
	$data['organismn_id'] = $_POST['organismn_id'];
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

	watchdog(
		WS_API_LOG_TYPE,
		'Rewrited Request :' . var_export($data, true),
		array(),
		WATCHDOG_DEBUG);
	
	$validationerrors = naturvielfalt_webservice_validate($data);
	if(!empty($validationerrors)) {
		watchdog(
			WS_API_LOG_TYPE,
			'Validation of arguments failed, please provide all mandatory fields in the correct format:',
			array(),
			WATCHDOG_ERROR);
		foreach ($validationerrors as $message) {
			watchdog(WS_API_LOG_TYPE, $message, array(), WATCHDOG_ERROR);
		}
		print('ERROR: Validation errors detected. Check the log file for details');
	}
	else {
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
			print('ERROR: Something went wrong.. check the log file for details');
		}
	}
}

function naturvielfalt_webservice_auth() {
	header('WWW-Authenticate: Basic realm="Naturvielfalt"');
	header('HTTP/1.0 401 Unauthorized');

	watchdog(WS_API_LOG_TYPE, 'Authorization failed.', array(), WATCHDOG_INFO);
	die('ERROR: Please authorize');
}

function naturvielfalt_webservice_validate($data) {
	$validationerrors = array();
	if(!isset($data['organismn_id'])) $validationerrors[] = t("Please pass key 'organismn_id'.");
	else if(!observation_validate_organism($data['organism']))  $validationerrors[] = t('Please enter an existing organism.');
	
	if(!isset($data['organism_artgroup_id'])) $validationerrors[] = t("Please pass key 'organism_artgroup_id'.");
	
	if(!isset($data['count'])) $validationerrors[] = t("Please pass key 'count'.");
	else if(!observation_validate_count($data['count']))  $validationerrors[] = t('The count has to be a number and bigger than zero');
	
	if(!isset($data['date'])) $validationerrors[] = t("Please pass key 'date'.");
	else if(!observation_validate_date($data['data']))  $validationerrors[] = t('Please pass a valid date (dd.mm.yyyy)');
	
	if(!isset($data['observer'])) $validationerrors[] = t("Please pass key 'observer'.");
	
	if(!isset($data['coordinate'])) $validationerrors[] = t("Please pass key'coordinate'.");
	else if(!observation_validate_coordinate($data['coordinate'])) $validationerrors[] = t("Please pass a correct location.");
	
	return $validationerrors;
}
