<?php
function inventory_overview() {
	drupal_set_title(t('Inventory overview'));

	$tableVariables['header'][] = t('Area');
	$tableVariables['header'][] = t('InventoryGroup');
	
	$result = db_query("SELECT a.field_name, hi.id, hi.name
		FROM head_inventory hi 
		LEFT JOIN area a ON hi.area_id = a.id
		ORDER BY hi.id desc;");
	
	foreach ($result as $record) {
		$inventoryGroupId = $record->id;
		$field_name = $record->field_name;
		$name = $record->name;
		$row = array();
		$row[] = $field_name;
		$row[] = l(t($name." (".$inventoryGroupId.")"), 'inventory/' . $inventoryGroupId);
		$rows[] = $row;
	}
	$tableVariables['rows'] = $rows;
	$link = l(t('new inventory'), 'inventory/new');

	$render_array['inventories'] = array(
    '#theme' => 'inventory_overview', 
   	'#items' => $tableVariables,
	'#introduction' => t('Create a !inventoryLink or open an existing one:', array('!inventoryLink' => $link)),
	);
	return $render_array;
}
?>