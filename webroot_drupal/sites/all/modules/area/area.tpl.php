<?php
// include CSS and JavaScripts
$baseModulPath = drupal_get_path('module', 'area') . '/';
$baseModulJsPath = $baseModulPath . 'js/';
$baseModulCssPath = $baseModulPath . 'css/';

global $user;

/**
 * Options set:
 *  - libraries: places (search functionality) and geometry (computation of geometric data on the surface of the earth)
 *  - sensor: We do not support GPS devices on the users end - so this is always false
 *  - region: Set this manually to Switzerland (CH). All requests will be biased by swiss rules
 *  - language: Localize the google maps service to the users language
 */
$libraries = 'geometry';

if ($search) {
	$libraries .= ',places';
}
if ($create) {
	$libraries .= ',drawing';
}

drupal_add_js(
	"http://maps.google.com/maps/api/js?sensor=false&libraries=$libraries&region=CH&language="
			. $user->language,
	array('group' => JS_LIBRARY));

drupal_add_js($baseModulJsPath . 'area.js');
drupal_add_css($baseModulCssPath . 'area.css');
if ($search) {
	drupal_add_css($baseModulCssPath . 'area-search.css');
	drupal_add_js($baseModulJsPath . 'area-search.js');
}
if ($create) {
	drupal_add_css($baseModulCssPath . 'area-create.css');
	drupal_add_js($baseModulJsPath . 'area-create.js');
}
?>

<div class="area-search-map">
  <div id="map" style="height: 600px; width: 100%;">
    <div id="map_canvas" style="height: 100%"></div>
  </div>
</div>
