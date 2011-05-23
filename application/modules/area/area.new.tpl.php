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

<h3 style="margin-top: 10px">
	<?php 
		echo t('You have two choices. You can:');
	?>
	<ul>
		<li>
			<div style="float: left; margin: 0px 5px 5px 0px" class="controlAreaChoose selected"></div>
			<?php 
				echo t('select an area by clicking on it in the map or in the table');
			?>
		</li>
		<li style="clear:both">
			<div style="float: left;  margin: 0px 5px 5px 0px" class="controlAreaCreate"></div>
			<?php
				echo t('create a new area by using the drawing symbol and drawing on the map');
			?>
		</li>
	</ul>
</h3>