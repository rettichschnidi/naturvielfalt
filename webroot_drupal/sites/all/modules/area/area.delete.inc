<?php

/**
 * @file area.delete.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');

/**
 * Return a form which asks the user if he really wants to delete the area
 * 
 * @param array $areadata
 */
function area_delete_form($form, &$form_state, $areadata = NULL) {
	assert(!empty($areadata));
	drupal_set_title(
		t("Delete @areaname.", array('@areaname' => $areadata['name'])));

	$question = t('Are you sure that you want to delete «@areaname»?',
						array('@areaname' => $areadata['name']));
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
 * Check if area can be deleted
 *
 * @param array $form
 * @param array $form_state
 */
function area_delete_form_validate($form, &$form_state) {
	// foobar does not exists
	// drupal_set_message(t('This area is still in use.'), 'error');
}

/**
 * Delete area
 *
 * @param array $form
 * @param array $form_state
 */
function area_delete_form_submit($form, &$form_state) {
	/* Escape values before submitting them to the database */
	$area_id = (int) $form_state['build_info']['args'][0]['id'];

	$num_deleted = db_delete('{area}')->condition('id', $area_id)
		->execute();
	assert($num_deleted == 1);
	drupal_goto('/area');
}
?>
