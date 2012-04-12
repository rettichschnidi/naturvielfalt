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
	if ($area_id == NULL) {
		drupal_set_title(t("No such area available."));
		return array();
	}
	$result = db_query(
		'SELECT
				-- area values
				a.id,
				a.area_surface_id,
				a.acl_id,
				a.name,
				a.comment,
				a.create_time,
				a.modify_time,
				a.protection_target,
				a.safety_precautions,
				a.tending_strategies,
				-- area_surface values
				a_s.altitude,
				a_s.zip,
				a_s.township,
				a_s.canton,
				a_s.country,
				-- either POINT, POLYGON or LINESTRING
				GeometryType(a_s.geom) AS geometry_type,
				-- Center of the geometry - CH1903, x value
				ST_X(ST_Centroid(Transform(a_s.geom, 21781))) AS ch1903_center_x,
				-- Center of the geometry - CH1903, y value
				ST_Y(ST_Centroid(Transform(a_s.geom, 21781))) AS ch1903_center_y,
				-- Center of the geometry - WGS84, latitude
				ST_X(ST_Centroid(Transform(a_s.geom, 4326))) AS wgs84_center_lat,
				-- Center of the geometry - WGS84
				ST_Y(ST_Centroid(Transform(a_s.geom, 4326))) AS wgs84_center_lng,
				-- Coordinates of the geometry - CH1903
				AsText(Transform(a_s.geom, 21781)) AS ch1903_coordinates,
				-- Coordinates of the geometry - WGS84
				AsText(Transform(a_s.geom, 4326)) AS wgs84_coordinates,
				-- Area surface in m^2
				Area(Transform(a_s.geom, 21781)) AS surface_area,
				-- Pathlength (if linestring) in meters
				ST_Length(Transform(a_s.geom, 21781)) AS pathlength,
				-- Geometry as JSON
				ST_AsGeoJSON(a_s.geom) AS wgs84_geom_json,
				-- users values
				u.name AS username,
				u.uid AS userid
			FROM
				{area} a
				JOIN {area_surface} a_s ON a.area_surface_id = a_s.id
				JOIN {acl} cacl ON cacl.id = a.acl_id
				JOIN {users} u ON u.uid = cacl.users_id
			WHERE
				a.id = :area_id',
		array(':area_id' => $area_id));

	/*
	 * Bail out if an non-existing area is requested,
	 * return empty array
	 */
	if ($result->rowCount() == 0) {
		drupal_set_title(t("No such area available."));
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
 * @param array $areadata
 * 		An array with all data about an area
 * @param $options array
 * 		Options array with true/false flags. Not set flags will be threaded as true.
 * 		Available options:
 * 			- 'username'
 * 			- 'township'
 * 			- 'zip'
 * 			- 'canton'
 * 			- 'country'
 * 			- 'altitude'
 * 			- 'coordinates'
 * 			- 'create_time'
 * 			- 'modify_time'
 * 			- 'surface_area'
 * 			- 'pathlength'
 * 
 */
function area_get_infotable_of_specific_area($areadata = NULL,
		$options = array()) {
	assert($areadata != NULL);
	$rows = array();
	if (!empty($areadata['username'])
			&& (!isset($options['pathlength']) || $options['pathlength'])) {
		$rows[] = array(
				array(
						'data' => t('Creator'),
						'header' => true
				),
				l(
					$areadata['username'],
					base_path() . "user/" . $areadata['userid'],
					array(
							'external' => true
					))
		);
	}
	if (!empty($areadata['township'])
			&& (!isset($options['township']) || $options['township'])) {
		$rows[] = array(
				array(
						'data' => t('Township'),
						'header' => true
				),
				$areadata['township']
		);
	}
	if (!empty($areadata['zip'])
			&& (!isset($options['zip']) || $options['zip'])) {
		$rows[] = array(
				array(
						'data' => t('ZIP'),
						'header' => true
				),
				$areadata['zip']
		);
	}
	if (!empty($areadata['canton'])
			&& (!isset($options['canton']) || $options['canton'])) {
		$rows[] = array(
				array(
						'data' => t('Canton'),
						'header' => true
				),
				$areadata['canton']
		);
	}
	if (!empty($areadata['country'])
			&& (!isset($options['country']) || $options['country'])) {
		$rows[] = array(
				array(
						'data' => t('Country'),
						'header' => true
				),
				$areadata['country']
		);
	}
	if (!empty($areadata['altitude'])
			&& (!isset($options['altitude']) || $options['altitude'])) {
		$rows[] = array(
				array(
						'data' => t('Altitude [m.a.s.l.]'),
						'header' => true
				),
				$areadata['altitude']
		);
	}
	if ((!empty($areadata['wgs84_center_lat'])
			&& !empty($areadata['wgs84_center_lng'])
			&& !empty($areadata['ch1903_center_x'])
			&& !empty($areadata['ch1903_center_y']))
			&& (!isset($options['coordinates']) || $options['coordinates'])) {
		$rows[] = array(
				array(
						'data' => t('Coordinates'),
						'header' => true
				),
				get_wgs84_coordinates_link($areadata) . "<br/>"
						. get_ch1903_coordinates_link($areadata)
		);
	}

	if (!empty($areadata['create_time'])
			&& (!isset($options['create_time']) || $options['create_time'])) {
		$rows[] = array(
				array(
						'data' => t('Create date'),
						'header' => true
				),
				date("j.m.Y", strtotime($areadata['create_time']))
		);
	}

	if (!empty($areadata['modify_time'])
			&& (!isset($options['modify_time']) || $options['modify_time'])) {
		$rows[] = array(
				array(
						'data' => t('Last modification'),
						'header' => true
				),
				date("j.m.Y", strtotime($areadata['modify_time']))
		);
	}

	/* Depending on what kind of geometry is shown,
	 * display the pathlength or the surface area.
	 * Or none at all if geometry is a point.
	 */
	if ($line = $areadata['geometry_type'] == "POLYGON"
			&& (!isset($options['surface_area']) || $options['surface_area'])) {
		$rows[] = array(
				array(
						'data' => t('Surface area [m²]'),
						'header' => true
				),
				(int) $areadata['surface_area']
		);
	} else if ($line = $areadata['geometry_type'] == "LINESTRING"
			&& (!isset($options['pathlength']) || $options['pathlength'])) {
		$rows[] = array(
				array(
						'data' => t('Path length [m]'),
						'header' => true
				),
				(int) $areadata['pathlength']
		);
	}

	/* add the table with the data prepared above */
	$form = array(
			'#theme' => 'table',
			'#rows' => $rows,
			'#id_table' => 'area_tabular_data',
	);

	return $form;
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
					'id' => 'hiddenfield-name'
			),
	);
	$form['addressdata']['zip'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-zip'
			),
	);
	$form['addressdata']['township'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-township'
			),
	);
	$form['addressdata']['canton'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-canton'
			),
	);
	$form['addressdata']['altitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-altitude'
			),
	);
	$form['addressdata']['country'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-country'
			),
	);
	$form['addressdata']['latitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-latitude'
			),
	);
	$form['addressdata']['longitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-longitude'
			),
	);
	$form['addressdata']['area-coords'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-area-coordinates'
			),
	);
	$form['addressdata']['area_type'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-area-type'
			),
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
 * 
 * Return a PostGIS geometry object from a JSON
 * 
 * The return value can be used by the method fields() of db_update/db_insert 
 * 
 * @param string $areaType
 * 	 Valid values: 'polyline', 'polygon' or 'marker'
 * @param object $coordinatesJson
 * @return geometry A geometry type, representing the given JSON coordinates
 */
function area_get_postgis_geometry_object($areaType = NULL,
		$coordinatesJson = NULL) {
	assert($coordinatesJson != NULL);
	assert($areaType != NULL);

	$points = array();
	foreach ($coordinatesJson as $currentCoordinatesJson) {
		$points[] = implode(' ', $currentCoordinatesJson);
	}
	assert(!empty($points));

	switch ($areaType) {
	case 'polyline':
		$linestring = implode(', ', $points);
		$geom = "ST_GeomFromText('LINESTRING($linestring)', 4326)"; // SRID 4326 = WGS 84
		break;
	case 'polygon':
		if (end($points) != reset($points)) {
			// make sure line is closed
			$points[] = reset($points);
		}
		$linestring = implode(', ', $points);
		$geom = "ST_GeomFromText('POLYGON(($linestring))', 4326)"; // SRID 4326 = WGS 84
		break;
	case 'marker':
		$geom = "ST_GeomFromText('POINT(" . reset($points) . ")', 4326)"; // SRID 4326 = WGS 84
		break;
	default:
		watchdog_exception(
			'area',
			t(
				'«@areatype» is not a valid area type.',
				array('@areatype' => $areaType)));
		return false;
	}
	$result = db_query("SELECT $geom AS geom");
	assert($result->rowCount() == 1);
	return $result->fetch()
		->geom;
}

/**
 * @brief Create a geometry in table area_surface.
 * 
 * @param array $data
 * 	The following variables can be submitted:
 * 	 - 'zip'
 * 	 - 'canton'
 * 	 - 'country'
 * 	 - 'township'
 * 	 - 'altitude'
 * 	 - 'area_points' JSON with all coordinates (mandatory)
 * 	 - 'type' (mandatory, either 'polyline', 'polygon' or 'marker')
 * @param boolean $coordinates_are_already_lng_lat
 * 	True if area_points are already in lng/lat format, false otherwise (default)
 * @return integer New geometry id
 */
function area_create_geometry($data = array(),
		$coordinates_are_already_lng_lat = false) {
	assert(!empty($data));
	$geometry_id = false;
	// Bail out early if input stinks too hefty.
	if (!isset($data['area_points']) || !isset($data['type'])) {
		assert(false);
		return;
	}
	$jsonCoords = filter_xss($data['area_points']);
	$areaType = filter_xss($data['type']);
	$coordinates = json_decode($jsonCoords, true);
	// Swap the cooridnates to the order expected by postgis
	if (!$coordinates_are_already_lng_lat) {
		area_swap_json_lnglat($coordinates);
	}

	$transaction = db_transaction();
	try {
		$geometryObject = area_get_postgis_geometry_object(
			$areaType,
			$coordinates);
		assert($geometryObject != NULL);
		$geometry_id = db_insert('area_surface')->fields(
				array('zip' => filter_xss($data['zip']),
						'canton' => filter_xss($data['canton']),
						'country' => filter_xss($data['country']),
						'township' => filter_xss($data['township']),
						'altitude' => filter_xss($data['altitude']),
						'geom' => $geometryObject
				))
			->execute();
	} catch (Exception $e) {
		$transaction->rollback();
		watchdog_exception('area', $e);
		drupal_set_message(t('New geometry could not be created.'), 'error');
	}
	return $geometry_id;
}

/**
 * @brief Update an exsting geometry in table area_surface.
 *
 * @param geometry_id
 * 	- Id of the geometry to update.
 * @param array $data
 * 	The following variables can be submitted:
 * 	 - 'zip'
 * 	 - 'canton'
 * 	 - 'country'
 * 	 - 'township'
 * 	 - 'altitude'
 * 	 - 'area_points' JSON with all coordinates (mandatory)
 * 	 - 'type' (mandatory, either 'polyline', 'polygon' or 'marker')
 * @param boolean $coordinates_are_already_lng_lat
 * 	True if area_points are already in lng/lat format, false otherwise (default)
 */
function area_update_geometry($geometry_id, $data = array(),
		$coordinates_are_already_lng_lat = false) {
	assert(!empty($data));
	assert($geometry_id != NULL);
	// Bail out early if input stinks too hefty.
	if (!isset($data['area_points']) || !isset($data['type']) || $geometry_id == NULL) {
		assert(false);
		return;
	}
	$jsonCoords = filter_xss($data['area_points']);
	$areaType = filter_xss($data['type']);
	$coordinates = json_decode($jsonCoords, true);
	// Swap the cooridnates to the order expected by postgis
	if (!$coordinates_are_already_lng_lat) {
		area_swap_json_lnglat($coordinates);
	}

	$transaction = db_transaction();
	try {
		$geometryObject = area_get_postgis_geometry_object(
			$areaType,
			$coordinates);
		assert($geometryObject != NULL);
		db_update('area_surface')->fields(
				array('zip' => filter_xss($data['zip']),
						'canton' => filter_xss($data['canton']),
						'country' => filter_xss($data['country']),
						'township' => filter_xss($data['township']),
						'altitude' => filter_xss($data['altitude']),
						'geom' => $geometryObject
				))
			->condition('id', $geometry_id)
			->execute();
	} catch (Exception $e) {
		$transaction->rollback();
		watchdog_exception('area', $e);
		drupal_set_message(t('Geometry could not be updated.'), 'error');
		throw $e;
	}
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

/**
 * Check if user has write access to area
 *
 * @param integer $area_id
 * @return true if access ok, false if not
 * @todo Check if this user has write access to the inventory
 */
function area_edit_access($area_id) {
	return true;
}
?>
