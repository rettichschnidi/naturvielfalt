<?php

/**
 * Set Title and invoke drupal form
 */
function inventory_new() {
	drupal_set_title(t('Create New Inventory'));

	return drupal_get_form('inventory_new_form');
}

/**
 * Form to create new inventory
 * 
 * @param unknown_type $form
 * @param unknown_type $form_state
 * @return form of new inventory
 */
function inventory_new_form($form, &$form_state) {
	if(isset($_SESSION['inventory']['name']))
		$name = $_SESSION['inventory']['name'];
	if(isset($_SESSION['inventory']['description']))
		$description = $_SESSION['inventory']['description']['value'];
		
	$progressElement = array(t('Inventory'), t('Area'), t('Element')); // List of elements
	$form['progress'] = array(
		'#theme' => 'progress_done',
		'#elements' => $progressElement,
		'#active' => 1, 		// Indicate active Element
	);
	
	// creating forms
	
	$form['new'] = array (
		'#type' => 'fieldset', 
		'#weight' => 5,
	);
	
	$form['new']['name'] = array(
  		'#type' => 'textfield', 
  		'#title' => t('Inventarname'), 
  		'#size' => 100, 
  		'#maxlength' => 128, 
  		'#required' => TRUE,
		'#default_value' => $name,
	);

	$form['new']['description'] = array(
			'#type' => 'text_format',
    		'#title' => t('Beschreibung'),
    		'#default_value' => $description,
    		'#format' => filter_default_format(),
	      	'#widget' => array (
        		'#type' => 'text_textarea',
			),
      		'#settings' => array (
        		'#text_processing' => '0',
			),
	);

	$form['save'] = array (
		'#type' => 'fieldset', 
		'#weight' => 10,
	);
	
	$form['save']['submit'] = array (
		'#type' => 'submit', 
		'#value' => t("Continue"),
	);

	return $form;
}

/**
 * Submit function of new inventory form
 * 
 * @param unknown_type $form
 * @param unknown_type $form_state
 * @return redirect-string to area
 */
function inventory_new_form_submit($form, &$form_state){
	$_SESSION['inventory']['name'] = $form_state['values']['name'];
	$_SESSION['inventory']['shared'] = true;
	$_SESSION['inventory']['description'] = $form_state['values']['description'];

	return $form_state['redirect'] = 'area/new';
}
?>