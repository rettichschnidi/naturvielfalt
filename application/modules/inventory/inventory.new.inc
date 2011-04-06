<?php
function inventory_new_form() {
	$form['name'] = array(
  		'#type' => 'textfield', 
  		'#title' => t('Name'), 
  		'#default_value' => '', 
  		'#size' => 100, 
  		'#maxlength' => 128, 
  		'#required' => TRUE,
	);

	$form['is_public'] = array(
  		'#type' => 'checkbox', 
  		'#title' => t('make public'),
	);

	$form['submit'] = array(
		'#type' => 'submit', 
		'#value' => t('new inventory'),
		'#submit' => array('inventory_save'),
	);

	return $form;
}

function inventory_new($area_id) {
	error_log("new in".$area_id);	
	drupal_set_title(t('New Inventory for '. get_are_name(arg(2))." (".$area_id.")"));

	return drupal_get_form('inventory_new_form');
}

function inventory_save($form, &$form_state){
	error_log("inventory_save");

	$name = $form_state['values']['name'];
	$area_id = arg(2);
	 
	 $nid = db_insert('head_inventory')->fields(array(
    	'area_id' => $area_id,
    	'name' => $name,
  	))
  ->execute();
  
	return $form_state['redirect'] = 'inventory/'.$nid;
}


function get_are_name($_area_id){
	
	$area_name = t("no name");

	$areas = db_query('	SELECT field_name
						FROM area
						WHERE id = :id;', array(':id' => $_area_id));
	
	foreach($areas as $area) {
		if(!is_null($area->field_name))
		$area_name =  $area->field_name;
	}

	return $area_name;
}

?>