<?php
module_load_include('inc', 'observation');

/**
 * Show all existing observations. If $onlyMyObservation is true,
 * restrict to observation where the current user is admin.
 *
 * @return render array
 */
function observation_show_all_singleobservations() {

	$observation_path= drupal_get_path('module', 'observation');
	drupal_add_js($observation_path . '/js/observation.js');

	$output['div'] = array(
			'#type' => 'fieldset',
			'#title' => "Google Map",
			'#attributes' => array(
					'style' => 'width: 95%;'
			)
	);

	$output['div']['map'] = array(
			'#theme' => 'area',
			'#show' => 'custom-show',
			'#search' => true,
			'#ch1903' => true,
			'#json_url' => 'singleobservation/geometry/json',
			'#height' => '350px',
	);

	$output['space'] = array(
			'#type' => 'markup',
			'#markup' => '<p/>'
	);

	$output['area_table'] = array(
			'#theme' => 'datatable',
			'#header' => observation_get_standart_datatable_headers(),
			'#title' => t('Singleobservations'),
			'#id_table' => 'observations',
			'#options' => array(
					'jsonUrl' => "'" . base_path()."observation/get/all'",
					'rowClick' => 'rowClick',
// 					'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
// 					window.location.href = Drupal.settings.basePath+'singleobservation/'+id+'/show';})}"
					'rowClickHandler' => "function rowClick(celDiv, id) {
						jQuery(celDiv).click(
							function() {
								observation.selectObservation(id);
							}
						)
					}",
			)
	);
	return $output;
}


/**
 * Return a form which shows some important facts about a given area.
 * Intended for usage in a infowindow on a map.
 *
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 * @param array $areadata Area data
 * @return array Drupal form
 */
function observation_map_overview_form($form, &$form_state, $observationdata) {
	$form['addressdata'] = array(
			'#type' => 'fieldset',
			'#title' => t('Observation'),
			'#attributes' => array(
					'style' => array('width:400px;'))
	);
	$form['addressdata']['table'] = observation_get_table($observationdata, true);
	/**
	 * Display the "edit" button only if user has write permission to this area
	 */
	if (observation_write_access($observationdata)) {
		$form['addressdata']['edit'] = array(
				'#type' => 'markup',
				'#markup' => l(t('Edit'), 'singleobservation/' . $observationdata['observation']['id'] . '/edit')
		);
		$form['addressdata']['whitespace'] = array(
				'#type' => 'markup',
				'#markup' => '&nbsp/&nbsp;'
		);
	}
	$form['addressdata']['show'] = array(
			'#type' => 'markup',
			'#markup' => l(t('Show'), 'singleobservation/' . $observationdata['observation']['id'] . '/show')
	);
	return $form;
}