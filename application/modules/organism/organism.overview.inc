<?php
/**
 * Menu callback. This will be called if someone visits drupal/showcase.
 */
function organism_overview() {
	drupal_set_title(t('Organisms - Grouped by Categories'));

	$header = array(t('Organism Groups'),t('Organism count'));
	
	$result = db_query("SELECT name, id FROM {inventory_type} order by name asc");
	
foreach ($result as $record) {
		$counts = db_query('SELECT COUNT (inventory_type_id) 
			from organism 
			WHERE inventory_type_id = :id', 
			array(':id' => $record->id)
		);
		foreach($counts as $count) {
			$row = array();
			$row[] = l($record->name,"organism/type/".$record->id);
			$row[] = $count->count;
			$rows[] = $row;			
		}	
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