<?php
/**
 * @file vote.ajax.inc
 * @author Emin Khateeb, 2013
 * @author Florian Gyger, 2013
 * @copyright 2013 Naturwerk, Brugg
 */

module_load_include('inc', 'observation');
module_load_include('inc', 'gallery', 'gallery.implementations');
module_load_include('inc', 'organism', 'organism.artgroup');
module_load_include('inc', 'observation', 'observation.delete');
module_load_include('inc', 'datatable');

function fetch_observation_data() {
	$sql['dbColumns'] = array(
		'o.id AS id',
		'o.organism_artgroup_id AS artgroup_id',
		'o.observer AS observer',
		'o.date AS date',
		'o.count AS count',
		'o.inventory_id',
		'o.acl_id AS acl_id',
		'o.organism_id AS organism_id',  // used to find portrait-pictures
		'o.users_uid',
		'i.name AS inventory',
		'i.area_id',
		'a.name AS area',
		'GeometryType(a_g.geom) AS areatype',
		'a_s.altitude AS altitude',
		'a_s.township AS township',
		'a_s.zip AS zip',
		'a_s.canton AS canton',
		'a_s.country AS country',
		'ST_X(ST_Centroid(st_transform(a_s.geom, 21781))) AS ch1903_center_x', // Center of the geometry - CH1903, x value
		'ST_Y(ST_Centroid(st_transform(a_s.geom, 21781))) AS ch1903_center_y', // Center of the geometry - CH1903, y value
		'ST_X(ST_Centroid(st_transform(a_s.geom, 4326))) AS wgs84_center_lat', // Center of the geometry - WGS84, latitude
		'ST_Y(ST_Centroid(st_transform(a_s.geom, 4326))) AS wgs84_center_lng', // Center of the geometry - WGS84, longitude
		'org.scientific_name AS organism_sc',
		'ol.name AS organism_l',
		'oa.name AS artgroup_n',
		// FIXME get more attributes than just Bemerkung
		'oattr.value AS comment',
		'coalesce(imagescount, 0) AS imagescount',
		'coalesce(videoscount, 0) AS videoscount',
		'coalesce(soundscount, 0) AS soundscount',
		'coalesce(documentscount, 0) AS documentscount'
			
	);
	
	$sql['dbSearchColumns'] = array(
		'o.id',
		'o.observer',
		'o.date',
		'o.count',
		'o.organism_id',
		'o.inventory_id',
		'i.name',
		'a.name',
		'a_s.altitude',
		'a_s.township',
		'a_s.zip',
		'a_s.canton',
		'a_s.country',
		'a_s.geom',
		'org.scientific_name',
		'ol.name',
		'oa.name',
		'oattr.value'
	);
	
	$sql['dbTable'] = '{observation} o';
	
	$imagetypes = join("','",_gallery_getMediaInfo(GALLERY_MEDIATYPE_IMAGE)->mimeTypes);
	$soundtypes = join("','",_gallery_getMediaInfo(GALLERY_MEDIATYPE_SOUND)->mimeTypes);
	$videotypes = join("','",_gallery_getMediaInfo(GALLERY_MEDIATYPE_VIDEO)->mimeTypes);
	
	$sql['dbJoins'] = 'INNER JOIN {area_geometry} a_s ON a_s.id = o.area_geometry_id'
		//.' LEFT JOIN {organism_synonym} osy ON osy.organism_id = o.organism_id '
		.' LEFT JOIN {organism_lang} ol ON ol.organism_id=o.organism_id AND ol.languages_language=\'' . db_escape_field($langcode) . '\''
		.' LEFT JOIN {organism} org ON org.id=o.organism_id'
		.' LEFT JOIN {organism_artgroup} oa ON oa.id = o.organism_artgroup_id'
		// FIXME get more attributes than just Bemerkung
		.' LEFT JOIN {observation_attribute} oattr ON oattr.observation_id=o.id AND oattr.organism_artgroup_attr_id=2'
		.' LEFT JOIN {inventory} i ON i.id = o.inventory_id'
		.' LEFT JOIN {area} a ON a.id = i.area_id'
		.' LEFT JOIN {area_geometry} a_g ON a_g.id = a.area_geometry_id'
		.' LEFT JOIN 
				(
				SELECT gi.item_id, COUNT(gi.id) AS imagescount 
				FROM {gallery_image} gi 
				LEFT JOIN {file_managed} fm ON gi.file_managed_fid = fm.fid 
				WHERE gi.item_type = \'observation\' 
				AND fm.filemime IN (\''.$imagetypes.'\')  
				GROUP BY gi.item_id
				) images ON images.item_id = o.id'    
		.' LEFT JOIN 
				(
				SELECT gi.item_id, COUNT(gi.id) AS videoscount 
				FROM {gallery_image} gi 
				LEFT JOIN {file_managed} fm ON gi.file_managed_fid = fm.fid 
				WHERE gi.item_type = \'observation\'
				AND fm.filemime IN (\''.$videotypes.'\') 
				GROUP BY gi.item_id
				) videos ON videos.item_id = o.id'
		.' LEFT JOIN 
				(
				SELECT gi.item_id, COUNT(gi.id) AS soundscount 
				FROM {gallery_image} gi 
				LEFT JOIN {file_managed} fm ON gi.file_managed_fid = fm.fid 
				WHERE gi.item_type = \'observation\'
				AND fm.filemime IN (\''.$soundtypes.'\') 
				GROUP BY gi.item_id
				) sounds ON sounds.item_id = o.id'
		.' LEFT JOIN 
				(
				SELECT ofm.observation_id, COUNT(*) AS documentscount 
				FROM {observation_file_managed} ofm 
				GROUP BY ofm.observation_id
				) documents ON documents.observation_id = o.id'
	;
	
	if ($observation_ids !== NULL) {
		if (is_int($observation_ids) && $observation_ids > 0)
			$observation_ids = array($observation_ids);
		if (is_array($observation_ids) && !empty($observation_ids)) {
			$sql['dbWhere'] = empty($sql['dbWhere'])
				? '('
				: $sql['dbWhere'] . ' AND (';
			foreach ($observation_ids as $observation_id) {
				$sql['dbWhere'] .= 'o.id=:observation_id_' . $observation_id . ' OR ';
				$arguments[':observation_id_'.$observation_id] = $observation_id;
			}
			$sql['dbWhere'] = substr($sql['dbWhere'], 0, -4) . ')';
		}
	}
	
	if($inventory_ids !== NULL) {
		if (is_array($inventory_ids) && !empty($inventory_ids)) {
			$sql['dbWhere'] = empty($sql['dbWhere'])
					? '('
					: $sql['dbWhere'] . ' AND (';
			foreach ($inventory_ids as $id) {
				$sql['dbWhere'] .= 'o.inventory_id=:inventory_id_' . $id . ' OR ';
				$arguments[':inventory_id_'.$id] = $id;
			}
			$sql['dbWhere'] = substr($sql['dbWhere'], 0, -4) . ')';
		} else {
			$sql['dbWhere'] = empty($sql['dbWhere'])
					? '('
					: $sql['dbWhere'] . ' AND (';
			$sql['dbWhere'] .= 'o.inventory_id = -1)';
		}
	}
	
	if ($organism_ids !== NULL) {
		if (is_int($organism_ids) && $organism_ids > 0)
			$organism_ids = array($organism_ids);
		if (is_array($organism_ids) && !empty($organism_ids)) {
			$sql['dbWhere'] = empty($sql['dbWhere'])
			? '('
					: $sql['dbWhere'] . ' AND (';
			foreach ($organism_ids as $organism_id) {
				$sql['dbWhere'] .= 'o.organism_id=:organism_id_' . $organism_id . ' OR ';
				$arguments[':organism_id_'.$organism_id] = $organism_id;
			}
			$sql['dbWhere'] = substr($sql['dbWhere'], 0, -4) . ')';
		}
	}
	
	$sql['id'] = 'id';
	
	/*
	 * Only show accessible observations.
	 * If own observations are requested, reset ACL level to readable observations. 
	 */
	if ($acl_level === 'own') {
		$sql['dbWhere'] = empty($sql['dbWhere'])
			? ''
			: $sql['dbWhere'] . ' AND ';
		$sql['dbWhere'] .= 'o.users_uid=:uid';
		$arguments[':uid'] = $user->uid;
		$sql['acl_level'] = 'ACL_READ';
	}
	else
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
	
	$sql['dbDefaultOrder'] = 'o.date DESC';
	
	/*
	 * fetch the observation data
	 */
	$arguments = array();
	$output = datatable_dbRequest($sql, $arguments);
	
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
			//$imageArray[$i]["imagesCount"] = print_r($row, true);
			$imageArray[$i]["imagesCount"] = $row['cell']['imagescount'];
			$imageArray[$i]["images"] = $images;
			$imageArray[$i]["wgs84_center_lat"] = $row['cell']['wgs84_center_lat'];
			$imageArray[$i]["wgs84_center_lng"] = $row['cell']['wgs84_center_lng'];
			// $row['cell']['thumbnail'] = '<img onclick="gallery_lightbox.openLightBoxEntry('.$row['id'].');" src="' . url('gallery/observation/' . $row['id'] . '/thumb/' . $images['images'][0]['id'] . '/gallery_thumbnail') . '" alt="' . $images['images'][0]['title'] . '" height="80" width="80" />';
		}
	
		$i++;
	}
	
	return drupal_json_output($imageArray);
}