<?php
function inventory_overview() {
	drupal_set_title(t('Inventory overview'));

	$tableVariables['header'][] = t('Area');
	$tableVariables['header'][] = t('Inventory');
	$tableVariables['header'][] = t('Owner');
	$tableVariables['header'][] = t('Shared');
	
	global $user;
	
	$result = db_query("SELECT a.field_name, hi.id, hi.name,hi.owner_id, hi.shared
		FROM head_inventory hi 
		LEFT JOIN area a ON hi.area_id = a.id
		WHERE (hi.shared='t') OR  (hi.owner_id=:id)
		ORDER BY hi.id desc;",
		array(':id' => $user->uid)
	);
	
	foreach ($result as $record) {
		$inventoryGroupId = $record->id;
		$field_name = $record->field_name;
		if(!is_null($record->name))
			$name = $record->name;
		else $name = t("no name");
		$owner=user_load($record->owner_id)->name;
		if($shared = $record->shared == TRUE)
			$shared = t("Yes");
		else $shared = t("No");
		$row = array();
		$row[] = $field_name;
		$row[] = l(t($name." (".$inventoryGroupId.")"), 'inventory/' . $inventoryGroupId);
		$row[] = $owner;
		$row[] = $shared;
		$rows[] = $row;
	}
	$tableVariables['rows'] = $rows;
	$link = l(t('new inventory'), 'inventory/new');

	$render_array['inventories'] = array(
    '#theme' => 'inventory_overview', 
   	'#items' => $tableVariables,
	);
	return $render_array;
}
?>