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
			'#show' => 'custom-show',
			'#search' => true,
			'#ch1903' => true,
			'#json_url' => 'singleobservation/geometry/json',
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
	        'dbExactField' => 'ol.name',
			'width' => 310,
	);

	$tableHeader[] = array(
			'name' => t('Species'),
			'dbfield' => 'organism_sc',
	        'dbExactField' => 'osn.name',
			'width' => 310,
	);

	$tableHeader[] = array(
			'name' => t('Date'),
			'dbfield' => 'date',
	        'dbExactField' => 'o.date',
			'width' => 60
	);

	$tableHeader[] = array(
			'name' => t('Altitude [m]'),
			'dbfield' => 'altitude',
	        'dbExactField' => 'a_s.altitude',
			'width' => 60
	);

	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
	        'dbExactField' => 'a_s.township',
			'width' => 150
	);

	$tableHeader[] = array(
			'name' => t('Canton'),
			'dbfield' => 'canton',
	        'dbExactField' => 'a_s.canton',
			'width' => 40
	);

	$tableHeader[] = array(
			'name' => t('Country'),
			'dbfield' => 'country',
	        'dbExactField' => 'a_s.country',
			'width' => 180
	);

	$tableHeader[] = array(
			'name' => t('Observer'),
			'dbfield' => 'observer',
	        'dbExactField' => 'o.observer',
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
					window.location.href = Drupal.settings.basePath+'singleobservation/'+id+'/show';})}"
			)
	);
	return $output;
}