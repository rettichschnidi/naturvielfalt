<?php
/**
 * Drupal hook for form validation
 *
 * @param array $form
 * @param array $form_state
 */
function inventory_form_validate($form, &$form_state) {
	
	/**
	 * Check the name
	 */
	if(!inventory_validate_name($form_state['values']['name'])) {
		form_set_error('name', t('Please enter a valid inventory name'));
	}
	
	/**
	 * Check the date
	 */
	if(!inventory_validate_date($form_state['values']['date'])) {
		form_set_error('date', t('Please enter a valid date (dd.mm.yyyy)'));
	}
	if(!inventory_validate_time($form_state['values']['time'])) {
		form_set_error('time', t('Please enter a valid time (hh:mm)'));
	}

	if(!inventory_validate_area($form_state['values']['area_id'])) {
		form_set_error('inventory_form', t('invalid area/permission denied'));
	}
	
	if(isset($_FILES['files'])) {
		foreach($_FILES['files']['name'] as $fkey => $filename) {
			if($_FILES['files']['name'][$fkey] != '') {
				$fileextension = array_pop(explode('.', $_FILES['files']['name'][$fkey]));
				if(!inventory_validate_fileextension($_FILES['files']['name'][$fkey])) {
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
function inventory_validate_fileextension($input) {
	return gallery_get_MediaInfo_by_filename($input);
}

/**
 * Name validation
 *
 * @param string $input
 */
function inventory_validate_name($input) {
	return isset($input);
}

/**
 * Date validation
 *
 * @param string $input
 * @return true if $input is a valid date (dd.mm.yyyy)
 */
function inventory_validate_date ($input) {
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
function inventory_validate_time ($input) {
	//matches hh:mm (00:00 - 23:59)
	$pattern = "@^(0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]$@";
	return preg_match($pattern,$input);
}

/**
 * Inventory Id Validation
 *
 * @param string $inventory_id
 * @return true if $inventory_id is an existing inventory
 */
function inventory_validate_inventory_id($inventory_id) {
	if(!ctype_digit($inventory_id)) return false;
	if(intval($inventory_id) == 0) return true;
	$result = inventory_get(filter_xss($inventory_id));
	return !empty($result);
}

/**
 * Area Id Validation
 *
 * @param string $area_id
 * @return true if area exists, and user has permission to add an inventory
 */
function inventory_validate_area($area_id) {
	module_load_include('inc', 'area', 'area.validate');
	if(!area_validate_area_id($area_id)) return false;
	//area must exist
	if (intval($area_id) == 0) return false;
	//check permission to add an inventory
	$area = area_get_full_dataset_of_specific_area($area_id);
	return area_check_access($area, 'ACL_RED_WRITE');
}

/**
 * Permission validation
 *
 * @param string $inventory_id
 * @return true if the current user has WRITE permissions for the given inventory
 */
function inventory_validate_permission($inventory_id) {
	if(!ctype_digit($inventory_id)) return false;
	if(intval($inventory_id) == 0) return true;
	return inventory_write_access_by_id(filter_xss($inventory_id));
}

