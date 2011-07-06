<?php
function area_delete($area_id) {
	$render_array['area_delete'] = drupal_get_form(area_delete_form, $area_id);
	return $render_array;
}

function area_delete_form($form, &$form_state, $area_id) {
	$form_state['storage']['area_id'] = $area_id;
	$form['area_delete'] = array(
		'#type' => 'fieldset',
 // 		'#title' => t('Delete'), 
  		//'#weight' => 7, 
  		'#collapsible' => FALSE, 
  		'#collapsed' => FALSE,
/*		'#attached' => array (
      		'js' => array (
      		'misc/form.js',
        	'misc/collapse.js',
      		),
    	),
    	'#attributes' => array(
      		'class' => array('collapsible'),
      		'id' => 'area-files'
    	),
*/	);	
	$form['area_delete']['confirm'] = array(
	    '#type' => 'checkbox', 
	    '#title' => t("I am aware that I will permanently delete this area"),
	    '#required' => TRUE,
  	);
	$form['area_delete']['delete_submit'] = array(
	    '#type' => 'submit', 
	    '#value' => t("Delete"),
	    '#submit' => array('area_delete_form_submit'),
  	);
  	
  	return $form;
}
/**
 * 
 * Deletes area record from DB. Only deletes if no inventories are assigned.
 * @param unknown_type $form
 * @param unknown_type $form_state
 */
function area_delete_form_submit($form, &$form_state) {
	$area_id = $form_state['storage']['area_id'];
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