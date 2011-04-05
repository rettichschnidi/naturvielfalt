<?php
function show_area_overview() {
	drupal_set_title(t('Area overview'));

	$tableVariables['header'][] = t('Name');
	$tableVariables['header'][] = t('Comment');
	$result = db_query("SELECT a.id, a.field_name, a.comment FROM area a;");
	foreach ($result as $record) {
		$field_name = l($record->field_name, 'area/' . $record->id);
		$comment = $record->comment;
		$row = array();
		$row[] = $field_name;
		$row[] = $comment;
		$rows[] = $row;
	}
	$tableVariables['rows'] = $rows;
	
	return theme('table', $tableVariables);
}
?>