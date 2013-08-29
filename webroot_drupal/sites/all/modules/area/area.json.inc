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
 * @param $acl_level
 * 	Required ACL level (e.g. ACL_READ, ACL_WRITE, ACL_ADMIN, etc)
 * @todo Include the user's nickname
*/
function area_datatable_json($acl_level = 'ACL_READ') {
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'a.id AS id',
			'a.acl_id AS acl_id',
			'a.name',
			'a.comment',
			'a.create_time AS date',
			'a_s.altitude',
			'a_s.township',
			'a_s.canton',
			'a_s.country',
			'a_s.zip',
			'GeometryType(a_s.geom) AS geometry_type',
			'u.name AS username',
			'u.uid'
	);

	$sql['dbSearchColumns'] = array(
			'a.name',
			'a.comment',
			'a_s.altitude',
			'a_s.township',
			'a_s.canton',
			'a_s.country',
			'a_s.zip',
			'u.name'
	);

	$sql['dbTable'] = '{area} a';
	$sql['dbJoins'] = 'JOIN {area_geometry} a_s ON a.area_geometry_id = a_s.id
			JOIN {acl} cacl ON cacl.id = a.acl_id
			JOIN {users} u ON u.uid = cacl.users_id';
	$sql['id'] = 'id';
	$sql['acl_level'] = $acl_level;
	$output = datatable_dbRequest($sql);

	global $base_url;
	$iconBaseUrl = '/' . path_to_theme() . '/images/icons/enabled/';
		
	foreach ($output['rows'] as &$row) {
		// select box
		$row['cell']['selectColumn'] = '<input class="gridSelect" type="checkbox" value="' . $row['id'] . '" title="' . t('Select') . '" onclick="javascript:areamap.selectMultipleAreas(event, ' . $row['id'] . ', !this.checked);" />';
		// show details
		$row['cell']['showColumn'] = l(
			'<img src="' . $iconBaseUrl . 'List.png" alt="' . t('Show') . '" title="' . t('Show') . '" />',
			$base_url . '/area/' . $row['id'] . '/show',
			array(
				'html' => true
			));;
		//TODO, check if write access
		// edit details
		$row['cell']['editColumn'] = (area_write_access_by_id($row['cell']['id']))
			? l(
				'<img src="' . $iconBaseUrl . 'Modify.png" alt="' . t('Edit') . '" title="' . t('Edit') . '" />',
				$base_url . '/area/' . $row['id'] . '/edit',
				array(
					'html' => true
				))
			: '';

		//zoom in
		$row['cell']['zoominColumn'] = '<img src="' . $iconBaseUrl . 'Zoom in.png" alt="' . t('zoomin') . '" title="' . t('zoomin') . '" onclick="javascript:areamap.zoomIn(event, ' . $row['id'] . ');" />';
		
		// type of area (picture)
		$row['cell']['geometry_type'] = '<img src="'. url(drupal_get_path('module', 'commonstuff'). '/images/' . $row['cell']['geometry_type'] . '.png') . '" alt="'. t('type') . '" />';
		
		$row['cell']['date'] = date("d.m.Y H:i:s", strtotime($row['cell']['date']));
		
		$row['cell']['username'] = l(
					check_plain($row['cell']['username']),
					base_path() . "user/" . $row['cell']['uid'],
					array(
							'external' => true
					));
		
		// get images
		$images = gallery_images(GALLERY_MEDIATYPE_IMAGE, 'area', $row['id']);
		
		// thumbnail
		$row['cell']['thumbnail'] = '';
		if ($images['count'] > 0) {
			// display the thumbnail (for the table view)
			$row['cell']['thumbnail'] = '<img onclick="event.stopPropagation(); gallery_lightbox.openLightBoxEntry('.$row['id'].');" src="' . url('gallery/area/' . $row['id'] . '/thumb/' . $images['images'][0]['id'] . '/gallery_thumbnail') . '" alt="' . $images['images'][0]['title'] . '" height="80" width="80" />';
		}
		$location = $row['cell']['township'];
		$location .= !empty($row['cell']['township']) ? ', '.$row['cell']['canton'] : $row['cell']['canton'];
		$location .= !empty($row['cell']['canton']) ? ', '.$row['cell']['country'] : $row['cell']['country'];
		
		$imageBaseUrl = $base_url . '/' . drupal_get_path('module', 'area') . '/images/';
		
		/**
		 * create the html source for the gallery view
		 */
		$row['cell']['gallery_image'] = datatable_get_gallery_area_image(
				$row['id'],
				array(
						'icons' => array(
								'<img src="' . $imageBaseUrl . '/pin.png" style="cursor: pointer;" title="' . t('Position') . '" onclick="javascript:areamap.selectGeometry(' . $row['id'] . ');window.scrollTo(0,0);">',
								$row['cell']['zoominColumn'],
								$row['cell']['showColumn'],
								$row['cell']['editColumn'],
								$row['cell']['selectColumn']
						),
						'data' => array(
								$row['cell']['name'],
								$row['cell']['geometry_type'],
								$location
								
						)
				),
				$images);
	}

	return drupal_json_output($output);
}

/**
 * Return one or all viewable areas as JSON.
 *
 * @param $area_id
 * 	Id of requested area
 * @param $acl_level
 * 	Required ACL level (e.g. ACL_READ, ACL_WRITE, ACL_ADMIN, etc)
 * @return JSON
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
			a_s.zip,
			ST_AsGeoJSON(a_s.geom) AS geom
			FROM
			{area} a
			JOIN {area_geometry} a_s ON a.area_geometry_id = a_s.id
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
			$item['name'] = check_plain($entry->name);
			$item['parcel_nr'] = NULL;
			$item['altitude'] = (int) $entry->altitude;
			$item['township'] = check_plain($entry->township);
			$item['canton'] = check_plain($entry->canton);
			$item['country'] = check_plain($entry->country);
			$item['comment'] = check_plain($entry->comment);
			$item['zip'] = check_plain($entry->zip);

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
			$item['entity'] = 'area';
			$items[] = $item;
		}
		if (isset($_REQUEST['observation_ids'])) {
			$observation_data = $_REQUEST['observation_ids'];
			$items = array_merge($items, observation_get_geometries_acl_checked($observation_data));
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
 * @param $areadata
 * 	Array with required area data
 */
function area_updategeometry_json($areadata = NULL) {
	assert($areadata != NULL);
	if (!isset($_POST['area_points']) || !isset($_POST['type'])) {
		assert(false);
		return;
	}
	$jsonCoords = $_POST['area_points'];
	$areaType = $_POST['type'];
	$geometry_id = $areadata['area_geometry_id'];

	$transaction = db_transaction();
	try {
		area_update_geometry(
		$areadata['area_geometry_id'],
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

/**
 * Get a all habitats within an area as datatable json.
 *
 * @param $areadata
 * 	Array with all data about the area
 */
function area_habitat_within_area_datatable_json($areadata = NULL) {
	assert($areadata != NULL);
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'label',
			'name'
	);

	$sql['dbSearchColumns'] = array(
			'label',
			'name'
	);

	$sql['dbTable'] = '{area_habitat} ah';
	$sql['dbJoins'] = 'JOIN {habitat} h ON ah.habitat_id = h.id';
	$sql['id'] = 'id';
	$output = datatable_dbRequest($sql);
	return drupal_json_output($output);
}

/**
 * Get a all areas which area linked to a habitat.
 *
 * @param $areadata
 * 	Array with all data about the area
 */
function area_linked_to_habitat_datatable_json($habitatId) {
	assert($habitatId != NULL);
	/* Create a temporary view to avoid the shortcommings of the datatable module */
	$sql['dbPreStatement'] = 'CREATE TEMPORARY VIEW linkedareas AS (
			SELECT
			DISTINCT ON (a.id)
			a.id,
			a.name AS areaname,
			a_s.township AS township,
			a_s.zip AS zip,
			u.name AS username
			FROM
			{area_habitat} AS a_h
			JOIN {area} AS a ON a_h.area_id = a.id
			JOIN {area_geometry} AS a_s ON a_s.id = a.area_geometry_id
			JOIN {acl} cacl ON cacl.id = a.acl_id
			JOIN {users} u ON u.uid = cacl.users_id
			WHERE
			a_h.habitat_id=:habitatId )';

	$sql['dbColumns'] = array(
			'id',
			'areaname',
			'township',
			'zip',
			'username'
	);
	$sql['dbSearchColumns'] = array(
			'areaname',
			'township',
			'zip',
			'username'
	);

	$sql['dbTable'] = 'linkedareas';
	$sql['id'] = 'id';
	$output = datatable_dbRequest(
			$sql,
			array(),
			array(':habitatId' => $habitatId));
	return drupal_json_output($output);
}
?>
