<?php
function show_area($areaId) {
	$areaResult = db_query("SELECT * FROM area WHERE id = :id", array(':id' => $areaId));
	$area = $areaResult->fetch();
	drupal_set_title($area->field_name);
	$output = theme('item_list', array('title' => $area->field_name));
	return $output;
}
?>