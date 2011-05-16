<?php
/**
 * Menu callback. This will be called if someone visits drupal/showcase.
 */
function organism_overview() {
	drupal_set_title(t('Organisms - Grouped by Categories'));

	$header = array(t('Organism Groups'),t('Organism count'));
	
	$result = db_query('
		SELECT name, inventory_type_id, count (inventory_type_id) FROM organism
			LEFT JOIN inventory_type it ON it.id = organism.inventory_type_id
		GROUP BY inventory_type_id, name
		ORDER BY name
	');
	
	foreach($result as $record){
		$row = array();
		$row[] = l($record->name,"organism/type/".$record->inventory_type_id);
		$row[] = $record->count;		
		$rows[] = $row;
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