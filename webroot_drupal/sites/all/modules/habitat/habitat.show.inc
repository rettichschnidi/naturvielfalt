<?php
/**
* @file habitat.show.inc
* @author ?, 2011
* @author Ramon Gamma, 2012
* @copyright 2011-2012 Naturwerk, Brugg
*/

/**
 * Return linked areas array
 *
 * @author Naturwerk, 2011
 * @author Ramon Gamma, 2012
 *
 * @param int $habitatGroupId
 * @return
 */
function habitat_show($habitatGroupId) {

// die($habitatGroupId);
// 	drupal_add_js(drupal_get_path('module', 'datatable') . '/js/custom.datatable.select.js');

	// set the page title
	$habitat = db_select('habitat', 'h')->condition('h.id', $habitatGroupId)->fields('h')->execute()->fetch();
	$title = $habitat->label . ' ' . $habitat->name_de;
	if ($habitat->name_lt)
		$title .= ' [' . $habitat->name_lt . ']';
	drupal_set_title($title);

	// Linked Areas
	$tableHeader[] = array( 'name' => t('Area name'), 'dbfield' => 'field_name');
	$tableHeader[] = array( 'name' => t('Locality'), 'dbfield' => 'locality', 'width' => 80);
	$tableHeader[] = array( 'name' => t('ZIP'), 'dbfield' => 'zip', 'width' => 60);
	$tableHeader[] = array( 'name' => t('Canton'), 'dbfield' => 'canton', 'width' => 40);
	$tableHeader[] = array( 'name' => t('Linked inventories'), 'dbfield' => 'count', 'width' => 120);
	$tableHeader[] = array( 'name' => t('Author'), 'dbfield' => 'u_name', 'width' => 80);
	$tableHeader[] = array( 'name' => t('ID'), 'dbfield' => 'area_id', 'hide' => true, 'width' => 30);

	$render_array['linked_areas'] = array(
				'#type' => 'fieldset',
				'#title' => t('Linked Areas'),
				'#weight' => 11,
				'#collapsible' => TRUE,
				'#collapsed' => FALSE,
				'#attached' => array(
								'js' => array(
										'misc/form.js',
										'misc/collapse.js',
										),
								),
				'#attributes' => array(
								'class' => array(
											'collapsible'
											),
								),
	);

	$render_array['linked_areas']['areas'] = array(
				'#theme' => 'datatable',
				'#header' => $tableHeader,
// 				'#dbFields' => $dbFields,
	// 			'#rows' => $rows,
				'#id_table' => 'areas',
				'#options' => array(
								'jsonUrl' => "'".$habitatGroupId."/json'",
								'rowClick' => 'rowClick',
								'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
												window.location.href = Drupal.settings.basePath+'area/'+id;})}"
				)
	);

	if (function_exists('gallery_list_renderer'))
	$render_array['Gallery'] = gallery_list_renderer('images', 'habitat', $habitatGroupId);

	return $render_array;

/**
 *
 * Unused below
 * --> function ajax below used!!!
 *
 */
	$result = db_query(
			"SELECT
				area_habitat.area_id,
				area_habitat.habitat_id,
				area.field_name,
				area.locality,
				area.zip,
				area.canton,
				ua.field_address_first_name || ' ' || ua.field_address_last_name u_name

				'COUNT(head_inventory.area_id) AS count'
			FROM
				area_habitat
				LEFT JOIN area ON area.id = area_habitat.area_id
				LEFT JOIN users ON users.uid = area.owner_id
				LEFT JOIN field_data_field_address ua ON ua.entity_id = users.uid

				LEFT JOIN head_inventory hi on area.id = hi.area_id AND hidden IS NOT TRUE
			WHERE
				habitat_id = :id AND area.hidden IS NOT TRUE", array(':id' => $habitatGroupId));

	foreach ($result as $record) {
		$inventories = db_query(
				'SELECT
					COUNT(area_id)
				FROM
					head_inventory
				WHERE
					area_id = :aid AND hidden IS NOT TRUE', array(':aid' => $record->area_id));
		$row = array();
		$row[] = $record->field_name;
		$row[] = $record->locality;
		$row[] = $record->zip;
		$row[] = $record->canton;
		$row[] = $inventories->fetch()->count;
		$row[] = $record->u_name;
		$rows[] = array(
				'data' => $row,
				'id' => 'area_' . $record->area_id
		);
	}

	$render_array['linked_areas'] = array(
			'#type' => 'fieldset',
			'#title' => t('Linked Areas'),
			'#weight' => 11,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
			),
	);

	//render only if there are any records returned
	if ($rows) {
		$render_array['linked_areas']['areas'] = array(
				'#theme' => 'datatable',
				'#header' => $tableHeader,
				'#rows' => $rows,
				'#id_table' => 'areas',
		);
	} else {
		$render_array['linked_areas']['areas'] = array(
				'#markup' => t("There are no linked areas yet"),
		);
	}
	if (function_exists('gallery_list_renderer'))
		$render_array['Gallery'] = gallery_list_renderer('images', 'habitat', $habitatGroupId);
	return $render_array;
}

/**
 *
 * Enter description here ...
 * @author Ramon Gamma, 2012
 *
 * @param unknown_type $habitatGroupId
 */
function habitat_show_ajax($habitatGroupId){

	$sql['dbColumns'] = array( 'area.field_name', 'area.locality', 'area.zip', 'area.canton', 'COUNT(hi.area_id)',
			  "ua.field_address_first_name || ' ' || ua.field_address_last_name u_name", 'area_habitat.area_id');

	$sql['dbSearchColumns'] = array( 'area.field_name', 'area.locality', 'area.zip',
			'area.canton', "ua.field_address_first_name", "ua.field_address_last_name" );

	$sql['dbSortColumns'] = array( 'area.field_name', 'area.locality', 'area.zip', 'area.canton', 'count',
			  "u_name", 'area_habitat.area_id');

	$sql['dbTable'] = "area_habitat";

	$sql['dbJoins'] = "LEFT JOIN area ON area.id = area_habitat.area_id
				LEFT JOIN users ON users.uid = area.owner_id
				LEFT JOIN field_data_field_address ua ON ua.entity_id = users.uid
				LEFT JOIN head_inventory hi on area.id = hi.area_id";

	$sql['dbWhere'] = "habitat_id = '".$habitatGroupId."' AND area.hidden IS NOT TRUE";

	$sql['dbGroupBy'] = "GROUP BY area_habitat.area_id, area_habitat.habitat_id, area.field_name, area.locality,
					area.zip, area.canton, ua.field_address_first_name, ua.field_address_last_name";

	$sql['id'] = "area_id";

	$output = datatable_dbRequest($sql);


// 	for ( $i=0 ; $i<count($output['aaData']) ; $i++ ){
// 		if (function_exists('gallery_view_link')) {
// 			$image_link = " ";
// 			if(gallery_view_link('habitat', $output['aaData'][$i]['id'])) $image_link = gallery_view_link('habitat', $output['aaData'][$i]['id']);
// 			$output['aaData'][$i]['link'] = $image_link;
// 		}
// 	}
// 	die(print_r($output));
	return drupal_json_output($output);
}

?>