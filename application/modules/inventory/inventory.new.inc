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
			if($inventory->owner_id != $user->uid) return null;
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
  		'#default_value' => '', 
  		'#size' => 100, 
  		'#maxlength' => 128, 
  		'#required' => TRUE,
		'#default_value' => $inventory_text,
	);

	$form['share'] = array(
  		'#type' => 'checkbox', 
  		'#title' => t('share'),
		'#default_value' => $shared_value, 
	);
	
	if($edit_mode && 1==2)
	{
		$form['upload'] = array(
			'#type' => 'file', 
			'#title' => t('Attach a file'), 
			'#size' => 48,
		);
	}

	$form['description'] = array(
			//'#type' => 'text_format', //->change to this for richtext editor
    		'#type' => 'textarea',
    		'#title' => t('Description'),
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
			'#value' => t("save"),
			'#submit' => array('inventory_update'),
		);
	}
	else {
		$form['save'] = array(
			'#type' => 'submit', 
			'#value' => t("new inventory"),
			'#submit' => array('inventory_save'),
		);
	}

	return $form;
}

function inventory_new($area_id) {
	drupal_set_title(t('New Inventory for '. get_are_name(arg(2))." (".arg(2).")"));

	return drupal_get_form('inventory_new_form');
}

function inventory_edit($head_inventory_id) {
	drupal_set_title(t('Edit Inventory'));

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
				'description' => $description
			))
			->execute();
	
	// File upload
	if(1==2)
	{
		$validators = array(
			//'file_validate_extensions' => array('doc txt pdf'),
			//'file_validate_size' => array(1000000, 0),
		);
	
		$destination = 'public://swissmon/inventory/'.$nid;
	
		file_prepare_directory($destination, FILE_CREATE_DIRECTORY);
	
		$file = file_save_upload('upload', $validators, $destination);
		if ($file) {
			$form_state['values']['upload'] = $file;
			error_log($file->fid);
	
			db_update('head_inventory')
			->condition('id', $nid)
			->fields(array('file_id' => $file->fid))
			->execute();
		}
		else
		{
			//form_set_error("nothing saved");
		}
	}
	
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
	'description' => $description,
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