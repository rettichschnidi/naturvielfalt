<?php
function area_delete($area_id) {
	$render_array['area_delete'] = drupal_get_form(area_delete_form, $area_id);
	return $render_array;
}

function area_delete_form($form, &$form_state, $area_id) {
  $form_state['area_id'] = $area_id;
  
  $name = area_title($area_id);
  return confirm_form(
    $form,
    t('Delete %name?', array('%name' => $name)),
    'inventory/' . $head_inventory_id,
    t('Are you sure you want to delete the area %name? All contained entries and files will also be deleted.', array('%name' => $name)),
    t('Delete'),  t('Cancel')
  );
}

function area_title($area_id) {
	$result = db_select('area', 'a')
		->fields('a')
		->condition('id', $area_id)
		->execute()
		->fetch();
	$title = $result->field_name;
	return $title;
}
/**
 * 
 * Deletes area record from DB and assigned files. Only deletes if no inventories are assigned.
 * @param unknown_type $form
 * @param unknown_type $form_state
 */
function area_delete_form_submit($form, &$form_state) {
	$area_id = $form_state['area_id'];
	$result = db_query('
		SELECT COUNT (hi.id)
		FROM area a
		JOIN head_inventory hi ON a.id = hi.area_id
		WHERE hi.area_id = :aid',
		array(':aid' => $area_id)
	);	
	if($result->fetch()->count >= '1') {
		$form_state['redirect'] = 'area/'.$area_id.'/delete';
		drupal_set_message(t('The area can not be deleted. There are still inventories assigned'), 'error');		
	}
	else {
		//delete assigned files
		$area_files = db_select('area_file_managed', 'afm')
			->fields('afm')
			->condition('area_id', $area_id)
			->execute();
		foreach($area_files as $area_file) {
				$file = file_load($area_file->file_id);
				file_delete($file);
		}
		
		//delete area
		$delete_area = db_delete('area')
			->condition('id', $area_id)
			->execute();	
	
		$form_state['redirect'] = 'area/';
		drupal_set_message(t('The area was successfully deleted'));
	}
}

?>