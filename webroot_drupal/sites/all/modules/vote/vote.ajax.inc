<?php
/**
 * @file vote.ajax.inc
 * @author Emin Khateeb, 2013
 * @author Florian Gyger, 2013
 * @copyright 2013 Naturwerk, Brugg
 */

module_load_include('inc', 'common');
module_load_include('inc', 'observation');
module_load_include('inc', 'gallery', 'gallery.implementations');
module_load_include('inc', 'organism', 'organism.artgroup');
module_load_include('inc', 'observation', 'observation.delete');
module_load_include('inc', 'datatable');

function fetch_observation_data() {
	$sql['dbColumns'] = array(
		'o.id AS id',
		'o.organism_artgroup_id AS artgroup_id',
		'o.date AS date',
		'o.inventory_id',
		'o.organism_id AS organism_id',  // used to find portrait-pictures
		'i.name AS inventory',
		'ST_X(ST_Centroid(st_transform(a_s.geom, 4326))) AS wgs84_center_lat', // Center of the geometry - WGS84, latitude
		'ST_Y(ST_Centroid(st_transform(a_s.geom, 4326))) AS wgs84_center_lng', // Center of the geometry - WGS84, longitude
		'(SELECT COUNT(1) FROM {vote} v WHERE v.observation_id = o.id) AS votes'
	);
	
	$sql['dbTable'] = '{observation} o';
	
	$imagetypes = join("','",_gallery_getMediaInfo(GALLERY_MEDIATYPE_IMAGE)->mimeTypes);
	
	$sql['dbJoins'] = 'INNER JOIN {area_geometry} a_s ON a_s.id = o.area_geometry_id'
		.' LEFT JOIN {organism_lang} ol ON ol.organism_id=o.organism_id AND ol.languages_language=\'' . db_escape_field($langcode) . '\''
		.' LEFT JOIN {organism_artgroup} oa ON oa.id = o.organism_artgroup_id'
		.' LEFT JOIN {inventory} i ON i.id = o.inventory_id'
	;
	
	$sql['id'] = 'id';
	
	/*
	 * Only show accessible observations.
	 */
	$sql['acl_level'] = $acl_level;
	
	/*
	 * Only show observations of a specific artgroup.
	 * artgroup id 1 means all artgroups.
	 */
	if (isset($_REQUEST['oaid']) && is_numeric($_REQUEST['oaid']) && $_REQUEST['oaid'] > 1) {
		$sql['dbWhere'] = empty($sql['dbWhere'])
			? ''
			: $sql['dbWhere'] . ' AND ';
		$sql['dbWhere'] .= 'o.organism_artgroup_id=:oaid';
		$arguments[':oaid'] = $_REQUEST['oaid'];
	}
	
	$sql['dbDefaultOrder'] = '(SELECT COUNT(1) FROM {vote} v WHERE v.observation_id = o.id), o.date DESC';
	
	/*
	 * fetch the observation data
	 */
	$arguments = array();
	$output = datatable_dbRequest($sql, $arguments);
	
	$labels = array();
	$labels['verifications'] = t('verifications');
	$labels['agree'] = t('Agree');
	$labels['noVerifications'] = t('No verifications found for this observation.');
	
	$i = 0;
	foreach ($output['rows'] as &$row) {
	
		// get images
		$images = gallery_images(GALLERY_MEDIATYPE_IMAGE, 'observation', $row['id']);
		
		// thumbnail
		$row['cell']['thumbnail'] = '';
		if ($images['count'] > 0) {			
			// display the thumbnail (for the table view)
			$imageArray[$i]["imageId"] = $row['id'];
			$imageArray[$i]["imageThumbPath"] = url('gallery/observation/' . $row['id'] . '/thumb/' . $images['images'][0]['id'] . '/gallery_thumbnail');
			$imageArray[$i]["imagePath"] = url('gallery/observation/' . $row['id'] . '/thumb/' . $images['images'][0]['id'] . '/fullsize');
			$imageArray[$i]["images"] = $images;
			$imageArray[$i]["table"] = observation_get_table(observation_get($row['cell']['id']));
			$imageArray[$i]["wgs84_center_lat"] = $row['cell']['wgs84_center_lat'];
			$imageArray[$i]["wgs84_center_lng"] = $row['cell']['wgs84_center_lng'];
			$imageArray[$i]["suggestionsFromOtherUsers"] = getSuggestions($row['id']);
			$imageArray[$i]["labels"] = $labels;
		}
	
		$i++;
	}
	
	return drupal_json_output($imageArray);
}

function getSuggestions($observationId) {
	$sql['dbColumns'] = array(
			'o.id AS organism_id',
			'o.scientific_name AS scientific_name',
			'ol.name AS translated_name',
			'COUNT(1) AS votes'
	);
	
	$sql['dbTable'] = 'vote v';
	
	$sql['dbJoins'] = 'LEFT JOIN organism o
				ON (v.organism_id = o.id)
			LEFT JOIN organism_lang ol
				ON (v.organism_id = ol.organism_id)';

	$sql['id'] = 'o.id';

	/*
	 * Only show accessible observations.
	*/
	$sql['acl_level'] = $acl_level;

	$sql['dbWhere'] = "v.observation_id = ". $observationId ." AND ol.languages_language = 'de'";

	$sql['dbDefaultOrder'] = 'COUNT(1) desc';
	
	$sql['dbGroupBy'] = "v.observation_id,
        o.id,
        o.scientific_name,
        ol.name";
	
	$arguments = array();
	$output = datatable_dbRequest($sql, $arguments);
	
	$totalVotes = 0;
	foreach ($output['rows'] as &$row) {
		$totalVotes += $row['cell']['votes'];
	}
	
	$returnArray;
	$i = 0;
	foreach ($output['rows'] as &$row) {
		$returnArray[$i]["id"] = $row['cell']['organism_id'];
		$returnArray[$i]["votes"] = $row['cell']['votes'];
		$returnArray[$i]["votes_percent"] = round($row['cell']['votes'] / $totalVotes * 100, 0);
		$returnArray[$i]["scientific_name"] = $row['cell']['scientific_name'];
		$returnArray[$i]["translated_name"] = $row['cell']['translated_name'];
		$i++;
	}
	return $returnArray;
}