<?php
// include JavaScripts
$areaJsPath = base_path() . drupal_get_path('module', 'area') . '/js/';
$areaCSSPath = base_path() . drupal_get_path('module', 'area') . '/css/';
$inventoryJsPath = base_path() . drupal_get_path('module', 'inventory') . '/js/';

$jsLibs = array(
	'http://maps.google.com/maps/api/js?key=ABQIAAAABuqUv_uyCZ4WzUTgK5G-thR8vyPbVAPvWWUSjekUdI5ADbIJJRSNaY0WIlEy744RJmMGHB5KrWGGKw&sensor=false',
$areaJsPath . 'lib/rwo_gmaps/v3_epoly_sphericalArea.js',
$areaCSSPath . 'overlay-style.js',
$inventoryJsPath . 'lib/jquery/jquery-ui-1.8.6.custom.min.js',
$inventoryJsPath . 'lib/jquery/ui.geo_autocomplete.js',
$inventoryJsPath . 'util.js',
$areaJsPath . 'lib/rwo_gmaps/Config.js',
$areaJsPath . 'lib/rwo_gmaps/MapGeometryOverlays.js',
$areaJsPath . 'lib/rwo_gmaps/GeometryOverlay.js',
$areaJsPath . 'lib/rwo_gmaps/GeometryOverlayControl.js',
$areaJsPath . 'lib/rwo_gmaps/Polygon.js',
$areaJsPath . 'lib/rwo_gmaps/Marker.js',
$areaJsPath . 'area-select.js',
);
foreach ($jsLibs as $jsLib) {
	echo "<script type=\"text/javascript\" src=\"" . $jsLib . "\"></script>\n";
}
?>
<p>
<?php echo t('Please search an area either by Google Maps or by it\'s field name.');?>
</p>