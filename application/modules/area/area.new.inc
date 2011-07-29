<?php
/**
 * Create new area
 * @return render array
 */
function area_new_form() {
	drupal_set_title(t('Create New Area'));
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-select.css');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/lib/infobubble.js');
	//drupal_add_js(drupal_get_path('module', 'area') . '/js/autocomplete-select.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/habitats.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/parcels.js');
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));

	if(@$_POST['locality'] != '') {
		$output = drupal_get_form('area_new_details_form');
	}
	else {
		$output['new'] = array(
		'#theme' => 'area.new',
		);
		$output['googleMaps'] = area_google_maps_form('create');
		$output['areaNew'] = drupal_get_form('area_create_form');
	  	//$output['areaNew']['#attributes']['class']='hidden';	
	}

	return $output;
}

/**
 * Button with js linker
 * @return a button linking to inventory/new
 */
function area_back_button() {
	$button = array(
		'#type' => 'button',
		'#attributes' => array('onClick' => 'location.replace("../inventory/new"); return false;'),
		'#value' => t('Back'),
  	);
  	return $button;
}

/**
 * Search by Google Maps (City, Street, etc.)
 * @return Google Maps input search field
 */
function area_google_maps_form($task) {
		if($task == 'create') {
			$form['GoogleMaps'] = array(
				'#type' => 'fieldset', 
		  		'#weight' => 3, 
		  		'#collapsible' => FALSE, 
				'searchByMaps' => array('#theme' => 'area.create'), // map
			);			
		}
		if($task == 'choose') {
			$form['GoogleMaps'] = array(
				'#type' => 'fieldset', 
		  		'#weight' => 3, 
		  		'#collapsible' => FALSE, 
				'searchByMaps' => array('#theme' => 'area.choose'), // map
			);
		}
		if($task == '') {
			$form['GoogleMaps'] = array(
				'#type' => 'fieldset', 
		  		'#weight' => 3, 
		  		'#collapsible' => FALSE, 
				'searchByMaps' => array('#theme' => 'area.search.byMaps'), // map
			);
		}
	
	return $form;
}

/**
 * Table with all existing areas and search function
 * @return datatable with all existing areas
 */
function area_field_name_form() {
	$area = db_query(
		"SELECT area.id a_id, field_name, zip, locality, canton, country, u.name u_name, art.desc art_dest FROM area
		LEFT JOIN users u ON u.uid = owner_id
		LEFT JOIN area_type art ON art.id = type_id
		ORDER BY field_name;"
	);
	
	foreach ($area as $record) {
		$row = array();
		$row[] = $record->field_name;
		$row[] = $record->zip;
		$row[] = $record->locality;
		$row[] = $record->canton;
		$row[] = $record->country;
		$row[] = $record->u_name;
		$row[] = $record->art_dest;
		$rows[] = array('data' => $row, 'id' => 'area_'. $record->a_id);
	}
	
	$tableHeader[] = t('Area Name');
	$tableHeader[] = t('Zip');
	$tableHeader[] = t('Locality');
	$tableHeader[] = t('Canton');
	$tableHeader[] = t('Country');
	$tableHeader[] = t('Area Creator');
	$tableHeader[] = t('Area Type');
		
	$form['FieldName'] = array(
		'#type' => 'fieldset',
  		'#title' => t('Search by field name:'),
  		'#weight' => 5,
	);

	$form['FieldName']['show_areas'] = array(
		'#theme' => 'datatable',
		'#header' => $tableHeader,
		'#rows' => $rows,
		'#id_table' => 'show_areas',
		'#len' => 220,
	);

	return $form;
}

/**
 * Textfield autocompleter
 * @return textfield element with autocompleter to search through old areas
 */
function area_search($form, &$form_state) {
	$form['AreaText'] = array(
		'#title' => t('Search through existing areas'),
		'#type' => 'textfield',   
		'#autocomplete_path' => 'area/area_saved_autocomplete',
	);
	return $form;
}

/**
 * Form is visible only if an area was selected and none created
 * Form gets hidden again if onControlAreaCreateClicked() in JS is called
 * 
 * @param array $form
 * @param *array $form_state
 * @return hidden form to submit area_id to next step
 */
function area_choose_form($form, &$form_state) {
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-select.css');
	drupal_set_title(t('Choose existing area'));
	drupal_add_js(drupal_get_path('module', 'area') . '/js/lib/infobubble.js');
	//drupal_add_js(drupal_get_path('module', 'area') . '/js/autocomplete-select.js');
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));

	$form['new'] = array(
	'#theme' => 'area.new',
	);
	$form['AreaChooseForm'] = array (
		'#type' => 'fieldset',
		'#weight' => -1,
	);
	$form['AreaChooseForm']['help'] = array (
		'#markup' => t('If you would like to create a new inventory, please select an existing area. If your desired area is not listed here, please create one first.')
	);
	
	$form['FieldName'] = area_field_name_form();
	$form['googleMaps'] = area_google_maps_form('choose');
	

	$form['id-area'] = array(
  		'#type' => 'hidden',
		'#attributes' => array('id' => 'edit-id-area'),
	);
	$form['actions'] = array('#type' => 'actions');
	$form['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Continue')
	);
	
  	return $form;
}

/**
 * Validation for Area Choose Form
 * @param array $form
 * @param *array $form_state
 */
function area_choose_form_validate($form, &$form_state) {
	if($form_state['values']['id-area'] == '') {
		form_set_error('id', t('You must select an area to continue.'));
	}
}

/**
 * Submit function for Area Choose Form
 * Redirect to step 2 of workflow
 * 
 * @param array $form
 * @param *array $form_state
 * @param string $area_id
 */
function area_choose_form_submit($form, &$form_state, $area_id) {
	// redirect to create a new inventory
	$form_state['redirect'] = 'inventory/new/area/'.$form_state['values']['id-area'];
}

/**
 * Form is hidden and used to store values handed over by JS
 * 
 * @param array $form
 * @param *array $form_state
 * @return form to submit area properties
 */
function area_create_form($form, &$form_state) {
	$form['AreaNewForm'] = array (
		'#type' => 'fieldset',
		'#weight' => 8,
	);
	$form['AreaNewForm']['locality'] = array(
	'#type' => 'hidden',
	'#attributes' => array('id' => 'edit-locality'),
	);
	$form['AreaNewForm']['zip'] = array(
	'#type' => 'hidden',
	'#attributes' => array('id' => 'edit-zip'),
	);
	$form['AreaNewForm']['township'] = array(
	'#type' => 'hidden',
	'#attributes' => array('id' => 'edit-township'),
	);
	$form['AreaNewForm']['canton'] = array(
	'#type' => 'hidden',
	'#attributes' => array('id' => 'edit-canton'),
	);
	$form['AreaNewForm']['altitude'] = array(
	'#type' => 'hidden',
	'#attributes' => array('id' => 'edit-altitude'),
	);
	$form['AreaNewForm']['surface-area'] = array(
	'#type' => 'hidden',
	'#attributes' => array('id' => 'edit-surface-area'),
	);
	$form['AreaNewForm']['country'] = array(
	'#type' => 'hidden',
	'#attributes' => array('id' => 'edit-country'),
	);
	$form['AreaNewForm']['latitude'] = array(
    '#type' => 'hidden',
	'#attributes' => array('id' => 'edit-latitude'),
	);
	$form['AreaNewForm']['longitude'] = array(
    '#type' => 'hidden',
	'#attributes' => array('id' => 'edit-longitude'),
	);
	$form['AreaNewForm']['area-coords'] = array(
    '#type' => 'hidden',
	'#attributes' => array('id' => 'edit-area-coords'),
	);
	$form['AreaNewForm']['area_type'] = array(
    '#type' => 'hidden',
	'#attributes' => array('id' => 'edit-area-type'),
	);
	$form['AreaNewForm']['actions-new'] = array('#type' => 'actions');
	$form['AreaNewForm']['actions-new']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Continue'),
	);
	return $form;
}

/**
 * 
 * Rebuilds the area map form to display area_new_details_form
 * @param unknown_type $form
 * @param unknown_type $form_state
 */
function area_create_form_submit($form, $form_state) {
	$form_state['rebuild'] = TRUE;
}

/**
 * Saves the new area after user entered details for the new area and clicks on the submit button
 * 
 * Creates a new entry in the DB with the area that was just created and redirects to inventory/new to create
 * new inventory assigned to the area that was just created
 * 
 * @param array $form
 * @param *array $form_state
 */
function area_new_details_form($form, $form_state) {
	drupal_set_title(t('Enter new area details'));

	$output['AreaNewForm']['basics'] = area_edit_basics_form($form, $form_state, '');
	$output['AreaNewForm']['map'] = area_edit_map_form($form, $form_state);
	$output['AreaNewForm']['comment'] = area_edit_comment_form($form, $form_state, '');
	$output['AreaNewForm']['habitats'] = area_edit_habitat_form($form, $form_state, '');
	$output['AreaNewForm']['concept'] = area_edit_concept_form($form, $form_state, '');
	$output['AreaNewForm']['hidden'] = area_edit_hidden_form($form, $form_state);
	$output['actions'] = array('#type' => 'actions');
	$output['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save')
	);
	return $output;
}
function area_new_details_form_submit($form, $form_state) {
	// get values from form
	$field_name = $form_state['input']['field_name'];
	$locality = $form_state['values']['locality'];
	$zip = $form_state['values']['zip'];
	$canton = $form_state['values']['canton'];
	$country = $form_state['values']['country'];
	$areaType = $form_state['values']['area_type'];
	$comment = $form_state['input']['comment'];
	$altitude = $form_state['values']['altitude'];
	$surface_area = $form_state['values']['surface-area'];
	$township = $form_state['values']['township'];
	$latitude = $form_state['values']['latitude'];
	$longitude = $form_state['values']['longitude'];
	$jsonCoords = $form_state['values']['area-coords'];
	$protectiontarget = $form_state['input']['protection_target'];
	$safetyprecautions = $form_state['input']['safety_precautions'];
	$tendingstrategies = $form_state['input']['tending_strategies'];
	
	// get area type id from table area_type
	$result = db_query('SELECT id FROM area_type at WHERE at.desc like :desc', array(':desc' => $areaType));
	$areaTypeRow = $result->fetchAssoc();
	$areaTypeId = $areaTypeRow['id'];

	global $user;

	// insert area
	$area_id = db_insert('area')->fields(array(
		'field_name' => $field_name,
		'locality' => $locality,
		'zip' => $zip,
		'canton' => $canton,
		'country' => $country,
		'type_id' => $areaTypeId,
		'comment' => $comment,
		'altitude' => $altitude,
		'surface_area' => $surface_area,
		'township' => $township,
		'owner_id' => $user->uid,
		'create_time' => 'now()',
		'modify_time' => 'now()',
		'protection_target' => $protectiontarget,
		'safety_precautions' => $safetyprecautions,
		'tending_strategies' => $tendingstrategies
	))
	->execute();
	
	// field coordinates
	$coords = json_decode($jsonCoords, true);
	$points = array();
	foreach ($coords as $coord) {
    	$lat = $coord[0];
    	$lng = $coord[1];
    	$points[] = $lng . ' ' . $lat;
	}
	
	if (count($points) > 1) {
	    // polygon
	
	    // make sure line is closed
	    if (end($points) != reset($points)) {
	        $points[] = reset($points);
	    }
	
	    $linestring = implode(', ', $points);
	    $geom = 'ST_GeomFromText(\'POLYGON((' . $linestring . '))\', 4326)'; // SRID 4326 = WGS 84
	
	} else {
	    // point
	
	    $geom = 'ST_GeomFromText(\'POINT(' . reset($points) . ')\', 4326)'; // SRID 4326 = WGS 84
    }
	db_update('area')->expression('geom', $geom)->condition('id', $area_id)->execute();

	// insert habitats
	if (array_key_exists('habitat', $form_state['input'])) {
		foreach (array_values($form_state['input']['habitat']) as $habitat) {
			db_insert('area_habitat')->fields(array(
				'area_id' => $area_id,
				'habitat_id' => $habitat,
			))
			->execute();
		}
	}
	
	//insert parcels
	$parcels = $form_state['input']['parcel'];
	area_insert_parcels($area_id, $parcels);

	// redirect to the new area	
	drupal_set_message('Area successfully created. If you would like to add a new inventory in this Area please click '.l('here', 'inventory/new/area/'.$area_id));
	drupal_goto('area/'.$area_id);
}

function area_new_details_form_validate($form, &$form_state) {
	if($form_state['input']['field_name'] == '') {
		form_set_error('id', t('An area name is required'));
	}
}

?>