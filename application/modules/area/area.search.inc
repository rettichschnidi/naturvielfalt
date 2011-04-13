<?php
function show_area_search() {
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-select.css');
	drupal_set_title(t('Areas'));

	$output['areaSearch'] = array(
		'#theme' => 'area.search',
	);
	$output['FieldName'] = drupal_get_form('fieldNameForm');
	$output['googleMaps'] = drupal_get_form('googleMapsForm');

	return $output;
}

function googleMapsForm() {
	$form['GoogleMaps'] = array(
		'#type' => 'fieldset', 
  		'#title' => t('Search by Google Maps:'), 
  		'#weight' => 3, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => FALSE,
		'searchByMaps' => array('#theme' => 'area.search.byMaps'),
	);
	return $form;
}

function fieldNameForm() {
	$form['FieldName'] = array(
		'#type' => 'fieldset',
  		'#title' => t('Search by field name:'), 
  		'#weight' => 5, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => TRUE,
	);

	$form['FieldName']['searchByField'] = array(
		'search' => array('#theme' => 'area.search.byField'),
	);

	return $form;
}
?>