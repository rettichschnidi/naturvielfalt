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
	global $user;
	
	$mainArray = array();
	$mainArray['general']['current_user_id'] = $user->uid;
	$mainArray['general']['current_user_name'] = $user->name;
	$mainArray['observations'] = getObservations();
	$mainArray['verifications'] = getVerifications();
	
	return drupal_json_output($mainArray);
}

function getObservations() {
	global $user;
	global $language;
	
	$langcode = (isset($user->language) && !empty($user->language)) ? $user->language : $language->language;

	$whereClause = '';
	if ($_SESSION["vote_observations_filter"] != null) {
		$whereClause = ' WHERE o.id IN (' . join(', ', $_SESSION["vote_observations_filter"]) . ')';
	}
	
	$sql =   'SELECT o.id AS id,'
			.' o.organism_artgroup_id AS artgroup_id,'
			.' o.date AS date,'
			.' o.inventory_id,'
			.' o.organism_id AS organism_id,'
			.' i.name AS inventory,'
			.' ST_X(ST_Centroid(st_transform(a_s.geom, 4326))) AS wgs84_center_lat,'
			.' ST_Y(ST_Centroid(st_transform(a_s.geom, 4326))) AS wgs84_center_lng,'
			.' (SELECT COUNT(1) FROM {vote} v WHERE v.observation_id = o.id) AS votes,'
			.' (SELECT COUNT(1) FROM {vote} v WHERE v.observation_id = o.id AND v.user_id = '.$user->uid.') AS current_user_votes'
			.' FROM {observation} o'
			.' INNER JOIN {area_geometry} a_s ON a_s.id = o.area_geometry_id'
			.' LEFT JOIN {organism_lang} ol ON ol.organism_id=o.organism_id AND ol.languages_language=\'' . db_escape_field($langcode) . '\''
			.' LEFT JOIN {organism_artgroup} oa ON oa.id = o.organism_artgroup_id'
			.' LEFT JOIN {inventory} i ON i.id = o.inventory_id'
			. $whereClause
			.' ORDER BY current_user_votes, votes, date DESC';
	
	$output = db_query($sql)->fetchAll();
	
	$i = 0;
	foreach ($output as &$row) {
	
		// get images
		$images = gallery_images(GALLERY_MEDIATYPE_IMAGE, 'observation', $row->id);
	
		// thumbnail
		$row->thumbnail = '';
		if ($images['count'] > 0) {
			// display the thumbnail (for the table view)
			$observationsArray[$i]["observation_id"] = $row->id;
			$observationsArray[$i]["thumbnail_image_path"] = url('gallery/observation/' . $row->id . '/thumb/' . $images['images'][0]['id'] . '/gallery_thumbnail');
			$observationsArray[$i]["fullsize_image_path"] = url('gallery/observation/' . $row->id . '/thumb/' . $images['images'][0]['id'] . '/fullsize');
			$observationsArray[$i]["observation_images"] = $images;
			$observationsArray[$i]["detail_information"] = observation_get_table(observation_get($row->id));
			$observationsArray[$i]["google_map_marker_lat"] = $row->wgs84_center_lat;
			$observationsArray[$i]["google_map_marker_lng"] = $row->wgs84_center_lng;
		}
	
		$i++;
	}
	
	unset($_SESSION["vote_observations_filter"]);
	
	return $observationsArray;
}

function getVerifications() {
	$sql['dbColumns'] = array(
			'v.vote_id AS vote_id',
			'v.observation_id AS observation_id',
			'o.id AS organism_id',
			'o.scientific_name AS scientific_name',
			'ol.name AS translated_name',
			'v.vote_timestamp AS vote_timestamp',
			'v.vote_comment AS comment',
			'v.user_id AS user_id',
			'u.name AS user_name'
	);
	
	$sql['dbTable'] = 'vote v';
	
	$sql['dbJoins'] = 'LEFT JOIN organism o
				ON (v.organism_id = o.id)
			LEFT JOIN organism_lang ol
				ON (v.organism_id = ol.organism_id)
			LEFT JOIN users u
				ON (v.user_id = u.uid)';

	global $language;
	$langcode = (isset($user->language) && !empty($user->language)) ? $user->language : $language->language;
	$sql['dbWhere'] = "ol.languages_language = '" . db_escape_field($langcode) . "'";
	
	$sql['id'] = 'vote_id';
	
	$arguments = array();
	$output = datatable_dbRequest($sql, $arguments);
	
	$returnArray;
	$i = 0;
	foreach ($output['rows'] as &$row) {
		$returnArray[$i]['observation_id'] = $row['cell']['observation_id'];
		$returnArray[$i]['organism_id'] = $row['cell']['organism_id'];
		$returnArray[$i]['scientific_name'] = $row['cell']['scientific_name'];
		$returnArray[$i]['translated_name'] = $row['cell']['translated_name'];
		$returnArray[$i]['vote_timestamp'] = $row['cell']['vote_timestamp'];
		$returnArray[$i]['comment'] = $row['cell']['comment'];
		$returnArray[$i]['user_id'] = $row['cell']['user_id'];
		$returnArray[$i]['user_name'] = $row['cell']['user_name'];
		
		$i++;
	}
	return $returnArray;
}

function vote_save() {
	$values = $_POST;
	
	global $user;
	$date = $values['date'];
	$time = $values['time'];

	$verification['user_id'] = filter_xss($user->uid);
	$verification['observation_id'] = filter_xss($values['observation_id']);
	$verification['vote_comment'] = filter_xss($values['vote_comment']);
	$organismId = organism_get_byname($values['organism_name']);
	
	$verification['organism_id'] = filter_xss($organismId['id']);
	// $verification['comment'] = filter_xss($values['comment']);
	
	//nasty workaround for webservice lightversion
	$date_parts = explode('.', filter_xss($date));
	$timesplit = array(null, null);
	if(isset($time)) {
		$timesplit = explode(':', filter_xss($time));
		$verification['vote_timestamp'] = mktime($timesplit[0], $timesplit[1], null, $date_parts[1], $date_parts[0], $date_parts[2]);
	} else {
		$verification['vote_timestamp'] = mktime(null, null, null, $date_parts[1], $date_parts[0], $date_parts[2]);
	}
	
	// check if the current user already voted for this observation and replace it with the new one, if yes
	$result = db_select('vote', 'v')
		->fields('v')
		->condition('observation_id', $verification['observation_id'], '=')
		->condition('user_id', $verification['user_id'], '=')
		->execute();
	$already_voted = $result->rowCount() > 0;
	
	if ($already_voted) {
		db_update('vote')
		->fields(array(
		    'organism_id' => $verification['organism_id'],
		    'vote_timestamp' => $verification['vote_timestamp'],
		    'vote_comment' => $verification['vote_comment']
		))
		->condition('observation_id', $verification['observation_id'], '=')
		->condition('user_id', $verification['user_id'], '=')
		->execute();
	} else {
		db_insert('vote')->fields($verification)->execute();
	}
	
	$organismToSave = organism_get($organismId['id']);
	
	return drupal_json_output($organismToSave);
}