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
 *
 * @param array $areadata
 * @return string Coordinates with link to wikipedia article
 */
function get_ch1903_coordinates_link($areadata) {
	assert(is_array($areadata));
	$area_protocol = isset($_SERVER['HTTPS']) && !empty($_SERVER['HTTPS'])
			? 'https://' : 'http://';
	global $user;
	global $language;
	
	$linkCH1903 = l(
		"CH1903",
		$area_protocol . $language->language . '.wikipedia.org/wiki/CH1903');

	$coordinates = number_format($areadata['ch1903_center_x'], 0, '.', '\'')
		.'/'
		.number_format($areadata['ch1903_center_y'], 0, '.', '\'');
	return $coordinates . " ($linkCH1903)";
}

/**
 * Return the WGS84 coordinates of a given area.
 * Include a link to the Wikipedia article about
 * WGS84.
 *
 * @param array $areadata
 * @return string Coordinates with link to wikipedia article
 */
function get_wgs84_coordinates_link($areadata) {
	assert(is_array($areadata));
	global $user;
	global $language;
	
	$area_protocol = isset($_SERVER['HTTPS']) && !empty($_SERVER['HTTPS'])
			? 'https://' : 'http://';
	;
	$linkWGS84 = l(
		"WGS84",
		$area_protocol . $language->language . '.wikipedia.org/wiki/WGS84');

	$coordinates = round($areadata['wgs84_center_lat'], 2) . '/'
			. round($areadata['wgs84_center_lng'], 2);
	return $coordinates . " ($linkWGS84)";
}

/**
 * Return an array with all data of a given $area_id.
 *
 * @return
 * array (
 *   'id' => '2',
 *   'area_geometry_id' => '2',
 *   'acl_id' => '127',
 *   'name' => 'asdf',
 *   'comment' => NULL,
 *   'create_time' => '2012-04-30 08:32:46.881145',
 *   'modify_time' => '2012-05-08 10:58:43.376642',
 *   'protection_target' => NULL,
 *   'safety_precautions' => NULL,
 *   'tending_strategies' => NULL,
 *   'altitude' => '347',
 *   'zip' => '5200',
 *   'township' => 'Brugg',
 *   'canton' => 'AG',
 *   'country' => 'Schweiz',
 *   'geometry_type' => 'POINT',
 *   'ch1903_center_x' => '658176.409277528',
 *   'ch1903_center_y' => '259971.342576873',
 *   'wgs84_center_lat' => '8.2105669577275',
 *   'wgs84_center_lng' => '47.487932375354',
 *   'ch1903_coordinates' => 'POINT(658176.409277528 259971.342576873)',
 *   'wgs84_coordinates' => 'POINT(8.2105669577275 47.487932375354)',
 *   'surface_area' => '0',
 *   'pathlength' => '0',
 *   'wgs84_geom_json' => '{"type":"Point","coordinates":[8.210566957727499,47.487932375353999]}',
 *   'username' => 'naturvielfalt',
 *   'userid' => '1',
 *   'coordinatejson' => 
 *   array (
 *     0 => 
 *     array (
 *       0 => 47.487932375354,
 *       1 => 8.2105669577275,
 *     ),
 *   ),
 * )
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
				a.parent_id,
				a.area_geometry_id,
				a.acl_id,
				a.name,
				a.comment,
				a.create_time,
				a.modify_time,
				a.protection_target,
				a.safety_precautions,
				a.tending_strategies,
				-- area_geometry values
				a_s.altitude,
				a_s.zip,
				a_s.township,
				a_s.canton,
				a_s.country,
				-- either POINT, POLYGON or LINESTRING
				GeometryType(a_s.geom) AS geometry_type,
				-- Center of the geometry - CH1903, x value
				ST_X(ST_Centroid(st_transform(a_s.geom, 21781))) AS ch1903_center_x,
				-- Center of the geometry - CH1903, y value
				ST_Y(ST_Centroid(st_transform(a_s.geom, 21781))) AS ch1903_center_y,
				-- Center of the geometry - WGS84, latitude
				ST_X(ST_Centroid(st_transform(a_s.geom, 4326))) AS wgs84_center_lat,
				-- Center of the geometry - WGS84
				ST_Y(ST_Centroid(st_transform(a_s.geom, 4326))) AS wgs84_center_lng,
				-- Coordinates of the geometry - CH1903
				st_astext(st_transform(a_s.geom, 21781)) AS ch1903_coordinates,
				-- Coordinates of the geometry - WGS84
				st_astext(st_transform(a_s.geom, 4326)) AS wgs84_coordinates,
				-- Area surface in m^2
				st_area(st_transform(a_s.geom, 21781)) AS surface_area,
				-- Pathlength (if linestring) in meters
				ST_Length(st_transform(a_s.geom, 21781)) AS pathlength,
				-- Geometry as JSON
				ST_AsGeoJSON(a_s.geom) AS wgs84_geom_json,
				-- users values
				u.name AS username,
				u.uid AS userid
			FROM
				{area} a
				JOIN {area_geometry} a_s ON a.area_geometry_id = a_s.id
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

	// format the username 
	$areadata['username'] = check_plain(format_username(user_load($areadata['userid'])));
	
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
	if (!empty($areadata['name'])
			&& (!isset($options['pathlength']) || $options['pathlength'])) {
		
		if(!empty($areadata['parent_id'])) {
			$parent_area = area_get_full_dataset_of_specific_area($areadata['parent_id']);
			if(!empty($parent_area)) {
				$rows[] = array(
						array(
								'data' => t('Parent Area'),
								'header' => true
						),
						l(
								check_plain($parent_area['name']),
								base_path() . "area/" . $parent_area['id'].'/show',
								array(
										'external' => true
								))
				);
			}
		}
		$rows[] = array(
				array(
						'data' => t('Area Name'),
						'header' => true
				),
				l(
						check_plain($areadata['name']), 
						base_path() . "area/" . $areadata['id'].'/show',
						array(
								'external' => true
						))
				);
	}
	if (!empty($areadata['username'])
			&& (!isset($options['pathlength']) || $options['pathlength'])) {
		$rows[] = array(
				array(
						'data' => t('Creator'),
						'header' => true
				),
				l(
					check_plain($areadata['username']),
					base_path() . "user/" . $areadata['userid'] . '/contact',
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
				check_plain($areadata['township'])
		);
	}
	if (!empty($areadata['zip'])
			&& (!isset($options['zip']) || $options['zip'])) {
		$rows[] = array(
				array(
						'data' => t('ZIP'),
						'header' => true
				),
				check_plain($areadata['zip'])
		);
	}
	if (!empty($areadata['canton'])
			&& (!isset($options['canton']) || $options['canton'])) {
		$rows[] = array(
				array(
						'data' => t('Canton'),
						'header' => true
				),
				check_plain($areadata['canton'])
		);
	}
	if (!empty($areadata['country'])
			&& (!isset($options['country']) || $options['country'])) {
		$rows[] = array(
				array(
						'data' => t('Country'),
						'header' => true
				),
				check_plain($areadata['country'])
		);
	}
	if (!empty($areadata['altitude'])
			&& (!isset($options['altitude']) || $options['altitude'])) {
		$rows[] = array(
				array(
						'data' => t('Altitude [m.a.s.l.]'),
						'header' => true
				),
				check_plain($areadata['altitude'])
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
				date("d.m.Y H:i:s", strtotime($areadata['create_time']))
		);
	}

	if (!empty($areadata['modify_time'])
			&& (!isset($options['modify_time']) || $options['modify_time'])) {
		$rows[] = array(
				array(
						'data' => t('Last modification'),
						'header' => true
				),
				date("d.m.Y H:i:s", strtotime($areadata['modify_time']))
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
			'#rows' => $rows
	);

	return $form;
}

/**
 * Return a hidden form which can be used to store values handed over by Javascript.
 *
 * @todo lets find a better way than this... please...
 * @return array Drupal form to submit area properties
 */
function area_address_hidden_form() {
	$form['addressdata'] = array(
			'#type' => 'fieldset',
			'#prefix' => '<div id="addressdata" style="display: none; height: 0px">',
			'#suffix' => '</div>'
	);
	$form['addressdata']['fieldname'] = array(
			'#type' => 'hidden',
			'#size' => 50,
			'#attributes' => array(
					'id' => 'hiddenfield-name',
					'name' => 'fieldname'
			),
	);
	$form['addressdata']['zip'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-zip',
					'name' => 'zip'
			),
	);
	$form['addressdata']['township'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-township',
					'name' => 'township'
			),
	);
	$form['addressdata']['canton'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-canton',
					'name' => 'canton'
			),
	);
	$form['addressdata']['altitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-altitude',
					'name' => 'altitude'
			),
	);
	$form['addressdata']['country'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-country',
					'name' => 'country'
			),
	);
	$form['addressdata']['latitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-latitude',
					'name' => 'latitude'
			),
	);
	$form['addressdata']['longitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-longitude',
					'name' => 'longitude'
			),
	);
	$form['addressdata']['area-coords'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-geometry-coordinates',
					'name' => 'area-coords'
			),
	);
	$form['addressdata']['area_type'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'hiddenfield-geometry-type',
					'name' => 'area_type'
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
	assert(is_array($coordinates));
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
 * Return a PostGIS geometry object from a JSON.
 *
 * The return value can be used by the method fields() of db_update/db_insert
 *
 * @param string $areaType
 * 	 Valid values: 'polyline', 'polygon' or 'marker'
 * @param object $coordinatesJson
 * @return geometry A geometry type, representing the given JSON coordinates
 * @throws Exception
 */
function area_get_postgis_geometry_object($areaType = NULL,
		$coordinatesJson = NULL) {
	assert($coordinatesJson != NULL);
	assert($areaType != NULL);

	$points = array();
	$sql_args = array();

	foreach ($coordinatesJson as $currentCoordinatesJson) {
		$points[] = implode(' ', $currentCoordinatesJson);
	}
	assert(!empty($points));

	switch ($areaType) {
	case 'polyline':
		$linestring = implode(', ', $points);
		$sql_args[':geom'] = 'LINESTRING(' . $linestring . ')';
		break;
	case 'polygon':
		if (end($points) != reset($points)) {
			// make sure line is closed
			$points[] = reset($points);
		}
		$linestring = implode(', ', $points);
		$sql_args[':geom'] = 'POLYGON((' . $linestring . '))';
		break;
	case 'marker':
		$sql_args[':geom'] = 'POINT(' . reset($points) . ')';
		break;
	default:
		watchdog(
			'area',
			t(
				'«@areatype» is not a valid area type.',
				array('@areatype' => $areaType)));
		throw new Exception(
			t(
				'«@areatype» is not a valid area type.',
				array('@areatype' => $areaType)));
	}
	$transaction = db_transaction();
	try {
		$result = db_query(
			'SELECT ST_GeomFromText(:geom, 4326) AS geom;',
			$sql_args); // SRID 4326 = WGS 84
		assert($result->rowCount() == 1);
		return $result->fetch()
			->geom;
	} catch (Exception $e) {
		$transaction->rollback();
		watchdog_exception('area', $e);
		drupal_set_message(
			t('Temporary geometry object could not be created.'),
			'error');
		throw $e;
	}
}

/**
 * @brief Create a geometry in table area_geometry.
 *
 * @param array $data
 * 	The following variables can be submitted:
 * 	 - 'zip'
 * 	 - 'canton'
 * 	 - 'country'
 * 	 - 'township'
 * 	 - 'altitude'
 * 	 - 'area_points' JSON-string with all coordinates (mandatory)
 * 	 - 'type' (mandatory, either 'polyline', 'polygon' or 'marker')
 * @param boolean $coordinates_are_already_lng_lat
 * 	True if area_points are already in lng/lat format, false otherwise (default)
 * @return integer
 *  New geometry id or false if a failure occurred
 * @note For an example usage, have a look at area_save()
 * @throws Exception
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
		$geometry_id = db_insert('area_geometry')->fields(
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
		throw $e;
	}
	return $geometry_id;
}

/**
 * Update an exsting geometry in table area_geometry.
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
 * @throws Exception
 */
function area_update_geometry($geometry_id, $data = array(),
		$coordinates_are_already_lng_lat = false) {
	assert(!empty($data));
	assert($geometry_id != NULL);
	// Bail out early if input stinks too hefty.
	if (!isset($data['area_points']) || !isset($data['type'])
			|| $geometry_id == NULL) {
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
		db_update('area_geometry')->fields(
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
 * Delete a geometry with given id
 * @param int $geometry_id
 * @return true on success, false on fail
 */
function area_delete_geometry($geometry_id) {
	$num_delted = db_delete('area_geometry')->condition('id', $geometry_id)
		->execute();
	if ($num_delted == 1)
		return true;
	return false;
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

function area_gallery($area) {
	return _area_presentation($area['id']);
}

function area_list($area) {
	return _area_presentation(
		$area['id'],
		'gallery_presentation_list',
		'presentation-list',
		1000000);
}

/**
 * @note PLEASE REMOVE THIS SHIT WHEN YOU HAVE REDONE THE GALLYERMODULE
 * @param unknown_type $areaId
 * @param unknown_type $size
 * @param unknown_type $cssClass
 * @param unknown_type $columns
 */
function _area_presentation($areaId, $size = 'gallery_presentation',
		$cssClass = false, $columns = GALLERY_COLUMNS) {
	if (!function_exists('gallery_presentation_renderer')) {
		return;
	}
	$area = db_select('area', 'a')->condition('id', $areaId)
		->fields('a')
		->execute()
		->fetch();
	drupal_set_title($area->name);

	$sorting = array(
			'default',
			'latin',
			'german',
			'family'
	);
	$sources = array(
			'default',
			'inventory',
			'presentation',
			'best'
	);

	$key = md5('area_presentation' . $areaId);
	if (isset($_GET[$key . '_source'])
			&& in_array($_GET[$key . '_source'], $sources)) {
		$_SESSION[$key . '_source'] = $_GET[$key . '_source'];
	}
	$source = isset($_SESSION[$key . '_source']) ? $_SESSION[$key . '_source']
			: false;

	$order = false;
	if (isset($_GET[$key . '_order'])
			&& in_array($_GET[$key . '_order'], $sorting)) {
		$_SESSION[$key . '_order'] = $_GET[$key . '_order'];
	}
	if (isset($_SESSION[$key . '_order'])) {
		$order = $_SESSION[$key . '_order'];
	}

	$groups = array();
	$weight = 1;

	$query = db_select('area_habitat', 'ah', array('fetch' => PDO::FETCH_ASSOC))->condition(
			'area_id',
			$areaId);
	$query->join('habitat', 'h', 'h.id = ah.habitat_id');
	$query->fields('h');
	$query->addField('ah', 'id', 'ah_id');
	switch ($order) {
	case 'latin':
		$query->orderBy('h.name, h.label', 'ASC');
		break;
	case 'german':
		$query->orderBy('h.name, h.label', 'ASC');
		break;
	case 'family':
		$query->orderBy('h.label', 'ASC');
		break;
	default:
		$query->orderBy('h.label', 'ASC');
		break;
	}

	$habitats = $query->execute()
		->fetchAll();

	foreach ($habitats as $habitat) {
		if (!array_key_exists(t('Habitats'), $groups))
			$groups[t('Habitats')] = array();
		array_push(
			$groups[t('Habitats')],
			array(
					'type' => 'area_habitat',
					'id' => (int) $habitat['ah_id'],
					'alternative' => array(
							'type' => 'habitat',
							'id' => (int) $habitat['id']
					),
					'content' => array(
							'label' => array(
									'#prefix' => '<h3>',
									'#suffix' => '</h3>',
									'#markup' => check_plain($habitat['label']),
							),
							'name_de' => array(
									'#prefix' => '<h4>',
									'#suffix' => '</h4>',
									'#markup' => l(
										check_plain($habitat['name_de']),
										'habitat/' . $habitat['id'])
							),
							'name_scientific' => array(
									'#prefix' => '<h5>',
									'#suffix' => '</h5>',
									'#markup' => check_plain(
										$habitat['name_scientific'])
											? l(
												$habitat['name_scientific'],
												'habitat/' . $habitat['id'])
											: ''
							)
					)
			));
	}

	foreach ($groups as $group => $items) {
		if ($items) {
			$page[$group] = array(
					'#weight' => $weight++
			);
			$page[$group]['title'] = array(
					'#prefix' => '<h2>',
					'#suffix' => '</h2>',
					'#markup' => $group
			);
			$page[$group]['galleryImages'] = gallery_presentation_renderer(
				'images',
				$items,
				$size,
				$cssClass,
				$columns);
		}
	}

	if (!isset($page) || !$page)
		$page['empty'] = array(
				'#prefix' => '<p>',
				'#suffix' => '<p>',
				'#markup' => t(
					'There are no organisms and no habitats available in this area.')
		);
	else {
		$url = drupal_get_destination();
		$url = drupal_parse_url($url['destination']);
		$url = $url['path'];

		$sselected = array();
		foreach ($sources as $s) {
			$sselected[$s] = $source == $s ? ' selected="selected"' : '';
		}
		$oselected = array();
		foreach ($sorting as $o)
			$oselected[$o] = $order == $o ? ' selected="selected"' : '';
		$page['form'] = array(
				'#markup' => '<form class="presentation-settings" method="get" action="'
						. url($url) . '">' . '<select size="1" name="' . $key
						. '_source">' . '<option value="default"'
						. $sselected['default'] . '>'
						. t('Prefer images from inventories') . '</option>'
						. '<option value="inventory"' . $sselected['inventory']
						. '>' . t('Show only images from inventories')
						. '</option>' . '<option value="presentation"'
						. $sselected['presentation'] . '>'
						. t('Show only presentation images from inventories')
						. '</option>' . '<option value="best"'
						. $sselected['best'] . '>'
						. t('Show the highest rated images of the site')
						. '</option>' . '</select> '
						. '<select size="1" name="' . $key . '_order">'
						. '<option value="default"' . $oselected['default']
						. '>' . t('Sort by position in inventory')
						. '</option>' . '<option value="german"'
						. $oselected['german'] . '>' . t('Sort by german name')
						. '</option>' . '<option value="latin"'
						. $oselected['latin'] . '>' . t('Sort by latin name')
						. '</option>' . '<option value="family"'
						. $oselected['family'] . '>' . t('Sort by family')
						. '</option>' . '</select> '
						. '<input type="submit" name="submit" value="'
						. t('apply') . '" />' . '</form>',
				'weight' => -1
		);
	}

	return $page;
}

/**
 * This method is responsible for using the Google Reverse Geocode API
 * to receive the canton, city, zip, etc. to store it later on in the database
 * as additional information to the longitude, latitude values.
 *
 * Return preformated array with all the needed fields for the naturvielfalt geometry
 * @param long $longitude
 * @param long $latitude
 * @param bool $original_response
 * 	 set to true, if you want the original answer from google
 * @return array
 */
function area_get_geocode_info($longitude = false, $latitude = false,
		$original_response = false) {
	$area_protocol = isset($_SERVER['HTTPS']) && !empty($_SERVER['HTTPS'])
			? 'https://' : 'http://';
	if (!$longitude || !$latitude) {
		return false;
	}

	$url = $area_protocol . 'maps.googleapis.com/maps/api/geocode/json?latlng='
			. $latitude . ',' . $longitude . '&sensor=true';
	$data = @file_get_contents($url);
	$jsondata = json_decode($data, true);

	$url = $area_protocol
			. 'maps.googleapis.com/maps/api/elevation/json?locations='
			. $latitude . ',' . $longitude . '&sensor=true';
	$data = @file_get_contents($url);
	$elevationData = json_decode($data, true);

	// if we get a placemark array and the status was good, get the address
	if (!is_array($jsondata) && $jsondata['Status']['code'] != 200) {
		return false;
	}

	if ($original_response) {
		$jsondata['elevationData'] = $elevationData;
		return $jsondata;
	}
	
	// get the correct values for 'zip', 'country', 'township' and 'canton'
	$myArray[] = array('zip', 'postal_code','long_name');
	$myArray[] = array('country', 'country','short_name');
	$myArray[] = array('township', 'locality','long_name');
	$myArray[] = array('canton', 'administrative_area_level_1','short_name');
	
	$output = getCorrectValues($jsondata, $myArray);
	
	$output['coordinate'] = '[[' . $latitude . ',' . $longitude . ']]';
/*	$output['zip'] = check_plain(
		$jsondata['results'][0]['address_components'][6]['long_name']);
	$output['canton'] = check_plain(
		$jsondata['results'][0]['address_components'][4]['long_name']);
	$output['country'] = check_plain(
		$jsondata['results'][0]['address_components'][5]['long_name']);
	$output['township'] = check_plain(
		$jsondata['results'][0]['address_components'][2]['long_name']);	*/
	$output['altitude'] = intval($elevationData['results'][0]['elevation']);
	$output['area_points'] = $output['coordinate'];
	$output['marker'] = 'marker';
	return $output;
}

// helper function used in area_get_geocode_info() to always return the correct values,
// since the data from the Google Maps API sometimes contains stuff like street number and sometimes not.
// for example the previous solution (in the comment block in area_get_geocode_info() above) sometimes returned country instead of zip, township instead of country etc.
function getCorrectValues($jsondata, $myArr) {
	$myCount = count($myArr);	// speed up the for loop
	for ($a = 0; $a < $myCount; $a++) {
		$myResultsCount = count($jsondata['results']);	// speed up the for loop
		for ($i = 0; $i < $myResultsCount; $i++) {	// go through every result block to get the correct values
			$myAddrCompsCount = count($jsondata['results'][$i]['address_components']);		// speed up the for loop
			for ($j = 0; $j < $myAddrCompsCount; $j++) {
				if ($jsondata['results'][$i]['address_components'][$j]['types'][0] === $myArr[$a][1] ) {
					//echo "{$myArr[$a][0]}: " . $jsondata['results'][$i]['address_components'][$j][$myArr[$a][2]] . " -> found in results[$i][address_components][$j]<br>\n";
					$output[$myArr[$a][0]] = check_plain($jsondata['results'][$i]['address_components'][$j][$myArr[$a][2]]);
					break 2;	// we found what we were looking for so continue with the next $myArr record
				}
			}
		}
	}
	return $output;
}

function area_get_standard_datatable_headers($init_sort = false,
		$init_sort2 = false) {
	
	global $base_url;
	$iconBaseUrl = $base_url . '/' . path_to_theme() . '/images/icons/';
	
	//All dbExactFields removed, because of incompatibility... (complex query in area.json.inc to achieve hierarchy)
	$tableHeader[] = array(
			'name' => t('Internal Id'),
			'idField' => true,
			'dbfield' => 'area_id',
			'hide' => true,
			'width' => 30
	);
	
	$tableHeader[] = array(
			'name' => '<img src="' . $iconBaseUrl . 'disabled/Forward.png" />',
			'dbfield' => 'hierarchyColumn',
			'width' => 30,
			'noSort' => true,
	);
	
	$tableHeader[] = array(
			'name' => '<input class="gridSelect" type="checkbox" disabled />',
			'dbfield' => 'selectColumn',
			'width' => 20,
			'noSort' => true,
	);
	
	$tableHeader[] = array(
			'name' => '<img src="' . $iconBaseUrl . 'disabled/List.png" />',
			'dbfield' => 'showColumn',
			'width' => 16,
			'noSort' => true,
	);
	
	$tableHeader[] = array(
			'name' => '<img src="' . $iconBaseUrl . 'disabled/Modify.png" />',
			'dbfield' => 'editColumn',
			'width' => 16,
			'noSort' => true,
	);
	
	$tableHeader[] = array(
			'name' => '<img src="' . $iconBaseUrl . 'disabled/Zoom in.png" />',
			'dbfield' => 'zoominColumn',
			'width' => 16,
			'noSort' => true,
	);
	
	$tableHeader[] = array(
			'name' => t('Picture'),
			'dbfield' => 'thumbnail',
			'width' => 80,
			'noSort' => true
	);
	
	$tableHeader[] = array(
			'name' => t('Name'),
			'dbfield' => 'area_name',
// 			'dbExactField' => 'name',
			'width' => 310,
	);
	
	$tableHeader[] = array(
			'name' => t('Altitude [m]'),
			'dbfield' => 'altitude',
// 			'dbExactField' => 'altitude',
			'width' => 80
	);
	
	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
// 			'dbExactField' => 'township',
			'width' => 150
	);
	
	$tableHeader[] = array(
			'name' => t('Canton'),
			'dbfield' => 'canton',
// 			'dbExactField' => 'canton',
			'width' => 40
	);
	
	$tableHeader[] = array(
			'name' => t('Country'),
			'dbfield' => 'country',
// 			'dbExactField' => 'country',
			'width' => 180
	);
	
	$tableHeader[] = array(
			'name' => t('Date'),
			'dbfield' => 'date',
			'width' => 120
	);
	
	$tableHeader[] = array(
			'name' => t('User'),
			'dbfield' => 'username',
// 			'dbExactField' => 'username',
			'width' => 120
	);
	
	$tableHeader[] = array(
			'name' => t('Type'),
			'dbfield' => 'geometry_type',
// 			'dbExactField' => 'geometry_type',
			'width' => 120
	);
	
	if ($init_sort && $init_sort2) {
		$tableHeader[$init_sort--]['initSort'] = true;
		$tableHeader[$init_sort--]['sortOrder'] = $init_sort2;
	}
	
	//apply user settings
	datatable_apply_user_settings(DATATABLE_AREA, $tableHeader);
	
	return $tableHeader;
}

/**
 * Returns all areas on which the current user has 'ACL_RED_WRITE' permission.
 * (must have red_write_access, and must not be a subarea)
 * 
 * @param $excluded_areas Areas which should not be returned in the result array
 */
function area_get_red_writeable_areas($exclude_areas, $json = true) {
	global $user;
	$minLevelArea = variable_get('ACL_RED_WRITE');
	$sql =   'SELECT DISTINCT a.id as area_id, a.name as areaname, a.acl_id as area_acl_id
			FROM {area} a
			WHERE a.parent_id IS NULL';
	
	if(!empty($exclude_areas)) {
		$exclude = ' AND (';
		foreach ($exclude_areas as $excluded_area) {
			$exclude .= 'a.id !='.$excluded_area . ' OR ';
		}
		$exclude = substr($exclude, 0, -4);
		$exclude .= ')';
		$sql .= $exclude;		
	}
	
	if (!check_admin_permission($user->uid)) {
		$sql .= ' LEFT JOIN {acl_sgroup} ag_a ON ag_a.acl_id = a.acl_id
			LEFT JOIN {sgroup} g_a ON g_a.id = ag_a.sgroup_id
			LEFT JOIN {acl_users} au_a ON (au_a.acl_id = a.acl_id OR au_a.acl_id = g_a.acl_id) AND au_a.users_id = ' . $user->uid.'
			AND ( (ag_a.sgroup_id = ' . variable_get('ACL_GROUP_ALL_ID').'
				AND ag_a.level >= ' . $minLevelArea . ') OR (au_a.acl_id = g_a.acl_id AND ag_a.level >= ' . $minLevelArea . ')
				OR (au_a.acl_id = a.acl_id AND au_a.level >= ' . $minLevelArea . ') )';
	}
	
	$sql .= ' ORDER BY a.name';
	$entries = db_query($sql)->fetchAll();
	
	$areas = array();
	$areasOptions = array();
	for($i = 0; $i < count((array)$entries); $i++) {
		$entry = (array)$entries[$i];
		if(!key_exists($entry['area_id'], $areas)) {
			//if area is not yet in areas array, move it there together with the inventory entry
			$areas[$entry['area_id']] = array('name' => $entry['areaname']);
			$areasOptions[$entry['area_id']] = $entry['areaname'];
		}
	}
	$output = array('count' => count($areas), 'areas' => $areas, 'areas_as_options' => $areasOptions);
	if($json) return drupal_json_output($output);
	return $output;
}
