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
	return drupal_get_form('observation_form', $observationdata);
// 	return observation_form($observationdata);
}

/**
 * Return all observations that match the given organism ID.
 * 
 * @param string $organism_id
 * @return array
 */
function observation_get_by_organism($organism_id = NULL) {
	$sql = 'SELECT * FROM {observation} o
				WHERE o.organism_id=:organism_id';
	$result = db_query(
			$sql,
			array('organism_id' => $organism_id))->fetchAll();
	return $result;
}

/**
 * Return all observations that match the given artgroup ID.
 *
 * @param string $artgoup_id
 * @return array
 */
function observation_get_by_artgroup($artgoup_id = NULL) {
	$sql = 'SELECT * FROM {observation} o
				WHERE o.organism_artgroup_id=:artgoup_id';
	$result = db_query(
			$sql,
			array('artgoup_id' => $artgoup_id))->fetchAll();
	return $result;
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
		$result['observation']['organism_artgroup_detmethod_name'] = t(organism_artgroup_get_detmethod(
			$result['observation']['organism_artgroup_detmethod_id'],
			true));
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
		$result['organism'] = organism_get($result['observation']['organism_id']);
		
		$attributes = db_query('SELECT * FROM {observation_attribute} oa WHERE oa.observation_id=:observation_id',
			array(':observation_id' => $observation_id))
			->fetchAllAssoc('organism_artgroup_attr_id');
		$result['attributes'] = array();
		// artgroup attributes
		$attributes_vis = ($result['observation']['organism_artgroup_id'] != '')
			? organism_artgroup_get_attributes_ids($result['observation']['organism_artgroup_id'])
			: array();
		foreach ($attributes_vis as $attribute_vis)
			$result['attributes']['artgroup_attr_'.$attribute_vis] = array(
				'attribute_name' => t(organism_artgroup_get_attribute($attribute_vis, true)),
				'value_id' => @$attributes[$attribute_vis]->organism_artgroup_attr_values_id,
				'value_id_name' => organism_artgroup_get_attributes_value_name(@$attributes[$attribute_vis]->organism_artgroup_attr_values_id),
				'value' => @$attributes[$attribute_vis]->value
			);

		// observation attributes
		foreach ($attributes as $attribute) {		
			$result['attributes']['observation_attr_'.$attribute->id] = array(
				'attribute_name' => t(organism_artgroup_get_attribute($attribute->organism_artgroup_attr_id, true)),
				'value_id' => $attribute->organism_artgroup_attr_values_id,
				'value_id_name' => organism_artgroup_get_attributes_value_name($attribute->organism_artgroup_attr_values_id),
				'value' => $attribute->value
			);
		}
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
	$arguments = array();
	
	$sql['dbColumns'] = array(
		'o.id',
		'o.date',
		'o.area_geometry_id',
		'o.acl_id',
		'a_s.altitude',
		'a_s.zip',
		'a_s.township',
		'a_s.canton',
		'a_s.country',
		// either POINT, POLYGON or LINESTRING
		'GeometryType(a_s.geom) AS geometry_type',
		// Center of the geometry - CH1903, x value
		'ST_X(ST_Centroid(st_transform(a_s.geom, 21781))) AS ch1903_center_x',
		// Center of the geometry - CH1903, y value
		'ST_Y(ST_Centroid(st_transform(a_s.geom, 21781))) AS ch1903_center_y',
		// Center of the geometry - WGS84, latitude
		'ST_X(ST_Centroid(st_transform(a_s.geom, 4326))) AS wgs84_center_lat',
		// Center of the geometry - WGS84
		'ST_Y(ST_Centroid(st_transform(a_s.geom, 4326))) AS wgs84_center_lng',
		// Coordinates of the geometry - CH1903
		'st_astext(st_transform(a_s.geom, 21781)) AS ch1903_coordinates',
		// Coordinates of the geometry - WGS84
		'st_astext(st_transform(a_s.geom, 4326)) AS wgs84_coordinates',
		// Area surface in m^2
		'st_area(st_transform(a_s.geom, 21781)) AS surface_area',
		// Pathlength (if linestring) in meters
		'ST_Length(st_transform(a_s.geom, 21781)) AS pathlength',
		// Geometry as JSON
		'ST_AsGeoJSON(a_s.geom) AS wgs84_geom_json');
	
	//select also area geometry, if observation belongs to an inventory
	if(isset($observation_data['observation']['inventory_id']) && $observation_data['observation']['inventory_id'] > 0) {
		$area_geometry = array(
		'a.id AS area_id',
		'area_s.altitude AS area_altitude',
		'area_s.zip AS area_zip',
		'area_s.township AS area_township',
		'area_s.canton AS area_canton',
		'area_s.country AS area_country',
		// either POINT, POLYGON or LINESTRING
		'GeometryType(area_s.geom) AS area_geometry_type',
		// Center of the geometry - CH1903, x value
		'ST_X(ST_Centroid(st_transform(area_s.geom, 21781))) AS area_ch1903_center_x',
		// Center of the geometry - CH1903, y value
		'ST_Y(ST_Centroid(st_transform(area_s.geom, 21781))) AS area_ch1903_center_y',
		// Center of the geometry - WGS84, latitude
		'ST_X(ST_Centroid(st_transform(area_s.geom, 4326))) AS area_wgs84_center_lat',
		// Center of the geometry - WGS84
		'ST_Y(ST_Centroid(st_transform(area_s.geom, 4326))) AS area_wgs84_center_lng',
		// Coordinates of the geometry - CH1903
		'st_astext(st_transform(area_s.geom, 21781)) AS area_ch1903_coordinates',
		// Coordinates of the geometry - WGS84
		'st_astext(st_transform(area_s.geom, 4326)) AS area_wgs84_coordinates',
		// Area surface in m^2
		'st_area(st_transform(area_s.geom, 21781)) AS area_surface_area',
		// Pathlength (if linestring) in meters
		'ST_Length(st_transform(area_s.geom, 21781)) AS area_pathlength',
		// Geometry as JSON
		'ST_AsGeoJSON(area_s.geom) AS area_wgs84_geom_json');
		
		//merge arrays
		$sql['dbColumns'] = array_merge($sql['dbColumns'], $area_geometry);
	}
	$sql['dbTable'] = '{observation} o';
	
	$sql['dbJoins'] = 'JOIN {area_geometry} a_s ON a_s.id=o.area_geometry_id';
	
	$sql['dbSearchColumns'] = '';
	
	if ($observation_data) {
		// single observation
		if (isset($observation_data['observation'])) {
			if(isset($observation_data['observation']['inventory_id']) && $observation_data['observation']['inventory_id'] > 0) {
				$inventory = inventory_get($observation_data['observation']['inventory_id']);
				$area = area_get_full_dataset_of_specific_area($inventory['area_id']);
				
				$sql['dbJoins'] .= ' JOIN {inventory} i ON i.id = o.inventory_id 
								  JOIN {area} a ON a.id = i.area_id
								  JOIN {area_geometry} area_s ON area_s.id=a.area_geometry_id';
				$sql['dbWhere'] = 'a.id=:area_id';
				$arguments[':area_id'] = $area['id'];
			}
			$sql['dbWhere'] = empty($sql['dbWhere'])
			? '('
					: $sql['dbWhere'] . ' AND (';
			$sql['dbWhere'] .= 'o.id=:observation_id )';
			$arguments[':observation_id'] = $observation_data['observation']['id'];
		}
		// array of observations
		else {
			$sql['dbWhere'] = '';
			foreach ($observation_data as $observation_id) {
				$sql['dbWhere'] .= 'o.id=:observation_id_' . $observation_id . ' OR ';
				$arguments[':observation_id_' . $observation_id] = $observation_id;
			}
			$sql['dbWhere'] = substr($sql['dbWhere'], 0, -4);
		}
	}
	
	$sql['id'] = 'id';
	
	$sql['acl_level'] = 'ACL_READ';
	
	if (!isset($_REQUEST['rp']))
		$_REQUEST['rp'] = '18446744073709551615';  // max limit for sql query, select all (2^64-1) rows
	
	/*
	 * fetch the data, acl checked
	 */
	$results = datatable_dbRequest($sql, $arguments);
	
	$items = array();
	$area_items = array();
	foreach ($results['rows'] as $result) {
		$item = array();
		$item['id'] = (int) $result['cell']['id'];
		$item['altitude'] = (int) $result['cell']['altitude'];
		$item['township'] = isset($result['cell']['township']) ? $result['cell']['township']
				: '';
		$item['canton'] = isset($result['cell']['canton']) ? $result['cell']['canton'] : '';
		$item['country'] = isset($result['cell']['country']) ? $result['cell']['country']
				: '';
		$item['comment'] = isset($result['cell']['comment']) ? $result['cell']['comment']
				: '';
		
		$item['type'] = '';
		$item['entity'] = 'observation';
		$item['area_points'] = array();
		$json = json_decode($result['cell']['wgs84_geom_json']);
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
	
	if(isset($observation_data['observation']['inventory_id']) && $observation_data['observation']['inventory_id'] > 0) {
		foreach ($results['rows'] as $result) {
			$item = array();
			$item['id'] = (int) $result['cell']['area_id'];
			$item['altitude'] = (int) $result['cell']['area_altitude'];
			$item['township'] = isset($result['cell']['area_township']) ? $result['cell']['area_township']
			: '';
			$item['canton'] = isset($result['cell']['area_canton']) ? $result['cell']['area_canton'] : '';
			$item['country'] = isset($result['cell']['area_country']) ? $result['cell']['area_country']
			: '';
			$item['comment'] = isset($result['cell']['area_comment']) ? $result['cell']['area_comment']
			: '';
			
			$item['type'] = '';
			$item['entity'] = 'area';
			$item['area_points'] = array();
			//add area geometry to index $item['area_area_points']
			$json = json_decode($result['cell']['area_wgs84_geom_json']);
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
			$area_items[] = $item;
		}
		$items = array_merge($items, $area_items);
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

	if($observationdata['observation']['organism_id'] == UNKNOWN_SPECIES) {
		$rows[] = array(
				array(
						'data' => t('Common name'),
						'header' => true
				),
				t('Unknown species'),
		);
	}
	else if (!empty($observationdata['organism']['name_lang'])) {
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
				l(
					$observationdata['organism']['name_lat'],
					base_path() . "organism/" . $observationdata['observation']['organism_id'],
					array(
					'external' => true
				))
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
				date('d.m.Y H:i:s', $observationdata['observation']['date']),
		);
	}

	if (!empty($observationdata['observation']['m_time']) && !$simple) {
		$rows[] = array(
				array(
						'data' => t('Last modification'),
						'header' => true
				),
				date('d.m.Y H:i:s', $observationdata['observation']['m_time'])
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
		if(!empty($attribute['value']) || !empty($attribute['value_id_name'])) {
			$rows[] = array(
					array(
							'data' => t($attribute['attribute_name']),
							'header' => true
					),
					($attribute['value_id'] == '') ? $attribute['value']
							: t($attribute['value_id_name']),
			);
		}
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
	return ($result == 1);
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
* Return all or a single observation attribute as an array or as html tags
* @param int $observation_id
* @param boolean $as_html
* @param string $html_before_desc
* @param string $html_after_desc
* @param string $html_before_form
* @param string $html_after_form
* @return array OR string
*/
function observation_get_attributes(
		$observation_id = false,
		$artgroup_id) {
	global $user;
	global $language;
	
	$langcode = (isset($user->language) && !empty($user->language))
	? $user->language
	: $language->language;
	
	$sql_select = 'SELECT oaa.id id, oaa.name, oaat.name type_name, oaat.format type_format';
	$sql_from = ' FROM {organism_artgroup_attr} oaa';
	$sql_join = ' LEFT JOIN {organism_artgroup_attr_type} oaat ON oaat.id=oaa.organism_artgroup_attr_type_id';
	$sql_where = '';
	
	if ($observation_id > 0) {
		$sql_select .= ", oa.id attribute_id,  COALESCE(to_char(oa.organism_artgroup_attr_values_id, '9999'), oa.value) AS value";
		$sql_join .= ' LEFT JOIN {observation_attribute} oa ON oa.organism_artgroup_attr_id=oaa.id AND oa.observation_id=:observation_id'.
					 ' LEFT JOIN {organism_artgroup_attr_subscription} oaas ON oaa.id = oaas.organism_artgroup_attr_id';
		$sql_where .= ' WHERE oaas.organism_artgroup_id =:artgroup_id OR oaas.organism_artgroup_id = 0';
		$attributes_results = db_query($sql_select . ' ' . $sql_from . ' ' . $sql_join . ' ' . $sql_where,
			array(':observation_id' => $observation_id, ':artgroup_id' => $artgroup_id));
	}
	else
	{
		$sql_join .= ' LEFT JOIN {organism_artgroup_attr_subscription} oaas ON oaa.id = oaas.organism_artgroup_attr_id';
		$sql_where .= ' WHERE oaas.organism_artgroup_id =:artgroup_id OR oaas.organism_artgroup_id = 0';
		$attributes_results = db_query($sql_select . ' ' . $sql_from . ' ' . $sql_join . ' ' . $sql_where,
			array(':artgroup_id' => $artgroup_id)
		);
	}

	$attributes_with_values = array();
	if($langcode == 'de') $field = 'value';
	else $field = 'value_'.$langcode;
	
	foreach ($attributes_results as $attributes_result) {
		$options = array();
		if($attributes_result->type_format == 'select') {
		//fetch option values for select
		$query = db_select('organism_artgroup_attr_values', 'oaav')
				->fields('oaav')
				->condition('oaav.organism_artgroup_attr_id', $attributes_result->id)
				->orderBy('id')
				->execute();
			$tmp = $query->fetchAll(PDO::FETCH_ASSOC);
			foreach($tmp as $option) {
				$options[$option['id']] = $option[$field];
			}
		}
		$attributes_with_values[] = array(
			'name' => $attributes_result->name,
			'id' => $attributes_result->id,
			'type_name' => $attributes_result->type_name,
			'type_format' => $attributes_result->type_format,
			'type_drupal' => $attributes_result->type_format,
			'value' => ($observation_id > 0) ? $attributes_result->value : '',
			'options' => ($attributes_result->type_format == 'select') ? $options : ''
		);
	}
	
	return $attributes_with_values;

}

/**
* parse a given array of attributes to html form elements
* @param array $form
* @param string $html_before_desc
* @param string $html_after_desc
* @param string $html_before_form
* @param string $html_after_form
* @return string
*/
function observation_parse_html_forms($form, $html_before_desc = false,
$html_after_desc = false, $html_before_form = false, $html_after_form = false) {
	$output = "";
	$predefinied_value = $form['values'];
	$name_trimd = str_replace(' ', '', $form['name']);
	$form_name = 'id="attributes_' . $name_trimd . '" name="attributes[' . $form['id'] . ']"';
	if ($html_before_desc) {
		$output = $html_before_desc;
		$output = str_replace('#id#', 'attributes_tr_' . $form['id'], $output);
		$output = str_replace('#name#', $form['name'], $output);
	}
	if ($html_after_desc)
	$output .= $html_after_desc;
	if ($html_before_form)
	$output .= $html_before_form;

	switch ($form['type_format']) {

		case 'string':
			$output .= '<input type="text" ' . $form_name . ' value="' . $predefinied_value . '">';
			break;

		case 'dropdown':
			$output .= '
			<select ' . $form_name . '><option value="0">' . t('Not defined') . '</option>';
			foreach ($form['values'] as $key => $value) {
				$selected = '';
				if (isset($predefinied_value_id) && $predefinied_value_id == $key)
				$selected = ' selected="selected"';
				$output .= '<option value="' . $key . '" ' . $selected . '>' . $value . '</option>';
			}
			$output .= '</select>';
			break;

		case 'date':
			$output .= '<input type="date" ' . $form_name . ' value="' . $predefinied_value . '">';
			break;

		case 'int':
			$output .= '<input type="number" ' . $form_name . ' value="' . $predefinied_value . '">';
			break;

		case 'textarea':
			$output .= '<textarea ' . $form_name . ' ">' . $predefinied_value . '</textarea>';
			break;

	}

	if ($html_after_form)
	$output .= $html_after_form;
	return $output;
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

	global $base_url;
	$iconBaseUrl = $base_url . '/' . path_to_theme() . '/images/icons/';
	
	$tableHeader[] = array(
		'name' => '<input class="gridSelect" type="checkbox" disabled />',
		'dbfield' => 'selectColumn',
		'width' => 20,			
		'noSort' => true
	);
	
	$tableHeader[] = array(
		'name' => '<img src="' . $iconBaseUrl . 'disabled/List.png" />',
		'dbfield' => 'showColumn',
		'width' => 16,
		'noSort' => true
	);

	$tableHeader[] = array(
		'name' => '<img src="' . $iconBaseUrl . 'disabled/Modify.png" />',
		'dbfield' => 'editColumn',
		'width' => 16,
		'noSort' => true
	);
		
	$tableHeader[] = array(
		'name' => '<img src="' . $iconBaseUrl . 'disabled/Zoom in.png" />',
		'dbfield' => 'zoominColumn',
		'width' => 16,
		'noSort' => true
	);

	$tableHeader[] = array(
			'name' => t('Picture'),
			'dbfield' => 'thumbnail',
			'width' => 80,
			'noSort' => true
	);

	$tableHeader[] = array(
			'name' => t('Common name'),
			'dbfield' => 'organism_l',
			'width' => 250,
			'dbExactField' => 'ol.name'
	);

	$tableHeader[] = array(
			'name' => t('Scientific name'),
			'dbfield' => 'organism_sc',
			'width' => 250,
			'dbExactField' => 'scientific_name'
	);
	
	$tableHeader[] = array(
			'name' => t('Artgroup'),
			'dbfield' => 'artgroup_n',
			'width' => 100,
			'dbExactField' => 'oa.name',
/*			'hide' => true					// Albert will nun alle Spalten standardmässig anzeigen lassen (19. April 2013) */
	);

	$tableHeader[] = array(
			'name' => t('Date'),
			'dbfield' => 'date',
			'width' => 120
	);
	
	$tableHeader[] = array(
			'name' => t('Inventory'),
			'dbfield' => 'inventory',
			'dbExactField' => 'i.name',
			'width' => 90
	);
	
	$tableHeader[] = array(
			'name' => t('Area'),
			'dbfield' => 'area',
			'dbExactField' => 'a.name',
			'width' => 90
	);
	
	$tableHeader[] = array(
			'name' => t('Area Type'),
			'dbfield' => 'areatype',
			'dbExactField' => 'a_g.geometry_type',
			'width' => 90
	);
	
	$tableHeader[] = array(
			'name' => t('X Coordinate'),
			'dbfield' => 'ch1903_center_x',
			'width' => 90,
//			'hide' => true
	);
	
	$tableHeader[] = array(
			'name' => t('Y Coordinate'),
			'dbfield' => 'ch1903_center_y',
			'width' => 90,
//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Altitude [m]'),
			'dbfield' => 'altitude',
			'width' => 60,
			'dbExactField' => 'a_s.altitude',
//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
			'width' => 120,
			'dbExactField' => 'a_s.township',
//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Canton'),
			'dbfield' => 'canton',
			'width' => 40,
			'dbExactField' => 'a_s.canton',
//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Country'),
			'dbfield' => 'country',
			'width' => 120,
			'dbExactField' => 'a_s.country',
//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Observer'),
			'dbfield' => 'observer',
			'width' => 100,
			'dbExactField' => 'o.observer',
			'exportAlways' => true,
//			'hide' => true
	);
	
	$tableHeader[] = array(
			'name' => t('Comment'),
			'dbfield' => 'comment',
			'width' => 200,
			'dbExactField' => 'oattr.value',
//			'hide' => true,
	);

	$tableHeader[] = array(
			'name' => t('Number of pictures'),
			'dbfield' => 'imagescount',
			'width' => 20,
//			'hide' => true,
	);

	$tableHeader[] = array(
			'name' => t('Number of videos'),
			'dbfield' => 'videoscount',
			'width' => 20,
//			'hide' => true,
	);

	$tableHeader[] = array(
			'name' => t('Number of sounds'),
			'dbfield' => 'soundscount',
			'width' => 20,
//			'hide' => true,
			
	);

	$tableHeader[] = array(
			'name' => t('Number of documents'),
			'dbfield' => 'documentscount',
			'width' => 20,
//			'hide' => true,
	);

	if ($init_sort && $init_sort2) {
		$tableHeader[$init_sort--]['initSort'] = true;
		$tableHeader[$init_sort--]['sortOrder'] = $init_sort2;
	}
	return $tableHeader;
}

/**
 * Return the observations standard table headers without species related filter items
 * @param int $init_sort
 * 				which header in the array should be the initsort array[$init_sort]
 * @param String $init_sort2
 * 				Sort directions ex. 'asc' or 'desc'
 * @return array
 */
function observation_get_organism_datatable_headers($init_sort = false,
		$init_sort2 = false) {

	$tableHeader[] = array(
			'name' => t('Internal Id'),
			'idField' => true,
			'dbfield' => 'id',
			'hide' => true,
			'width' => 30
	);

	global $base_url;
	$iconBaseUrl = $base_url . '/' . path_to_theme() . '/images/icons/';

	$tableHeader[] = array(
			'name' => '<input class="gridSelect" type="checkbox" disabled />',
			'dbfield' => 'selectColumn',
			'width' => 20,
			'noSort' => true
	);

	$tableHeader[] = array(
			'name' => '<img src="' . $iconBaseUrl . 'disabled/Zoom.png" />',
			'dbfield' => 'showColumn',
			'width' => 16,
			'noSort' => true
	);

	$tableHeader[] = array(
			'name' => '<img src="' . $iconBaseUrl . 'disabled/Modify.png" />',
			'dbfield' => 'editColumn',
			'width' => 16,
			'noSort' => true
	);

	$tableHeader[] = array(
			'name' => '<img src="' . $iconBaseUrl . 'disabled/Zoom in.png" />',
			'dbfield' => 'zoominColumn',
			'width' => 16,
			'noSort' => true
	);

	$tableHeader[] = array(
			'name' => t('Picture'),
			'dbfield' => 'thumbnail',
			'width' => 80,
			'noSort' => true
	);

	$tableHeader[] = array(
			'name' => t('Common name'),
			'dbfield' => 'organism_l',
			'width' => 250
	);

	$tableHeader[] = array(
			'name' => t('Scientific name'),
			'dbfield' => 'organism_sc',
			'width' => 250
	);

	$tableHeader[] = array(
			'name' => t('Artgroup'),
			'dbfield' => 'artgroup_n',
			'width' => 100
			/*			'hide' => true					// Albert will nun alle Spalten standardmässig anzeigen lassen (19. April 2013) */
	);

	$tableHeader[] = array(
			'name' => t('Date'),
			'dbfield' => 'date',
			'width' => 120
	);

	$tableHeader[] = array(
			'name' => t('Inventory'),
			'dbfield' => 'inventory',
			'dbExactField' => 'i.name',
			'width' => 90
	);

	$tableHeader[] = array(
			'name' => t('Area'),
			'dbfield' => 'area',
			'dbExactField' => 'a.name',
			'width' => 90
	);

	$tableHeader[] = array(
			'name' => t('Area Type'),
			'dbfield' => 'areatype',
			'dbExactField' => 'a_g.geometry_type',
			'width' => 90
	);

	$tableHeader[] = array(
			'name' => t('X Coordinate'),
			'dbfield' => 'ch1903_center_x',
			'width' => 90,
			//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Y Coordinate'),
			'dbfield' => 'ch1903_center_y',
			'width' => 90,
			//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Altitude [m]'),
			'dbfield' => 'altitude',
			'width' => 60,
			'dbExactField' => 'a_s.altitude',
			//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
			'width' => 120,
			'dbExactField' => 'a_s.township',
			//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Canton'),
			'dbfield' => 'canton',
			'width' => 40,
			'dbExactField' => 'a_s.canton',
			//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Country'),
			'dbfield' => 'country',
			'width' => 120,
			'dbExactField' => 'a_s.country',
			//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Observer'),
			'dbfield' => 'observer',
			'width' => 100,
			'dbExactField' => 'o.observer',
			'exportAlways' => true,
			//			'hide' => true
	);

	$tableHeader[] = array(
			'name' => t('Comment'),
			'dbfield' => 'comment',
			'width' => 200,
			'dbExactField' => 'oattr.value',
			//			'hide' => true,
	);

	$tableHeader[] = array(
			'name' => t('Number of pictures'),
			'dbfield' => 'imagescount',
			'width' => 20,
			//			'hide' => true,
	);

	$tableHeader[] = array(
			'name' => t('Number of videos'),
			'dbfield' => 'videoscount',
			'width' => 20,
			//			'hide' => true,
	);

	$tableHeader[] = array(
			'name' => t('Number of sounds'),
			'dbfield' => 'soundscount',
			'width' => 20,
			//			'hide' => true,
				
	);

	$tableHeader[] = array(
			'name' => t('Number of documents'),
			'dbfield' => 'documentscount',
			'width' => 20,
			//			'hide' => true,
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
function observation_new($inventory_id = NULL) {
	return drupal_get_form('observation_form', false, $inventory_id);
// 	return observation_form();
}

/**
 * Return the number of observations for the given organism.
 * 
 * @param int $organism_id
 */
function get_number_of_observations_for_organism($organism_id) {
	$result = db_query("SELECT * FROM {observation} WHERE organism_id=:organism_id", array(':organism_id' => $organism_id));
	return $result->rowCount();
}

?>