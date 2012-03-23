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

	return $output;
}

function area_new_areaname_form($form, &$form_state) {
	$form = area_address_hidden_form();

	$form['fieldname'] = array(
			'#type' => 'textfield',
			'#title' => t('Field name/area name'),
			'#description' => t('Please insert a custom name for this area.'),
			'#size' => 20,
			'#maxlength' => 80
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
	$name = !empty($form_state['values']['fieldname']) ? $form_state['values']['fieldname']
			: date("r");
	$zip = $form_state['values']['zip'];
	$canton = $form_state['values']['canton'];
	$country = $form_state['values']['country'];
	$township = $form_state['values']['township'];

	$areaType = $form_state['values']['area_type'];
	$altitude = $form_state['values']['altitude'];
	$surface_area = empty($form_state['values']['surface-area']) ? 0
			: floor($form_state['values']['surface-area'] + 0.5);
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
		$geom = "ST_GeomFromText('LINESTRING($linestring)', 4326)"; // SRID 4326 = WGS 84
		break;
	case 'polygon':
	// make sure line is closed
		if (end($points) != reset($points)) {
			$points[] = reset($points);
		}
		$linestring = implode(', ', $points);
		$geom = "ST_GeomFromText('POLYGON(($linestring))', 4326)"; // SRID 4326 = WGS 84
		break;
	case 'marker':
		$geom = "ST_GeomFromText('POINT(" . reset($points) . ")', 4326)"; // SRID 4326 = WGS 84
		break;
	}

	if (!empty($name)) {
		// insert area
		$area_id = db_insert('area')->fields(
				array('name' => $name,
						'zip' => $zip,
						'canton' => $canton,
						'country' => $country,
						'altitude' => $altitude,
						'surface_area' => $surface_area,
						'township' => $township,
						'acl_id' => 1
				))
			->execute();

		// No idea why I can not use this when I create the record
		// FIXME: set 'geom' already at db_insert() above
		db_update('area')->expression('geom', $geom)
			->condition('id', $area_id)
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
 * Show all existing areas.
 * @return render array
 */
function area_show_all_areas() {
	drupal_set_title(t('Show all existing areas'));

	$output['map'] = array(
			'#search' => true,
			'#create' => false,
			'#showall' => true,
			'#theme' => 'area'
	);

	$tableHeader[] = array(
			'name' => t('Internal Id'),
			'idField' => true,
			'dbfield' => 'id',
			'hide' => true,
	);

	$tableHeader[] = array(
			'name' => t('Name'),
			'dbfield' => 'name'
	);

	$tableHeader[] = array(
			'name' => t('Altitude [m]'),
			'dbfield' => 'altitude',
	);

	// all markers have a surface of 0m² - does not look nice to the user
	// 	$tableHeader[] = array(
	// 			'name' => t('Surface area [m²]'),
	// 			'dbfield' => 'surface_area',
	// 	);

	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
	);

	$tableHeader[] = array(
			'name' => t('Canton'),
			'dbfield' => 'canton',
	);

	$tableHeader[] = array(
			'name' => t('Country'),
			'dbfield' => 'country',
	);

	$output['space'] = array(
			'#type' => 'markup',
			'#markup' => '<p/>'
	);

	$output['area_table'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#title' => 'Areas',
			'#id_table' => 'areas',
			'#options' => array(
					'jsonUrl' => "'" . base_path() . "area/datatablejson'",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id) {
						jQuery(celDiv).click(
							function() {
								window.location.href = Drupal.settings.basePath+'area/' + id;
							}
						)
					}"
			)
	);
	return $output;

}

/**
 * Generate a JSON with all areas that are visible to the user
 * @todo Include the user's nickname
 */
function area_all_areas_datatablejson_json() {
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'a.id AS id',
			'a.name',
			'a.altitude',
			'a.surface_area',
			'a.township',
			'a.canton',
			'a.country',
			'ST_AsGeoJSON(a.geom) AS geom',
			'a.commenttext'
	);

	$sql['dbSearchColumns'] = array(
			'a.name',
			'a.altitude',
			'a.township',
			'a.canton',
			'a.country',
			'a.commenttext'
	);

	$sql['dbTable'] = "{area} a";
	$sql['id'] = 'id';
	$output = datatable_dbRequest($sql);
	return drupal_json_output($output);
}

/**
 * Return all viewable areas as JSON.
 * @note Probably could be merged with area_all_areas_datatablejson_json
 * @return JSON
 */
function area_all_areas_json($area_id = false) {
	$areas = db_query(
		'SELECT
			a.id AS id,
			a.name,
			a.altitude,
			a.surface_area,
			a.township,
			a.canton,
			a.country,
			ST_AsGeoJSON(a.geom) AS geom,
			a.commenttext
			FROM
				{area} a
			WHERE
				-- let this condition always be true if no id set
				(a.id = :areaid OR :have_no_area_id)
			ORDER BY
				id',
		array(':areaid' => $area_id,
				':have_no_area_id' => !$area_id
		));

	$items = array();
	foreach ($areas as $entry) {
		$item = array();
		$item['id'] = (int) $entry->id;
		$item['name'] = $entry->name;
		$item['parcel_nr'] = NULL;
		$item['altitude'] = (int) $entry->altitude;
		$item['surface_area'] = (int) $entry->surface_area;
		$item['township'] = $entry->township;
		$item['canton'] = $entry->canton;
		$item['country'] = $entry->country;
		$item['commenttext'] = $entry->commenttext;

		$item['type'] = '';
		$item['area_points'] = array();
		$json = json_decode($entry->geom);
		if ($json) {
			if ('Polygon' == $json->type) {
				$item['type'] = 'polygon';
				foreach ($json->coordinates[0] as $coordinate) {
					$item['area_points'][] = array(
							'lat' => $coordinate[1],
							'lng' => $coordinate[0]
					);
				}
			} else if ($json->type == 'LineString') {
				$item['type'] = 'polyline';
				foreach ($json->coordinates as $coordinate) {
					$item['area_points'][] = array(
							'lat' => $coordinate[1],
							'lng' => $coordinate[0]
					);
				}
			} else if ($json->type == 'Point') {
				$item['type'] = 'marker';
				$item['area_points'][] = array(
						'lat' => $json->coordinates[1],
						'lng' => $json->coordinates[0]
				);
			} else {
				assert(false);
			}
		}
		$items[] = $item;
	}
	// json_encode wants that keys are sequential and start with 0, otherwise it'll treat it as an object and
	// not as an array. However, we used area_id as $key, so we have to index them numerically
	$items = array_values($items);
	return drupal_json_output($items);
}
/**
 * Return a hidden form which can be used to store values handed over by Javascript
 * @return form to submit area properties
 */
function area_address_hidden_form() {
	$form['addressdata'] = array(
			'#type' => 'fieldset',
			'#prefix' => '<div id="addressdata" style="visibility: hidden; height: 0px">',
			'#suffix' => '</div>'
	);
	$form['addressdata']['fieldname'] = array(
			'#type' => 'hidden',
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
	$form['addressdata']['surface-area'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-surface-area'
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
 * Return a form which shows some important facts about a given area
 * @param integer $area_id
 */
function area_overview_ajax_callback($area_id) {
	
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
