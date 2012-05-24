<?php
/**
 * @file observation.ajax.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'observation');
module_load_include('inc', 'gallery', 'gallery.implementations');
module_load_include('inc', 'organism', 'organism.artgroup');

/**
 * Save a existing/new observation
 * Post vars are used
 * @param array $observation_data
 * 				not implemented, maybe in the future
 * @param unknown_type $from_api
 * 				set true for handling request from the webservice api
 */
function observation_save($observation_data = false, $custom_data =false, $from_api = false){
global $user;

// die(print_r($_FILES));
// die(print_r($_POST));
	$values = $_POST;
	if($custom_data) $values = $custom_data;
	$error = false;
	$message = array();
// 	$output['post'] = $values;
	$observation = array();
	$observation['m_time'] = time();
	if(isset($values['observation_id'] )) $observation['id'] = $values['observation_id'];
	if(isset($values['area_geometry_id'] )) $observation['area_geometry_id'] = $values['area_geometry_id'];


	/**
	 * check the organism id
	 */
	$organismn_id = $observation['organism_id'] = $values['organismn_id'];
	if($organismn_id < 1 || empty($organismn_id)){
		$error = true;
		$message[] = t('The organism is value not permitted');
	}
	if(is_bool((bool) $values['observation_found_as_lang'])) $observation['found_as_lang'] = $values['observation_found_as_lang'];
	if(is_bool((bool) $values['observation_found_as_latin'])) $observation['found_as_lat'] = $values['observation_found_as_latin'];

	/**
	 * Check the determation and artgroup value
	 */
	// prevent wrong artgroup, set it each time automatically
	// set the last value as artgroup
// 	$artgroup_id = $observation['organism_artgroup_id'] = intval($values['artgroup']);
// 	if($observation['organism_artgroup_id'] == 0){
	if($organismn_id > 0) $observation['organism_artgroup_id'] = organism_artgroup_get_organsim_in_artgroup($observation['organism_id'], true);

// 	}
// 	$observation['organism_artgroup_id'] = NULL;
	if(!isset($values['determination_method_id'])) $values['determination_method_id'] = 0;
	$determination_method_id = $observation['organism_artgroup_detmethod_id'] = intval($values['determination_method_id']);
	if($artgroup_id > 0){
		$determination_methods_db = organism_artgroup_get_determation_methods($artgroup_id);
		$artgroup_id_checked = false;

		if($determination_method_id > 0){
			foreach ($determination_methods_db as $determination_method_db){
				if($determination_method_db->id == $determination_method_id){
					$artgroup_id_checked = true;
				}
			}
			if(!$artgroup_id_checked){
				$error = true;
				$message[] = t('Artgroup with this determination method not allowed');
			}
		}
	}

	/**
	 * Check observer
	 */
	$observation['observer'] = check_plain($values['observer']);
	$observation['users_uid'] = $user->uid;

	/**
	 * Check the count value
	 */
	$count = $observation['count'] = $values['count'];
	if($count < 1 || empty($count)){
		$error = true;
		$message[] = t('The count should be a number and bigger than zero');
	}

	/**
	 * Check the date
	 */
	$date = $values['date'];
	$date_split = explode('.', $date);
	$observation['date'] = mktime(null,null,null,$date_split[1], $date_split[0], $date_split[2]);
	if(empty($date) || !checkdate($date_split[1], $date_split[0], $date_split[2])){
		$error = true;
		$message[] = t('Please enter a valid date');
	}

	/**
	 * Check if all attributes are permitted
	 */
	$attributes = $values['attributes'];
	$attributes_checked = array();
	foreach ($attributes as $key => $value){
// 		$output['attro'][] = $key.': '.$value;
		$attribute = organism_artgroup_get_attribute($key);
// 		$output['org_attr'][] = $attribute;
		$attribute = $attribute[0];
		switch($attribute['type_format']){
			case 'string':
				$value = check_plain($value);
				if(!empty($value)){
					$attributes_checked[$key] = $value;
				}
				break;
			case 'dropdown':
				if($value != '0'){
					if(array_key_exists($value, $attribute['values'])){
						$attributes_checked[$key] = 'db_id-->'.$value;
					}else{
						$error = true;
						$message[] = t('The value of '.$attribute['name'].' not permitted');
					}
				}
				break;
			case 'date':
				$value_split = explode('.', $value);

				if(count($value_split) == '3'
						&& checkdate(intval($value_split[1]), intval($value_split[0]), intval($value_split[2]))){
					$attributes_checked[$key] = $value;
				}else{
					if($value != ""){
						$error = true;
						$message[] = t('The value of '.$attribute['name'].' should be a date');
					}
				}
				break;
			case 'int':
				$value = intval($value);
				if(is_int($value) && !empty($value)){
					$attributes_checked[$key] = $value;
				}elseif($value != ""){
					$error = true;
					$message[] = t('The value of '.$attribute['name'].' should be a number');
				}
				break;
		}
// 		$output['attr'][] = $attribute;
	}



	/**
	 * check the map data
	 */

	if(filter_xss($values['coordinate']) == ""){
		$error = true;
		$message[] = t('Please mark a point on the map');
	}

	$geometry['zip'] = intval(filter_xss($values['zip']));
	$geometry['canton'] = filter_xss($values['canton']);
	$geometry['country'] = filter_xss($values['country']);
	$geometry['township'] = filter_xss($values['township']);
	$geometry['altitude'] = intval(filter_xss($values['altitude']));
	$geometry['area_points'] = filter_xss($values['coordinate']);
	$geometry['type'] = 'marker';



	/**
	 * Check custom attributes
	 */
	$custom_names = array();
	if(isset($values['attributes_custom_names'])){
		$custom_names = $values['attributes_custom_names'];
		$custom_values = $values['attributes_custom_values'];
		if(count($custom_names > 0)){
			// Sometimes "Invalid argument supplied for foreach" don't know why... bö
			// supress warnings
			foreach ($custom_names as $key => $val){
				if($val == t('Please enter a name') && $custom_values[$key] == t('Please enter a value')) continue;
				if(empty($val) && empty($custom_values[$key])) continue;
				if($val == t('Please enter a name') || empty($val) ||
						$custom_values[$key] == t('Please enter a value')){
					$error = true;
					$message[] = t('Please enter for each custom attribute a text, leave both empty to ignore it');
				}
			}
		}
	}



// 	check_plain($text)

	if(!$error){
		/**
		 * Save to the DB
		 */


		/**
		 * add the new custom attributes to the db and the values to the attr check array
		 */
		if(count($custom_names > 0)){
			foreach ($custom_names as $key => $val){
				$attr_id = organism_artgroup_add_custom_attribute($val, $observation['organism_artgroup_id']);
				if($attr_id != false) $attributes_checked[$attr_id] = $custom_values[$key];
			}
		}

		/**
		 * save the observation
		 */
		if($observation['id']){
			// do update
			area_update_geometry($observation['area_geometry_id'], $geometry);
			observation_save_attribute_values($attributes_checked, $observation['id']);
			db_update('observation')->fields($observation)->condition('id', $observation['id'])->execute();
			$output['update'] = true;
		}else{
			// $geometry_id = area_save_geometry_blablablab();
			$observation['area_geometry_id'] = area_create_geometry($geometry);

			//save the observation
			$observation_id = $observation['id'] = db_query("SELECT nextval('{observation}_id_seq') as nextval")->fetchField();
// 			$observation_id = db_next_id('observation_id');
			$observation['c_time'] = $observation['m_time'];
			$observation['acl_id'] = new_acl('singleobservation');
			db_insert('observation')->fields($observation)->execute();
			observation_save_attribute_values($attributes_checked, $observation['id']);

		}

		/**
		 * Medias
		 */

		$image = array(
				'item_id' => $observation['id'],
				'author' => $observation['observer'],
				'location' => $geometry['township'].' '.$geometry['canton'],
		);
		gallery_implementation_save_manual('observation', $image, 'observation_id');

		if(isset($values['file_description'])){
			gallery_implementation_file_updateComment('observation', $values['file_description']);
		}

		if(isset($values['delete'])){
			gallery_implementation_delete($values['delete']);
		}

		if(isset($values['file_delete'])){
			gallery_implementation_files_delete($values['file_delete'], 'observation');
		}

		// Set the return statement
		$output['success'] = true;

	}else{
		$output['success'] = false;
		$output['message'] = $message;
	}

	//
	if($from_api){
		return $output;
	}

	return drupal_json_output($output);
	return print_r($output);
}


function observation_get_all_json(){
	global $user;

	// if the search is for a date, convert it to timestamp to find the results in the db
	if(isset($_REQUEST['query'])){
		$date_split = explode('.', $_REQUEST['query']);
		if(count($date_split) == 3 && checkdate($date_split[1], $date_split[0], $date_split[2])){
			$_REQUEST['query'] = mktime(null,null,null,$date_split[1], $date_split[0], $date_split[2]);
		}
	}


	$sql['dbColumns'] = array(
			'o.id AS id',
			'o.observer AS observer',
			'o.date AS date',
			'o.count AS count',
			'o.acl_id',
			'a_s.altitude AS altitude',
			'a_s.township AS township',
			'a_s.zip AS zip',
			'a_s.canton AS canton',
			'a_s.country AS country',
			'a_s.geom AS geom',
			'osn.name AS organism_sc',
			'ol.name AS organism_l',
			'oa.name AS artgroup_n'
	);

	$sql['dbSearchColumns'] = array(
			'o.id',
			'o.observer',
			'o.date',
			'o.count',
			'a_s.altitude',
			'a_s.township',
			'a_s.zip',
			'a_s.canton',
			'a_s.country',
			'a_s.geom',
			'osn.name',
			'ol.name',
			'oa.name',
	);

	$sql['dbTable'] = '{observation} o';
	$sql['dbJoins'] = 'INNER JOIN {area_geometry} a_s ON a_s.id = o.area_geometry_id '
						.'INNER JOIN {organism_scientific_name} osn ON osn.organism_id = o.organism_id '
						."LEFT JOIN {organism_lang} ol ON ol.organism_id=o.organism_id AND ol.languages_language='".db_escape_field($user->language)."' "
						.'LEFT JOIN {organism_artgroup} oa ON oa.id = o.organism_artgroup_id ';
	//$sql['dbGroupBy'] = 'h.id, h.label, h.name';
	$sql['id'] = 'id';
	$output = datatable_dbRequest($sql);

// 	// Add a link to the gallery of each habitat
// 	global $base_url;
// 	$imageUrl = $base_url . '/' . drupal_get_path('module', 'habitat') . '/images/image.png';
	foreach ($output['rows'] as &$row) {
// 		$link = l(
// 				"<img src='$imageUrl' />",
// 				base_path() . 'gallery/' . $row['id'] . '/images',
// 				array(
// 						'html' => true
// 				));
// 		$row['cell']['image_link'] = $link;
		if($row['cell']['organism_l'] == "") $row['cell']['organism_l'] = t('No translation available');
		$row['cell']['date'] = date('d.m.Y', $row['cell']['date']);
	}
	return drupal_json_output($output);

}

/**
 * Delete a custom attribute and all entries in all own observations
 * @param int $attribute_id
 */
function observation_delete_custom_attribute($attribute_id){
	if(!organism_artgroup_custom_attribute_exist($attribute_id))
		return drupal_json_output(array('success' => false));
	db_delete('observation_attribute')->condition('organism_artgroup_attr_id', $attribute_id)->execute();
	if(organism_artgroup_delete_custom_attribute($attribute_id))
		return drupal_json_output(array('success' => true));
	return drupal_json_output(array('success' => false));
}

function observation_get_geometry_json($observation_data = false){
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


















