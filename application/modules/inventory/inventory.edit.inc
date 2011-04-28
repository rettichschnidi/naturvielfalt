<?php

/**
 * MENU_NORMAL_ITEM. This will be called if someone visits for example drupal/inventory/3/edit.
 * @param $head_inventory_id
 * 	The id of inventory to be modified
 * @return all inventory forms to modify the inventory
 */
function inventory_edit($head_inventory_id) {
	
	drupal_set_title("Modify ".getInventoryTitle($head_inventory_id)." (".$head_inventory_id.")");
	
	//------------------
	$render_array['basics'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Basics'), 
  		'#weight' => 2, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => TRUE,
		'#attached' => array (
	        'js' => array (
	          'misc/form.js',
	          'misc/collapse.js',
        	),
        ),
        '#attributes' => array(
        	'class' => array('collapsible', 'collapsed'),
        ),
	);
	
	$render_array['basics']['edit'] = drupal_get_form('inventory_edit_basics', $head_inventory_id, false);

	//------------------
	$render_array['elements'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Elements'), 
  		'#weight' => 6, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => FALSE,
		'#attached' => array (
	        'js' => array (
	          'misc/form.js',
	          'misc/collapse.js',
        	),
        ),
        '#attributes' => array(
        	'class' => array('collapsible'),
        ),
	);
	
	$render_array['elements']['edit'] = drupal_get_form('inventory_edit_elemnts', $head_inventory_id);
	
	//------------------
	$render_array['rights'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Rights'), 
  		'#weight' => 8, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => TRUE,
		'#attached' => array (
	        'js' => array (
	          'misc/form.js',
	          'misc/collapse.js',
        	),
        ),
        '#attributes' => array(
        	'class' => array('collapsible', 'collapsed'),
        ),
	);
	
	$render_array['rights']['edit'] = drupal_get_form('inventory_edit_rights', $head_inventory_id);

	//------------------
	$render_array['file'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Files'), 
  		'#weight' => 10, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => TRUE,
		'#attached' => array (
	        'js' => array (
	          'misc/form.js',
	          'misc/collapse.js',
        	),
        ),
        '#attributes' => array(
        	'class' => array('collapsible', 'collapsed'),
        ),
	);

	$render_array['file']['upload'] = drupal_get_form('inventory_edit_files', $head_inventory_id);
	
	return $render_array;
}

/**
 * @return form of invenory elements
 */
function inventory_edit_elemnts($form, &$form_state, $head_inventory_id) {
	$json = getInitialInventoryJSON($head_inventory_id);

	drupal_add_js('var exInvs = ' . $json, 'inline');
	drupal_add_js('jQuery(document).ready(function(){initInventory();});', 'inline');
	
	$edit_mode = true;
	
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-ui-1.8.6.custom.min.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-texotela-numeric.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/ext/ext-jquery-adapter-debug.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/ext/ext-base-debug.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/JSON.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/util.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/inventory.js');

	drupal_add_css(drupal_get_path('module', 'inventory') . '/css/main.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));

	// inventories ajax form
	$form['inventories']['#markup'] = '<div id="inventories"></div>';
		
	$form['head_inventory_id'] = array('#type' => 'hidden', '#value' => $head_inventory_id);
	$form['editMode'] = array('#type' => 'hidden', '#value' => $edit_mode);

	$options = array();

	$result = db_query("
		SELECT it.id, it.name
		FROM inventory_type it
		ORDER BY it.id desc;"
	);

	foreach ($result as $record) {
		$options[$record->id] = $record->name;
	}

	$form['inventory_types'] = array(
    	'#type' => 'select',
    	'#title' => t('Inventory Group'),
    	'#default_value' => variable_get('name', 259200),
    	'#options' => $options,
	);
	
	$form['add_inventory']['#markup'] = '<div id="add_inventory"></div><input onclick="onAddInventory();" type="button" id="add-inventory" name="button" value="Inventargruppe hinzufügen">';

	$form['last_saved']['#markup'] = '<div id="last_saved"><br/></div>';
	$form['error_save']['#markup'] = '<div id="error_save"><br/></div>';
	
	return $form;
}

/**
 * @return form to upload files
 */
function inventory_edit_files($form, &$form_state, $head_inventory_id) {
	
	$tableHeader[] = t('Description');
	$tableHeader[] = t('Filename');
	$tableHeader[] = t('Typ');
	$tableHeader[] = t('Size');
	$tableHeader[] = t('Date');

	$result = db_query('SELECT fm.filename, fm.filemime, fm.timestamp, fm.fid, fm.filesize, hifm.description
						FROM head_inventory_file_managed hifm, file_managed fm
						WHERE fm.fid = hifm.file_id
						AND hifm.head_inventory_id = :id;', array(':id' => $head_inventory_id)
	);

	if(!empty($result))
	{
		foreach ($result as $record) {
			$description = $record->description;
			$filename = l($record->filename, 'sites/default/files/swissmon/inventory/'.$head_inventory_id.'/'.$record->filename);
			$filemime = '<img class="file-icon" alt="" title="' . $record->filemime . '" src="../../modules/inventory/images/' . getInventoryIconMap($record->filemime) . '.png" />';
			$filesize = ($record->filesize / 1000)." KB";
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
			$form['file_overview'] = array(
    			'#theme' => 'table', 
   				'#header' => $tableHeader,
				'#rows' => $rows,
			);
		}
	}

	$form['upload'] = array(
			'#type' => 'file', 
			'#title' => t('Datei uploaden'), 
			'#size' => 48,
	);

	$form['file_description'] = array(
  			'#type' => 'textfield', 
  			'#title' => t('Beschreibung der Datei'), 
  			'#size' => 100, 
  			'#maxlength' => 128, 
  			'#required' => FALSE,
	);

	$form['upload_submit'] = array(
			'#type' => 'submit', 
			'#value' => t("Upload"),
			'#submit' => array('inventory_update_file'),
	);
	
	// Saveing something to the form_state makes it accessible in the submit function.
	// We do this to make sure the sgid is taken from the url which has passed security checkpoints.
	$form_state['storage']['hi_id'] = $head_inventory_id;
	
	return $form;
}

/**
 * @return form to edit inventory rights
 */
function inventory_edit_rights($form, &$form_state, $head_inventory_id) {
	
	$tableHeader[] = t('Group');
	$tableHeader[] = t('Write Access');
	$tableHeader[] = t('Read Access');
	
	global $user;
	
	$result = db_query(
		'SELECT sg.sgid, sg.sgroup_name, read, write 
  		FROM sgroup sg
		LEFT JOIN sgroup_users sgu ON sgu.sgid = sg.sgid
		LEFT JOIN users u ON sgu.uid = u.uid
		LEFT JOIN sgroups_inventory sgi ON sgi.sgid = sg.sgid AND sgi.hiid = :hiid
		LEFT JOIN head_inventory hi ON sgi.hiid = hi.id
		WHERE u.uid = :uid', array(':uid' => $user->uid, ':hiid' => $head_inventory_id)
	);
	$i = 0;
	if(!empty($result)) {
		foreach ($result as $record) {
			// Saveing something to the form_state makes it accessible in the submit function.
			// We do this to make sure the sgid is taken from the url which has passed security checkpoints.
			$form_state['storage']['sg_id'.$i] = $record->sgid;
			
			$name = $record->sgroup_name;
			$read = false;
			$write = false;
			$form_state['storage']['set_db'.$i] = false;
			if($record->read == '1') $read = true;
			elseif ($record->read == '0') $read = false;
			else $form_state['storage']['set_db'.$i] = true;	// no db entry yet -> set in submit fct
			if($record->write == '1') $write = true;
			$form['read'.$i] = array(
				'#type' => 'checkbox',
				'#value' => $read,
				'#checked' => $read,
				'#name' => 'read'.$i,
			);
			$form['write'.$i] = array(
				'#type' => 'checkbox',
				'#value' => $write,
				'#checked' => $write,
				'#name' => 'write'.$i,
			);
			$row = array();
			$row[] = $name;
			$row[] = array('data' => drupal_render($form['write'.$i]));
			$row[] = array('data' => drupal_render($form['read'.$i]));
			$rows[] = $row;
			$i++;
		}

		if(isset($rows)) {
			$form['right_overview'] = array(
    			'#theme' => 'table', 
   				'#header' => $tableHeader,
				'#rows' => $rows,
			);
			
			$form['upload_submit'] = array(
				'#type' => 'submit', 
				'#value' => t("Save"),
				'#submit' => array('inventory_update_rights'),
			);
		}
	}
	
	// Saveing something to the form_state makes it accessible in the submit function.
	// We do this to make sure the sgid is taken from the url which has passed security checkpoints.
	$form_state['storage']['hi_id'] = $head_inventory_id;
	$form_state['storage']['sg_cnt'] = $i;
	
	return $form;
}

/**
 * 
 * Update Basic information of Inventory. Corresponding form in inventory.inc
 * @param array $form
 * @param array $form_state
 * @return Form with modified entries
 */
function inventory_update_basics($form, &$form_state) {

	$head_inventory_id = $form_state['storage']['hi_id'];
	$name = $form_state['input']['name'];
	$shared = $form_state['input']['share'];
	$description = $form_state['input']['description'];

	$affectedRows = db_update('head_inventory')
	->condition('id', $head_inventory_id)
	->fields(array(
				'name' => $name,
				'shared' => $shared,
				'description' => $description['value']
	))
	->execute();
	
	// check success
	if ($affectedRows == 1) {
		// show success message
		drupal_set_message(t('Inventory @name was successfully updated', array('@name'=> $name)));
	} else {
		drupal_set_message(t('Inventory could not be updated!'), 'error');
	}

	return $form_state['redirect'] = 'inventory/'.$head_inventory_id.'/edit';
}

function inventory_update_file($form, &$form_state){

	$head_inventory_id = $form_state['storage']['hi_id'];

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
		drupal_set_message(t('File saved'));
	}
	else {
		drupal_set_message(t('File could not be saved!'), 'error');
	}

	return $form_state['redirect'] = 'inventory/'.$head_inventory_id.'/edit';
}

/**
 * @return form to edit inventory rights
 */
function inventory_update_rights($form, &$form_state) {
	
	$head_inventory_id = $form_state['storage']['hi_id'];
	
	for($i=0; $i<$form_state['storage']['sg_cnt']; $i++) {
		$sgid = $form_state['storage']['sg_id'.$i];
		$read = '0';
		$write = '0';
		if(isset($form_state['input']['read'.$i]) && $form_state['input']['read'.$i]) $read = '1';
		if(isset($form_state['input']['write'.$i]) && $form_state['input']['write'.$i]) $write = '1';
		if($write == '1') $read = '1'; // write access implies also read access
		if($form_state['storage']['set_db'.$i]) {
			// entry missing in db
			$nid = db_insert('sgroups_inventory')->fields (
				array (
					'sgid' => $sgid,
					'hiid' => $head_inventory_id,
					'read' => $read,
					'write' => $write,
				)
			)
			->execute();
		}
		else {
			// entry already in db
			$affectedRows = db_update('sgroups_inventory')
			->fields (
				array (
					'read' => $read,
					'write' => $write,
				)
			)
			->condition(db_and()->condition('sgid', $sgid, '=')->condition('hiid', $head_inventory_id, '='))
			->execute();
		}

	}
	
	return $form_state['redirect'] = 'inventory/'.$head_inventory_id.'/edit';
}

function getInitialInventory($head_inventory_id) {	
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
		// ugly hack to use Uppercase columnnames
		$inventoryEntries = db_query('
		SELECT e.id, e.organism_id, fauna.name_de faunanamede, fauna.genus, fauna.species, o.organism_type, flora.name_de floranamede, flora."Gattung" floragattung, flora."Art" floraart
			FROM inventory_entry e
			LEFT JOIN organism o on e.organism_id = o.id
			LEFT JOIN fauna_organism fauna ON o.organism_id = fauna.id
			LEFT JOIN flora_organism flora ON o.organism_id = flora.id
		WHERE e.inventory_id = :id;',array(':id' => $inventory_id));

		foreach($inventoryEntries as $inventoryEntry) {
			$inventoryEntryId = $inventoryEntry->id;
			$invArr[$inventory_id][$inventoryEntryId] = array();
			$invArr[$inventory_id][$inventoryEntryId]["orgId"] = $inventoryEntry->organism_id;

			if($inventoryEntry->organism_type==1){
				// organism_type 1 is Fauna
				$invArr[$inventory_id][$inventoryEntryId]["label"] = $inventoryEntry->faunanamede." [".$inventoryEntry->genus." ".$inventoryEntry->species."]";
			} else {
				// organism_type 2 is Flora
				$invArr[$inventory_id][$inventoryEntryId]["label"] = $inventoryEntry->floranamede." [".$inventoryEntry->floragattung." ".$inventoryEntry->floraart."]";
			}

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

	return $invArr;
}

function getInitialInventoryJSON($head_inventory_id) {
	return drupal_json_encode(getInitialInventory($head_inventory_id));
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

function getInventoryIconMap($filemime) {

	switch ($filemime) {
		// Word document types.
		case 'application/msword':
		case 'application/vnd.ms-word.document.macroEnabled.12':
		case 'application/vnd.oasis.opendocument.text':
		case 'application/vnd.oasis.opendocument.text-template':
		case 'application/vnd.oasis.opendocument.text-master':
		case 'application/vnd.oasis.opendocument.text-web':
		case 'application/vnd.openxmlformats-officedocument.wordprocessingml.document':
		case 'application/vnd.stardivision.writer':
		case 'application/vnd.sun.xml.writer':
		case 'application/vnd.sun.xml.writer.template':
		case 'application/vnd.sun.xml.writer.global':
		case 'application/vnd.wordperfect':
		case 'application/x-abiword':
		case 'application/x-applix-word':
		case 'application/x-kword':
		case 'application/x-kword-crypt':
			return 'x-office-document';

			// Spreadsheet document types.
		case 'application/vnd.ms-excel':
		case 'application/vnd.ms-excel.sheet.macroEnabled.12':
		case 'application/vnd.oasis.opendocument.spreadsheet':
		case 'application/vnd.oasis.opendocument.spreadsheet-template':
		case 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet':
		case 'application/vnd.stardivision.calc':
		case 'application/vnd.sun.xml.calc':
		case 'application/vnd.sun.xml.calc.template':
		case 'application/vnd.lotus-1-2-3':
		case 'application/x-applix-spreadsheet':
		case 'application/x-gnumeric':
		case 'application/x-kspread':
		case 'application/x-kspread-crypt':
			return 'x-office-spreadsheet';

			// Presentation document types.
		case 'application/vnd.ms-powerpoint':
		case 'application/vnd.ms-powerpoint.presentation.macroEnabled.12':
		case 'application/vnd.oasis.opendocument.presentation':
		case 'application/vnd.oasis.opendocument.presentation-template':
		case 'application/vnd.openxmlformats-officedocument.presentationml.presentation':
		case 'application/vnd.stardivision.impress':
		case 'application/vnd.sun.xml.impress':
		case 'application/vnd.sun.xml.impress.template':
		case 'application/x-kpresenter':
			return 'x-office-presentation';

			// Compressed archive types.
		case 'application/zip':
		case 'application/x-zip':
		case 'application/stuffit':
		case 'application/x-stuffit':
		case 'application/x-7z-compressed':
		case 'application/x-ace':
		case 'application/x-arj':
		case 'application/x-bzip':
		case 'application/x-bzip-compressed-tar':
		case 'application/x-compress':
		case 'application/x-compressed-tar':
		case 'application/x-cpio-compressed':
		case 'application/x-deb':
		case 'application/x-gzip':
		case 'application/x-java-archive':
		case 'application/x-lha':
		case 'application/x-lhz':
		case 'application/x-lzop':
		case 'application/x-rar':
		case 'application/x-rpm':
		case 'application/x-tzo':
		case 'application/x-tar':
		case 'application/x-tarz':
		case 'application/x-tgz':
			return 'package-x-generic';

			// Script file types.
		case 'application/ecmascript':
		case 'application/javascript':
		case 'application/mathematica':
		case 'application/vnd.mozilla.xul+xml':
		case 'application/x-asp':
		case 'application/x-awk':
		case 'application/x-cgi':
		case 'application/x-csh':
		case 'application/x-m4':
		case 'application/x-perl':
		case 'application/x-php':
		case 'application/x-ruby':
		case 'application/x-shellscript':
		case 'text/vnd.wap.wmlscript':
		case 'text/x-emacs-lisp':
		case 'text/x-haskell':
		case 'text/x-literate-haskell':
		case 'text/x-lua':
		case 'text/x-makefile':
		case 'text/x-matlab':
		case 'text/x-python':
		case 'text/x-sql':
		case 'text/x-tcl':
			return 'text-x-script';

			// HTML aliases.
		case 'application/xhtml+xml':
			return 'text-html';

			// Executable types.
		case 'application/x-macbinary':
		case 'application/x-ms-dos-executable':
		case 'application/x-pef-executable':
			return 'application-x-executable';

			// PDF
		case 'application/pdf':
			return 'application-pdf';

		default:
			return $return;
	}
}

/**
 * @return name of actual inventory to be modified
 */
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

?>