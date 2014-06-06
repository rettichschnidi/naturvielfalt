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

define('VOTE_PAGESIZE', 24);

function fetch_observation_data($page) {
	global $user;
	
	$mainArray = array();
	$mainArray['general']['current_user_id'] = $user->uid;
	$mainArray['general']['current_user_name'] = $user->name;
	$mainArray['general']['total'] = getTotalObservationsCount();
	
	$mainArray['observations'] = getObservations($page);
	$mainArray['verifications'] = getVerifications($mainArray['observations']);
	
	return drupal_json_output($mainArray);
}

function getTotalObservationsCount() {
	global $user;
	if ($_SESSION["vote_observations_filter"] != null) {
		$whereClause = ' WHERE o.id IN (' . join(', ', $_SESSION["vote_observations_filter"]) . ')';
	}
	
	if(empty($whereClause)) $whereClause = ' WHERE imagescount > 0';
	else $whereClause .= ' AND imagescount > 0';
	
	/*
	 *  Check for permission if acl_id is in the columns array
	*/
	$joins = '';
	if (!check_admin_permission($user->uid)) {
		$minLevel = variable_get('ACL_READ');
	
		$joins = ' LEFT JOIN {acl_sgroup} ag ON ag.acl_id = o.acl_id'
				.' LEFT JOIN {sgroup} g ON g.id = ag.sgroup_id'
				.' LEFT JOIN {acl_users} au ON (au.acl_id = o.acl_id OR au.acl_id = g.acl_id) AND au.users_id = ' . $user->uid;
	
		$whereClause .= empty($whereClause) ? ' WHERE' : ' AND';
		// level of ALL group
		$whereClause .= ' ( (ag.sgroup_id = ' . variable_get('ACL_GROUP_ALL_ID')
		.' AND ag.level >= ' . $minLevel . ')';
		// level of user group
		$whereClause .= ' OR (au.acl_id = g.acl_id'
				.' AND ag.level >= ' . $minLevel . ')';
		$whereClause .= ' OR (au.acl_id = o.acl_id'
				.' AND au.level >= ' . $minLevel . ') )';
	
	}
	
	$imagetypes = join("','",_gallery_getMediaInfo(GALLERY_MEDIATYPE_IMAGE)->mimeTypes);
	
	$sql = 'SELECT COUNT(1) AS total FROM {observation} o'
			.' LEFT JOIN 
			(
				SELECT gi.item_id, COUNT(gi.id) AS imagescount 
				FROM {gallery_image} gi 
				LEFT JOIN {file_managed} fm ON gi.file_managed_fid = fm.fid 
				WHERE gi.item_type = \'observation\' 
				AND fm.filemime IN (\''.$imagetypes.'\')  
				GROUP BY gi.item_id
			) images ON images.item_id = o.id'
			.$joins
		    .$whereClause;
	debug($sql);
	 return db_query($sql)->fetchField();
}

function getObservations($page) {
	global $user;
	global $language;
	
	$langcode = $language->language;

	$whereClause = '';

	if ($_SESSION["vote_observations_filter"] != null) {
		$whereClause = ' WHERE o.id IN (' . join(', ', $_SESSION["vote_observations_filter"]) . ')';
	}

	if(empty($whereClause)) $whereClause = ' WHERE imagescount > 0';
	else $whereClause .= ' AND imagescount > 0';
	
	$imagetypes = join("','",_gallery_getMediaInfo(GALLERY_MEDIATYPE_IMAGE)->mimeTypes);


	/*
	 *  Check for permission if acl_id is in the columns array
	*/
	$joins = '';
	if (!check_admin_permission($user->uid)) {
		$minLevel = variable_get('ACL_READ');
		
		$joins = ' LEFT JOIN {acl_sgroup} ag ON ag.acl_id = o.acl_id'
				.' LEFT JOIN {sgroup} g ON g.id = ag.sgroup_id'
				.' LEFT JOIN {acl_users} au ON (au.acl_id = o.acl_id OR au.acl_id = g.acl_id) AND au.users_id = ' . $user->uid;
		
		$whereClause .= empty($whereClause) ? ' WHERE' : ' AND';
		// level of ALL group
		$whereClause .= ' ( (ag.sgroup_id = ' . variable_get('ACL_GROUP_ALL_ID')
		.' AND ag.level >= ' . $minLevel . ')';
		// level of user group
		$whereClause .= ' OR (au.acl_id = g.acl_id'
				.' AND ag.level >= ' . $minLevel . ')';
		$whereClause .= ' OR (au.acl_id = o.acl_id'
		.' AND au.level >= ' . $minLevel . ') )';
				
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
			.' LEFT JOIN 
			(
				SELECT gi.item_id, COUNT(gi.id) AS imagescount 
				FROM {gallery_image} gi 
				LEFT JOIN {file_managed} fm ON gi.file_managed_fid = fm.fid 
				WHERE gi.item_type = \'observation\' 
				AND fm.filemime IN (\''.$imagetypes.'\')  
				GROUP BY gi.item_id
			) images ON images.item_id = o.id'    	
			. $joins
			. $whereClause
			.' ORDER BY current_user_votes, votes, date DESC'
			.' OFFSET '.($page-1)*VOTE_PAGESIZE.' LIMIT '.VOTE_PAGESIZE;
	
	$output = db_query($sql)->fetchAll();
	
	$i = 0;
	foreach ($output as &$row) {
	
		// get images
		$images = gallery_images(GALLERY_MEDIATYPE_IMAGE, 'observation', $row->id);
	
		// thumbnail
		$row->thumbnail = '';
		// display the thumbnail (for the table view)
		$observationsArray[$i]["observation_id"] = $row->id;
		$observationsArray[$i]["thumbnail_image_path"] = image_style_url('gallery_thumbnail', $images['images'][0]['uri']);
		$observationsArray[$i]["fullsize_image_path"] = image_style_url('gallery_large', $images['images'][0]['uri']);
		$observationsArray[$i]["observation_images"] = $images;
		$observationsArray[$i]["detail_information"] = observation_get_table(observation_get($row->id));
		$observationsArray[$i]["comment_information"] = observation_get_details_table(observation_get($row->id));
		$observationsArray[$i]["google_map_marker_lat"] = $row->wgs84_center_lat;
		$observationsArray[$i]["google_map_marker_lng"] = $row->wgs84_center_lng;
		$i++;
	}
	
	return $observationsArray;
}

function getVerifications($observations) {
	global $language;
	global $user;
	
	$langcode = $language->language;
	
	$whereClause = ' WHERE (';
	 foreach($observations as $observation) {
	 		$whereClause .= 'v.observation_id = '.$observation['observation_id'].' OR ';
	}
	$whereClause = substr($whereClause, 0, -4) . ')';
	
	$sql =	'SELECT v.vote_id AS vote_id,'
			.' v.observation_id AS observation_id,'
			.' o.id AS organism_id,'
			.' o.scientific_name AS scientific_name,'
			.' ol.name AS translated_name,'
			.' v.vote_timestamp AS vote_timestamp,'
			.' v.vote_comment AS comment,'
			.' v.user_id AS user_id,'
			.' u.name AS user_name'
			.' FROM {vote} v'
			.' LEFT JOIN {organism} o ON (v.organism_id = o.id)'
			.' LEFT JOIN {organism_lang} ol ON ol.organism_id = o.id AND ol.languages_language = \'' . db_escape_field($langcode) . '\''
			.' LEFT JOIN {users} u ON (v.user_id = u.uid)'
			.$whereClause;
	
	$output = db_query($sql)->fetchAll();
	
	$i = 0;
	foreach ($output as &$row) {
		$returnArray[$i]['observation_id'] = $row->observation_id;
		$returnArray[$i]['artgroup_id'] = organism_artgroup_get_organsim_in_artgroup($row->organism_id,true);
		$returnArray[$i]['organism_id'] = $row->organism_id;
		$returnArray[$i]['scientific_name'] = $row->scientific_name;
		$returnArray[$i]['translated_name'] = $row->translated_name;
		$returnArray[$i]['vote_timestamp'] = $row->vote_timestamp;
		$returnArray[$i]['comment'] = $row->comment;
		$returnArray[$i]['user_id'] = $row->user_id;
		$returnArray[$i]['user_name'] = $row->user_name;
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
	
	// check if the current user already voted for this observation and replace it with the new one, if yes only update his vote
	$result = db_select('vote', 'v')
		->fields('v')
		->condition('observation_id', $verification['observation_id'], '=')
		->condition('user_id', $verification['user_id'], '=')
		->execute();
	$already_voted = $result->rowCount() > 0;
	$output = array();
	
	$transaction = db_transaction();
	try {
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
		$output['success'] = true;
		$output['alreadyVoted'] = $already_voted;
	} catch (Exception $e) {
		$transaction->rollback();
		$output['success'] = false;
	}
	
	if($output['success']) {
		$output['organism'] = organism_get($organismId['id']);
		$output['organism']['artgroup_id'] = organism_artgroup_get_organsim_in_artgroup(
			$organismId['id'],
			true);
	}
	return drupal_json_output($output);
}