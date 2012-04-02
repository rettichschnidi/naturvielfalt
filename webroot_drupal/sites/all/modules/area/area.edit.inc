<?php

/**
 * @file area.edit.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');

/**
 * Returns the area edit site
 * @param array $areadata
 */
function area_edit($areadata = NULL) {
	return drupal_get_form('area_edit_form', $areadata);
}

/**
 * Returns the area geometry edit site
 * @param array $areadata
 */
function area_edit_geometry($areadata = NULL) {
	return drupal_get_form('area_edit_geometry_form', $areadata);
}

function area_edit_geometry_form($form, &$form_state, $areadata = NULL) {
	$output['helptext'] = array(
			'#type' => 'markup',
			'#markup' => t(
				'<p>Please note: Changes will be saved instantly.</p>')
	);
	$output['map'] = array(
			'#theme' => 'area',
			'#search' => false,
			'#create' => false,
			'#showall' => false,
			'#edit' => true,
			'#area_id' => (int) $areadata['id']
	);
	return $output;
}

/**
 * Return a form which allows all textual data of an area to be edited
 * @note Permission checking has to be done by calling function
 * 
 * @param array $areadata
 */
function area_edit_form($form, &$form_state, $areadata = NULL) {
	assert(!empty($areadata));
	drupal_set_title($areadata['name']);
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-edit.css');

	/* call area_get_static_image_url() to get an url to the google maps visualization */
	$staticimageurl = area_get_static_image_url(
		array('coordinates' => json_encode($areadata['coordinatejson']),
				'type' => $areadata['geometry_type']
		),
		450,
		320);
	$editgeometryurl = l(
		"<img src='$staticimageurl' alt='Static Map'/>",
		'area/' . $areadata['id'] . '/editgeometry',
		array('html' => true));

	/* create a fieldset for the tabular data */
	$form['area_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area details'),
			'#weight' => 1,
			'#attributes' => array(
					'id' => 'area-edit-details' // required for CSS
			),
	);

	/* add a table with the most imporant information, leave out empty fields */
	$form['area_edit']['table'] = area_get_infotable_of_specific_area($areadata);

	/* Create a fieldset for the comment text field */
	$form['area_description_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Description'),
			'#weight' => 10,
			'#collapsible' => true,
			'#collapsed' => false,
			'#attributes' => array(
					'id' => 'area-edit-comment'
			)
	);

	/* add the name of the area as an editable textfield */
	$form['area_description_edit']['areaname'] = array(
			'#type' => 'textfield',
			'#title' => t('Area name'),
			'#size' => 25,
			'#required' => true,
			'#default_value' => $areadata['name']
	);

	/* add the comments for the area as an editable textarea */
	$form['area_description_edit']['comment'] = array(
			'#type' => 'textarea',
			'#title' => t('Description'),
			'#default_value' => $areadata['comment']
	);

	/* create a fieldset for the strategies text fields */
	$form['area_concept_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area concept'),
			'#weight' => 11,
			'#collapsible' => true,
			'#collapsed' => true
	);

	/* add the protectiont target textarea */
	$form['area_concept_edit']['protection_target'] = array(
			'#type' => 'textarea',
			'#title' => t('Protection target'),
			'#default_value' => $areadata['protection_target']
	);

	/* add the tending strategies target textarea */
	$form['area_concept_edit']['tending_strategies'] = array(
			'#type' => 'textarea',
			'#title' => t('Tending strategies'),
			'#default_value' => $areadata['tending_strategies']
	);

	/* add the tending strategies target textarea */
	$form['area_concept_edit']['safety_precautions'] = array(
			'#type' => 'textarea',
			'#title' => t('Safety precautions'),
			'#default_value' => $areadata['safety_precautions']
	);

	/* create a fieldset for the static google maps */
	$form['area_static_map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 3,
			'#attributes' => array(
					'id' => 'area-edit-map' // required for CSS
			)
	);

	/* add the picture */
	$form['area_static_map']['image'] = array(
			'#type' => 'markup',
			'#markup' => $editgeometryurl
	);

	/* add a submit button to save the changes */
	$form['submit'] = array(
			'#type' => 'submit',
			'#weight' => 99,
			'#value' => t('Save changes')
	);

	return $form;
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
 * @param array $form
 * @param array $form_state
 */
function area_edit_form_submit($form, &$form_state) {
	/* Escape values before submitting them to the database */
	$area_id = (int) $form_state['build_info']['args'][0]['id'];
	$name = check_plain($form_state['values']['areaname']);
	$comment = check_plain($form_state['values']['comment']);
	$protection_target = check_plain($form_state['values']['protection_target']);
	$tending_strategies = check_plain(
		$form_state['values']['tending_strategies']);
	$safety_precautions = check_plain(
		$form_state['values']['safety_precautions']);

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
?>
