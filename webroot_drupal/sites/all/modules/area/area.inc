<?php

/**
 * @file area.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

/**
 * Create new area.
 * @return render array
 */
function area_new_area() {
	$output = array();
	drupal_set_title(t('Create new area'));

	$output['map'] = array(
			'#search' => true,
			'#create' => true,
			'#theme' => 'area'
	);

	// 	$output['data'] = drupal_get_form('area_address_form');

	return $output;
}

function area_new_areaname_form($form, &$form_state) {
	$form = area_address_form();

	$form['fieldname'] = array(
			'#type' => 'textfield',
			'#title' => t('Field name/area name'),
			'#description' => t('Please insert a custom name for this area.'),
			'#size' => 20,
			'#maxlength' => 80,
			'#required' => true,
	);

	$form['button'] = array(
			'#type' => 'submit',
			'#value' => t('Create'),
			'#submit' => array(
					'area_create_new_area'
			)
	);
	return $form;
}

function area_get_new_areaname_form_callback() {
	print drupal_render(drupal_get_form('area_new_areaname_form'));
}

function area_get_new_areaname_form() {
	return drupal_get_form('area_new_areaname_form');
}

function area_create_new_area($form, &$form_state) {
	if (false) {
		drupal_set_message(
			"Creating area: <pre>" . var_export($form_state['values'], true)
					. "</pre>");
	}

	// get values from form
	$name = $form_state['values']['fieldname'];
	$zip = $form_state['values']['zip'];
	$canton = $form_state['values']['canton'];
	$country = $form_state['values']['country'];
	$township = $form_state['values']['township'];

	$areaType = $form_state['values']['area_type'];
	$altitude = $form_state['values']['altitude'];
	$surface = empty($form_state['values']['surface']) ? 0
			: floor($form_state['values']['surface'] + 0.5);
	$jsonCoords = $form_state['values']['area-coords'];
	$latitude = $form_state['values']['latitude'];
	$longitude = $form_state['values']['longitude'];

	$coordinates = json_decode($jsonCoords, true);
	$points = array();
	foreach ($coordinates as $coord) {
		$lat = $coord[0];
		$lng = $coord[1];
		$points[] = $lng . ' ' . $lat;
	}

	switch ($areaType) {
	case 'polyline':
		$linestring = implode(', ', $points);
		$geom = 'ST_GeomFromText(\'LINESTRING(' . $linestring . ')\', 4326)'; // SRID 4326 = WGS 84
		break;
	case 'polygon':
	// make sure line is closed
		if (end($points) != reset($points)) {
			$points[] = reset($points);
		}
		$linestring = implode(', ', $points);
		$geom = 'ST_GeomFromText(\'POLYGON((' . $linestring . '))\', 4326)'; // SRID 4326 = WGS 84
		break;
	case 'marker':
		$geom = "ST_GeomFromText('POINT(" . reset($points) . ")', 4326)"; // SRID 4326 = WGS 84
		break;
	}

	if (!empty($name) && !empty($altitude) && !empty($zip) && !empty($township)
			&& !empty($canton) && !empty($country)) {

		// insert area
		$area_id = db_insert('area')->fields(
				array('name' => $name,
						'zip' => $zip,
						'canton' => $canton,
						'country' => $country,
						'altitude' => $altitude,
						'surface_area' => $surface,
						'township' => $township,
						'acl_id' => 1,
						'geom' => $geom
				))
			->execute();

		drupal_set_message(
			t(
				"Area @areaname got created.",
				array('@areaname' => $form_state['values']['fieldname'])));
		drupal_goto('area/' . $area_id);
	} else {
		drupal_set_message(
			t(
				"Area @areaname could not be created, missing values.",
				array('@areaname' => $form_state['values']['fieldname'])),
			'error');
		drupal_goto('area/new');
	}
}

/**
 * Return a hidden form which can be used to store values handed over by Javascript
 * @param array $form
 * @param array $form_state
 * @return form to submit area properties
 */
function area_address_form() {
	$form['addressdata'] = array(
			'#type' => 'fieldset',
			'#prefix' => '<div id="addressdata" style="visibility: hidden; height: 0px">',
			'#suffix' => '</div>'
	);
	$form['addressdata']['fieldname'] = array(
			'#type' => 'textfield',
			'#size' => 50,
			'#attributes' => array(
					'id' => 'edit-name'
			),
	);
	$form['addressdata']['zip'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-zip'
			),
	);
	$form['addressdata']['township'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-township'
			),
	);
	$form['addressdata']['canton'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-canton'
			),
	);
	$form['addressdata']['altitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-altitude'
			),
	);
	$form['addressdata']['surface'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-surface'
			),
	);
	$form['addressdata']['country'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-country'
			),
	);
	$form['addressdata']['latitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-latitude'
			),
	);
	$form['addressdata']['longitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-longitude'
			),
	);
	$form['addressdata']['area-coords'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-area-coords'
			),
	);
	$form['addressdata']['area_type'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-area-type'
			),
	);
	return $form;
}

/**
 * Show a specific area
 * @param integer $area_id
 * @return renderarray
 */
function area_show_area($area_id) {
	return array();
}
/**
 * A simple testview to demonstrate the usage of the static google maps API
 */

function area_naturwerkstatic() {
	$form = array();
	// coordinates for  a point in Brugg, near naturwerk
	$params['latitude'] = 47.48135013015885;
	$params['longitude'] = 8.210649490356445;

	$url = area_get_static_image_url($params, 800, 800);
	$form['naturwerk'] = array(
			'#type' => 'markup',
			'#markup' => "<img src='$url' alt='Naturwerk in Brugg'/>"
	);
	return $form;
}
?>
