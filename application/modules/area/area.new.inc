<?php
/**
 * Chose or create new area
 * @return render array
 */
function area_new() {
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-select.css');
	drupal_set_title(t('Choose or Create New Area'));
	drupal_add_js(drupal_get_path('module', 'area') . '/js/lib/infobubble.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/autocomplete-select.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/habitats.js');
	
	// Display Progress Bar
	$progressElement = array(t('Choose or Create New Area'), t('Create New Inventory'), t('Edit Inventory')); // List of elements
	$output['progress'] = array(
		'#theme' => 'progress_done',
		'#elements' => $progressElement,
		'#active' => 1, 		// Indicate active Element
	);
	
	$output['new'] = array(
		'#theme' => 'area.new',
	);

	/* Laut Albert nicht mehr benoetigt
	$output['LastUsed'] = array(
		'#type' => 'fieldset',
		'#title' => t('Last Used Areas'),
	);
	$output['LastUsed']['table'] = area_last_used_table();
	*/
	
	$output['googleMaps'] = drupal_get_form('area_google_maps_form');
	$output['FieldName'] = drupal_get_form('area_field_name_form');
  	$output['areaNew'] = drupal_get_form('area_new_form');
  	$output['areaNew']['#attributes']['class']='hidden';
	$output['areaChose'] = drupal_get_form('area_choose_form');
	
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
function area_google_maps_form() {
	$form['GoogleMaps'] = array(
		'#type' => 'fieldset', 
  		'#title' => t('Map'),
  		'#weight' => 3, 
  		'#collapsible' => FALSE, 
		'searchByMaps' => array('#theme' => 'area.search.byMaps'), // map
	);
	
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
 * Table with the 5 last used areas
 * @return table element to render
 */
function area_last_used_table() {
	global $user;

	$head_inventories= db_query(
		"SELECT * FROM area 
		WHERE owner_id = :owner_id
		ORDER BY modify_time DESC
		LIMIT 5",
		array(':owner_id'=>$user->uid)
	);

	foreach ($head_inventories as $record) {
		$row = array();
		$row[] = '<a href="#" onclick="AreaSelect.prototype.selectArea('.$record->id.')">'.$record->field_name.'</a>';
		$row[] = $record->zip;
		$row[] = $record->locality;
		$row[] = $record->canton;
		$rows[] = $row;
	}

	$header = array(t('Area Name'),t('Zip Code'),t('Locality'),t('Canton'),);

	if ($rows){
		$table['organisms'] = array(
		    '#theme' => 'table', 
			'#header' => $header,
		    '#rows' => $rows,
		);
	} else {
		$table['noArea'] = array (
			'#markup'=> t('You haven\'t used an area yet')
		);
	}
	return $table;
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
	$form['AreaChoseForm']['id-area'] = array(
  		'#type' => 'hidden',
		'#attributes' => array('id' => 'edit-id-area'),
	);
	$form['save_choose'] = array (
		'#type' => 'fieldset',
		'#weight' => 10,
	);

	$form['save_choose']['AreaChoseForm_submit'] = array(
  		'#type' => 'submit',
  		'#value' => t('Continue'),
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
 * Form is visible only if onControlAreaCreateClicked() in JS was called, happens when user starts editing on the map
 * Form gets hidden again if onControlAreaChooseClicked() in JS is called
 * 
 * @param array $form
 * @param *array $form_state
 * @return form to submit area properties
 */
function area_new_form($form, &$form_state) {
	$form['AreaNewForm'] = array(
		'#type' => 'fieldset',
  		'#title' => t('Enter area properties:'), 
  		'#weight' => 7, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => FALSE,
	);
	$form['AreaNewForm']['field_name'] = array(
  	'#type' => 'textfield',
  	'#title' => t('Area name'),
	'#required' => true,
	);
	$form['AreaNewForm']['habitatText'] = array(
	'#title' => 'Habitats',
	'#type' => 'textfield',   
    '#autocomplete_path' => 'area/habitat_autocomplete',
	'#suffix' => '<div id="habitatSelected"><table></table></div>',
	);
	$form['AreaNewForm']['locality'] = array(
	'#type' => 'textfield',
	'#title' => t('Locality'),
	);
	$form['AreaNewForm']['zip'] = array(
	'#type' => 'textfield',
	'#title' => t('Zip'),
	);
	$form['AreaNewForm']['township'] = array(
	'#type' => 'textfield',
	'#title' => t('Township'),
	);
	$form['AreaNewForm']['canton'] = array(
	'#type' => 'textfield',
	'#title' => t('Canton'),
	);
	$form['AreaNewForm']['altitude'] = array(
	'#type' => 'textfield',
	'#title' => t('Altitude'),
	);
	$form['AreaNewForm']['surface-area'] = array(
	'#type' => 'textfield',
	'#title' => t('Surface area (m2)'),
	);
	$form['AreaNewForm']['country'] = array(
	'#type' => 'textfield',
	'#title' => t('Country'),
	);
	$form['AreaNewForm']['comment'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#cols' => 60,
    '#rows' => 10,
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
	$form['save_new'] = array (
		'#type' => 'fieldset',
		'#weight' => 8,
	);

	$form['save_new']['AreaNewForm_submit'] = array(
	  	'#type' => 'submit',
	  	'#value' => t('Continue'),
	);

	return $form;
}

/**
 * Saves the new area after user created markers or polys and clicks on the submit button
 * 
 * Creates a new entry in the DB with the area that was just created and redirects to inventory/new to create
 * new inventory assigned to the area that was just created
 * 
 * @param array $form
 * @param *array $form_state
 */
function area_new_form_submit($form, &$form_state) {
	// get values from form
	$field_name = $form_state['values']['field_name'];
	$locality = $form_state['values']['locality'];
	$zip = $form_state['values']['zip'];
	$canton = $form_state['values']['canton'];
	$country = $form_state['values']['country'];
	$areaType = $form_state['values']['area_type'];
	$comment = $form_state['values']['comment'];
	$altitude = $form_state['values']['altitude'];
	$surface_area = $form_state['values']['surface-area'];
	$township = $form_state['values']['township'];
	$latitude = $form_state['values']['latitude'];
	$longitude = $form_state['values']['longitude'];
	$jsonCoords = $form_state['values']['area-coords'];

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
		'centroid_lat' => $latitude,
		'centroid_lng' => $longitude,
		'owner_id' => $user->uid,
		'create_time' => 'now()',
		'modify_time' => 'now()',
	))
	->execute();

	// insert field coordinates
	$coords = json_decode($jsonCoords, true);
	
	if(sizeof($coords)!=0){
		/*	contains at least two points added by area-select.js
		 *	we will insert all coordinates as points into the DB  */
		$i = 0;
		foreach ($coords as $coord) {
			$lat = $coord[0];
			$lng = $coord[1];
			db_insert('area_point')->fields(array(
				'area_id' => $area_id,
				'lat' => $lat,
				'lng' => $lng,
				'seq' => $i,
			))
			->execute();
			$i++;
		}
	} else {
		/*	contains NO points in coords because area-select.js doesn't add coordinates 
		 *	to coords variable if it is only a marker. To save the marker nonetheless we 
		 *	will	use its centroid lat and lng */
		$abc= 'aewfwef';
		db_insert('area_point')->fields(array(
			'area_id' => $area_id,
			'lat' => $latitude,
			'lng' => $longitude,
			'seq' => 0,
		))
		->execute();
	}

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

	// redirect to create a new inventory assigned to the area just created
	$form_state['redirect'] = 'inventory/new/area/'.$area_id;
}
?>