<?php
/**
 * @file area.create.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');
module_load_include('inc', 'area', 'area.ajax');

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

?>
