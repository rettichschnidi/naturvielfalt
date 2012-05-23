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
		$result['observation']['organism_artgroup_detmethod_name'] = organism_artgroup_get_detmethod($result['observation']['organism_artgroup_detmethod_id'], true);
		$result['observation']['organism_artgroup_name'] = organism_artgroup_get_artgroup($result['observation']['organism_artgroup_id'], true);
		$result['detmethods_vis'] = ($result['observation']['organism_artgroup_id'] != '') ? organism_artgroup_get_detmethods_ids($result['observation']['organism_artgroup_id']) : array();
		$result['geometry'] = observation_get_geometry($result['observation']['area_geometry_id']);
		($result['observation']['found_as_lang'] == 1) ? $result['observation']['found_as_lang'] = 'true' : $result['observation']['found_as_lang'] = 'false';
		($result['observation']['found_as_lat'] == 1) ? $result['observation']['found_as_lat'] = 'true' : $result['observation']['found_as_lat'] = 'false';

		/**
		 * cheap hack to work with the very special weard
		 * 'json' format from area, don't ask and don't change it!
		 */
		$tmp = (json_decode($result['geometry']['wgs84_geom_json'])->coordinates);
		$result['geometry']['coordinatejson'][0][0] = $tmp[1];
		$result['geometry']['coordinatejson'][0][1] = $tmp[0];
		/**
		 * hack end
		 */

		$result['organism'] = organism_get($result['observation']['organism_id']);
		$attributes = db_query("SELECT * FROM {observation_attribute} oa WHERE oa.observation_id=:observation_id", array(':observation_id' => $observation_id))->fetchAllAssoc('organism_artgroup_attr_id');
		$attributes_vis = ($result['observation']['organism_artgroup_id'] != '') ? organism_artgroup_get_attributes_ids($result['observation']['organism_artgroup_id']): array();
		$result['attributes'] = array();
		foreach ($attributes_vis as $attribute_vis)
			$result['attributes'][$attribute_vis] = array(
					'attribute_name' => organism_artgroup_get_attribute($attribute_vis, true),
					'value_id' => @$attributes[$attribute_vis]->organism_artgroup_attr_values_id,
					'value_id_name' => organism_artgroup_get_attributes_value_name(@$attributes[$attribute_vis]->organism_artgroup_attr_values_id),
					'value' => @$attributes[$attribute_vis]->value);

		return $result;
	}
	return false;
}

function observation_get_geometries_acl_checked($observation_data = false){
	$filter = array();
	if($observation_data){
		$filter[':observation_id'] = $observation_data['observation']['id'];
		$where = " WHERE o.id=:observation_id ";
	}
	$sql = "SELECT o.id, o.area_geometry_id, o.acl_id,
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
	FROM {observation} o
	JOIN {area_geometry} a_s ON a_s.id=o.area_geometry_id".$where;
	$results = db_query($sql, $filter)->fetchAllAssoc('id', PDO::FETCH_ASSOC);
	$items = array();
// 		die(print_r($results));

	foreach($results as $result){
		$tmp['observation'] = $result;
		if(observation_read_access($tmp) == true){
			$item = array();
			$item['id'] = (int) $result['id'];
			$item['altitude'] = (int) $result['altitude'];
			$item['township'] = $result['township'];
			$item['canton'] = $result['canton'];
			$item['country'] = $result['country'];
			$item['comment'] = $result['comment'];

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
	FROM {area_geometry} a_s WHERE a_s.id=:geometry_id";
	return db_query($sql,array(':geometry_id' => $geometry_id))->fetchAssoc();
}

/**
 * Return the acl permission form for a observation
 * @param array $observation
 */
function observation_edit_acl($observation = NULL){
	return observation_acl_template($observation);
}

/**
 * Return a formated table with the observation details
 * @param array $observationdata
 */
function observation_get_table($observationdata = NULL){

	if($observationdata == NULL || !is_array($observationdata)) return false;
	$rows = array();

	if (!empty($observationdata['organism']['name_lang'])) {
		$rows[] = array(
				array('data' => t('Organism Name'),'header' => true),
				$observationdata['organism']['name_lang'],
		);
	}

	if (!empty($observationdata['organism']['name_lat'])) {
		$rows[] = array(
				array('data' => t('Scientific name'),'header' => true),
				$observationdata['organism']['name_lat'],
		);
	}


	if (!empty($observationdata['observation']['organism_artgroup_name'])) {
		$rows[] = array(
				array('data' => t('Artgroup'),'header' => true),
				t($observationdata['observation']['organism_artgroup_name']),
		);
	}

	if (!empty($observationdata['observation']['observer'])) {
		$rows[] = array(
				array('data' => t('Observer'),'header' => true),
				$observationdata['observation']['observer'],
		);
	}

	if (!empty($observationdata['observation']['date'])) {
		$rows[] = array(
				array('data' => t('Date'),'header' => true),
				date("j.m.Y", $observationdata['observation']['date'])
		);
	}

	if (!empty($observationdata['observation']['m_time'])) {
		$rows[] = array(
				array('data' => t('Last modification'),'header' => true),
				date("j.m.Y", $observationdata['observation']['m_time'])
		);
	}

	if (!empty($observationdata['observation']['count'])) {
		$rows[] = array(
				array('data' => t('Count'),'header' => true),
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
function observation_get_details_table($observationdata = NULL){
	if($observationdata == NULL || !is_array($observationdata)) return false;
	$rows = array();

// 	if (!empty($observationdata['observation']['organism_artgroup_detmethod_name'])) {
		$rows[] = array(
				array('data' => t('Determination method'),'header' => true),
				($observationdata['observation']['organism_artgroup_detmethod_name'] == '') ?
					t('Not definied') :
					t($observationdata['observation']['organism_artgroup_detmethod_name']),

		);
// 	}

	foreach ($observationdata['attributes'] as $attribute){
		$rows[] = array(
				array('data' => t($attribute['attribute_name']),'header' => true),
				($attribute['value_id'] == '') ?
					$attribute['value'] : t($attribute['value_id_name']),

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



function observation_get_geometry_table($observationdata = NULL){
	/**
	 * Geometry Details
	 */
	if (!empty($observationdata['geometry']['township'])) {
		$rows[] = array(
				array('data' => t('Township'),'header' => true),
				$observationdata['geometry']['township']
		);
	}
	if (!empty($observationdata['geometry']['zip'])) {
		$rows[] = array(
				array('data' => t('ZIP'),'header' => true),
				$observationdata['geometry']['zip']
		);
	}
	if (!empty($observationdata['geometry']['canton'])) {
		$rows[] = array(
				array('data' => t('Canton'),'header' => true),
				$observationdata['geometry']['canton']
		);
	}
	if (!empty($observationdata['geometry']['country'])) {
		$rows[] = array(
				array('data' => t('Country'),'header' => true),
				$observationdata['geometry']['country']
		);
	}
	if (!empty($observationdata['geometry']['altitude'])) {
		$rows[] = array(
				array('data' => t('Altitude [m.a.s.l.]'),'header' => true),
				$observationdata['geometry']['altitude']
		);
	}
	if ((!empty($observationdata['geometry']['wgs84_center_lat'])
			&& !empty($observationdata['geometry']['wgs84_center_lng'])
			&& !empty($observationdata['geometry']['ch1903_center_x'])
			&& !empty($observationdata['geometry']['ch1903_center_y']))) {
		$rows[] = array(
				array('data' => t('Coordinates'),'header' => true),
				get_wgs84_coordinates_link($observationdata['geometry']) . "<br/>"
				. get_ch1903_coordinates_link($observationdata['geometry'])
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
function observation_save_attribute_values($attributes_checked, $observation_id){
	// save each attributes
	foreach ($attributes_checked as $key => $value){
		$val = explode('-->', $value);
		$db_id = NULL; $db_val = NULL;
		if($val[0] == 'db_id'){
			$db_id = $val[1];
		}else{
			$db_val = $value;
		}
		$entry = array(
				'observation_id' => $observation_id,
				'organism_artgroup_attr_id' => $key,
				'organism_artgroup_attr_values_id' => $db_id,
				'value' => $db_val,
		);
		if(observation_attribute_value_exist($observation_id, $key)){
			observation_update_attribute_value($entry);
		}else{
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
function observation_attribute_value_exist($observation_id, $attr_id){
	$result = db_query("SELECT * FROM {observation_attribute} oa
			WHERE oa.observation_id=:observation_id AND
			oa.organism_artgroup_attr_id=:attr_id",
			array(':observation_id' => $observation_id, ':attr_id' => $attr_id))
			->rowCount();
	if($result == 1) return true;
	return false;
}

/**
 * update a observation attribute
 * @param array $entry
 */
function observation_update_attribute_value($entry){
	db_update('observation_attribute')
	->fields($entry)
	->condition('observation_id', $entry['observation_id'])
	->condition('organism_artgroup_attr_id', $entry['organism_artgroup_attr_id'])
	->execute();
}

/**
 * add a observation attribute
 * @param array $entry
 */
function observation_add_attribute_value($entry){
	db_insert('observation_attribute')->fields($entry)->execute();
}



/**
 * BETA!!!!
 * @param array $php_files
 * @param string $module_name
 * @param array $entry_array
			'item_id' => ID of the obervation/area/...,
			'title' => Title,
			'description' => little desc,
			'author' => the author,
			'location' => a location like Brugg AG,
			'type' => 'images' OR 'mp3' OR 'mp4' (the file inputs need to named files[])!!!
	@param string $file_managed_table
			The module db table to add the entry
	@param string $file_managed_module_col
			the column in the module db table where's the item_id should be inserted
 */
function observation_gallery_save_manual($module_name, $entry_array, $file_managed_module_col=false){

	$mediatype = $entry_array['type'];

	// if no files to work with abort
	if(count($_FILES['files']['tmp_name']) < 1) return false;

	$categories = array();
	$destination = array();



	// check the directory, create it if not exists
	$key = $module_name . ':' . $entry_array['item_id'];
// 	if (!array_key_exists($key, $destination)) {
		$destination[$key] = 'public://naturvielfalt/gallery/'
		. $module_name . '/' . $entry_array['item_id'];
// 		$gallery_path = 'public://naturvielfalt/gallery/'.$module_name.'/'.$entry_array['item_id'];
// 		die($gallery_path);
		file_prepare_directory($destination[$key], FILE_CREATE_DIRECTORY);
// 	}

	$img_ids = array();
// die(print_r($_FILES));

	// save each files
	foreach($_FILES['files']['error'] as $fkey => $message){
		if($message != 0) continue;
		$add_to_gallery = true;

		// build the db entry for the gallery module
		$image = array(
				'item_type' => $module_name,
				'item_id' => $entry_array['item_id'],
				'title' => $_REQUEST['meta_title'][$fkey],
				'description' => $_REQUEST['meta_description'][$fkey],
				'author' => empty($_REQUEST['meta_author'][$fkey]) ? $entry_array['author'] : $_REQUEST['meta_author'][$fkey],
				'location' => empty($_REQUEST['meta_location'][$fkey]) ? $entry_array['location'] : $_REQUEST['meta_location'][$fkey],
				'categories' => array(),
				'subtypes' => array()
		);

		// make the validator for each file
		if (gallery_validate_file_extension($_FILES['files']['name'][$fkey], gallery_get_image_extensions())) {
			$extensions = gallery_get_image_extensions();
			$file_size = variable_get('gallery_image_size', 2048);
		} elseif (gallery_validate_file_extension($_FILES['files']['name'][$fkey], gallery_get_mp3_extensions())) {
			$extensions = gallery_get_mp3_extensions();
			$file_size = variable_get('gallery_sound_size', 2048);
		} elseif (gallery_validate_file_extension($_FILES['files']['name'][$fkey], gallery_get_video_extensions())) {
			$extensions = gallery_get_video_extensions();
			$file_size = variable_get('gallery_video_size', 8192);
		}elseif (gallery_validate_file_extension($_FILES['files']['name'][$fkey], gallery_get_file_extensions())){
			$extensions = gallery_get_file_extensions();
			$file_size = variable_get('gallery_video_size', 8192);
			$add_to_gallery = false;
		}else{
			//FIXME
			// handlich for pdf or others, going only in filemanaged
			continue;
		}
		$file_size = $file_size * 1024;
		$validators = array(
				'file_validate_extensions' => array(
						$extensions
				),
				'file_validate_size' => array(
						parse_size($file_size),
						0
				),
		);


		$file = file_save_upload($fkey, $validators, $destination[$key]);
		if ($file) {
			$file->status |= FILE_STATUS_PERMANENT;
			$file = file_save($file);
			$image['file_managed_fid'] = $file->fid;
			if($add_to_gallery){
				$img_ids[$fkey] = gallery_image_save($image);
			}else{
				$img_ids[$fkey] = $file->fid;
				if($file_managed_module_col){
					$file_managed_table_entry[$file_managed_module_col] = $entry_array['item_id'];
					$file_managed_table_entry['file_managed_fid'] = $file->fid;
					$file_managed_table_entry['description'] = $image['description'];
					db_insert($module_name.'_file_managed')->fields($file_managed_table_entry)->execute();
				}
			}
		}else{
			//FIXME
// 			error about didn't saved...
// die('fehlfer');
			$img_ids[$fkey] = 'Error! File not saved!';
		}
	}
	return $img_ids;
}

/**
 *
 * @param string $module
 * @param int $id
 */
function observation_gallery_edit_existing_file_list($module=false, $id=false){

	if(!$module || !$id) return false;


	drupal_add_css(
			drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
	drupal_add_js(
			drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js');
	drupal_add_js(
			drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js');
	drupal_add_css(
			drupal_get_path('module', 'gallery') . '/css/divbox.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
	drupal_add_js(drupal_get_path('module', 'gallery') . '/js/divbox.js');
	drupal_add_js(
			drupal_get_path('module', 'gallery') . '/js/gallery.divbox.js');
	$types = _gallery_types();

	$subtypes = _gallery_get_subtypes($type, false);

	$header = array(
			'',
			t('Filename'),
			t('Title')
	);
	if ($subtypes)
		$header[] = t('Type');
	$header[] = t('Author');
	$header[] = t('Location');
	$header[] = t('Caption');
	$header[] = t('Operations');
	$header[] = t('Delete');

	$mediatype = 'medias';

	$images = gallery_images($mediatype, $module, $id, 'edit');
// die(print_r($images));
	$rows = array();
	foreach ($images[$mediatype] as $image) {

		if ($mediatype == "images"
				|| ($mediatype == "medias"
						&& ($image['filemime'] == "image/png"
								|| $image['filemime'] == "image/jpg"
								|| $image['filemime'] == "image/jpeg"
								|| $image['filemime'] == "image/gif"))) {

			$url_href = url(
					'gallery/' . $image['item_type'] . '/' . $image['item_id']
					. '/thumb/' . $image['id'] . '/gallery_large');
			$url_image = url(
					'gallery/' . $image['item_type'] . '/' . $image['item_id']
					. '/thumb/' . $image['id'] . '/gallery_mini');
			$extras = 'rel="lightbox"';
		} elseif ($mediatype == "sounds"
				|| ($mediatype == "medias"
						&& ($image['filemime'] == "audio/mpeg"))){

			$url_href = file_uri_target($image['uri']);
			$url_href = url('sites/default/files/' . $url_href);
			$url_image = url(
					drupal_get_path('module', 'gallery')
					. '/images/music_icon_mini.png');
			$extras = 'class="divbox_sound"';
		} elseif ($mediatype == "videos"
				|| ($mediatype == "medias"
						&& ($image['filemime'] == "video/mp4"))) {

			$url_href = file_uri_target($image['uri']);
			$url_href = url('sites/default/files/' . $url_href);
			$url_image = url(
					drupal_get_path('module', 'gallery')
					. '/images/video_icon_mini.png');
			$extras = 'class="divbox_video"';
		}

		$caption = '';
		if (array_key_exists($image['item_type'], $types)
				&& array_key_exists(
						'image_caption',
						$types[$image['item_type']]))
			$caption = call_user_func_array(
					$types[$image['item_type']]['image_caption'],
					array($image['item_id']));
		$editable = gallery_edit_access_callback(
				$image['item_type'],
				$image['item_id'],
				$image);
		$row = array();
		$row[] = '<a ' . $extras . ' href="' . $url_href . '"><img src="'
		. $url_image . '" alt="' . $image['title'] . '" /></a>';
		$row[] = $image['filename'];
		$row[] = $image['title'];
		if ($subtypes) {
			$stypes = array();
			foreach ($image['subtypes'] as $subtype)
				$stypes[] = $subtypes[$subtype];
			$row[] = implode(', ', $stypes);
		}
		$row[] = $image['author'];
		$row[] = $image['location'];
		$row[] = $caption;
		$row[] = $editable
		? l(
				t('edit'),
				'gallery/' . $image['item_type'] . '/' . $image['item_id']
				. '/' . $mediatype . '/edit/' . $image['id'],
				$camefrom
				? array(
						'query' => array(
								'camefrom' => $camefrom
						)
				) : array()) : '&nbsp;';
		$row[] = $editable
		? '<input type="checkbox" name="delete[' . $image['id']
		. ']" value="1" />' : '&nbsp;';
		$rows[] = $row;
	}

	if (!$rows)
		$form['empty'] = array(
				'#markup' => '<p>'
				. t('There are no ' . $mediatype . ' available yet.')
				. '</p>',
		);
	else
		$form['overview'] = array(
				'#theme' => 'table',
				'#header' => $header,
				'#rows' => $rows,
		);


	return $form;

}

/**
 * Delete gallery images
 * @param array $delete_arr
 * @return array
 * 			with img id's with true on success
 */
function observation_gallery_delete($delete_arr){

	if(!isset($delete_arr) || !is_array($delete_arr)) return false;
	$state = array();

	foreach ($delete_arr as $iid => $del) {
		if (!$del)
			continue;
		$image = gallery_image_load($iid);
		if (!gallery_edit_access_callback(
				$image['item_type'],
				$image['item_id'],
				$image)) {
			continue;
		}
		$state[$iid] = gallery_image_delete($image);
	}
	return $state;
}

function observation_gallery_get_files($module, $item_id) {
	return db_query(
			'SELECT fm.*, mfm.description FROM {'.$module.'_file_managed} mfm
			JOIN {file_managed} fm ON fm.fid=mfm.file_managed_fid
			WHERE '.$module.'_id = :item_id',
			array(':item_id' => $item_id))->fetchAllAssoc('fid');

}

function observation_gallery_get_file($fid) {
	return (object) db_query(
			'SELECT * FROM {file_managed} WHERE fid = :fid',
			array(':fid' => $fid))->fetchAssoc();

}

function observation_gallery_files_table($modulename = false, $item_id = false, $edit = false) {
	global $user;
	$render_array = array();
	if (!$modulename || !$item_id) return $render_array;

	$files = observation_gallery_get_files($modulename, $item_id);
	if(count($files) < 1) return $render_array;

	foreach ($files as $record) {
		$mimeIconMap = array(
		//FIXME
		//why are these images in the inventory module???
				'path' => drupal_get_path('module', 'inventory') . '/images/'
				. commonstuff_get_icon_map($record->filemime) . '.png',
				'alt' => $record->filemime,
				'title' => $record->filemime,
		);
		$img_mime = theme('image', $mimeIconMap);

		$description = isset($record->description) ? $record->description : '';
		$filename = l(
				$record->filename,
				"sites/default/files/naturvielfalt/$modulename/" . $id . '/'
				. $record->filename);
		$filemime = $img_mime;
		$filesize = ($record->filesize / 1000) . " KB";
// 		$timestamp = date('d.m.Y', $record->timestamp);
// 		$author = isset($record->author) ? $record->author : '';
		$edit ? $desc = '<input type="text" name="file_description['.$record->fid.']" id="file_description['.$record->fid.']" value="'.$description.'">' : $desc = $description;
		$row = array();
		$row[] = $filemime;
		$row[] = $filename;
		$row[] = $desc;
		$row[] = $filesize;
// 		$row[] = $timestamp;
// 		$row[] = '<input type="text" name="file_author['.$record->fid.']" id="file_author['.$record->fid.']" value="'.$author.'">';
// 		$row[] = $user->name;
		if($edit) $row[] = '<input type="checkbox" value="1" name="file_delete['.$record->fid.']"';
		$rows[] = $row;
	}

	$tableHeader[] = t('Typ');
	$tableHeader[] = t('Filename');
	$tableHeader[] = t('Description');
	$tableHeader[] = t('Size');
// 	$tableHeader[] = t('Date');
// 	$tableHeader[] = t('Author');
// 	$tableHeader[] = t('Owner');
	if($edit) $tableHeader[] = t('Delete');

	$render_array = array(
			'#type' => 'fieldset',
			'#title' => t('Files'),
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
			),
	);

	$render_array['data'] = array(
			'#theme' => 'table',
			'#header' => $tableHeader,
			'#rows' => $rows,
			'#tableWidth' => 900,
			'#id_table' => 'files',
	);
	return $render_array;
}

/**
 * Delete a file managed
 * IMPORTANT: acl not checked
 * @param array $file_managed_ids
 * @param string $module
 */
function observation_gallery_files_delete($file_managed_ids = array(), $module = false){
	if(!is_array($file_managed_ids) || count($file_managed_ids) < 1
			 || !$module) return false;
	foreach($file_managed_ids as $key => $value){
		db_delete($module.'_file_managed')->condition('file_managed_fid', $key)->execute();
		file_delete(observation_gallery_get_file($key));
	}

}

function observation_gallery_file_update($file = array()){
	db_update('file_managed')->fields($file)->condition('fid', $file['fid'])->execute();
}

function observation_gallery_file_updateComment($module,$file_managed_descriptions){
	if(!isset($module) || !isset($file_managed_descriptions)
			|| empty($module) || !is_array($file_managed_descriptions)) return false;

	foreach ($file_managed_descriptions as $key => $value){
		$value = check_plain($value);
		$key = intval($key);
		db_update($module.'_file_managed')->fields(array('description' => $value))
					->condition('file_managed_fid', $key)->execute();
	}
}










