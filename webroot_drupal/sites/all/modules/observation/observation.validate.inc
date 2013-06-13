<?php 
/**
 * @file observation_validate.inc
 * @author André Kälin, 2012
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
	/**
	 * Check the count value
	 */
	if(!observation_validate_count($form_state['values']['group']['count'])) {
		form_set_error('group][count', t('The count has to be a number and bigger than zero'));
	}
	/**
	 * Check the organism value
	 */
	if(!observation_validate_organism($form_state['values']['group']['organism'])) {
		form_set_error('group][organism', t('Please enter an existing organism.'));
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
 * Count Validation
 *
 * @param string $input
 * @return true if $input is a positive integer
 */
function observation_validate_count($input) {
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
function observation_validate_determination_method ($input) {
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
		$determination_methods_db = organism_artgroup_get_determation_methods($observation_id, $artgroup_id);
		$artgroup_id_checked = false;
	
		if ($determination_method_id > 0) {
			foreach ($determination_methods_db as $determination_method_db) {
				if ($determination_method_db->id == $determination_method_id) {
					$artgroup_id_checked = true;
				}
			}
			if (!$artgroup_id_checked) {
				$errormessages['group][determination_method_id'] = t('Artgroup with this determination method not allowed');
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
