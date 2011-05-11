<?php
// include JavaScripts
$areaJsPath = base_path() . drupal_get_path('module', 'area') . '/js/';
$areaCSSPath = base_path() . drupal_get_path('module', 'area') . '/css/';
$inventoryJsPath = base_path() . drupal_get_path('module', 'inventory') . '/js/';
$inventoryServerPublicPath = base_path() . '../inventoryServer/web_root/public/';

$jsLibs = array(
	'http://maps.google.com/maps/api/js?key=ABQIAAAABuqUv_uyCZ4WzUTgK5G-thR8vyPbVAPvWWUSjekUdI5ADbIJJRSNaY0WIlEy744RJmMGHB5KrWGGKw&sensor=false',
	$areaJsPath . 'lib/rwo_gmaps/v3_epoly_sphericalArea.js',
	$areaCSSPath . 'overlay-style.js',
	$inventoryServerPublicPath . 'js/lib/jquery/jquery-ui-1.8.6.custom.min.js',
	$inventoryJsPath . 'lib/jquery/ui.geo_autocomplete.js',
	$inventoryJsPath . 'util.js',
	$areaJsPath . 'lib/rwo_gmaps/Config.js',
	$areaJsPath . 'lib/rwo_gmaps/MapCache.js',
	$areaJsPath . 'lib/rwo_gmaps/MapAjaxProxy.js',
	$areaJsPath . 'lib/rwo_gmaps/GeometryOverlayControl.js',
	$areaJsPath . 'lib/rwo_gmaps/GeometryOverlay.js',
	$areaJsPath . 'lib/rwo_gmaps/PolygonControl.js',
	$areaJsPath . 'lib/rwo_gmaps/Polygon.js',
	$areaJsPath . 'lib/rwo_gmaps/MarkerControl.js',
	$areaJsPath . 'lib/rwo_gmaps/Marker.js',
	$areaJsPath . 'area.js',
);
foreach ($jsLibs as $jsLib) {
	echo "<script type=\"text/javascript\" src=\"" . $jsLib . "\"></script>\n";
}
 ?>

<dt id="map_search-label">
	<label for="map_search"></label>
</dt>
<dd id="map_search-element">
	<input type="text" id="map_search" name="map_search" value="" />
	<button id="map_search_button">
	<?php echo t('Search map'); ?>
	</button>
</dd>

<div id="map">
	<div id="map_canvas" style="height: 100%"></div>
</div>

<div style="display: none">
	<form id="Area" enctype="application/x-www-form-urlencoded" method="post" action="new_prop">
		<input type="hidden" name="locality" id="locality" value="" /> 
		<input type="hidden" name="zip" id="zip" value="" />
		<input type="hidden" name="township" id="township" value="" />
		<input type="hidden" name="canton" id="canton" value="" />
		<input type="hidden" name="country" id="country" value="" />
		<input type="hidden" name="altitude" id="altitude" value="" />
		<input type="hidden" name="surface_area" id="surface_area" value="" />
		<input type="hidden" name="latitude" id="latitude" value="" />
		<input type="hidden" name="longitude" id="longitude" value="" />
		<input type="hidden" name="area_coords" id="area_coords" value="" />
		<input type="hidden" name="area_type" id="area_type" value="" />
	</form>
</div>