<?php
/**
 * @file observation.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'observation', 'observation.template');
module_load_include('inc', 'organism', 'organism.artgroup');
module_load_include('inc', 'organism', 'organism');

/**
 * Return the single observation form to add a observation
 * @param array $observationdata
 */
function observation_edit($observationdata = NULL){
	return observation_form($observationdata);
}

/**
 * Return a array with all data for a observation
 * @param int $observation_id
 * @return array
 */
function observation_get($observation_id){
	$observation_id = db_escape_field(filter_xss($observation_id));
	if($observation_id > 0){
		$sql = "SELECT * FROM {observation} o
				WHERE o.id=:observation_id";
		$result['observation'] = db_query($sql,array('observation_id' => $observation_id))->fetchAssoc();
		// If no artgroup, try to find a artgroup
		if($result['observation']['organism_artgroup_id'] == 0){
			$artgroups = organism_artgroup_get_organsim_in_artgroup($result['observation']['organism_id']);
			foreach ($artgroups as $artgroup) $result['observation']['organism_artgroup_id'] = $artgroup;
		}
		$result['detmethods_vis'] = organism_artgroup_get_detmethods_ids($result['observation']['organism_artgroup_id']);
		$result['geometry'] = observation_get_geometry($result['observation']['area_geometry_id']);
		$result['organism'] = organism_get($result['observation']['organism_id']);
		$attributes = db_query("SELECT * FROM observation_attribute oa WHERE oa.observation_id=:observation_id", array(':observation_id' => $observation_id))->fetchAllAssoc('organism_artgroup_attr_id');
		$attributes_vis = organism_artgroup_get_attributes_ids($result['observation']['organism_artgroup_id']);
		$result['attributes'] = array();
		foreach ($attributes_vis as $attribute_vis)
			$result['attributes'][$attribute_vis] = array(
					'value_id' => $attributes[$attribute_vis]->organism_artgroup_attr_values_id,
					'value' => $attributes[$attribute_vis]->value);

		return $result;
	}
	return false;
}

function observation_get_geometry($geometry_id){
// 	die($geometry_id);
	$sql = "SELECT
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
				ST_AsGeoJSON(a_s.geom) AS wgs84_geom_json
	FROM {area_surface} a_s WHERE a_s.id=:geometry_id";
	return db_query($sql,array(':geometry_id' => $geometry_id))->fetchAssoc();
}

function observation_edit_acl($observation = NULL){
	observation_acl_template($observation);
}