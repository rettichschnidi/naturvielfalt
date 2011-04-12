<?php
/**
 * Menu callback. This will be called if someone visits for example drupal/inventory/3.
 */
function inventory_form() {
	
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-ui-1.8.6.custom.min.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-texotela-numeric.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/ext/ext-jquery-adapter-debug.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/ext/ext-base-debug.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/JSON.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/util.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/inventory.js');

	drupal_add_css(drupal_get_path('module', 'inventory') . '/css/main.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));

	
	$form['inventory'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Inventar'), 
  		'#weight' => 3, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => FALSE,
	);
	
	// inventories ajax form
	$form['inventory']['inventories']['#markup'] = '<div id="inventories"></div>';
		 
	$form['inventory']['head_inventory_id'] = array('#type' => 'hidden', '#value' => arg(1));
	
	$options = array();

	$result = db_query("SELECT it.id, it.name
		FROM inventory_type it
		ORDER BY it.id desc;");

	foreach ($result as $record) {
		$options[$record->id] = $record->name;
	}

	$form['inventory']['inventory_types'] = array(
    	'#type' => 'select',
    	'#title' => t('Inventargruppe'),
    	'#default_value' => variable_get('name', 259200),
    	'#options' => $options,
	);

	$form['inventory']['add_inventory']['#markup'] = '<div id="add_inventory"></div><input onclick="onAddInventory();" type="button" id="add-inventory" name="button" value="Inventargruppe hinzufügen">';
   
	
   	$form['inventory_set'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Details zum Inventar'), 
  		'#weight' => 4, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => TRUE,
	);
	
		$tableHeaderInventory[] = t('Id');
		$tableHeaderInventory[] = t('Name');
		$tableHeaderInventory[] = t('Gebiet');
		$tableHeaderInventory[] = t('Öffentlich');
		$tableHeaderInventory[] = t('Besitzer');

		$result = db_query('SELECT hi.id, hi.area_id, a.field_name, hi.name, hi.shared, hi.owner_id 
							FROM head_inventory hi, area a
							WHERE a.id = hi.area_id
							AND hi.id = :id;', array(':id' => arg(1))
		);
		
		global $user;

		if(!empty($result))
		{
			foreach ($result as $record) {
				$id = $record->id;
				$area = $record->field_name;
				$name = $record->name;
				$owner=user_load($record->owner_id)->name;
				//$owner = 'hallo';
				if($shared = $record->shared == TRUE)
					$shared = t("Ja");
				else $shared = t("Nein");
				$rowInventory = array();
				$rowInventory[] = $id;
				$rowInventory[] = $name;
				$rowInventory[] = $area;
				$rowInventory[] = $shared;
				$rowInventory[] = $owner;
				$rowsInventory[] = $rowInventory;
			}

			if(isset($rowsInventory))
			{				
				$form['inventory_set']['overview'] = array(
    			'#theme' => 'table', 
   				'#header' => $tableHeaderInventory,
				'#rows' => $rowsInventory,
				);
			}
		}
	
	$form['inventory_set']['edit'] = array(
   		'#type' => 'submit',
   		'#value' => t('Bearbeiten'),
   		//'#weight' => 15,
   		'#submit' => array('inventory_edit'),
   );
	
	$form['fileset'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Dateien'), 
  		'#weight' => 5, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => TRUE,
	);
	
		$tableHeader[] = t('Beschreibung');
		$tableHeader[] = t('Dateiname');
		$tableHeader[] = t('Typ');
		$tableHeader[] = t('Grösse');
		$tableHeader[] = t('Datum');

		$result = db_query('SELECT fm.filename, fm.filemime, fm.timestamp, fm.fid, fm.filesize, hifm.description
						FROM head_inventory_file_managed hifm, file_managed fm
						WHERE fm.fid = hifm.file_id
						AND hifm.head_inventory_id = :id;', array(':id' => arg(1))
		);

		if(!empty($result))
		{
			foreach ($result as $record) {
				$description = $record->description;
				$filename = l($record->filename, 'sites/default/files/swissmon/inventory/'.arg(1).'/'.$record->filename);
				$filemime = $record->filemime;
				$filesize = $record->filesize;
				$timestamp = date('d.m.Y',$record->timestamp);
				$row = array();
				$row[] = $description;
				$row[] = $filename;
				$row[] = $filemime;
				$row[] = $filesize;
				$row[] = $timestamp;
				$rows[] = $row;
			}

			if(isset($rows))
			{				
				$form['fileset']['file_overview'] = array(
    			'#theme' => 'table', 
   				'#header' => $tableHeader,
				'#rows' => $rows,
				);
			}
		}
		
		$form['fileset']['upload'] = array(
			'#type' => 'file', 
			'#title' => t('Datei uploaden'), 
			'#size' => 48,
		);
		
		$form['fileset']['file_description'] = array(
  			'#type' => 'textfield', 
  			'#title' => t('Beschreibung der Datei'), 
  			'#size' => 100, 
  			'#maxlength' => 128, 
  			'#required' => FALSE,
		);

		$form['fileset']['upload_submit'] = array(
			'#type' => 'submit', 
			'#value' => t("Upload"),
			'#submit' => array('file_upload'),
		);
		
		/*
		$form['image_example_image_fid'] = array(
  			'#title' => t('Image'),
  			'#type' => 'managed_file',
  			'#description' => t('The uploaded image will be displayed on this page using the image style choosen below.'),
  			'#default_value' => variable_get('image_example_image_fid', ''),
  			'#upload_location' => 'public://image_example_images/',
		);
		*/
	
	return $form;
}

/**
 * Shows the inventories
 */
function inventory_show($head_inventory_id) {

	if(inventory_permission_check($head_inventory_id)==true)
	{
		drupal_set_title(t(getInventoryTitle($head_inventory_id)." (".$head_inventory_id.")"));
		
		$json = getInitialInventoryJSON($head_inventory_id);

		drupal_add_js('var exInvs = ' . $json, 'inline');
		drupal_add_js('jQuery(document).ready(function(){initInventory();});', 'inline');
		return drupal_get_form('inventory_form');
	}
	else 
	{		
		drupal_set_title(t("Inventar nicht vorhanden"));
		drupal_set_message("Inventar nicht vorhanden", 'error');
		
		$form['error'] = array(
		'#type' => 'item',
		'#title' => t('Inventar wurde nicht gefunden. Bitte stellen sie sicher, dass das Inventar existiert und sie &uuml;ber die n&ouml;tigen Berechtigungen verf&uuml;gen.'),
    	);
		return $form;
	}
}

function inventory_permission_check($head_inventory_id)
{
	$result = db_query("	SELECT * FROM head_inventory hi 
								WHERE ((hi.shared='t') OR  (hi.owner_id=1))
								AND hi.id = :id"
								, array(':id' => arg(1))
	);
	
 	if ($result->rowCount()==0) {
    	return false;
  	}
  	return true;
}

function getInventoryTitle($head_inventory_id) {
	$inventory_title = t("-");
	
	$inventories = db_query('	SELECT name 
								FROM head_inventory
								WHERE id = :id;', array(':id' => $head_inventory_id));
	foreach($inventories as $inventory) {
		if(!is_null($inventory->name))
			$inventory_title =  $inventory->name;
	}
	
	return $inventory_title;
}

function inventory_edit($form, &$form_state){
	$head_inventory_id = arg(1);
	return $form_state['redirect'] = 'inventory/edit/'.$head_inventory_id;
}

function getInitialInventoryJSON($head_inventory_id) {

	$invArr = array();

	$inventories = db_query('SELECT id
							FROM inventory 
							WHERE head_inventory_id = :id;', array(':id' => $head_inventory_id));
	foreach($inventories as $inventory) {

		$inventory_id = $inventory->id;
		$invArr[$inventory_id] = array();

		// Get the inventory column description
		$invArr[$inventory_id]["invDesc"] = getInventoryColumnDescription($inventory_id);

		// Get inventory entries
		$inventoryEntries = db_query('SELECT e.id, e.organism_id, fo.name_de
		FROM inventory_entry e
		LEFT JOIN fauna_organism fo ON e.organism_id = fo.id
		WHERE e.inventory_id = :id;', array(':id' => $inventory_id));
		foreach($inventoryEntries as $inventoryEntry) {
			$inventoryEntryId = $inventoryEntry->id;
			$invArr[$inventory_id][$inventoryEntryId] = array();
			$invArr[$inventory_id][$inventoryEntryId]["orgId"] = $inventoryEntry->organism_id;
			$invArr[$inventory_id][$inventoryEntryId]["label"] = $inventoryEntry->name_de;

			// Get inventory columns
			$entries = db_query('SELECT * FROM inventory_type_attribute_inventory_entry WHERE inventory_entry_id = :id', array(':id' => $inventoryEntryId));
			foreach ($entries as $entry) {
				// Dropdown
				$colVal = "";
				if($entry->inventory_type_attribute_dropdown_value_id > 0) {
					$colVal = $entry->inventory_type_attribute_dropdown_value_id;
				} else {
					$colVal = $entry->value;
				}
				$invArr[$inventory_id][$inventoryEntryId]["col_".$entry->inventory_type_attribute_id] = isset($colVal) ? $colVal : "";
			}
		}
	}
	
	$result = drupal_json_encode($invArr);

	return $result;
}

function getInventoryColumnDescription($inventoryId){
	$invJson = array();

	$resInventory = db_query_range("SELECT t.name, i.inventory_type_id FROM inventory i LEFT JOIN inventory_type t ON i.inventory_type_id = t.id WHERE i.id = :id", 0, 1, array(':id' => $inventoryId));
	foreach ($resInventory as $inventory) {
		$typeId = $inventory->inventory_type_id;
		$invJson["name"] = $inventory->name;
		$invJson["id"] = $inventory->inventory_type_id;
	}

	$resAttributes = db_query('SELECT a.id, a.name, f.format
		FROM inventory_type_attribute a 
		LEFT JOIN attribute_format f ON a.attribute_format_id = f.id 
		WHERE inventory_type_id = :typeId', array(':typeId' => $typeId));

	$i = 0;
	foreach ($resAttributes as $attribute) {
		$i++;
		$invJson["cols"][$i] = array(
			"name" => $attribute->name,
			"id" => $attribute->id,
			"format" => $attribute->format,
		);

		// If it is a dropdown list
		if($attribute->format == "dropdown") {
			$invJson["cols"][$i]["dropdown_values"] = array();
			$dropdownValues = db_query('SELECT * FROM inventory_type_attribute_dropdown_value WHERE inventory_type_attribute_id = :id', array(':id' => $attribute->id));
			foreach($dropdownValues as $dropdownValue) {
				array_push($invJson["cols"][$i]["dropdown_values"], array("id" =>$dropdownValue->id, "value" => $dropdownValue->value));
			}
		}

	}
	return $invJson;
}

function file_upload($form, &$form_state){

	$head_inventory_id = arg(1);

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
					'file_id' => $file->fid,
					'description' => $form_state['values']['file_description']
		))
		->execute();
		drupal_set_message(t('Datei gespeichert'));
	}
	else {
		drupal_set_message(t('Datei nicht gespeichert!'), 'error');	
	}
	
	return $form_state['redirect'] = 'inventory/'.$head_inventory_id;
}

?>