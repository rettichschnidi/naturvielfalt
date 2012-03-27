<?php

/**
 * @file area.json.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');

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
			'a.township',
			'a.canton',
			'a.country',
			'ST_AsGeoJSON(a.geom) AS geom',
			'a.comment'
	);

	$sql['dbSearchColumns'] = array(
			'a.name',
			'a.altitude',
			'a.township',
			'a.canton',
			'a.country',
			'a.comment'
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
			a.township,
			a.canton,
			a.country,
			ST_AsGeoJSON(a.geom) AS geom,
			a.comment
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
	// json_encode wants that keys are sequential and start with 0, otherwise it'll treat it as an object and
	// not as an array. However, we used area_id as $key, so we have to index them numerically
	$items = array_values($items);
	return drupal_json_output($items);
}
?>
