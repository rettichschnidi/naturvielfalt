<?php

/**
 * Show all existing observations. If $onlyMyObservation is true,
 * restrict to observation where the current user is admin.
 *
 * @return render array
 */
function observation_show_all_singleobservations() {


	$output['div'] = array(
			'#type' => 'fieldset',
			'#title' => "Google Map",
			'#attributes' => array(
					'style' => 'width: 95%;'
			)
	);

	$output['div']['map'] = array(
			'#theme' => 'area',
			'#action' => 'getcoordinate',
			'#search' => true,
			'#ch1903' => true
	);

	$tableHeader[] = array(
			'name' => t('Internal Id'),
			'idField' => true,
			'dbfield' => 'id',
			'hide' => true,
			'width' => 30
	);

	$tableHeader[] = array(
			'name' => t('Organism'),
			'dbfield' => 'organism_l',
			'width' => 310,
	);

	$tableHeader[] = array(
			'name' => t('Species'),
			'dbfield' => 'organism_sc',
			'width' => 310,
	);

	$tableHeader[] = array(
			'name' => t('Date'),
			'dbfield' => 'date',
			'width' => 60
	);

	$tableHeader[] = array(
			'name' => t('Altitude [m]'),
			'dbfield' => 'altitude',
			'width' => 60
	);

	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
			'width' => 150
	);

	$tableHeader[] = array(
			'name' => t('Canton'),
			'dbfield' => 'canton',
			'width' => 40
	);

	$tableHeader[] = array(
			'name' => t('Country'),
			'dbfield' => 'country',
			'width' => 180
	);

	$tableHeader[] = array(
			'name' => t('Observer'),
			'dbfield' => 'observer',
			'width' => 120
	);

	$output['space'] = array(
			'#type' => 'markup',
			'#markup' => '<p/>'
	);

	$output['area_table'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#title' => 'Observations',
			'#id_table' => 'observations',
			'#options' => array(
					'jsonUrl' => "'" . base_path()."observation/get/all'",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
					window.location.href = Drupal.settings.basePath+'singleobservation/'+id+'show';})}"
			)
	);
	return $output;
}