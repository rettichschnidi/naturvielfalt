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
 * @param $form
 *  Drupal form
 * @param &$form_state
 *  An associative array containing the current state of the form.
 * @param $areadata
 * 	All data about the area to delete
 * @return
 * 	Drupal form
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
 * Delete area and its area_geometry.
 *
 * @param $form
 *  Drupal form
 * @param $form_state
 *  An associative array containing the current state of the form.
 */
function area_delete_form_submit($form, &$form_state) {
	$areadata = $form_state['area_data'];
	area_delete_by_id($areadata['id']);

	$form_state['redirect'] = 'areas/show';
}

/**
 * Deletes an area
 * @param $area_id
 */
function area_delete_by_id($area_id) {
	$transaction = db_transaction();
	try {
		$area = db_select('area', 'a')->condition('id', $area_id)
		->fields('a')
		->execute()
		->fetch();
		
		$num_deleted_area = db_delete('area')->condition('id', $area_id)
		->execute();
		assert($num_deleted_area == 1);
		
		$num_deleted_surfaces = db_delete('area_geometry')->condition(
				'id',
				$area->area_geometry_id)
				->execute();
		assert($num_deleted_surfaces == 1);
	
		$num_deleted_habitats = db_delete('area_habitat')->condition(
				'area_id',
				$area_id)
				->execute();
	
	} catch (Exception $e) {
		$transaction->rollback();
		drupal_set_message(
		t(
		'Could not delete @areaname.',
		array('@areaId' => $area_id)),
		'error');
		watchdog('area', $e->getMessage());
	}
}

?>
