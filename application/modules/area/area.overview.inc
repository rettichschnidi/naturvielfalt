<?php
function show_area_overview() {
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-select.css');
	drupal_set_title(t('Area overview'));

	// call our own theme
	return theme('area.overview');
}
?>