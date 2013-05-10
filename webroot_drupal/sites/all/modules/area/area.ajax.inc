<?php
/**
 * @file area.ajax.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */
module_load_include('inc', 'area', 'area.create');
module_load_include('inc', 'area', 'area.show');
module_load_include('inc', 'area', 'area.delete');

/**
 * Renders a form which allows the user to submit an area name.
 * For usage with ajax.
 */
function area_get_new_areaname_form_callback() {
	print drupal_render(drupal_get_form('area_new_areaname_form'));
}

/**
 * Renders a form which allows the user to submit an area name.
 * For usage with ajax.
 */
function area_overview_ajax_callback($areadata = NULL) {
	print drupal_render(drupal_get_form('area_overview_form', $areadata));
}

function area_show_ajax_callback($areadata = NULL) {
	$output['map'] = array(
			'#search' => true,
			'#create' => true,
			'#showall' => true,
			'#theme' => 'area'
	);

	$output['button'] = array(
			'#type' => 'submit',
			'#value' => t('Close')
	);

	return $output;
}

/**
 * Deletes a set of areas. The parameter "areas" is a
 * list of all areas to delete separated with a coma and brackets:
 * {1,23,443,4343)
 */
function area_delete() {
	global $user;

	if ($user->uid == 0) {
		$output['success'] = false;
		$output['type'] = 'error';
		$output['message'] = t('Please log in first');
		return drupal_json_output($output);
	}

	if (!isset($_REQUEST['areas'])) {
		$output['success'] = false;
		$output['type'] = 'error';
		$output['message'] = t('Parameter \'areas\' missing');
		return drupal_json_output($output);
	}

	$param = filter_xss($_REQUEST['areas']);
	$areas = explode(',', substr($param, 1, strlen($param) - 2));

	$deleteCount = 0;
	foreach ($areas as $area_id) {
		if (area_write_access_by_id($area_id)) {
			area_delete_by_id($area_id);
			$deleteCount++;
		}
	}

	$output['success'] = true;
	if ($deleteCount == count($areas)) {
		$output['type'] = 'status';
		$output['message'] = ($deleteCount == 1)
		? t('Areas deleted')
		: t('Areas deleted');
	}
	else {
		$output['type'] = 'warning';
		$output['message'] = t('Some areas could not be deleted (unauthorized access)');
	}

	return drupal_json_output($output);
}
?>
