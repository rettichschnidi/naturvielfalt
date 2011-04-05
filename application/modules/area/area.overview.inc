<?php
function show_area_overview() {
	drupal_set_title(t('Area overview'));

	// call our own theme
	return theme('area.overview');
}
?>