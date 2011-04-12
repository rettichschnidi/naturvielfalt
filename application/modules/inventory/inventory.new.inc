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
				'#title' => t('Die Daten d&uuml;rfen nur vom Ersteller des Invnetars betrachtet und ge&auml;ndert werden.'),
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
  		'#title' => t('&ouml;ffentlich'),
		'#default_value' => $shared_value, 
	);
	
	/*
	if($edit_mode)
	{
		$form['upload'] = array(
			'#type' => 'file', 
			'#title' => t('Datei uploaden'), 
			'#size' => 48,
		);

		$form['upload_submit'] = array(
			'#type' => 'submit', 
			'#value' => t("Upload"),
			'#submit' => array('file_upload'),
		);

		$tableHeader[] = t('Dateiname');
		$tableHeader[] = t('Typ');
		$tableHeader[] = t('Grösse');
		$tableHeader[] = t('Datum');

		$result = db_query('SELECT fm.filename, fm.filemime, fm.timestamp, fm.fid, fm.filesize
						FROM head_inventory_file_managed hifm, file_managed fm
						WHERE fm.fid = hifm.file_id
						AND hifm.head_inventory_id = :id;', array(':id' => arg(2))
		);

		if(!empty($result))
		{
			foreach ($result as $record) {
				$filename = l($record->filename, 'sites/default/files/swissmon/inventory/'.arg(2).'/'.$record->filename);
				$filemime = $record->filemime;
				$filesize = $record->filesize;
				$timestamp = date('d.m.Y',$record->timestamp);
				$row = array();
				$row[] = $filename;
				$row[] = $filemime;
				$row[] = $filesize;
				$row[] = $timestamp;
				$rows[] = $row;
			}

			if(isset($rows))
			{				
				$form['file_overview'] = array(
    			'#theme' => 'table', 
   				'#header' => $tableHeader,
				'#rows' => $rows,
				);
			}
		}
	}
	*/

	$form['description'] = array(
			'#type' => 'text_format', //->change to this for richtext editor
    		//'#type' => 'textarea',
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
	drupal_set_title(t('Neues Inventar f&uml;r: '. get_are_name(arg(2))." (".arg(2).")"));

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

/*
function file_upload($form, &$form_state){

	$head_inventory_id = arg(2);

	// File upload
	$validators = array(
		'file_validate_extensions' => array('doc txt pdf docx'),
		'file_validate_size' => array(1000000, 0),
	);
	
	$destination = 'public://swissmon/inventory/'.$head_inventory_id;

	file_prepare_directory($destination, FILE_CREATE_DIRECTORY);

	$file = file_save_upload('upload', $validators, $destination);
	if ($file) {
		
		$file->status |= FILE_STATUS_PERMANENT;
		$file = file_save($file);
		
		$form_state['values']['upload'] = $file;

		$nid = db_insert('head_inventory_file_managed')->fields(array(
					'head_inventory_id' => $head_inventory_id,
					'file_id' => $file->fid
		))
		->execute();
		drupal_set_message(t('Datei gespeichert'));
	}
	else {
		drupal_set_message(t('Datei nicht gespeichert!'), 'error');	
	}
	
	return $form_state['redirect'] = 'inventory/edit/'.$head_inventory_id;
}
*/

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