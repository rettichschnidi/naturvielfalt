<?php
/**
 * @file observation.ajax.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'observation');
module_load_include('inc', 'gallery', 'gallery.implementations');
module_load_include('inc', 'organism', 'organism.artgroup');
module_load_include('inc', 'observation', 'observation.delete');
module_load_include('inc', 'datatable');

/**
 * Save a existing/new observation
 * Post vars are used
 * @param array $observation_data
 * 				not implemented, maybe in the future
 * @param unknown_type $from_api
 * 				set true for handling request from the webservice api
 */
function observation_save($observation_data = false, $custom_data = false, $from_api = false) {
	global $user;
	
	$values = $_POST;

// 	if ($custom_data) {
// 		$values = $custom_data;
// 	}

	if($from_api) $values = $custom_data;
	
	debug($values);

	$errormessages = array();
	$observation = array();
	$observation['m_time'] = time();
	if (isset($values['observation_id'])) {
		$observation['id'] = $values['observation_id'];
	}
	else $observation['id'] = 0;
	if (isset($values['area_geometry_id'])) {
		$observation['area_geometry_id'] = $values['area_geometry_id'];
	}

	/**
	 * check the organism id
	 */
// 	if (!organism_get(filter_xss($values['organismn_id']))) {
// 		$errormessages[] = t('This organism is not permitted!');
// 	} else {
// 		$observation['organism_id'] = filter_xss($values['organismn_id']);
// 	}


	$result = organism_get_byname(filter_xss($values['organism_name']));
	
	$organism = filter_xss($values['organism']);
	
	$observation['organism_id'] = $result['id'];
	
// 		if (is_bool((bool) $values['observation_found_as_lang'])) {
// 			$observation['found_as_lang'] = $values['observation_found_as_lang'];
// 		}
// 		if (is_bool((bool) $values['observation_found_as_latin'])) {
// 			$observation['found_as_lat'] = $values['observation_found_as_latin'];
// 		}
	
	/**
	 * Check the determation and artgroup value
	 */
	// prevent wrong artgroup, set it each time automatically
	// set the last value as artgroup
	// $artgroup_id = $observation['organism_artgroup_id'] = intval($values['artgroup']);
	// if($observation['organism_artgroup_id'] == 0){
	if (array_key_exists('organism_id', $observation) && $observation['organism_id'] > 0) {
		$observation['organism_artgroup_id'] = organism_artgroup_get_organsim_in_artgroup(
			$observation['organism_id'],
			true);
	}

	if (!isset($values['determination_method_id']))
		$values['determination_method_id'] = 0;
	$determination_method_id = $observation['organism_artgroup_detmethod_id'] = intval(
		$values['determination_method_id']);
	if (isset($artgroup_id) && $artgroup_id > 0) {
		$determination_methods_db = organism_artgroup_get_determation_methods($artgroup_id);
		$artgroup_id_checked = false;

		if ($determination_method_id > 0) {
			foreach ($determination_methods_db as $determination_method_db) {
				if ($determination_method_db->id == $determination_method_id) {
					$artgroup_id_checked = true;
				}
			}
			if (!$artgroup_id_checked) {
				$errormessages[] = t('Artgroup with this determination method not allowed');
			}
		}
	}

	/**
	 * Check observer
	 */
	$observation['observer'] = check_plain($values['observer']);
	$observation['users_uid'] = isset($values['set_user_uid']) && $values['set_user_uid'] ? $values['set_user_uid']
			: $user->uid;

	/**
	 * Check the count value
	 */
	$count = (integer) $observation['count'] = $values['count'];
	if ($count < 1 || empty($count) || !is_int($count)) {
		$errormessages[] = t('The count has to be a number and bigger than zero');
	}

	/**
	 * Check the date
	 */
	$date = $values['date'];
// 	$date_split = explode('.', $date);
	$date_split = explode('/', $date);
	$observation['date'] = mktime(null, null, null, $date_split[1], $date_split[0], $date_split[2]);
	if (empty($date) || !checkdate($date_split[1], $date_split[0], $date_split[2])) {
		$errormessages[] = t('Please enter a valid date');
	}

	/**
	 * Check if all attributes are permitted
	 */
	$attributes = isset($values['attributes']) ? $values['attributes'] : array();
	$attributes_checked = array();
	foreach ($attributes as $key => $value) {
		$attribute = organism_artgroup_get_attribute($key);

		$attribute = $attribute[0];
		switch ($attribute['type_format']) {
		case 'textarea':
		case 'string':
			$value = check_plain($value);
			if (!empty($value)) {
				$attributes_checked[$key] = $value;
			}
			break;
		case 'dropdown':
			if ($value != '0') {
				if (array_key_exists($value, $attribute['values'])) {
					$attributes_checked[$key] = 'db_id-->' . $value;
				} else {
					$errormessages[] = t('The value of ' . $attribute['name'] . ' not permitted');
				}
			}
			break;
		case 'date':
			$value_split = explode('.', $value);

			if (count($value_split) == '3'
					&& checkdate(
						intval($value_split[1]),
						intval($value_split[0]),
						intval($value_split[2]))) {
				$attributes_checked[$key] = $value;
			} else {
				if ($value != "") {
					$errormessages[] = t('The value of ' . $attribute['name'] . ' should be a date');
				}
			}
			break;
		case 'int':
			$value = intval($value);
			if (is_int($value) && !empty($value)) {
				$attributes_checked[$key] = $value;
			} elseif ($value != "") {
				$errormessages[] = t('The value of ' . $attribute['name'] . ' should be a number');
			}
			break;
		}
		// 		$output['attr'][] = $attribute;
	}

	/**
	 * check the map data
	 */

	if (filter_xss($values['coordinate']) == "") {
		$errormessages[] = t('Please mark a point on the map');
	}

	$geometry['zip'] = intval(filter_xss($values['zip']));
	$geometry['canton'] = filter_xss($values['canton']);
	$geometry['country'] = filter_xss($values['country']);
	$geometry['township'] = filter_xss($values['township']);
	$geometry['altitude'] = intval(filter_xss($values['altitude']));
	$geometry['area_points'] = filter_xss($values['coordinate']);
	$geometry['type'] = 'marker';

	debug($errormessages);
	/**
	 * Save to the DB
	 */
	if (empty($errormessages)) {
		
		debug('lets save the shizzle');
		/**
		 * save the observation
		 */
		$transaction = db_transaction();
		// Yes, it's by far too much for a try element - but adding this try clause when the rest is already
		// done leads to such smells. :( At least there is no loop in it :)
		// Feel free to refactor it.
		try {
// 			if ($observation['id']) {
			if ($observation['id'] > 0) {
				// do update
				area_update_geometry($observation['area_geometry_id'], $geometry);
				observation_save_attribute_values($attributes_checked, $observation['id']);
				db_update('observation')->fields($observation)
					->condition('id', $observation['id'])
					->execute();
				$output['update'] = true;
			} else {
				// $geometry_id = area_save_geometry_blablablab();
				$observation['area_geometry_id'] = area_create_geometry($geometry);

				//save the observation
				$observation_id = $observation['id'] = db_query(
					"SELECT nextval('{observation}_id_seq') as nextval")->fetchField();
				$observation['c_time'] = $observation['m_time'];
				$observation['acl_id'] = new_acl('observation', $observation['users_uid']);
				db_insert('observation')->fields($observation)
					->execute();
				observation_save_attribute_values($attributes_checked, $observation['id']);
				
				// Read access for ALL group
				add_group_to_acl($observation['acl_id'], variable_get('ACL_GROUP_ALL_ID'), variable_get('ACL_READ'));
			}

			/**
			 * Medias
			 */

			$image = array(
					'item_id' => $observation['id'],
					'author' => $observation['observer'],
					'location' => $geometry['township'] . ' ' . $geometry['canton'],
			);
			
			gallery_implementation_save_manual('observation', $image, 'observation_id');

			if (isset($values['file_description'])) {
				gallery_implementation_file_updateComment(
					'observation',
					$values['file_description']);
			}

			if (isset($values['delete'])) {
				gallery_implementation_delete($values['delete']);
			}

			if (isset($values['file_delete'])) {
				gallery_implementation_files_delete($values['file_delete'], 'observation');
			}
			debug('success');
			// Set the return statement
			$output['success'] = true;
		} catch (Exception $e) {
			$transaction->rollback();
			watchdog_exception('observation', $e);
			$output['success'] = false;
			$output['message'] = array(
					t('Could not save the observation because of an unknown error.')
			);
			debug($e->getMessage());
		}
	} else {
		$output['success'] = false;
		$output['message'] = $errormessages;
	}

	//
	if ($from_api) {
		return $output;
	}

	return drupal_json_output($output);

}

/**
* Exports a set of observations.
* The parameter "columns" is an array of the column indexes to export.
* The parameter "observations" is an array of all observation ids to export.
*/
function observation_export() {
	// check if user is logged in
	global $user;
//	if (!isset($user)) {
	if ($user->uid == 0) {
		$output['success'] = false;
		$output['type'] = 'error';
		$output['message'] = t('Please log in first');
		return drupal_json_output($output);
	}
	
	// set header information
	header('Content-type: text/plain');
	header('Content-Disposition: attachment; filename="observations.csv"');
	
	// source information in first row
	echo '"' . t('This export originates from Naturvielfalt, a platform initiated by Naturwerk Brugg www.naturwerk.info') . '"' . "\n";
	
	// get header data
	$headers = observation_get_standard_datatable_headers();
	if (isset($_REQUEST['col'])) {
		if (is_array($_REQUEST['col'])) {
			$adummy = $_REQUEST['col'];
			$columns = array();
			for ($i = 0; $i < count($adummy); $i++)
				$columns[] = filter_xss($adummy[$i]);
		}
		else {
			$columns = filter_xss($_REQUEST['col']);
		}
	}
	else {
		// if no columns are set, export all columns
		$columns = array();
		for ($i = 0; $i < count($headers); $i++)
			$columns[] = $i;
	}
	// echo header names in a csv format, get DB field names in the order of the output
	$dbFields = array();
	$csv_output = '';
	foreach ($columns as $column) {
		if (!isset($headers[$column]['noSort']) || !$headers[$column]['noSort']) {
			$dbFields[] = $headers[$column]['dbfield'];
			$csv_output .= '"' . $headers[$column]['name'] . '",';
		}
	}
	// loop through $headers (again) to find columns which we always export
	// didn't do it in $columns loop in order to add these always exported columns at the
	// end and don't mix up $dbFields order
	foreach ($headers as $headerIdx => $header) {
		if (isset($header['exportAlways']) && $header['exportAlways'] && !in_array($headerIdx, $columns)) {
			$dbFields[] = $header['dbfield'];
			$csv_output .= '"' . $header['name'] . '",';
		}
	}
	//echo substr($csv_output, 0, -1) . "\n";
	$ausgabe = substr($csv_output, 0, -1) . "\n";
	
	// get all observation data, and echo requested columns in a csv format
	// if no observation ids are set, get all observations
	// observation_get_tabledata checks for read access
/*	$observation_ids = (isset($_REQUEST['oid']))
		? filter_xss($_REQUEST['oid'])
		: NULL;
*/	
	$observation_ids = NULL;	// default value if no ids are set
	if (isset($_REQUEST['oid'])) {
		if (is_array($_REQUEST['oid'])) {
			$adummy = $_REQUEST['oid'];
			$observation_ids = array();
			for ($i = 0; $i < count($adummy); $i++)
				$observation_ids[] = filter_xss($adummy[$i]);
		}
		else {
			$observation_ids = filter_xss($_REQUEST['oid']);
		}
	}
	
	$observationdata = observation_get_tabledata('ACL_READ', $observation_ids);
	foreach ($observationdata['rows'] as $observationrow) {
		$csv_output = '';
		foreach ($dbFields as $dbField) {
			$feld = mb_convert_encoding($observationrow['cell'][$dbField], 'pass', 'HTML-ENTITIES');
			$feld = str_replace('"', "''", $feld); // replace double quotes with two single quotes to make it look the same and still be csv compatible
			//$feld = str_replace(";", "", $feld); // if exported with IE then Excel doesn't like semikolons
			$csv_output .= '"' . $feld . '",';
		}
		$ausgabe .= substr($csv_output, 0, -1) . "\n";
	}

	$ausgabe = mb_convert_encoding($ausgabe, 'pass', 'UTF-8');
	echo $ausgabe;
}

/**
 * Deletes a set of observations. The parameter "observations" is a 
 * list of all observations to delete separated with a coma and brackets:
 * {1,23,443,4343)
 */
function observation_delete() {
	global $user;
//	if (!isset($user)) {
	if ($user->uid == 0) {
		$output['success'] = false;
		$output['type'] = 'error';
		$output['message'] = t('Please log in first');
		return drupal_json_output($output);
	}

	if (!isset($_REQUEST['observations'])) {
		$output['success'] = false;
		$output['type'] = 'error';
		$output['message'] = t('Parameter \'observations\' missing');
		return drupal_json_output($output);
	}

	$param = filter_xss($_REQUEST['observations']);
	$observations = explode(',', substr($param, 1, strlen($param) - 2));

	$deleteCount = 0;
	foreach ($observations as $observation_id) {
		if (observation_write_access_by_id($observation_id)) {
			observation_delete_by_id($observation_id);
			$deleteCount++;
		}
	}
	
	$output['success'] = true;
	if ($deleteCount == count($observations)) {
		$output['type'] = 'status';
		$output['message'] = ($deleteCount == 1)
			? t('Observation deleted')
			: t('Observations deleted');
	}
	else {
		$output['type'] = 'warning';
		$output['message'] = t('Some observations could not be deleted (unauthorized access)');
	}
	
	return drupal_json_output($output);

}

/**
 * Fetches all observation information and formats the rows for display.
 * Gets all observations if no $observation_id is given
 * 
 * @param string $acl_level
 * @param array(int) $observation_ids
 */
function observation_get_tabledata($acl_level = 'ACL_READ', $observation_ids = NULL) {
	global $user;
	global $language;
	
	$arguments = array();
	
	// if the search is for a date, convert it to timestamp to find the results in the db
	if (isset($_REQUEST['query'])) {
		$date_split = explode('.', $_REQUEST['query']);
		if (count($date_split) == 3 && checkdate($date_split[1], $date_split[0], $date_split[2])) {
			// cast to string because datatable_dbRequest checks for query to be a string
			$_REQUEST['query'] = (string) mktime(
			null,
			null,
			null,
			$date_split[1],
			$date_split[0],
			$date_split[2]);
		}
	}
	
	$langcode = (isset($user->language) && !empty($user->language))
		? $user->language
		: $language->language;
	
	$sql['dbColumns'] = array(
		'o.id AS id',
		'o.organism_artgroup_id AS artgroup_id',
		'o.observer AS observer',
		'o.date AS date',
		'o.count AS count',
		'o.acl_id AS acl_id',
		'o.organism_id AS organism_id',  // used to find portrait-pictures
		'a_s.altitude AS altitude',
		'a_s.township AS township',
		'a_s.zip AS zip',
		'a_s.canton AS canton',
		'a_s.country AS country',
		'ST_X(ST_Centroid(st_transform(a_s.geom, 21781))) AS ch1903_center_x',  // Center of the geometry - CH1903, x value
		'ST_Y(ST_Centroid(st_transform(a_s.geom, 21781))) AS ch1903_center_y',  // Center of the geometry - CH1903, y value
		'org.scientific_name AS organism_sc',
		'ol.name AS organism_l',
		'oa.name AS artgroup_n',
		// FIXME get more attributes than just Bemerkung
		'oattr.value AS comment'
	);
	
	$sql['dbSearchColumns'] = array(
		'o.id',
		'o.observer',
		'o.date',
		'o.count',
		'o.organism_id',
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
	
	$sql['dbJoins'] = 'INNER JOIN {area_geometry} a_s ON a_s.id = o.area_geometry_id'
		//.' LEFT JOIN {organism_synonym} osy ON osy.organism_id = o.organism_id '
		.' LEFT JOIN {organism_lang} ol ON ol.organism_id=o.organism_id AND ol.languages_language=\'' . db_escape_field($langcode) . '\''
		.' JOIN {organism} org ON org.id=o.organism_id'
		.' LEFT JOIN {organism_artgroup} oa ON oa.id = o.organism_artgroup_id'
		// FIXME get more attributes than just Bemerkung
		.' LEFT JOIN {observation_attribute} oattr ON oattr.observation_id=o.id AND oattr.organism_artgroup_attr_id=2'
	;
	
	if ($observation_ids != NULL) {
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
	
	//$sql['dbGroupBy'] = 'h.id, h.label, h.name';
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
	
	/**
	 * fetch the observation data
	 */
	$output = datatable_dbRequest($sql, $arguments);
	
	/**
	 * format output
	 */
	global $base_url;
	$imageBaseUrl = $base_url . '/' . drupal_get_path('module', 'observation') . '/img/';
	$iconBaseUrl = '/' . path_to_theme() . '/images/icons/enabled/';
	
	foreach ($output['rows'] as &$row) {	 	
		// select box
		$row['cell']['selectColumn'] = '<input class="gridSelect" type="checkbox" value="' . $row['id'] . '" title="' . t('Select') . '" onclick="javascript:observationmap.selectMultipleObservation(event, ' . $row['id'] . ', !this.checked);" />';
		// show details
		$row['cell']['showColumn'] = l(
			'<img src="' . $iconBaseUrl . 'Zoom.png" alt="' . t('Show') . '" title="' . t('Show') . '" />',
			$base_url . '/observation/' . $row['id'] . '/show',
			array(
				'html' => true
			));;
		// edit details
		$row['cell']['editColumn'] = (observation_write_access_by_acl_id($row['cell']['acl_id']))
			? l(
				'<img src="' . $iconBaseUrl . 'Modify.png" alt="' . t('Edit') . '" title="' . t('Edit') . '" />',
				$base_url . '/observation/' . $row['id'] . '/edit',
				array(
					'html' => true
				))
			: '';
		
		// zoom in
		$row['cell']['zoominColumn'] = '<img src="' . $iconBaseUrl . 'Zoom in.png" style="cursor: pointer;" alt="' . t('zoomin') . '" title="' . t('zoomin') . '" onclick="javascript:observationmap.zoomIn(event, ' . $row['id'] . ');window.scrollTo(0,0);" />';
				
		// translated organism name
		if (empty($row['cell']['organism_l']))
			$row['cell']['organism_l'] = t('No translation available');
		// date, TODO: time
		$row['cell']['date'] = date('d.m.Y H:i:s', $row['cell']['date']);
		// coordinates
		$row['cell']['ch1903_center_x'] = number_format($row['cell']['ch1903_center_x'], 0, '.', '\'');
		$row['cell']['ch1903_center_y'] = number_format($row['cell']['ch1903_center_y'], 0, '.', '\'');
		
		// get images
		$images = gallery_images(GALLERY_MEDIATYPE_IMAGE, 'observation', $row['id']);
		// number of images
		$row['cell']['imagescount'] = $images['count'];
		
		// thumbnail
		$row['cell']['thumbnail'] = '';
		if ($images['count'] > 0) {
			// display the thumbnail (for the table view)
			$row['cell']['thumbnail'] = '<img src="' . url('gallery/observation/' . $row['id'] . '/thumb/' . $images['images'][0]['id'] . '/gallery_thumbnail') . '" alt="' . $images['images'][0]['title'] . '" height="80" width="80" />';
		}
		
		// number of videos
		$row['cell']['videoscount'] = gallery_videos_count('observation', $row['id']);
		
		// number of sounds
		$row['cell']['soundscount'] = gallery_sounds_count('observation', $row['id']);
		
		// number of documents
		$row['cell']['documentscount'] = observation_documents_count($row['id']);
		
		/**
		 * create the html source for the gallery view
		 */
		$row['cell']['gallery_image'] = datatable_get_gallery_image(
			$row['id'],
			$row['cell']['organism_id'],
			array(
				'icons' => array(
					'<img src="' . $imageBaseUrl . '/pin.png" style="cursor: pointer;" title="' . t('Position') . '" onclick="javascript:observationmap.selectObservation(' . $row['id'] . ');window.scrollTo(0,0);">',
					$row['cell']['zoominColumn'],
					$row['cell']['showColumn'],
					$row['cell']['editColumn'],
					$row['cell']['selectColumn']
				),
				'data' => array(
					$row['cell']['artgroup_n'],
					l(
						$row['cell']['organism_sc'],
						'organism/' . $row['cell']['organism_id'],
						array(
							'attributes' => array(
								'style' =>
									'color: black;
									text-decoration: underline;
									font-style: italic;')
						)),
					$row['cell']['organism_l']
				)
			),
			$row['cell']['acl_id'],
			DATATABLE_IMGSRC_BELEG,
			$images);
	}
			
	return $output;
}

/**
 * This method handle the request from flexigrid
 * @return JSON
 */
function observation_get_all_json($acl_level = 'ACL_READ') {
	$output = observation_get_tabledata($acl_level);
	return drupal_json_output($output);
}

/**
 * Returns the JSON data of geometries. If $observation_data is false, all geometries
 * are returned.
 * 
 * @param array $observation_data
 */
function observation_get_geometry_json($observation_data = false) {
	if (!$observation_data && isset($_REQUEST['observation_ids']))
		$observation_data = $_REQUEST['observation_ids'];
	drupal_json_output(observation_get_geometries_acl_checked($observation_data));
}

/**
 * Renders a form which allows the user to submit an area name.
 * For usage with ajax.
 */
function observation_overview_ajax_callback($observation_data = false) {
	module_load_include('inc', 'observation', 'observation.show');
	print drupal_render(drupal_get_form('observation_map_overview_form', $observation_data));
}

