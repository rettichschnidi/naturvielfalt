<?php

function observation_save(){

	module_load_include('inc', 'organism', 'organism.artgroup');

	$values = $_POST;
	$error = false;
	$message = array();
	$output['post'] = $values;

	$organismn_id = $values['organismn_id'];
	if($organismn_id < 1 || empty($organismn_id)){
		$error = true;
		$message[] = t('The organism is value not permitted');
	}

	/**
	 * Check the determation value
	 */
	$artgroup_id = $values['artgroup'];
	$determination_method_id = intval($values['determination_method_id']);
	$determination_methods_db = organism_artgroup_get_determation_methods($artgroup_id);
	$artgroup_id_checked = false;

	if($determination_method_id > 0){
		foreach ($determination_methods_db as $determination_method_db){
			if($determination_method_db->id == $determination_method_id){
				$artgroup_id_checked = true;
			}
		}
	}

	/**
	 * Check the count value
	 */
	$count = $values['count'];
	if($count < 1 || empty($count)){
		$error = true;
		$message[] = t('The count should be a number and bigger than zero');
	}

	/**
	 * Check the date
	 */
	$date = $values['date'];
	$date_split = explode('.', $date);
	if(empty($date) || !checkdate($date_split[1], $date_split[0], $date_split[2])){
		$error = true;
		$message[] = t('Please enter a valid date');
	}

	/**
	 * Check if the all attributes are permitted
	 */
	$attributes = $values['attributes'];
	$attributes_checked = array();
	foreach ($attributes as $key => $value){
// 		$output['attro'][] = $key.': '.$value;
		$attribute = organism_artgroup_get_attribute($key);
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
					if(array_search($value, $attribute['values'])){
						$attributes_checked[$key] = $value;
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





		// Set the return statement
		$output['success'] = true;
	}
// 	foreach ($message as $messag){





		$output['message'] = $message;
// 	}

	return drupal_json_output($output);
	return print_r($output);
}


?>