<?php
function show_area_new_map() {
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css');
	drupal_set_title(t('Create a new area'));

	// call our own theme
	return theme('area.new.map');
}
?>