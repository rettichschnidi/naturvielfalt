<?php
/**
* @file habitat.list.inc
* @author ?, 2011
* @author Ramon Gamma, 2012
* @copyright 2011-2012 Naturwerk, Brugg
*/


/**
 * Return all habitats from database
 * @return
 */
function habitat_list() {
	drupal_add_js(drupal_get_path('module', 'habitat') . '/js/habitat.datatable.select.js');

	// Own Inventories
	$tableHeader[] = t('Label');
	$tableHeader[] = t('Habitat name');
	$tableHeader[] = t('Linked Areas');
	$tableHeader[] = t('id');

	$result = db_query(
			'SELECT
				habitat.label,
				habitat.name_de,
				habitat.id,
				COUNT(area_habitat.area_id) AS count
			FROM habitat
				LEFT JOIN area_habitat ON area_habitat.habitat_id = habitat.id
				LEFT JOIN area ON area.id = area_habitat.area_id
			GROUP BY
				habitat.label,
				habitat.name_de,
				habitat.id
			ORDER BY
				habitat.label');

	$has_gallery = false;
	foreach ($result as $record) {
		$row = array();
		$row[] = $record->label;
		$row[] = $record->name_de;
		$row[] = $record->count;
		$row[] = $record->id;
		if (function_exists('gallery_view_link')) {
			$row[] = gallery_view_link('habitat', $record->id);
			if ($row[3])
				$has_gallery = true;
		}
		$rows[] = array(
				'data' => $row,
				'id' => 'habitat_' . $record->id
		);
	}

	if ($has_gallery)
		$tableHeader[] = '&nbsp;';
	else
		array_walk($rows, create_function('&$row', 'array_pop($row[\'data\']);'));

	$render_array['habitats'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#rows' => $rows,
			'#id_table' => 'habitats',
	);
	return $render_array;
}
?>