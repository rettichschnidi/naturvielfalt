<?php
function show_area_search() {
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-select.css');
	drupal_set_title(t('Search area'));

	// call our own theme
	return theme('area.search');
}
?>