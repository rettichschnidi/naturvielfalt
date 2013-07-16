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
		
		$num_deleted_surfaces = db_delete('area_geometry')->condition(
				'id',
				$area->area_geometry_id)
				->execute();
		assert($num_deleted_surfaces == 1);
		
		//delete inventories
		foreach(inventory_get_area_inventories($area_id) as $inventory) {
			inventory_delete_by_id($inventory->id);
		}
		
		//delete files of area
		$files = gallery_implementation_get_files('area', $area_id);
		if(!empty($files))
			gallery_implementation_files_delete($files, 'area');
		
		//delete gallery images of area
		$images = gallery_images('medias', 'area', $area_id);
		
		$image_ids = array();
		foreach($images['medias'] as $image) {
			$image_ids[$image['id']] = 1;
		}
		if(!empty($image_ids))
			gallery_implementation_delete($image_ids);
		
		//delete gallery images of linked habitats
		$habitats = db_select('area_habitat', 'a_h')->condition('area_id', $area_id)
		->fields('a_h')
		->execute();
		foreach ($habitats->fetchAll() as $habitat) {
			$images = gallery_images('medias', 'area_habitat', $habitat->id);
		
			$image_ids = array();
			foreach($images['medias'] as $image) {
				$image_ids[$image['id']] = 1;
			}
			if(!empty($image_ids))
				gallery_implementation_delete($image_ids);
		}
	
		$num_deleted_habitats = db_delete('area_habitat')->condition(
				'area_id',
				$area_id)
				->execute();
		
		$num_deleted_area = db_delete('area')->condition('id', $area_id)
		->execute();
		assert($num_deleted_area == 1);
		drupal_set_message(t('area @areaname deleted', array('@areaname' => $area->name)));
		
	} catch (Exception $e) {
		$transaction->rollback();
		drupal_set_message(
		t(
		'Could not delete @areaname.',
		array('@areaname' => $area->name)),
		'error');
		watchdog('area', $e->getMessage());
	}
}

?>
