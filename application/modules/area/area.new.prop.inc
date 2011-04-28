<?php
function show_area_new_prop() {
	drupal_set_title(t('Create a new area'));
	drupal_add_js('modules/area/js/habitats.js');
	return drupal_get_form('areaPropertyForm');
}

function areaPropertyForm($form, &$form_state) {
	// get address information from post request
	$locality = getValueFromPost('locality');
	$canton = getValueFromPost('canton');
	$country = getValueFromPost('country');
	$township = getValueFromPost('township');
	$zip = getValueFromPost('zip');
	$altitude = getValueFromPost('altitude');
	$surface_area = getValueFromPost('surface_area');
	$canton = getValueFromPost('canton');
	$latitude = getValueFromPost('latitude');
	$longitude = getValueFromPost('longitude');
	$area_coords = getValueFromPost('area_coords');
	$area_type = getValueFromPost('area_type');


	$form['field_name'] = array(
  	'#type' => 'textfield',
  	'#title' => t('Area name'),
	'#required' => true,
	);
	$form['habitatText'] = array(
	'#title' => 'Habitats',
	'#type' => 'textfield',   
    '#autocomplete_path' => 'area/habitat_autocomplete',
	'#suffix' => '<div id="habitatSelected"><table></table></div>',
	);
	$form['locality'] = array(
	'#type' => 'textfield',
	'#title' => t('Locality'),
	'#value' => $locality,
	);
	$form['zip'] = array(
	'#type' => 'textfield',
	'#title' => t('Zip'),
	'#value' => $zip,
	);
	$form['township'] = array(
	'#type' => 'textfield',
	'#title' => t('Township'),
	'#value' => $township,
	);
	$form['canton'] = array(
	'#type' => 'textfield',
	'#title' => t('Canton'),
	'#value' => $canton,
	);
	$form['altitude'] = array(
	'#type' => 'textfield',
	'#title' => t('Altitude'),
	'#value' => $altitude,
	);
	$form['surface_area'] = array(
	'#type' => 'textfield',
	'#title' => t('Surface area (m2)'),
	'#value' => $surface_area,
	);
	$form['country'] = array(
	'#type' => 'textfield',
	'#title' => t('Country'),
	'#value' => $country,
	);
	$form['comment'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
	'#value' => '',
    '#cols' => 60,
    '#rows' => 10,
	);
	$form['latitude'] = array(
    '#type' => 'hidden',
	'#value' => $latitude,
	);
	$form['longitude'] = array(
    '#type' => 'hidden',
	'#value' => $longitude,
	);
	$form['area_coords'] = array(
    '#type' => 'hidden',
	'#value' => $area_coords,
	);
	$form['area_type'] = array(
    '#type' => 'hidden',
	'#value' => $area_type,
	);
	$form['submit'] = array(
  	'#type' => 'submit',
  	'#value' => t('Create area'),
	'#submit' => array('areaPropertyForm_save'),
	);

	return $form;
}

function habitat_autocomplete($string) {
	if (preg_match('/^[0-9]/', $string) > 0) {
		$habitats = db_query('SELECT id, name_de, label FROM {habitat} WHERE label LIKE :s ORDER BY label', array (':s' => $string . '%'));
	} else {
		$habitats = db_query('SELECT id, name_de, label FROM {habitat} WHERE name_de ILIKE :s ORDER BY label', array (':s' => $string . '%'));
	}

	$items = array();
	foreach ($habitats as $habitat) {
		$value = $habitat->label . ': ' . $habitat->name_de;
		$key = $habitat->id . '|' . $habitat->label . '|' . $habitat->name_de;
		$items[$key] = check_plain($value);
	}
	return drupal_json_output($items);
}

function areaPropertyForm_save($form, &$form_state) {
	$tx = db_transaction();

	// get values from form
	$field_name = $form['field_name']['#value'];
	$locality = $form['locality']['#value'];
	$zip = $form['zip']['#value'];
	$canton = $form['canton']['#value'];
	$country = $form['country']['#value'];
	$areaType = $form['area_type']['#value'];
	$comment = $form['comment']['#value'];
	$altitude = $form['altitude']['#value'];
	$surface_area = $form['surface_area']['#value'];
	$township = $form['township']['#value'];
	$latitude = $form['latitude']['#value'];
	$longitude = $form['longitude']['#value'];
	$jsonCoords = $form['area_coords']['#value'];


	// get area type id from table area_type
	$result = db_query('SELECT id FROM area_type at WHERE at.desc like :desc', array(':desc' => $areaType));
	$areaTypeRow = $result->fetchAssoc();
	$areaTypeId = $areaTypeRow['id'];

	global $user;
	
	// insert area
	$aid = db_insert('area')->fields(array(
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
	$i = 0;
	foreach ($coords as $coord) {
		$lat = $coord[0];
		$lng = $coord[1];
		db_insert('area_point')->fields(array(
			'area_id' => $aid,
			'lat' => $lat,
			'lng' => $lng,
			'seq' => $i,
		))
		->execute();
		$i++;
	}

	// insert habitats
	if (array_key_exists('habitat', $form_state['input'])) {
		foreach (array_values($form_state['input']['habitat']) as $habitat) {
			db_insert('area_habitat')->fields(array(
				'area_id' => $aid,
				'habitat_id' => $habitat,
			))
			->execute();
		}
	}

	// send a message
	drupal_set_message(t('Area @field_name has been saved. Continue with creating a new inventory.', array('@field_name' => $field_name)));

	// redirect to create a new inventory
	$form_state['redirect'] = 'inventory/new/' . $aid;
}

function getValueFromPost($variableName) {
	$value = '';
	if (array_key_exists($variableName, $_POST)) {
		$value = $_POST[$variableName];
	}
	return $value;
}
?>