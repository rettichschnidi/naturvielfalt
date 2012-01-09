<?php

drupal_add_library('system', 'ui.autocomplete');

// include JavaScripts
$areaJsPath = base_path() . drupal_get_path('module', 'area') . '/js/';
$areaCSSPath = base_path() . drupal_get_path('module', 'area') . '/css/';
$inventoryJsPath = base_path() . drupal_get_path('module', 'inventory') . '/js/';

$jsLibs = array(
		'http://maps.google.com/maps/api/js?sensor=false&libraries=geometry,places',
		$areaJsPath . 'lib/rwo_gmaps/v3_epoly_sphericalArea.js',
		$areaCSSPath . 'overlay-style.js',
		$inventoryJsPath . 'lib/jquery/ui.geo_autocomplete.js',
		$inventoryJsPath . 'util.js',
		$areaJsPath . 'geometry.js',
		$areaJsPath . 'area-select.js',
);
foreach ($jsLibs as $jsLib) {
	echo "<script type=\"text/javascript\" src=\"" . $jsLib . "\"></script>\n";
}
?>