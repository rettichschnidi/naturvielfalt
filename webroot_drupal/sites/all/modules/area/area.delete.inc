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
	$transaction = db_transaction();
	try {
		$num_deleted_area = db_delete('area')->condition('id', $areadata['id'])
			->execute();
		assert($num_deleted_area == 1);
		$num_deleted_surfaces = db_delete('area_geometry')->condition(
				'id',
				$areadata['area_geometry_id'])
			->execute();
		assert($num_deleted_surfaces == 1);

		$num_deleted_habitats = db_delete('area_habitat')->condition(
				'area_id',
				$areadata['id'])
			->execute();

	} catch (Exception $e) {
		$transaction->rollback();
		drupal_set_message(
			t(
				'Could not delete @areaname.',
				array('@areaname' => $areadata['name'])),
			'error');
		watchdog('area', $e->getMessage());
	}

	$form_state['redirect'] = 'areas/show';
}
?>
