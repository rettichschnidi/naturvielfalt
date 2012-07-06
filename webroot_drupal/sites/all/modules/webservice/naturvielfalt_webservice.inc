<?php
/**
 * @file naturvielfalt_webservice.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */
module_load_include('inc', 'observation', 'observation.ajax');

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

	// Set content-length
	// header('Content-Length: ' . ob_get_length()); <---- MADE THE APP CRASH.... Don't really understand why

	// Authenticate the user (Drupal Authentification)
	$uid = user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']);
	if (!$uid)
		return naturvielfalt_webservice_auth();
	$user = user_load($uid);

	watchdog(WS_API_LOG_TYPE, 'REQUEST :' . var_export($_POST, true), array(), WATCHDOG_DEBUG);
	watchdog(WS_API_LOG_TYPE, 'FILES :' . var_export($_FILES, true), array(), WATCHDOG_DEBUG);

	/**
	 * parse not useable post vars to be compatible with th eobservation module
	 * @var unknown_type
	 */

	$data = area_get_geocodeInfo(filter_xss($_POST['longitude']), filter_xss($_POST['latitude']));
	// 		die(print_r($data));
	$data['organismn_id'] = $_POST['organismn_id'];
	$data['organism_artgroup_id'] = $_POST['organism_artgroup_id'];
	$data['count'] = $_POST['count'];
	$data['date'] = $_POST['date'];
	$data['observer'] = $_POST['observer'];
	$data['attributes_custom_names'][0] = t('Accuracy');
	$data['attributes_custom_values'][0] = $_POST['accuracy'] . 'm';
	$data['attributes_custom_names'][1] = t('Comment');
	$data['attributes_custom_values'][1] = $_POST['comment'];
	$data['meta_description'][] = $_POST['comment'];
	$data['set_user_uid'] = $user->uid;

	watchdog(
		WS_API_LOG_TYPE,
		'Rewrited Request :' . var_export($data, true),
		array(),
		WATCHDOG_DEBUG);

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

function naturvielfalt_webservice_auth() {
	header('WWW-Authenticate: Basic realm="Naturvielfalt"'); // FIXME: Use auth-digest instead of auth basic
	header('HTTP/1.0 401 Unauthorized');

	watchdog(WS_API_LOG_TYPE, 'Authorization failed.', array(), WATCHDOG_INFO);
	die('ERROR: Please authorize');
}
