<?php

/**
 * @file area.show.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

/**
 * Show all existing areas.
 * @return render array
 */
function area_show_all_areas() {
	drupal_set_title(t('Show all existing areas'));

	$output['map'] = array(
			'#search' => true,
			'#create' => false,
			'#showall' => true,
			'#theme' => 'area'
	);

	$tableHeader[] = array(
			'name' => t('Internal Id'),
			'idField' => true,
			'dbfield' => 'id',
			'hide' => true,
	);

	$tableHeader[] = array(
			'name' => t('Name'),
			'dbfield' => 'name'
	);

	$tableHeader[] = array(
			'name' => t('Altitude [m]'),
			'dbfield' => 'altitude',
	);

	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
	);

	$tableHeader[] = array(
			'name' => t('Canton'),
			'dbfield' => 'canton',
	);

	$tableHeader[] = array(
			'name' => t('Country'),
			'dbfield' => 'country',
	);

	$output['space'] = array(
			'#type' => 'markup',
			'#markup' => '<p/>'
	);

	$output['area_table'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#title' => 'Areas',
			'#id_table' => 'areas',
			'#options' => array(
					'jsonUrl' => "'" . base_path() . "area/datatablejson'",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id) {
						jQuery(celDiv).click(
							function() {
								window.location.href = Drupal.settings.basePath+'area/' + id;
							}
						)
					}"
			)
	);
	return $output;

}

/**
 * Show a specific area
 * @param integer $area_id
 * @return renderarray
 */
function area_show_specific_area($area_id = NULL) {
	drupal_set_title("Show area #$area_id.");
	
	return array();
}
?>
