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
			'name' => t('Label'),
			'dbfield' => 'label',
			'initSort' => true,
			'width' => 150
	);
	$tableHeader[] = array(
			'name' => t('Habitat name'),
			'dbfield' => 'name',
			'width' => 900,
	);
	$tableHeader[] = array(
			'name' => t('Linked Areas'),
			'dbfield' => 'area_count',
			'align' => 'center',
			'width' => 80
	);
	$tableHeader[] = array(
			'name' => t('ID'),
			'dbfield' => 'id',
			'align' => 'center',
			'hide' => true,
			'width' => 40
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
 * Return linked areas array
 *
 * @author Naturwerk, 2011
 * @author Ramon Gamma, 2012
 *
 * @param int $habitatGroupId
 * @return
 */
function habitat_show_form($form, &$form_state, $habitatId) {
	// set the page title
	// 	$habitat = db_select('habitat', 'h')->condition('h.id', $habitatId)
	// 		->fields('h')
	// 		->execute()
	// 		->fetch();
	// 	$title = $habitat->label . ' ' . t($habitat->name);
	// 	drupal_set_title($title);

	// 	// Linked Areas
	// 	$tableHeader[] = array(
	// 			'name' => t('Area name'),
	// 			'dbfield' => 'field_name'
	// 	);
	// 	$tableHeader[] = array(
	// 			'name' => t('Locality'),
	// 			'dbfield' => 'locality',
	// 			'width' => 80
	// 	);
	// 	$tableHeader[] = array(
	// 			'name' => t('ZIP'),
	// 			'dbfield' => 'zip',
	// 			'width' => 60
	// 	);
	// 	$tableHeader[] = array(
	// 			'name' => t('Canton'),
	// 			'dbfield' => 'canton',
	// 			'width' => 40
	// 	);
	// 	$tableHeader[] = array(
	// 			'name' => t('Linked inventories'),
	// 			'dbfield' => 'count',
	// 			'width' => 120
	// 	);
	// 	$tableHeader[] = array(
	// 			'name' => t('Author'),
	// 			'dbfield' => 'u_name',
	// 			'width' => 80
	// 	);
	// 	$tableHeader[] = array(
	// 			'name' => t('ID'),
	// 			'dbfield' => 'area_id',
	// 			'hide' => true,
	// 			'width' => 30
	// 	);

	// 	$render_array['linked_areas'] = array(
	// 			'#type' => 'fieldset',
	// 			'#title' => t('Linked Areas'),
	// 			'#weight' => 11,
	// 			'#collapsible' => TRUE,
	// 			'#collapsed' => FALSE,
	// 			'#attached' => array(
	// 					'js' => array(
	// 							'misc/form.js',
	// 							'misc/collapse.js',
	// 					),
	// 			),
	// 			'#attributes' => array(
	// 					'class' => array(
	// 							'collapsible'
	// 					),
	// 			),
	// 	);

	// 	$render_array['linked_areas']['areas'] = array(
	// 			'#theme' => 'datatable',
	// 			'#header' => $tableHeader,
	// 			// 				'#dbFields' => $dbFields,
	// 			// 			'#rows' => $rows,
	// 			'#id_table' => 'areas',
	// 			'#options' => array(
	// 					'jsonUrl' => "'" . $habitatGroupId . "/json'",
	// 					'rowClick' => 'rowClick',
	// 					'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
	// 												window.location.href = Drupal.settings.basePath+'area/'+id;})}"
	// 			)
	// 	);

	if (function_exists('gallery_list_renderer'))
		$render_array['Gallery'] = gallery_list_renderer(
			'images',
			'habitat',
			$habitatId);

	return $render_array;
}
?>
