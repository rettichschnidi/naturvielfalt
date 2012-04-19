<?php
/**
 * @file habitat.show.inc
 * @author ?, 2011
 * @author Ramon Gamma, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

function habitat_show_all() {
	// Set the table header
	$tableHeader[] = array(
			'name' => t('ID'),
			'dbfield' => 'id',
			'align' => 'center',
			'hide' => true,
			'width' => 40
	);
	$tableHeader[] = array(
			'name' => t('Label'),
			'dbfield' => 'label',
			'initSort' => true,
			'width' => 150
	);
	$tableHeader[] = array(
			'name' => t('Habitat name'),
			'dbfield' => 'name',
			'width' => 620,
	);
	$tableHeader[] = array(
			'name' => t('Linked Areas'),
			'dbfield' => 'area_count',
			'align' => 'center',
			'width' => 80
	);
	$tableHeader[] = array(
			'name' => t('Image'),
			'dbfield' => 'image_link',
			'align' => 'center',
			'width' => 50
	);

	$render_array['habitats'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#id_table' => 'habitats',
			'#tableWidth' => 900,
			'#options' => array(
					'jsonUrl' => "'habitat/datatablejson'",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
					window.location.href = Drupal.settings.basePath+'habitat/'+id;})}"
			)
	);
	return $render_array;
}

/**
 * Show an image gallery and a tabel with all areas that use this habitat.
 *
 * @param $form
 * 	Array Drupal Form
 * @param $form_state
 * 	Array with Drupal form state
 * @param $habitatdata
 * 	Array with all data about this habitat
 * @return
 */
function habitat_show_form($form, &$form_state, $habitatdata = array()) {
	$render_array = array();
	if (!empty($habitatdata)) {
		$title = $habitatdata['label'] . ' ' . t($habitatdata['name']);
		drupal_set_title($title);

		if (function_exists('area_habitat_table')) {
			$render_array['areas'] = array(
					'#type' => 'fieldset',
					'#title' => t('Linked areas'),
					'#collapsible' => true
			);
			$render_array['areas']['areatable'] = area_habitat_table($habitatdata['id']);
		}

		if (function_exists('gallery_list_renderer'))
			$render_array['gallery'] = gallery_list_renderer('images',
				'habitat',
				$habitatdata['id']);
	}
	return $render_array;
}
?>
