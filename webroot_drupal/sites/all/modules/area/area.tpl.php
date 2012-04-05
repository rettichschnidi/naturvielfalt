<?php
/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area.tpl.php
 */
?>
<div class="area-search-map">
  <div id="map">
    <div id="map_canvas"></div>
  </div>
</div>
<?php
// include CSS and JavaScripts
function area_add_js_url($url) {
	print "<script type='text/javascript' src='$url'></script>";
}

function area_add_css_url($url) {
	print "<link href='$url' type='text/css' rel='stylesheet'/>\n";
}
// Basepaths
$baseModulPath = base_path() . drupal_get_path('module', 'area') . '/';
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

area_add_js_url(
	"http://maps.google.com/maps/api/js?sensor=false&libraries=$libraries&region=CH&language="
			. $user->language . "\n");

if ($area_id > 0) {
	// should be included before area.js
	print "<script>areaid = $area_id;</script>\n";
}

print 
	"<script>scalecontrol = " . ($scalecontrol ? "true" : "false")
			. ";</script>\n";

area_add_js_url($baseModulJsPath . 'contrib/v3_epoly_sphericalArea.js');
area_add_js_url($baseModulJsPath . 'area-googlemapsapi-extensions.js');
area_add_js_url($baseModulJsPath . 'area.js');

area_add_css_url($baseModulCssPath . 'area-theme.css');

if ($search) {
	area_add_js_url($baseModulJsPath . 'area-search.js');
}

if ($ch1903) {
	area_add_js_url($baseModulJsPath . 'contrib/wgs84_ch1903.js');
	area_add_js_url($baseModulJsPath . 'area-ch1903.js');
}

if ($create) {
	area_add_js_url($baseModulJsPath . 'area-create.js');
}

if ($showall) {
	area_add_js_url($baseModulJsPath . 'area-show-all.js');
}

if ($edit) {
	area_add_js_url($baseModulJsPath . 'area-edit.js');
}
?>
