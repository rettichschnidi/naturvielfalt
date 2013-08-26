<?php
/**
 * @file area.create.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');

/**
 * Create new area.
 * @return string Rendered array
 */
function area_new_area() {
	$output = array();
	drupal_set_title(t('Create new area'));

	$output['map'] = array(
			'#theme' => 'area',
			'#ch1903' => true,
			'#search' => true,
			'#height' =>  '550px',
			'#action' => 'create',
			'#infowindow_createform_fetch_url' => base_path() . 'area/getnewareanameajaxform',
			'#geometries_fetch_url' => base_path() . 'areas/json',
			'#infowindow_content_fetch_url_area' => base_path() . 'area/{ID}/areaoverview/ajaxform'
	);
	return drupal_render($output);
}

/**
 * Return a form which allows the user to submit an area name.
 * The submitted results will be passed to area_save();
 * 
 * @param array $form
 * 	Drupal form
 * @param array &$form_state
 *  An associative array containing the current state of the form.
 * @return array
 *  Drupal form
 */
function area_new_areaname_form($form, &$form_state) {
	$form = area_address_hidden_form();
	$form['fieldname'] = array(
			'#type' => 'textfield',
			'#title' => t('Field name/area name'),
			'#description' => t('Please insert a custom name for this area.'),
			'#size' => 20,
			'#maxlength' => 80,
			'#attributes' => array(
					'placeholder' => t('My area')
			)
	);

	$form['button'] = array(
			'#type' => 'submit',
			'#value' => t('Create'),
			'#submit' => array(
					'area_save'
			)
	);
	return $form;
}

/**
 * Create a new area or update it (only from webservice).
 * 
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 *  @param boolean $from_api
 *  true if method was invoked from the webservice api
 * @todo Maybe use the ajax version?
 */
function area_save($form = false, &$form_state, $from_api = false) {
	global $user;
	if (false) {
		$debugmessage = var_export($form_state['values'], true);
		drupal_set_message("Creating area: <pre>$debugmessage</pre>");
	}
	$areaid = 0;
	/* 
	 * Get submitted values and escape values before submitting
	 * them to the database.
	 */
	if($from_api) {
		$name = filter_xss($form_state['fieldname']);
		$zip = filter_xss($form_state['zip']);
		$canton = filter_xss($form_state['canton']);
		$country = filter_xss($form_state['country']);
		$township = filter_xss($form_state['township']);
		$areaType = filter_xss($form_state['area_type']);
		$altitude = !empty($form_state['altitude'])
		? filter_xss($form_state['altitude']) : 0;
		$jsonCoordinates = filter_xss($form_state['area-coords']);
		$comment = isset($form_state['comment']) ? filter_xss($form_state['comment']) : '';
		
		$date_parts = explode('.', filter_xss($form_state['date']));
		$timesplit = array(null, null);
		if(isset($form_state['time'])) $timesplit = explode(':', filter_xss($form_state['time']));
		
		$timestamp = mktime($timesplit[0], $timesplit[1], null, $date_parts[1], $date_parts[0], $date_parts[2]);
		$createdate = date('Y-m-d H:i:s', $timestamp);
		
		$areaid = intval($form_state['area_id']);
	} else {
		$name = filter_xss(
			!empty($form_state['values']['fieldname'])
					? $form_state['values']['fieldname'] : date("r"));
		$zip = filter_xss($form_state['values']['zip']);
		$canton = filter_xss($form_state['values']['canton']);
		$country = filter_xss($form_state['values']['country']);
		$township = filter_xss($form_state['values']['township']);
		$areaType = filter_xss($form_state['values']['area_type']);
		$altitude = !empty($form_state['values']['altitude'])
				? filter_xss($form_state['values']['altitude']) : 0;
		$jsonCoordinates = filter_xss($form_state['values']['area-coords']);
		$comment = '';
		$createdate = 'NOW()';
	}
	
	$userid = isset($form_state['set_user_uid']) && $form_state['set_user_uid'] ? $form_state['set_user_uid']
	: $user->uid;
	
	if (empty($jsonCoordinates) || empty($areaType)) {
		debug($areaType, 'areaType');
		drupal_set_message(t('New area could not be created.'), 'error');
		$form_state['redirect'] = 'area/new';
		return;
	}
	$transaction = db_transaction();
	try {
		//update area
		if($areaid > 0) {
			$area = area_get_full_dataset_of_specific_area($areaid);
			$area_geometry_id = $area['area_geometry_id'];
			$acl_id = $area['acl_id'];
			
			area_update_geometry($area_geometry_id, array('area_points' => $jsonCoordinates,
						'type' => $areaType,
						'zip' => $zip,
						'canton' => $canton,
						'country' => $country,
						'township' => $township,
						'altitude' => $altitude,
				), $from_api ? true : false);
			db_update('area')->fields(
					array(
							'name' => $name,
							'comment' => $comment,
							'create_time' => $createdate,
							'modify_time' => 'NOW()',
							'acl_id' => $acl_id,
							'area_geometry_id' => $area_geometry_id
					))
					->condition('id', $areaid)
				->execute();
				$output['update'] = true;
		}
		//create new area
		else {
			$geometry_id = area_create_geometry(
				array('area_points' => $jsonCoordinates,
						'type' => $areaType,
						'zip' => $zip,
						'canton' => $canton,
						'country' => $country,
						'township' => $township,
						'altitude' => $altitude,
				), $from_api ? true : false);
			assert($geometry_id);
			$acl_id = new_acl('area', $userid);
			$areaid = db_insert('area')->fields(
					array(
							'name' => $name,
							'comment' => $comment,
							'create_time' => $createdate,
							'modify_time' => 'NOW()',
							'acl_id' => $acl_id,
							'area_geometry_id' => $geometry_id
					))
				->execute();
			
			// Read access for ALL group
			add_group_to_acl($acl_id, variable_get('ACL_GROUP_ALL_ID'), variable_get('ACL_READ'));
		}
		
		/**
		 * Store incoming medias from webservice
		 */
		if($from_api) {
			
			$image = array(
					'item_id' => $areaid,
					'author' => $user->name,
					'location' => $township . ' ' . $canton,
			);
			
			gallery_implementation_save_manual('area', $image, 'area_id');
		}
		
		$output['success'] = true;
		$output['guid'] = $areaid;
		
	} catch (Exception $e) {
		$transaction->rollback();
		watchdog_exception('area', $e);
		$output['success'] = false;
		$output['message'] = array(
				t('Could not save the area because of an unknown error.'),
				$e->getMessage()
		);
		drupal_set_message(t('New area could not be created.'), 'error');
		$form_state['redirect'] = 'area/new';
	}
	
	if ($from_api) {
		return $output;
	}
	else {
		drupal_set_message(
		t('Area "@areaname" got created.', array('@areaname' => $name)));
		$form_state['redirect'] = 'area/' . $areaid . '/show';
	}
}
?>
