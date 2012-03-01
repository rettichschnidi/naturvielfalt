<?php
/**
* @file habitat.list.inc
* @author ?, 2011
* @author Ramon Gamma, 2012
* @copyright 2011-2012 Naturwerk, Brugg
*/

/**
 * Return habitats from database start
 * @return
 */
function habitat_list() {
// 	drupal_add_js(drupal_get_path('module', 'datatable') . '/js/custom.datatable.select.js');

	// Set the table header
	$tableHeader[] = array( 'name' => t('Label'), 'dbfield' => 'label', 'width' => 90);
	$tableHeader[] = array( 'name' => t('Habitat name'), 'dbfield' => 'name_de');
	$tableHeader[] = array( 'name' => t('Linked Areas'), 'dbfield' => 'count', 'width' => 120);
	$tableHeader[] = array( 'name' => t('ID'), 'dbfield' => 'id', 'hide' => true, 'width' => 40);
	$tableHeader[] = array( 'name' => t('Image'), 'dbfield' => 'link', 'width' => 30);

	$render_array['habitats'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
// 			'#dbFields' => $dbFields,
// 			'#rows' => $rows,
			'#id_table' => 'habitats',
			'#options' => array(
				'jsonUrl' => "'habitat/json'",
				'rowClick' => 'rowClick',
				'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
								window.location.href = Drupal.settings.basePath+'habitat/'+id;})}"
				)
	);
	return $render_array;

}


/**
 * Return a filtered, sortable list of habitats
 *
 * @param unknown_type $sortfield
 * @param unknown_type $sortorder
 * @param unknown_type $start
 * @param unknown_type $offset
 * @return
 */
function habitat_list_ajax() {

	/* Set the db fields to select */
	$sql['dbColumns'] = array( 'habitat.label', 'habitat.name_de', 'COUNT(area_habitat.area_id)', 'habitat.id', 'COUNT(gi.id) as link' );
	$sql['dbSearchColumns'] = array( 'habitat.label', 'habitat.name_de' );
	$sql['dbSortColumns'] = array( 'habitat.label', 'habitat.name_de', 'count', 'habitat.id', 'link' );

	/* DB table to use */
	$sql['dbTable'] = "habitat";

	/* Joins to use */
	$sql['dbJoins'] = "LEFT JOIN area_habitat ON area_habitat.habitat_id = habitat.id
						LEFT JOIN area ON area.id = area_habitat.area_id
						LEFT JOIN gallery_image gi ON item_type = 'habitat' and gi.item_id = habitat.id";

	/* Group */
	$sql['dbGroupBy'] = "GROUP BY
				habitat.label,
				habitat.name_de,
				habitat.id";

	$output = datatable_dbRequest($sql);


// 	for ( $i=0 ; $i<count($output['aaData']) ; $i++ ){
// 		if (function_exists('gallery_view_link')) {
// 			$image_link = " ";
// 			if(gallery_view_link('habitat', $output['aaData'][$i]['id'])) $image_link = gallery_view_link('habitat', $output['aaData'][$i]['id']);
// 			$output['aaData'][$i]['link'] = $image_link;
// 		}
// 	}

	if(is_array($output)){
		for ( $i=0 ; $i<count($output['rows']) ; $i++ ){
			$image_link = " ";
			if (function_exists('gallery_view_link')) {
				if(gallery_view_link('habitat', $output['rows'][$i]['cell']['id']))
						$image_link = gallery_view_link('habitat', $output['rows'][$i]['cell']['id']);
			}
			$output['rows'][$i]['cell']['link'] = $image_link;
		}
	}
// 	print_r($output); exit();
	return drupal_json_output($output);
}


?>