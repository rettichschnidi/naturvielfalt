<?php
function show_area_new_prop() {
	drupal_set_title(t('Create a new area'));
	return drupal_get_form('areaPropertyForm');
}

function areaPropertyForm($form_state) {
	$form['field_name'] = array(
  	'#type' => 'textfield',
  	'#title' => t('Field name'),
	);
	$form['locality'] = array(
	'#type' => 'textfield',
	'#title' => t('Locality'),
	);
	$form['zip'] = array(
	'#type' => 'textfield',
	'#title' => t('Township'),
	);
	$form['canton'] = array(
	'#type' => 'textfield',
	'#title' => t('Canton'),
	);
	$form['surface_area'] = array(
	'#type' => 'textfield',
	'#title' => t('Surface area (m2)'),
	);
	$form['country'] = array(
	'#type' => 'textfield',
	'#title' => t('Country'),
	);
	$form['submit'] = array(
  	'#type' => 'submit',
  	'#value' => t('Save'),
	);
	return $form;
}

function areaPropertyForm_submit($form, $form_state) {
	$field_name = $form['field_name']['#value'];
	$locality = $form['locality']['#value'];
	$zip = $form['zip']['#value'];
	$canton = $form['canton']['#value'];
	$country = $form['country']['#value'];
	
	$aid = db_insert('area')->fields(array(
    	'field_name' => $field_name,
    	'locality' => $locality,
	 	'zip' => $zip,
	 	'canton' => $canton,
	 	'country' => $country,
  	))
  ->execute();
	
	drupal_set_message(t('Area @field_name has been saved', array('@field_name' => $field_name)));
}
?>