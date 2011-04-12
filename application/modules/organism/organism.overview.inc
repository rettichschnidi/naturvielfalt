<?php
/**
 * Menu callback. This will be called if someone visits drupal/showcase.
 */
function organism_overview() {
	drupal_set_title(t('Organismen - Übersicht nach Kategorien'));

	
	$header = array(t('Organismengruppe'),t('id'));
	
	$result = db_query("SELECT name, id FROM {inventory_type} order by name asc");
	
foreach ($result as $record) {
		$row = array();
		$row[] = l($record->name,"organism/type/".$record->id);
		$row[] = $record->id;
		//$link = l(t('new inventory'), 'inventory/new');
		
		$rows[] = $row;
	}
	
	$render_array['organisms'] = array(
    '#theme' => 'table', 
	'#header' => $header,
    '#rows' => $rows,
	);
	return $render_array;
}
?>