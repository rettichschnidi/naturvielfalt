<?php 

/**
 * Date validation
 *
 * @param string $input
 * @return true if $input is a valid date (dd.mm.yyyy)
 */
function area_validate_date ($input) {
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
 * Fileextension validation
 *
 */
function area_validate_fileextension($input) {
	return gallery_get_MediaInfo_by_filename($input);
}

/**
 * Time validation
 *
 * @param string $input
 * @return true if $input is a valid time (HH:mm)
 */
function area_validate_time ($input) {
 //matches hh:mm (00:00 - 23:59)
 $pattern = "@^(0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]$@";
 return preg_match($pattern,$input);
}

/**
 * Area Id Validation
 *
 * @param string $areaid
 * @return true if $areaid is the id an existing area
 */
function area_validate_area_id($areaid) {
	if(intval($areaid) == 0) return true;
	if(!ctype_digit($areaid)) return false;
	$result = area_get_full_dataset_of_specific_area(filter_xss($areaid));
	return !empty($result);
}

/**
 * Permission validation
 *
 * @param string $areaid
 * @return true if the current user has WRITE permissions for the given area
 */
function area_validate_permission($areaid) {
	if(intval($areaid) == 0) return true;
	if(!ctype_digit($areaid)) return false;
	$result = area_get_full_dataset_of_specific_area(filter_xss($areaid));
	return check_permission($result['acl_id'], variable_get('ACL_WRITE'));
}

/**
 * Area name Validation
 *
 * @param string $input
 * @return true if $input contains only alphanumeric characters
 */
function area_validate_area_name($input) {
	return is_string($input) && !empty($input);
}

/**
 * Coordinate Validation
 *
 * @todo validate coordinates from webservice! (Regex)
 * @param array $input
 * @return true if valid coordinates
 */
function area_validate_coordinates ($input) {
	return isset($input) && is_array($input) && !empty($input);
}

