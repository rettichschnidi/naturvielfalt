<?php

/**
 * @file area.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

/**
 * Return the CH1903 coordinates of a given area.
 * Include a link to the Wikipedia article about
 * CH1903.
 * @param array $areadata
 */
function get_ch1903_coordinates_link($areadata) {
	assert(is_array($areadata));
	global $user;
	$linkCH1903 = l(
		"CH1903",
		'http://' . $user->language . '.wikipedia.org/wiki/CH1903');

	$coordinates = (int) $areadata['ch1903_center_x'] . '/'
			. (int) $areadata['ch1903_center_y'];
	return $coordinates . " ($linkCH1903)";
}

/**
 * Return the WGS84 coordinates of a given area.
 * Include a link to the Wikipedia article about
 * WGS84.
 * @param array $areadata
 */
function get_wgs84_coordinates_link($areadata) {
	assert(is_array($areadata));
	global $user;
	$linkWGS84 = l(
		"WGS84",
		'http://' . $user->language . '.wikipedia.org/wiki/WGS84');

	$coordinates = round($areadata['wgs84_center_lat'], 2) . '/'
			. round($areadata['wgs84_center_lng'], 2);
	return $coordinates . " ($linkWGS84)";
}

/**
 * Return an array with all data of a given $area_id
 * @todo document all elements
 * @param integer $area_id
 */
function area_get_full_dataset_of_specific_area($area_id) {
	assert($area_id != NULL);
	$result = db_query(
		'SELECT
				name,
				altitude,
				zip,
				township,
				canton,
				country,
				comment,
				create_time,
				modify_time,
				protection_target,
				safety_precautions,
				tending_strategies,
				-- either POINT, POLYGON or LINESTRING
				GeometryType(geom) AS geometry_type,
				-- Center of the geometry - CH1903, x value
				ST_X(ST_Centroid(Transform(geom, 21781))) AS ch1903_center_x,
				-- Center of the geometry - CH1903, y value
				ST_Y(ST_Centroid(Transform(geom, 21781))) AS ch1903_center_y,
				-- Center of the geometry - WGS84, latitude
				ST_X(ST_Centroid(Transform(geom, 4326))) AS wgs84_center_lat,
				-- Center of the geometry - WGS84
				ST_Y(ST_Centroid(Transform(geom, 4326))) AS wgs84_center_lng,
				-- Coordinates of the geometry - CH1903
				AsText(Transform(geom, 21781)) AS ch1903_coordinates,
				-- Coordinates of the geometry - WGS84
				AsText(Transform(geom, 4326)) AS wgs84_coordinates,
				-- Area surface in m^2
				Area(Transform(geom, 21781)) AS surface_area,
				-- Pathlength (if linestring) in meters
				ST_Length(Transform(geom, 21781)) AS pathlength,
				-- Geometry as JSON
				ST_AsGeoJSON(geom) AS wgs84_geom_json
			FROM
				{area}
			WHERE
				id = :id',
		array(':id' => $area_id));

	/*
	 * Bail out if an non-existing area is requested,
	 * return empty array
	 */
	if ($result->rowCount() == 0) {
		drupal_set_title(t("No such area available"));
		return array();
	}
	assert($result->rowCount() == 1);
	$areadata = (array) $result->fetch();

	/* extract the coordinates of the requested geometry */
	if ($areadata['geometry_type'] == "POINT") {
		$areadata['coordinatejson'] = array(
				json_decode($areadata['wgs84_geom_json'])->coordinates
		);
	} else if ($areadata['geometry_type'] == "LINESTRING") {
		$areadata['coordinatejson'] = json_decode($areadata['wgs84_geom_json'])->coordinates;
	} else {
		$areadata['coordinatejson'] = json_decode($areadata['wgs84_geom_json'])->coordinates[0];
	}

	/* swap the coordinates from lat/lng to lng/lat */
	area_swap_json_lnglat($areadata['coordinatejson']);
	return $areadata;
}

/**
 * Return all usefull data about an area in a tabular form.
 * Leave out datafields which are not set.
 * 
 * @param array An array with all needed data
 */
function area_get_infotable_of_specific_area($areadata = NULL) {
	assert($areadata != NULL);
	$rows = array();
	if (!empty($areadata['township'])) {
		$rows[] = array(
				array(
						'data' => t('Township'),
						'header' => true
				),
				$areadata['township']
		);
	}
	if (!empty($areadata['zip'])) {
		$rows[] = array(
				array(
						'data' => t('ZIP'),
						'header' => true
				),
				$areadata['zip']
		);
	}
	if (!empty($areadata['canton'])) {
		$rows[] = array(
				array(
						'data' => t('Canton'),
						'header' => true
				),
				$areadata['canton']
		);
	}
	if (!empty($areadata['country'])) {
		$rows[] = array(
				array(
						'data' => t('Country'),
						'header' => true
				),
				$areadata['country']
		);
	}
	if (!empty($areadata['altitude'])) {
		$rows[] = array(
				array(
						'data' => t('Altitude [m.a.s.l.]'),
						'header' => true
				),
				$areadata['altitude']
		);
	}
	if (!empty($areadata['wgs84_center_lat'])
			&& !empty($areadata['wgs84_center_lng'])) {
		$rows[] = array(
				array(
						'data' => t('Coordinates'),
						'header' => true
				),
				get_wgs84_coordinates_link($areadata) . "<br/>"
						. get_ch1903_coordinates_link($areadata)
		);
	}

	/* Depending on what kind of geometry is shown,
	 * display the pathlength or the surface area.
	 * Or none at all if geometry is a point.
	 */
	if ($line = $areadata['geometry_type'] == "POLYGON") {
		$rows[] = array(
				array(
						'data' => t('Surface area [m²]'),
						'header' => true
				),
				(int) $areadata['surface_area']
		);
	} else if ($line = $areadata['geometry_type'] == "LINESTRING") {
		$rows[] = array(
				array(
						'data' => t('Path length [m]'),
						'header' => true
				),
				(int) $areadata['pathlength']
		);
	}

	/* add the table with the data prepared above */
	$table = array(
			'#theme' => 'table',
			'#rows' => $rows,
			'#id_table' => 'area_tabular_data',
	);

	return $table;
}

/**
 * Return a hidden form which can be used to store values handed over by Javascript
 * @todo lets find a better way than this... please...
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
function area_overview_form($area_id) {

	$form['addressdata'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area')
	);
	$form['addressdata']['fieldname'] = array(
			'#type' => 'item',
			'#size' => 50,
			'#markup' => "FIELDNAME"
	);
	return $form;
}

/**
 *
 * Converts coordinates from WGS84 to system CH1903 (ROW).
 * For details how to convert from WGS84 to CH1903, have a loot at the
 * Wikipedia article:
 *  - http://de.wikipedia.org/wiki/Schweizer_Landeskoordinaten
 *  
 * @param array Array with two elements: ['lat'] => $lat, ['lng'] => $lng
 * @return array Array with two elements in CH1903: ['x'] => $x, ['y'] => $y
 */
function area_convert_wgs84_to_ch1903($coordinates = NULL) {
	assert($coordinates);
	assert(count($coordinates) == 2);

	// extract the given wgs84 coordinates
	$lat = $coordinates['0'];
	$lng = $coordinates['1'];

	$latsex = $lat * 3600;
	$lngsex = $lng * 3600;

	$lathlp = ($latsex - 169028.66) / 10000;
	$lnghlp = ($lngsex - 26782.5) / 10000;

	$x = 200147.07 + 308807.95 * $lathlp + 3745.25 * pow($lnghlp, 2)
			+ 76.63 * pow($lathlp, 2) + 119.79 * pow($lathlp, 3)
			- 194.56 * pow($lnghlp, 2) * $lathlp;
	$y = 600072.37 + 211455.93 * $lnghlp - 10938.51 * $lnghlp * $lathlp
			- 0.36 * $lnghlp * pow($lathlp, 2) - 44.54 * pow($lnghlp, 3);

	$result = array(
			'x' => $x,
			'y' => $y
	);
	return $result;
}

/**
 * Swap from lat/lng to lng/lat or vice versa.
 * This function is needed as posgis can export a json
 * of each geometry type, but the coordinates are in a
 * different order than what google maps expects.
 * 
 * @param &$json an array with coordinates, passed by reference
 *  - Example:
 *  		array (
 *  		  0 => 
 *  		  array (
 *  			0 => 9.3715184183853,
 *  			1 => 47.461692049315,
 *  		  ),
 *  		  1 => 
 *  		  array (
 *  		  	0 => 9.3712126465576,
 *  			1 => 47.461755518134,
 *  		  )
 *  		)
 */
function area_swap_json_lnglat(&$json) {
	if (false)
		die("<pre>" . var_export($json, true) . "</pre>");
	foreach ($json as &$currentelement) {
		$tmp = $currentelement[0];
		$currentelement[0] = $currentelement[1];
		$currentelement[1] = $tmp;
	}
}
?>
