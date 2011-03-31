<?php
function inventory_overview() {
	drupal_set_title(t('Inventory overview'));

	$tableVariables['header'][] = t('Area');
	$tableVariables['header'][] = t('Inventory');
	$result = db_query("SELECT a.field_name, i.id AS iid
		FROM inventory i
		LEFT JOIN head_inventory hi ON i.head_inventory_id = hi.id 
		LEFT JOIN area a ON hi.area_id = a.id
		ORDER BY i.id desc;");
	foreach ($result as $record) {
		$iid = $record->iid;
		$field_name = $record->field_name;
		$row = array();
		$row[] = $field_name;
		$row[] = l(t('open'), 'inventory/' . $iid);
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