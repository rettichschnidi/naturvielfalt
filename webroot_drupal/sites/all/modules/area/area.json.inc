<?php
/**
 * @file area.json.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');

/**
 * Generate a JSON with all areas which are available to the user.
 * 
 * @param $acl_level Required ACL level (e.g. ACL_READ, ACL_WRITE, ACL_ADMIN, etc)
 * @todo Include the user's nickname
 */
function area_all_areas_datatable_json($acl_level = 'ACL_READ') {
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'a.id AS id',
			'a.acl_id AS acl_id',
			'a.name',
			'a.comment',
			'a_s.altitude',
			'a_s.township',
			'a_s.canton',
			'a_s.country',
			'ST_AsGeoJSON(a_s.geom) AS geom',
			'u.name AS username'
	);

	$sql['dbSearchColumns'] = array(
			'a.name',
			'a.comment',
			'a_s.altitude',
			'a_s.township',
			'a_s.canton',
			'a_s.country',
			'u.name'
	);

	$sql['dbTable'] = "{area} a";
	$sql['dbJoins'] = "JOIN {area_surface} a_s ON a.area_surface_id = a_s.id
			JOIN {acl} cacl ON cacl.id = a.acl_id
			JOIN {users} u ON u.uid = cacl.users_id";
	$sql['id'] = 'id';
	$sql['acl_level'] = $acl_level;
	$output = datatable_dbRequest($sql);
	return drupal_json_output($output);
}

/**
 * Return one or all viewable areas as JSON.
 * 
 * @param integer $area_id 
 * 	Id of requested area
 * @param integer $acl_level
 * 	Required ACL level (e.g. ACL_READ, ACL_WRITE, ACL_ADMIN, etc)
 * @return string JSON
 */
function area_json($area_id = false, $acl_level = 'ACL_READ') {
	$areas = db_query(
		'SELECT
			a.id AS id,
			a.acl_id AS acl_id,
			a.name,
			a.comment,
			a_s.altitude,
			a_s.township,
			a_s.canton,
			a_s.country,
			ST_AsGeoJSON(a_s.geom) AS geom
			FROM
				{area} a
				JOIN {area_surface} a_s ON a.area_surface_id = a_s.id
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
		if (check_permission($entry->acl_id, variable_get($acl_level))) {
			$item = array();
			$item['id'] = (int) $entry->id;
			$item['name'] = $entry->name;
			$item['parcel_nr'] = NULL;
			$item['altitude'] = (int) $entry->altitude;
			$item['township'] = $entry->township;
			$item['canton'] = $entry->canton;
			$item['country'] = $entry->country;
			$item['comment'] = $entry->comment;

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
	}
	// json_encode wants that keys are sequential and start with 0, otherwise it'll treat it as an object and
	// not as an array. However, we used area_id as $key, so we have to index them numerically
	$items = array_values($items);
	return drupal_json_output($items);
}

/**
 * Take JSON and update existing area geometry.
 * 
 * @param array $areadata Area data
 */
function area_updategeometry_json($areadata = NULL) {
	assert($areadata != NULL);
	if (!isset($_POST['area_points']) || !isset($_POST['type'])) {
		assert(false);
		return;
	}
	$jsonCoords = $_POST['area_points'];
	$areaType = $_POST['type'];
	$geometry_id = $areadata['area_surface_id'];

	$transaction = db_transaction();
	try {
		area_update_geometry(
			$areadata['area_surface_id'],
			array('zip' => filter_xss($_POST['zip']),
					'canton' => filter_xss($_POST['canton']),
					'country' => filter_xss($_POST['country']),
					'township' => filter_xss($_POST['township']),
					'altitude' => filter_xss($_POST['altitude']),
					'area_points' => $jsonCoords,
					'type' => $areaType
			));
		db_update('area')->fields(array('modify_time' => 'NOW()',))
			->condition('id', $areadata['id'])
			->execute();
	} catch (Exception $e) {
		$transaction->rollback();
		watchdog_exception('area', $e);
		drupal_set_message(
			t(
				'Changed geometry on area «@areaname» could not be saved.',
				array('@areaname' => $areadata['name'])),
			'error');
	}
}
?>
