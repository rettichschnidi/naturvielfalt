<?php
// include CSS and JavaScripts
$baseModulPath = drupal_get_path('module', 'area') . '/';
$baseModulJsPath = $baseModulPath . 'js/';
$baseModulCssPath = $baseModulPath . 'css/';

global $user;

/**
 * Options set:
 *  - libraries: 
 *  	- geometry (computation of geometric data on the surface of the earth)
 *  	- places (search functionality)
 *  	- drawing (toosl to create overlays)
 *  - sensor: We do not support GPS devices on the users end - so this is always false
 *  - region: Set this manually to Switzerland (CH). All requests will be biased by swiss «rules».
 *  - language: Localize the google maps service to the desired language (most likely the users language)
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

if (is_int((int)$area_id)) {
	// has to be included before area.js
	drupal_add_js("areaid = $area_id;", array('type' => 'inline'));
}

drupal_add_js($baseModulJsPath . 'contrib/v3_epoly_sphericalArea.js');
drupal_add_js($baseModulJsPath . 'area-googlemapsapi-extensions.js');
drupal_add_js($baseModulJsPath . 'area-overlay-style.js');
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

if ($showall) {
	drupal_add_css($baseModulCssPath . 'area-show-all.css');
	drupal_add_js($baseModulJsPath . 'area-show-all.js');
}

if ($edit) {
	drupal_add_css($baseModulCssPath . 'area-edit.css');
	drupal_add_js($baseModulJsPath . 'area-edit.js');
}
?>

<div class="area-search-map">
  <div id="map">
    <div id="map_canvas"></div>
  </div>
</div>
