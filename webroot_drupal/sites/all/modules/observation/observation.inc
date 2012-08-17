<?php
/**
 * @file observation.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'observation', 'observation.template');
module_load_include('inc', 'organism', 'organism.artgroup');
module_load_include('inc', 'organism', 'organism');
module_load_include('inc', 'area', 'area');

/**
 * Return the single observation form to add a observation
 * @param array $observationdata
 */
function observation_edit($observationdata = NULL) {
	return observation_form($observationdata);
}

/**
 * Return a array with all data for a observation
 * @param int $observation_id
 * @return array
 */
function observation_get($observation_id) {
	$observation_id = db_escape_field($observation_id);
	if ($observation_id > 0) {
		$sql = 'SELECT * FROM {observation} o
				WHERE o.id=:observation_id';
		$result['observation'] = db_query(
			$sql,
			array('observation_id' => $observation_id))->fetchAssoc();
		// If no artgroup, try to find a artgroup
		if ($result['observation']['organism_artgroup_id'] == 0) {
			$artgroups = organism_artgroup_get_organsim_in_artgroup(
				$result['observation']['organism_id']);
			foreach ($artgroups as $artgroup) {
				$result['observation']['organism_artgroup_id'] = $artgroup;
			}
		}
		$result['observation']['organism_artgroup_detmethod_name'] = organism_artgroup_get_detmethod(
			$result['observation']['organism_artgroup_detmethod_id'],
			true);
		$result['observation']['organism_artgroup_name'] = organism_artgroup_get_artgroup(
			$result['observation']['organism_artgroup_id'],
			true);
		$result['detmethods_vis'] = ($result['observation']['organism_artgroup_id']
				!= '')
				? organism_artgroup_get_detmethods_ids(
					$result['observation']['organism_artgroup_id']) : array();
		$result['geometry'] = observation_get_geometry(
			$result['observation']['area_geometry_id']);
		($result['observation']['found_as_lang'] == 1)
				? $result['observation']['found_as_lang'] = 'true'
				: $result['observation']['found_as_lang'] = 'false';
		($result['observation']['found_as_lat'] == 1)
				? $result['observation']['found_as_lat'] = 'true'
				: $result['observation']['found_as_lat'] = 'false';

		/**
		 * cheap hack to work with the very special weird
		 * 'json' format from area, don't ask and don't change it!
		 */
		$tmp = (json_decode($result['geometry']['wgs84_geom_json'])->coordinates);
		$result['geometry']['coordinatejson'][0][0] = $tmp[1];
		$result['geometry']['coordinatejson'][0][1] = $tmp[0];
		/**
		 * hack end
		 */

		$result['organism'] = organism_get(
			$result['observation']['organism_id']);
		$attributes = db_query(
			'SELECT * FROM {observation_attribute} oa WHERE oa.observation_id=:observation_id',
			array(':observation_id' => $observation_id))->fetchAllAssoc(
				'organism_artgroup_attr_id');
		$attributes_vis = ($result['observation']['organism_artgroup_id'] != '')
				? organism_artgroup_get_attributes_ids(
					$result['observation']['organism_artgroup_id']) : array();
		$result['attributes'] = array();
		foreach ($attributes_vis as $attribute_vis)
			$result['attributes'][$attribute_vis] = array(
					'attribute_name' => organism_artgroup_get_attribute(
						$attribute_vis,
						true),
					'value_id' => @$attributes[$attribute_vis]->organism_artgroup_attr_values_id,
					'value_id_name' => organism_artgroup_get_attributes_value_name(
						@$attributes[$attribute_vis]->organism_artgroup_attr_values_id),
					'value' => @$attributes[$attribute_vis]->value
			);

		return $result;
	}
	return false;
}

/**
 * Return geometries of all observations or for a single
 * acl checked
 * @param array $observation_data
 * @return array
 */
function observation_get_geometries_acl_checked($observation_data = false) {
	$filter = array();
	$where = '';
	if ($observation_data) {
		$filter[':observation_id'] = $observation_data['observation']['id'];
		$where = ' WHERE o.id=:observation_id ';
	}
	$sql = 'SELECT o.id, o.area_geometry_id, o.acl_id,
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
				ST_AsGeoJSON(a_s.geom) AS wgs84_geom_json
	FROM
		{observation} o
		JOIN {area_geometry} a_s ON a_s.id=o.area_geometry_id' . $where;
	$results = db_query($sql, $filter)->fetchAllAssoc('id', PDO::FETCH_ASSOC);
	$items = array();
	// 		die(print_r($results));

	foreach ($results as $result) {
		$tmp['observation'] = $result;
		if (observation_read_access($tmp) == true) {
			$item = array();
			$item['id'] = (int) $result['id'];
			$item['altitude'] = (int) $result['altitude'];
			$item['township'] = isset($result['township']) ? $result['township']
					: '';
			$item['canton'] = isset($result['canton']) ? $result['canton'] : '';
			$item['country'] = isset($result['country']) ? $result['country']
					: '';
			$item['comment'] = isset($result['comment']) ? $result['comment']
					: '';

			$item['type'] = '';
			$item['area_points'] = array();
			$json = json_decode($result['wgs84_geom_json']);
			if ($json) {
				if ($json->type == 'Point') {
					$item['type'] = 'marker';
					$item['area_points'][] = array(
							'lat' => $json->coordinates[1],
							'lng' => $json->coordinates[0]
					);
				}
			}
			$items[] = $item;
		}
	}
	return $items;

}

/**
 * Return the geometry array for a observation
 * @param int $geometry_id
 * @return array
 */
function observation_get_geometry($geometry_id) {
	$sql = 'SELECT
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
				ST_AsGeoJSON(a_s.geom) AS wgs84_geom_json
	FROM {area_geometry} a_s WHERE a_s.id=:geometry_id';
	return db_query($sql, array(':geometry_id' => $geometry_id))->fetchAssoc();
}

/**
 * Return the acl permission form for a observation
 * @param array $observation
 */
function observation_edit_acl($observation = NULL) {
	return observation_acl_template($observation);
}

/**
 * Return a formated table with the observation details
 * @param array $observationdata
 * @param boolean $simple
 *  Return just a few information
 */
function observation_get_table($observationdata = NULL, $simple = false) {
	if ($observationdata == NULL || !is_array($observationdata)) {
		return false;
	}
	$rows = array();

	if (!empty($observationdata['organism']['name_lang'])) {
		$rows[] = array(
				array(
						'data' => t('Common name'),
						'header' => true
				),
				$observationdata['organism']['name_lang'],
		);
	}

	if (!empty($observationdata['organism']['name_lat'])) {
		$rows[] = array(
				array(
						'data' => t('Scientific name'),
						'header' => true
				),
				$observationdata['organism']['name_lat'],
		);
	}

	if (!empty($observationdata['observation']['organism_artgroup_name'])
			&& !$simple) {
		$rows[] = array(
				array(
						'data' => t('Artgroup'),
						'header' => true
				),
				t($observationdata['observation']['organism_artgroup_name']),
		);
	}

	if (!empty($observationdata['observation']['observer']) && !$simple) {
		$rows[] = array(
				array(
						'data' => t('Observer'),
						'header' => true
				),
				l(
						check_plain($observationdata['observation']['observer']),
						base_path() . "user/" . $observationdata['observation']['users_uid'],
						array(
								'external' => true
						))
		);
	}

	if (!empty($observationdata['observation']['date'])) {
		$rows[] = array(
				array(
						'data' => t('Date'),
						'header' => true
				),
				date('j.m.Y', $observationdata['observation']['date']),
		);
	}

	if (!empty($observationdata['observation']['m_time']) && !$simple) {
		$rows[] = array(
				array(
						'data' => t('Last modification'),
						'header' => true
				),
				date('j.m.Y', $observationdata['observation']['m_time'])
		);
	}

	if (!empty($observationdata['observation']['count'])) {
		$rows[] = array(
				array(
						'data' => t('Count'),
						'header' => true
				),
				$observationdata['observation']['count']
		);
	}

	/* add the table with the data prepared above */
	$form = array(
			'#theme' => 'table',
			'#rows' => $rows,
			'#id_table' => 'observation_table_wd',
	);

	return $form;
}

/**
 * Return a formated table with the observation details
 * @param array $observationdata
 */
function observation_get_details_table($observationdata = NULL) {
	if ($observationdata == NULL || !is_array($observationdata))
		return false;
	$rows = array();

	// 	if (!empty($observationdata['observation']['organism_artgroup_detmethod_name'])) {
	$rows[] = array(
			array(
					'data' => t('Determination method'),
					'header' => true
			),
			($observationdata['observation']['organism_artgroup_detmethod_name']
					== '') ? t('Not defined')
					: t(
						$observationdata['observation']['organism_artgroup_detmethod_name']),
	);
	// 	}

	foreach ($observationdata['attributes'] as $attribute) {
		$rows[] = array(
				array(
						'data' => t($attribute['attribute_name']),
						'header' => true
				),
				($attribute['value_id'] == '') ? $attribute['value']
						: t($attribute['value_id_name']),
		);
	}

	/* add the table with the data prepared above */
	$form = array(
			'#theme' => 'table',
			'#rows' => $rows,
			'#id_table' => 'observation_table_wd',
	);

	return $form;
}

/**
 * Return the drupal table as array, with geometry details for the observation
 * @param array $observationdata
 * @return array
 */
function observation_get_geometry_table($observationdata = NULL) {
	/**
	 * Geometry Details
	 */
	if (!empty($observationdata['geometry']['township'])) {
		$rows[] = array(
				array(
						'data' => t('Township'),
						'header' => true
				),
				$observationdata['geometry']['township']
		);
	}
	if (!empty($observationdata['geometry']['zip'])) {
		$rows[] = array(
				array(
						'data' => t('ZIP'),
						'header' => true
				),
				$observationdata['geometry']['zip']
		);
	}
	if (!empty($observationdata['geometry']['canton'])) {
		$rows[] = array(
				array(
						'data' => t('Canton'),
						'header' => true
				),
				$observationdata['geometry']['canton']
		);
	}
	if (!empty($observationdata['geometry']['country'])) {
		$rows[] = array(
				array(
						'data' => t('Country'),
						'header' => true
				),
				$observationdata['geometry']['country']
		);
	}
	if (!empty($observationdata['geometry']['altitude'])) {
		$rows[] = array(
				array(
						'data' => t('Altitude [m.a.s.l.]'),
						'header' => true
				),
				$observationdata['geometry']['altitude']
		);
	}
	if ((!empty($observationdata['geometry']['wgs84_center_lat'])
			&& !empty($observationdata['geometry']['wgs84_center_lng'])
			&& !empty($observationdata['geometry']['ch1903_center_x'])
			&& !empty($observationdata['geometry']['ch1903_center_y']))) {
		$rows[] = array(
				array(
						'data' => t('Coordinates'),
						'header' => true
				),
				get_wgs84_coordinates_link($observationdata['geometry'])
						. '<br/>'
						. get_ch1903_coordinates_link(
							$observationdata['geometry'])
		);
	}

	/* add the table with the data prepared above */
	$form = array(
			'#theme' => 'table',
			'#rows' => $rows,
			'#id_table' => 'observation_table_wd',
	);

	return $form;
}

/**
 * save all attributes for a observation
 * @param array $attributes_checked
 * @param int $observation_id
 */
function observation_save_attribute_values($attributes_checked, $observation_id) {
	// save all attributes
	foreach ($attributes_checked as $key => $value) {
		$val = explode('-->', $value);
		$db_id = NULL;
		$db_val = NULL;
		if ($val[0] == 'db_id') {
			$db_id = $val[1];
		} else {
			$db_val = $value;
		}
		$entry = array(
				'observation_id' => $observation_id,
				'organism_artgroup_attr_id' => $key,
				'organism_artgroup_attr_values_id' => $db_id,
				'value' => $db_val,
		);
		if (observation_attribute_value_exist($observation_id, $key)) {
			observation_update_attribute_value($entry);
		} else {
			observation_add_attribute_value($entry);
		}

	}
}

/**
 * checks if a attribute for an observation already exists
 * @param int $observation_id
 * @param int $attr_id
 * @return boolean true on success|false on fails
 */
function observation_attribute_value_exist($observation_id, $attr_id) {
	$result = db_query(
		'SELECT * FROM {observation_attribute} oa
			WHERE oa.observation_id=:observation_id AND
			oa.organism_artgroup_attr_id=:attr_id',
		array(':observation_id' => $observation_id,
				':attr_id' => $attr_id
		))->rowCount();
	if ($result == 1)
		return true;
	return false;
}

/**
 * update a observation attribute
 * @param array $entry
 */
function observation_update_attribute_value($entry) {
	db_update('observation_attribute')->fields($entry)
		->condition('observation_id', $entry['observation_id'])
		->condition(
			'organism_artgroup_attr_id',
			$entry['organism_artgroup_attr_id'])
		->execute();
}

/**
 * add a observation attribute
 * @param array $entry
 */
function observation_add_attribute_value($entry) {
	db_insert('observation_attribute')->fields($entry)
		->execute();
}

/**
 * Return the observations standard table headers
 * @param int $init_sort
 * 				which header in the array should be the initsort array[$init_sort]
 * @param String $init_sort2
 * 				Sort directions ex. 'asc' or 'desc'
 * @return array
 */
function observation_get_standard_datatable_headers($init_sort = false,
		$init_sort2 = false) {
	$tableHeader[] = array(
			'name' => t('Internal Id'),
			'idField' => true,
			'dbfield' => 'id',
			'hide' => true,
			'width' => 30
	);

	$tableHeader[] = array(
			'name' => t('Picture'),
			'dbfield' => 'thumbnail',
			'width' => 80,
			'noSort' => true,
	);

	$tableHeader[] = array(
			'name' => t('Common name'),
			'dbfield' => 'organism_l',
			'width' => 330,
			'dbExactField' => 'ol.name',
	);

	$tableHeader[] = array(
			'name' => t('Scientific name'),
			'dbfield' => 'organism_sc',
			'width' => 330,
			'dbExactField' => 'osn.name',
	);

	$tableHeader[] = array(
			'name' => t('Date'),
			'dbfield' => 'date',
			'width' => 60,
	);

	$tableHeader[] = array(
			'name' => t('Altitude [m]'),
			'dbfield' => 'altitude',
			'width' => 60,
			'hide' => true,
			'dbExactField' => 'a_s.altitude',
	);

	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
			'width' => 150,
			'hide' => true,
			'dbExactField' => 'a_s.township',
	);

	$tableHeader[] = array(
			'name' => t('Canton'),
			'dbfield' => 'canton',
			'width' => 40,
			'hide' => true,
			'dbExactField' => 'a_s.canton',
	);

	$tableHeader[] = array(
			'name' => t('Country'),
			'dbfield' => 'country',
			'width' => 180,
			'hide' => true,
			'dbExactField' => 'a_s.country',
	);

	$tableHeader[] = array(
			'name' => t('Observer'),
			'dbfield' => 'observer',
			'width' => 120,
			'hide' => true,
			'dbExactField' => 'o.observer',
	);

	$tableHeader[] = array(
			'name' => t('Number of pictures'),
			'dbfield' => 'piccount',
			'width' => 20,
			'hide' => true,
			'noSort' => true,
	);

	if ($init_sort && $init_sort2) {
		$tableHeader[$init_sort--]['initSort'] = true;
		$tableHeader[$init_sort--]['sortOrder'] = $init_sort2;
	}
	return $tableHeader;
}

/**
 * Return the single observation form to add a observation
 */
function observation_new() {
	return observation_form();
}
