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
 * @param string $input
 * @return true if $input is the id an existing area
 */
function area_validate_area_id($input) {
	if(!ctype_digit($input)) return false;
	$result = area_get_full_dataset_of_specific_area(filter_xss($input));
	return !empty($result);
}

/**
 * Area name Validation
 *
 * @param string $input
 * @return true if $input contains only alphanumeric characters
 */
function area_validate_area_name($input) {
	return ctype_alnum($input);
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

