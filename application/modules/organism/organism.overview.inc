<?php
/**
 * Menu callback. This will be called if someone visits drupal/showcase.
 */
function organism_overview() {
	drupal_set_title(t('Organisms - Grouped by Categories'));
	drupal_add_css(drupal_get_path('module', 'organism') . '/css/organism.show.css');
	drupal_add_js(drupal_get_path('module', 'organism') . '/js/selectTableRow.js');

	$header = array(t('Organism Groups'),t('Organism count'));
	
	$result = db_query('
		SELECT name, inventory_type_id, count (inventory_type_id) FROM organism
			LEFT JOIN inventory_type it ON it.id = organism.inventory_type_id
		GROUP BY inventory_type_id, name
		ORDER BY name
	');
	
	foreach($result as $record){
		$row = array();
		$row[] = $record->name;
		$row[] = $record->count;		
		$rows[] = array('data' => $row, 'id' => 'organismtype_'. $record->inventory_type_id);
	}
	
	$render_array['organisms'] = array(
    '#theme' => 'datatable', 
	'#header' => $header,
    '#rows' => $rows,
	'#id_table' => 'organisms',
	);
	return $render_array;
}
?>