<?php
function inventory_new_form() {

	$inventory_text = "";
	$description_text = "";
	$shared_value = true;

	// determine if page is in edit mode
	if(arg(1) == 'edit') {
		$edit_mode = true;

		$inventories = db_query('	SELECT name,shared,description,owner_id
									FROM head_inventory
									WHERE id = :id;', array(':id' => arg(2)));

		foreach($inventories as $inventory) {
			global $user;
			if($inventory->owner_id != $user->uid) {
				drupal_set_title(t('Zugriff verweigert'));
				drupal_set_message(t('Zugriff verweigert'), 'error');
				$form['error'] = array(
				'#type' => 'item',
				'#title' => t('Die Daten dürfen nur vom Ersteller des Invnetars betrachtet und geändert werden.'),
				);
				return $form;
			}
			$inventory_text = $inventory->name;
			$description_text = $inventory->description;
			$shared_value = $inventory->shared;
		}
	}
	else {
		$edit_mode = false;
	}

	// creating forms
	$form['name'] = array(
  		'#type' => 'textfield', 
  		'#title' => t('Name'), 
  		'#size' => 100, 
  		'#maxlength' => 128, 
  		'#required' => TRUE,
		'#default_value' => $inventory_text,
	);

	$form['share'] = array(
  		'#type' => 'checkbox', 
  		'#title' => t('öffentlich'),
		'#default_value' => $shared_value, 
	);

	$form['description'] = array(
			'#type' => 'text_format',
    		'#title' => t('Beschreibung'),
    		'#default_value' => $description_text,
    		'#format' => filter_default_format(),
	      	'#widget' => array(
        	'#type' => 'text_textarea',
	),
      	'#settings' => array(
        	'#text_processing' => '0',
	),
	);

	if($edit_mode) {
		$form['update'] = array(
			'#type' => 'submit', 
			'#value' => t("Speichern"),
			'#submit' => array('inventory_update'),
		);
	}
	else {
		$form['save'] = array(
			'#type' => 'submit', 
			'#value' => t("Neues Inventar anlegen"),
			'#submit' => array('inventory_save'),
		);
	}

	return $form;
}

function inventory_new($area_id) {
	drupal_set_title(t('Neues Inventar für: '. get_are_name(arg(2))." (".arg(2).")"));

	return drupal_get_form('inventory_new_form');
}

function inventory_edit($head_inventory_id) {
	drupal_set_title(t('Inventar bearbeiten'));

	return drupal_get_form('inventory_new_form');
}

function inventory_update($form, &$form_state){

	$name = $form_state['values']['name'];
	$shared = $form_state['values']['share'];
	$description = $form_state['values']['description'];
	$inventory_id = arg(2);

	db_update('head_inventory')
	->condition('id', $inventory_id)
	->fields(array(
				'name' => $name,
				'shared' => $shared,
				'description' => $description['value']
	))
	->execute();

	drupal_set_message(t('Inventar aktualisiert'));

	return $form_state['redirect'] = 'inventory/'.$inventory_id;
}

function inventory_save($form, &$form_state){
	// Save Data
	$name = $form_state['values']['name'];
	$shared = $form_state['values']['share'];
	$description = $form_state['values']['description'];
	$area_id = arg(2);
	global $user;
	$owner = $user->uid;

	$nid = db_insert('head_inventory')->fields(array(
	'area_id' => $area_id,
	'name' => $name,
	'shared' => $shared,
	'owner_id' => $owner,
	'description' => $description['value']
	))
	->execute();

	drupal_set_message(t('Neues Inventar erstellt'));

	return $form_state['redirect'] = 'inventory/'.$nid;
}

function get_are_name($_area_id){

	$area_name = t("-");

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