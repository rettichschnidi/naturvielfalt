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
// include CSS ...
function area_add_js_url($url) {
	print "<script type='text/javascript' src='$url'></script>\n";
}
// ... and JavaScripts
function area_add_css_url($url) {
	print "<link href='$url' type='text/css' rel='stylesheet'/>\n";
}

// Set basepaths for usage later on...
$baseModulPath = base_path() . drupal_get_path('module', 'area') . '/';
$baseModulJsPath = $baseModulPath . 'js/';
$baseModulCssPath = $baseModulPath . 'css/';

global $user;

/**
 * Options set for the google maps api:
 *  - libraries: 
 *  	- geometry (computation of geometric data on the surface of the earth)
 *  	- places (search functionality)
 *  	- drawing (tools to create overlays)
 *  - sensor: We do not support GPS devices on the users end - so this is always false
 *  - region: Set this manually to Switzerland (CH). All requests will be biased by swiss «rules».
 *  - language: Localize the google maps service to the desired language (most likely the users language)
 */

$libraries = 'geometry';
if ($search) {
	$libraries .= ',places';
}
if ($action == 'create' || $action == 'getcoordinate') {
	$libraries .= ',drawing';
}

area_add_js_url(
	"http://maps.google.com/maps/api/js?sensor=false&libraries=$libraries&region=CH&language="
			. $user->language . "\n");

/**
 * To edit a geometry, the area_id has to be set in javascript.
 */
if ($area_id > 0) {
	// should be included before area.js
	print "<script>areaid = $area_id;</script>\n";
}

/**
 * If requested, include an scale on the map.
 */
print 
	"<script>scalecontrol = " . ($scalecontrol ? "true" : "false")
			. ";</script>\n";

area_add_js_url($baseModulJsPath . 'contrib/v3_epoly_sphericalArea.js');
area_add_js_url($baseModulJsPath . 'area-googlemapsapi-extensions.js');
area_add_js_url($baseModulJsPath . 'area.js');

area_add_css_url($baseModulCssPath . 'area-theme.css');

/**
 * Include a searchbar if requested.
 */
if ($search) {
	area_add_js_url($baseModulJsPath . 'area-search.js');
}

/**
 * Include a searchbar/display for 1903 coordinates if requested.
 */
if ($ch1903) {
	area_add_js_url($baseModulJsPath . 'contrib/wgs84_ch1903.js');
	area_add_js_url($baseModulJsPath . 'area-ch1903.js');
}

/**
 * If existing area should be shown...
 */
switch ($show) {
case 'allareas':
	/**
	 * Display all of them
	 */
	area_add_js_url($baseModulJsPath . 'area-show-allareas.js');
	break;
case 'myareas':
	/**
	 * Display just the ones the user owns.
	 */
	area_add_js_url($baseModulJsPath . 'area-show-myareas.js');
	break;
}

/**
 * Decide which actions should be exectured...
 */
switch ($action) {
case 'create':
	/**
	 * Create a new area
	 */
	area_add_js_url($baseModulJsPath . 'area-create.js');
	break;
case 'edit':
	/**
	 * Create an existing area geometry
	 */
	area_add_js_url($baseModulJsPath . 'area-edit.js');
	break;
case 'getcoordinate':
	/**
	 * Allow the user of this theme to set a hidden filed to store the coordinates
	 * (encoded as JSON string)
	 */
	if ($coordinate_storage_id != false) {
		print 
			"<script>coordinate_storage_id = '$coordinate_storage_id';</script>\n";
	}
	/**
	 * Set a marker and update the hidden fields (provided by the user of this theme)
	 */
	area_add_js_url($baseModulJsPath . 'area-getcoordinate.js');
	break;
}
?>
