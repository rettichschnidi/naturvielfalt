<?php
/**
 * @file  webservice_inventory.inc
 * @author André Kälin, 2013
 * @copyright 2012 Naturwerk, Brugg
 */
module_load_include('inc', 'inventory');
module_load_include('inc', 'inventory', 'inventory.validate');

define('WS_API_LOG_TYPE', 'webservice api');

/**
 * Serverside script for receiving the data from the iPhone/Android Application.
 * It receives POST requests and stores it directly in the database. Normal drupal
 * authentication is used.
 *
 * @author André Kälin, 2013
 */
function  webservice_parse_import_inventories() {
	global $user;
	watchdog(WS_API_LOG_TYPE, 'invoked', array(), WATCHDOG_DEBUG);

	if (!isset($_SERVER['PHP_AUTH_USER'])) {
		return  webservice_auth();
	}

	// Authenticate the user (Drupal Authentification)
	$uid = user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']);
	if (!$uid) {
		return  webservice_auth();
	}
	
	
	$user = user_load($uid);

	/**
	 * parse not useable post vars to be compatible with th observation module
	 * @var unknown_type
	 */
	$data['id'] = $_POST['guid'];
	$data['name'] = $_POST['name'];
	if (isset($_POST['description'])) $data['description'] = $_POST['description'];
	$data['area_id'] = $_POST['area_id'];
	$data['date'] = $_POST['date'];
	$data['time'] = $_POST['time'];
	$data['observer'] = $_POST['observer'];
	$data['set_user_uid'] = $user->uid;
	debug($data);
	debug('inventory webservice');
	$validationerrors =  webservice_validate($data);
	if(!empty($validationerrors)) {
		header('HTTP/1.0 400 Bad Request');
		print('ERROR: Validation errors detected.<br>');
		foreach ($validationerrors as $message) {
			print($message.'<br>');
		}
	}
	else {
		//if empty string has been passed, use login name
		if(empty($data['observer'])) $data['observer'] = $user->name;
		
		$inventory_status = inventory_save(false, $data, true);
		// get the data
		if ($inventory_status['success'] == true) {
			header('HTTP/1.0 200 OK');
			$i=0;
			//pass return values in "key=value&key2=value2" notation
			foreach($inventory_status as $key => $message) {
				if($i > 0 && $i < sizeof($inventory_status)) print('&');
				print($key.'='.$message);
				$i++;
			}
		} else {
			header('HTTP/1.0 400 Bad Request');
			print('ERROR: Something went wrong.. Could not persist the inventory. <br>');
			foreach ($inventory_status['message'] as $key => $message) {
				print($message.'<br>');
			}
		}
	}
}

function  webservice_validate($data) {
	$validationerrors = array();
	
	if(!isset($data['id'])) $validationerrors[] = t("Please pass key 'guid'.");
	else if(!inventory_validate_inventory_id($data['id']))  $validationerrors[] = t('Please pass a valid inventory id.');
	else if(!inventory_validate_permission($data['id'])) $validationerrors[] = t("Permission denied.");
	
	if(!inventory_validate_area($data['area_id'])) $validationerrors[] = t('Please pass a valid area / permission denied');
	
	if(!isset($data['name'])) $validationerrors[] = t("Please pass key 'name'.");
	else if(!inventory_validate_name($data['name']))  $validationerrors[] = t('Please pass a valid inventory name.');
	
	if(!isset($data['date'])) $validationerrors[] = t("Please pass key 'date'.");
	else if(!inventory_validate_date($data['date']))  $validationerrors[] = t('Please pass a valid date (dd.mm.yyyy)');
	
	if(!isset($data['time'])) $validationerrors[] = t("Please pass key 'time'.");
	else if(!inventory_validate_time($data['time']))  $validationerrors[] = t('Please pass a valid time (HH:mm)');

	//files not yet implemented
// 	if(isset($_FILES['files'])) {
// 		foreach($_FILES['files']['name'] as $fkey => $filename) {
// 			if($_FILES['files']['name'][$fkey] != '') {
// 				$fileextension = array_pop(explode('.', $_FILES['files']['name'][$fkey]));
// 				if(!inventory_validate_fileextension($_FILES['files']['name'][$fkey])) {
// 					$validationerrors[] = t('.@extension is an unallowed fileextension!', array('@extension' => $fileextension));
// 				}
// 			}
// 		}
// 	}
	
	return $validationerrors;
}
