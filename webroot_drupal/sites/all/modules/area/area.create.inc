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
			'#search' => true,
			'#ch1903' => true,
			'#showall' => true,
			'#action' => 'create',
			'#theme' => 'area'
	);
	return drupal_render($output);
}

/**
 * Return a form which allows the user to submit an area name.
 * The submitted results will pass area_create_new_area();
 */
function area_new_areaname_form($form, &$form_state) {
	$form = area_address_hidden_form();
	$form['fieldname'] = array(
			'#type' => 'textfield',
			'#title' => t('Field name/area name'),
			'#description' => t('Please insert a custom name for this area.'),
			'#size' => 20,
			'#maxlength' => 80
	);

	$form['button'] = array(
			'#type' => 'submit',
			'#value' => t('Create'),
			'#submit' => array(
					'area_create_new_area'
			)
	);
	return $form;
}

/**
 * Create a new area
 * 
 * @param array $form Drupal form
 * @param array $form_state Drupal form state with all data
 * @todo Maybe use the ajax version?
 */
function area_create_new_area($form, &$form_state) {
	if (false) {
		$debugmessage = var_export($form_state['values'], true);
		drupal_set_message("Creating area: <pre>$debugmessage</pre>");
	}
	/* 
	 * Get submitted values and escape values before submitting
	 * them to the database.
	 */
	$name = filter_xss(
		!empty($form_state['values']['fieldname']) ? $form_state['values']['fieldname']
				: date("r"));
	$zip = filter_xss($form_state['values']['zip']);
	$canton = filter_xss($form_state['values']['canton']);
	$country = filter_xss($form_state['values']['country']);
	$township = filter_xss($form_state['values']['township']);

	$areaType = filter_xss($form_state['values']['area_type']);
	$altitude = !empty($form_state['values']['altitude']) ? filter_xss(
				$form_state['values']['altitude']) : 0;
	$jsonCoordinates = filter_xss($form_state['values']['area-coords']);
	$latitude = filter_xss($form_state['values']['latitude']);
	$longitude = filter_xss($form_state['values']['longitude']);

	$transaction = db_transaction();
	try {
		$geometry_id = area_create_geometry(
			array('area_points' => $jsonCoordinates,
					'type' => $areaType,
					'zip' => $zip,
					'canton' => $canton,
					'country' => $country,
					'township' => $township,
					'altitude' => $altitude,
			));
		assert($geometry_id);
		$acl_id = new_acl("Area: $name");
		$area_id = db_insert('area')->fields(
				array('name' => $name,
						'create_time' => 'NOW()',
						'modify_time' => 'NOW()',
						'acl_id' => $acl_id,
						'area_surface_id' => $geometry_id
				))
			->execute();

		drupal_set_message(
			t('Area "@areaname" got created.', array('@areaname' => $name)));
		drupal_goto('area/' . $area_id . '/show');
	} catch (Exception $e) {
		$transaction->rollback();
		watchdog_exception('area', $e);
		drupal_set_message(
			t('New area could not be created.', array('@areaname' => $name)),
			'error');
		drupal_goto('area/new');
	}
}
?>
