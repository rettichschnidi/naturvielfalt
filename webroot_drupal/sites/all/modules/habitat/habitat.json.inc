<?php
/**
 * @file habitat.json.inc
 * @author Reto Schneider, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

/**
 * Export all habitats as datatable JSON.
 * 
 * @return JSON
 */
function habitat_list_datatablejson() {
	$sql['dbColumns'] = array(
			'h.id AS id',
			'h.label AS label',
			'h.name AS name',
			'h.name_scientific AS name_scientific',
			'COUNT(ah) AS area_count',
	);

	$sql['dbSearchColumns'] = array(
			'h.label',
			'h.name',
			'h.name_scientific'
	);

	$sql['dbTable'] = '{habitat} h';
	$sql['dbJoins'] = 'LEFT JOIN {area_habitat} ah ON h.id = ah.habitat_id';
	$sql['dbGroupBy'] = 'h.id, h.label, h.name, h.name_scientific';
	$sql['id'] = 'id';
	$output = datatable_dbRequest($sql);

	// Add a link to the gallery of each habitat
	global $base_url;
	$imageUrl = $base_url . '/' . drupal_get_path('module', 'habitat') . '/images/image.png';
	foreach ($output['rows'] as &$row) {
		$link = l(
			"<img src='$imageUrl' />",
			'gallery/habitat/' . $row['id'] . '/images',
			array(
					'html' => true
			));
		$row['cell']['image_link'] = $link;
	}
	return drupal_json_output($output);
}

?>
