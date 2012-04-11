<?php

/**
 *
 */
function observation_save(){
global $user;
	module_load_include('inc', 'organism', 'organism.artgroup');

	$values = $_POST;
	$error = false;
	$message = array();
// 	$output['post'] = $values;
	$observation = array();
	$observation['m_time'] = time();

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
	$artgroup_id = $observation['organism_artgroup_id'] = intval($values['artgroup']);
// 	$observation['organism_artgroup_id'] = NULL;
	if($artgroup_id > 0){
		$determination_method_id = $observation['organism_artgroup_detmethod_id'] = intval($values['determination_method_id']);
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
				if(checkdate($value_split[1], $value_split[0], $value_split[2])){
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

// 	check_plain($text)

	if(!$error){
		/**
		 * Save to the DB
		 */


		/**
		 * Save the geometry and get the id
		 */
		// $geometry_id = area_save_geometry_blablablab();

		/**
		 * save the observation
		 */

		if($observation_id){
			// do update
		}else{
			$observation_id = $observation['id'] = db_query("SELECT nextval('observation_id_seq') as nextval")->fetchField();
			$observation['c_time'] = $observation['m_time'];
			$observation['acl_id'] = new_acl('singleobservation');
			db_insert('observation')->fields($observation)->execute();
		}


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
				db_insert('observation_attribute')->fields($entry)->execute();
// 				$output['db_attr'][] = $entry;

			}
			// Set the return statement
			$output['success'] = true;



	}
// 	foreach ($message as $messag){




// 		$output['db_fields'] = $observation;

		$output['message'] = $message;
// 	}

	return drupal_json_output($output);
	return print_r($output);
}


?>