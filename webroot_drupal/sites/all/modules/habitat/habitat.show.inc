<?php
/**
 * @file habitat.show.inc
 * @author ?, 2011
 * @author Ramon Gamma, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

function habitat_show_all() {
	module_load_include('inc', 'datatable', 'datatable.user');
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
			'dbExactField' => 'h.label',
			'initSort' => true,
			'width' => 150
	);
	$tableHeader[] = array(
			'name' => t('Name'),
			'dbfield' => 'name',
			'dbExactField' => 'h.name',
			'width' => 350,
	);
	$tableHeader[] = array(
			'name' => t('Latin name'),
			'dbfield' => 'name_scientific',
			'dbExactField' => 'h.name_scientific',
			'width' => 200,
	);
	$tableHeader[] = array(
			'name' => t('Linked Areas'),
			'dbfield' => 'area_count',
			'align' => 'center',
			'width' => 75
	);
	$tableHeader[] = array(
			'name' => t('Image'),
			'dbfield' => 'image_link',
			'align' => 'center',
			'width' => 50
	);
	
	//apply user settings
	datatable_apply_user_settings(DATATABLE_HABITATS, $tableHeader);

	$render_array['habitats'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#id_table' => DATATABLE_HABITATS,
			'#tableWidth' => 900,
			'#options' => array(
					'jsonUrl' => "habitat/datatablejson",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
					window.location.href = Drupal.settings.basePath+'habitat/'+id;})}"
			)
	);
	return $render_array;
}

module_load_include('inc', 'area', 'area.show');

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

		if (function_exists('gallery_list_renderer')) {
			$render_array['gallery'] = gallery_list_renderer(
				'images',
				'habitat',
				$habitatdata['id']);
		}

		if (function_exists('area_habitat_table')) {
			$render_array['areas'] = array(
					'#type' => 'fieldset',
					'#title' => t('Linked areas'),
					'#collapsible' => true
			);
			$render_array['areas']['areatable'] = area_habitat_table(
				$habitatdata['id']);
		}
	}
	return $render_array;
}
?>
