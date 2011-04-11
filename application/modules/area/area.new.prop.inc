<?php
function show_area_new_prop() {
	drupal_set_title(t('Create a new area'));
	return drupal_get_form('areaPropertyForm');
}

function areaPropertyForm($form_state) {
	// get address information from post request
	$locality = getValueFromPost('locality');
	$canton = getValueFromPost('canton');
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
  	'#title' => t('Field name'),
	'#required' => true,
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
  	'#value' => t('Save'),
	);
	return $form;
}

function areaPropertyForm_submit($form, $form_state) {
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

	drupal_set_message(t('Area @field_name has been saved', array('@field_name' => $field_name)));
}

function getValueFromPost($variableName) {
	$value = '';
	if (array_key_exists($variableName, $_POST)) {
		$value = $_POST[$variableName];
	}
	return $value;
}
?>