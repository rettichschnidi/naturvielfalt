<?php

/**
 * @file area.edit.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');

/**
 * Check the permissions and create the edit form.
 * 
 * @todo check c_acl
 * @param integer $area_id
 */
function area_edit($area_id = NULL) {
	assert($area_id);
	return drupal_get_form('area_edit_form', $area_id);
}

/**
 * Validate the changes to the area
 * 
 * @param array $form
 * @param array $form_state
 */
function area_edit_form_validate($form, &$form_state) {
	if (empty($form_state['values']['areaname'])) {
		form_set_error('areaname', t('You have to enter an area name.'));
	}
}

/**
 * Save changes to the area texts
 * 
 * @todo check c_acl
 * @param array $form
 * @param array $form_state
 */
function area_edit_form_submit($form, &$form_state) {
	$area_id = $form_state['build_info']['args'][0];
	$name = $form_state['values']['areaname'];
	$comment = $form_state['values']['comment'];
	$protection_target = $form_state['values']['protection_target'];
	$tending_strategies = $form_state['values']['tending_strategies'];
	$safety_precautions = $form_state['values']['safety_precautions'];

	db_update('{area}')->fields(
			array('name' => $name,
					'comment' => $comment,
					'protection_target' => $protection_target,
					'tending_strategies' => $tending_strategies,
					'safety_precautions' => $safety_precautions,
					'modify_time' => 'NOW()',
			))
		->condition('id', $area_id)
		->execute();
	drupal_set_message(t("Area Saved"));
}

/**
 * Return a form which allows all textual data of an area to be edited
 * @note Permission checking has to be done by calling function
 * 
 * @param integer $area_id
 */
function area_edit_form($form, &$form_state, $area_id = NULL) {
	$output = array();
	assert($area_id);
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-edit.css');

	$area = area_get_full_dataset_of_specific_area($area_id);
	assert(!empty($area));

	/* call area_get_static_image_url() to get an url to the google maps visualization */
	$staticimageurl = area_get_static_image_url(
		array('coordinates' => json_encode($area['coordinatejson']),
				'type' => $area['geometry_type']
		));

	/* Create a fieldset for the tabular data */
	$output['area_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area details'),
			'#weight' => 1,
			'#attributes' => array(
					'id' => 'area-edit-details' // required for CSS
			),
	);

	/* Build the content of the table, leave out empty fields */
	$output['area_edit']['table'] = area_get_infotable_of_specific_area($area);

	/* add the name of the area as an editable textfield */
	$output['area_edit']['areaname'] = array(
			'#type' => 'textfield',
			'#weight' => 2,
			'#title' => t('Area name'),
			'#required' => true,
			'#size' => 25,
			'#required' => true,
			'#default_value' => $area['name']
	);

	/* Create a fieldset for the comment text field */
	$output['area_description_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Description'),
			'#weight' => 10,
			'#collapsible' => true,
			'#collapsed' => false,
			'#attributes' => array(
					'id' => 'area-edit-comment'
			)
	);

	/* add the comments for the area as an editable textarea */
	$output['area_description_edit']['comment'] = array(
			'#type' => 'textarea',
			'#title' => t('Description'),
			'#default_value' => $area['comment']
	);

	/* Create a fieldset for the strategies text fields */
	$output['area_concept_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area concept'),
			'#weight' => 11,
			'#collapsible' => true,
			'#collapsed' => true
	);

	/* add the protectiont target textarea */
	$output['area_concept_edit']['protection_target'] = array(
			'#type' => 'textarea',
			'#title' => t('Protection target'),
			'#default_value' => $area['protection_target']
	);

	/* add the tending strategies target textarea */
	$output['area_concept_edit']['tending_strategies'] = array(
			'#type' => 'textarea',
			'#title' => t('Tending strategies'),
			'#default_value' => $area['tending_strategies']
	);

	/* add the tending strategies target textarea */
	$output['area_concept_edit']['safety_precautions'] = array(
			'#type' => 'textarea',
			'#title' => t('Safety precautions'),
			'#default_value' => $area['safety_precautions']
	);

	/* Create a fieldset for the static google maps */
	$output['area_static_map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 3,
			'#attributes' => array(
					'id' => 'area-edit-map' // required for CSS
			)
	);

	/* add the picture */
	$output['area_static_map']['image'] = array(
			'#type' => 'markup',
			'#markup' => "<img src='$staticimageurl' alt='Static Map'/>"
	);
	/* add a link to the picture, mainly for debug usage. Can be removed later on. */
	// 	$output['area_static_map']['url'] = array(
	// 			'#type' => 'markup',
	// 			'#markup' => l('Link to google static map', $staticimageurl)
	// 	);
	/* add a submit button to save the changes */
	$output['submit'] = array(
			'#type' => 'submit',
			'#weight' => 99,
			'#value' => t('Save changes')
	);
	return $output;
}
?>
