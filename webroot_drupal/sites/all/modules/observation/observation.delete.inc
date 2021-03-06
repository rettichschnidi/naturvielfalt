<?php
/**
 * @file observation.delete.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'observation', 'observation');

/**
 * Return a form which asks the user if he really wants to delete the observation
 *
 * @param array $observationdata
 */
function observation_delete_form($form, &$form_state, $observationdata = NULL) {
	assert(!empty($observationdata));
	drupal_set_title(
		t(
			'Delete Observation @observation_organism from @observation_date.',
			array('@observation_organism' => $observationdata['organism']['name_lang'] . '('
							. $observationdata['organism']['name_lat'] . ')',
					'@observation_date' => date('d.m.Y', $observationdata['observation']['date'])
			)));

	$question = t(
		'Are you sure that you want to delete this observation «@observation_organism»?',
		array('@observation_organism' => $observationdata['organism']['name_lang'] . '('
						. $observationdata['organism']['name_lat'] . ')'
		));
	/* create a fieldset for the tabular data */
	$form['question'] = array(
			'#type' => 'markup',
			'#markup' => "<p>$question</p>"
	);

	$form['button'] = array(
			'#type' => 'submit',
			'#value' => t('Delete')
	);
	return $form;
}

/**
 * Check if observation can be deleted
 *
 * @param array $form
 * @param array $form_state
 */
function observation_delete_form_validate($form, &$form_state) {
	// foobar does not exists
	// drupal_set_message(t('This observation is still in use.'), 'error');
}

/**
 * Delete observation
 *
 * @param array $form
 * @param array $form_state
 */
function observation_delete_form_submit($form, &$form_state) {
	/* Escape values before submitting them to the database */
	$observation_id = (int) $form_state['build_info']['args'][0]['observation']['id'];
	observation_delete_by_id($observation_id);
	area_delete_geometry(
		(int) $form_state['build_info']['args'][0]['observation']['area_geometry_id']);

	drupal_goto('/observation/show');
}

/**
 * Deletes an observation with all its files
 * @param $observation_id 
 */
function observation_delete_by_id($observation_id) {
	$observation = observation_get($observation_id);
	
	db_delete('observation')->condition('id', $observation_id)
	->execute();
	
	//delete acl entries
	delete_acl($observation['observation']['acl_id']);
	
	db_delete('area_geometry')->condition('id', $observation['observation']['area_geometry_id'])
		->execute();

	db_delete('observation_attribute')->condition('observation_id', $observation_id)
		->execute();
	

	//delete files of observation
	$files = gallery_implementation_get_files('observation', $observation_id);
	$images = gallery_images('medias', 'observation', $observation_id);

	$image_ids[] = array();
	foreach($images['medias'] as $image) {
		$image_ids[$image['id']] = 1;
	}
	if(!empty($image_ids))
		gallery_implementation_delete($image_ids);
	
	if(!empty($files))
		gallery_implementation_files_delete($files, 'observation');
}

?>
