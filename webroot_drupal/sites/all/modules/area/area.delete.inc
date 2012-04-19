<?php
/**
 * @file area.delete.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');

/**
 * Return a form which asks the user if he really wants to delete the area.
 * 
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 * @param array $areadata
 * @return array Drupal form
 */
function area_delete_form($form, &$form_state, $areadata = NULL) {
	assert(!empty($areadata));
	$form_state['area_data'] = $areadata;

	return confirm_form(
		$form,
		t('Delete @areaname?', array('@areaname' => $areadata['name'])),
		'area/' . $areadata['id'] . '/show',
		t(
			'Are you sure you want to delete the area @areaname?',
			array('@areaname' => $areadata['name'])),
		t('Delete'),
		t('Cancel'));
}

/**
 * Delete area and its area_surface.
 *
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 */
function area_delete_form_submit($form, &$form_state) {
	$areadata = $form_state['area_data'];
	$num_deleted_area = db_delete('area')->condition('id', $areadata['id'])
		->execute();
	assert($num_deleted_area == 1);
	$num_deleted_geometry = db_delete('area_surface')->condition(
			'id',
			$areadata['area_surface_id'])
		->execute();
	assert($num_deleted_geometry == 1);

	$form_state['redirect'] = 'areas/show';
}
?>
