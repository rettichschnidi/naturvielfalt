<?php 
/**
 * @file observation_validate.inc
 * @author André Kälin, 2013
 * @copyright 2013 Naturwerk, Brugg
 */

module_load_include('inc', 'organism', 'organism.artgroup');

/**
 * Drupal hook for form validation
 *
 * @param array $form
 * @param array $form_state
 */
function observation_form_validate($form, &$form_state) {
	/**
	 * Check the date
	 */
	if(!observation_validate_date($form_state['values']['group']['date'])) {
		form_set_error('group][date', t('Please enter a valid date (dd.mm.yyyy)'));
	}
	if(!observation_validate_time($form_state['values']['group']['time'])) {
		form_set_error('group][time', t('Please enter a valid time (hh:mm)'));
	}
	/**
	 * Check the count value
	 */
	if(!observation_validate_count($form_state['values']['group']['count'])) {
		form_set_error('group][count', t('The count has to be a number and bigger than zero'));
	}
	/**
	 * Check the organism value (only if organism is not unknown)
	 */
	if(!$form_state['values']['group']['unknown_check']) {
		if(!observation_validate_organism($form_state['values']['group']['organism'])) {
			form_set_error('group][organism', t('Please enter an existing organism.'));
		}
	}
	
	if($form_state['values']['group']['unknown_check']) {
		if(!observation_validate_artgroup_id($form_state['values']['group']['artgroup_id'])) {
			form_set_error('group][artgroup_id', t('Please select an artgroup other than "All".'));
		}
	}
	
	if(isset($form_state['values']['inventory_id'])) {
		if(!observation_validate_inventory($form_state['values']['inventory_id'])) {
			form_set_error('observation_form', t('Invalid inventory / permission denied'));
		}
	}
	/**
	 * check the coordinate
	 */
	if(!observation_validate_coordinate($_POST['coordinate'])) {
		form_set_error('observation_form', t('Please mark a point on the map'));
	}

	if(isset($_FILES['files'])) {
		foreach($_FILES['files']['name'] as $fkey => $filename) {
			if($_FILES['files']['name'][$fkey] != '') {
				$fileextension = array_pop(explode('.', $_FILES['files']['name'][$fkey]));
				if(!observation_validate_fileextension($_FILES['files']['name'][$fkey])) {
					form_set_error('media][upload]['.$fkey, t('.@extension is an unallowed fileextension!', array('@extension' => $fileextension)));
				}
			}
		}
	}
	
}

/**
 * Fileextension validation
 *
 */
function observation_validate_fileextension($input) {
	return gallery_get_MediaInfo_by_filename($input);
}
/**
 * Date validation
 *
 * @param string $input
 * @return true if $input is a valid date (dd.mm.yyyy)
 */
function observation_validate_date ($input) {
	$date_parts = explode('.', $input);
	if(count($date_parts) != 3) return false;
	else {
		$day = $date_parts[0];
		$month = $date_parts[1];
		$year = $date_parts[2];
		return checkdate($month, $day, $year);
	}
}

/**
 * Time validation
 *
 * @param string $input
 * @return true if $input is a valid time (HH:mm)
 */
function observation_validate_time ($input) {
 //matches hh:mm (00:00 - 23:59)
 $pattern = "@^(0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]$@";
 return preg_match($pattern,$input);
}

/**
 * Count Validation
 *
 * @param string $input
 * @return true if $input is a positive integer
 */
function observation_validate_count($input) {
	if(!ctype_digit($input)) return false;
	$count =  intval($input);
	return $count > 0;
}

/**
 * Organism Validation
 *
 * @param string $input
 * @return true if $input is an existing organism
 */
function observation_validate_organism($input) {
	isset($input) ?
	$result = organism_get_byname(filter_xss($input)) : array();
	
	return isset($result['id']) ?  true : false;
}

/**
 * artgroup Id Validation
 *
 * @param string $input
 * @return true if $input is the id an existing artgroup
 */
function observation_validate_artgroup_id($input) {
	if(!ctype_digit($input)) return false;
	if($input == 1) return false;
	if(intval($input) == UNKNOWN_ARTGROUP) return true;
	$result = organism_artgroup_get_artgroup(filter_xss($input), true);
	return !empty($result);
}

/**
 * organism Id Validation
 *
 * @param string $input
 * @return true if $input is the id an existing organism
 */
function observation_validate_organism_id($input) {
	if(!ctype_digit($input)) return false;
	if(intval($input) == UNKNOWN_SPECIES) return true;
	$result = organism_get(filter_xss($input));
	return !empty($result);
}

/**
 * Observation Id Validation
 *
 * @param string $observationid
 * @return true if $observationid is an existing observation
 */
function observation_validate_observation_id($observationid) {
	if(!ctype_digit($input)) return false;
	if(intval($observationid) == 0) return true;
	$result = observation_get(filter_xss($observationid));
	return !empty($result);
}

/**
 * Inventory Id Validation
 *
 * @param string $inventory_id
 * @return true if inventory exists, and user has permission to add an observation
 */
function observation_validate_inventory($inventory_id) {
	module_load_include('inc', 'inventory', 'inventory.validate');
	if(!inventory_validate_inventory_id($inventory_id)) return false;
	//inventory must exist
	if (intval($inventory_id) == 0) return false;
	return true;
	$inventory = inventory_get($inventory_id);
	//user must have access to the inventory
	return inventory_check_access($inventory, 'ACL_RED_WRITE');
}
 
/**
 * Permission validation
 *
 * @param string $observationid
 * @return true if the current user has WRITE permissions for the given observation id
 */
function observation_validate_permission($observationid) {
	if(!ctype_digit($observationid)) return false;
	if(intval($observationid) == 0) return true;
	$result = observation_get(filter_xss($observationid));
	return observation_write_access_by_acl_id($result['acl_id']);
}


/**
 * Coordinate Validation
 *
 * @todo validate coordinates from webservice! (Regex)
 * @param array $input
 * @return true if valid coordinates
 */
function observation_validate_coordinate ($input) {
	return isset($input) && $input != '';
}

/**
 * Determination Method Validation
 *
 * @param array $input
 * @return true if valid attributes
 */
function observation_validate_determination_method ($input, $artgroup_id) {
	if(!isset($input) || !isset($artgroup_id)) return false;
	if (isset($artgroup_id) && $artgroup_id > 0) {
		$determination_methods_db = organism_artgroup_get_determation_methods($observation_id, $artgroup_id);
		$artgroup_id_checked = false;
	
		if ($determination_method_id > 0) {
			foreach ($determination_methods_db as $determination_method_db) {
				if ($determination_method_db->id == $determination_method_id) {
					$artgroup_id_checked = true;
				}
			}
			if (!$artgroup_id_checked) {
				return false;
			}
		}
	}
}

/**
 * Check if all attributes are permitted
 */
function observation_validate_attributes ($input) {
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
}
