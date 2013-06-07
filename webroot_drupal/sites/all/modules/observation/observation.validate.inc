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
	$error = false;
	/**
	 * Check the date
	 */
	if(!observation_validate_date($form_state['values']['group']['date'])) {
		form_set_error('group][date', t('Please enter a valid date (dd.mm.yyyy)'));
		$error = true;
	}
	/**
	 * Check the count value
	 */
	if(!observation_validate_count($form_state['values']['group']['count'])) {
		form_set_error('group][count', t('The count has to be a number and bigger than zero'));
		$error = true;
	}
	/**
	 * Check the organism value
	 */
	if(!observation_validate_organism($form_state['values']['group']['organism'])) {
		form_set_error('group][organism', t('Please enter an existing organism.'));
		$error = true;
	}
	/**
	 * check the coordinate
	 */
// 	if(!observation_validate_coordinate($_POST['coordinate'])) {
// 		form_set_error('observation_form', t('Please mark a point on the map'));
// 	}
	/**
	 * set coordinates 
	 */
	if($error && isset($_POST['coordinate'])) {
	}
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
		$day = intval($date_parts[0]);
		$month = intval($date_parts[1]);
		$year = intval($date_parts[2]);
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
 * Attributes Validation
 *
 * @param array $input
 * @return true if valid attributes
 */
function observation_validate_attributes ($input) {
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
				$errormessages['group][determination_method_id'] = t('Artgroup with this determination method not allowed');
			}
		}
	}
}
