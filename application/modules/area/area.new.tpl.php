<?php

drupal_add_library('system', 'ui.autocomplete');

// include JavaScripts
$areaJsPath = base_path() . drupal_get_path('module', 'area') . '/js/';
$areaCSSPath = base_path() . drupal_get_path('module', 'area') . '/css/';
$inventoryJsPath = base_path() . drupal_get_path('module', 'inventory') . '/js/';

$jsLibs = array(
  'http://maps.google.com/maps/api/js?key=ABQIAAAABuqUv_uyCZ4WzUTgK5G-thR8vyPbVAPvWWUSjekUdI5ADbIJJRSNaY0WIlEy744RJmMGHB5KrWGGKw&sensor=false',
$areaJsPath . 'lib/rwo_gmaps/v3_epoly_sphericalArea.js',
$areaCSSPath . 'overlay-style.js',
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

<h3 class="choice">
  <?php 
    echo t('You have two choices. You can:');
  ?>
</h3>
<ul class="area-choices">
  <li>
    <div class="controlAreaChoose selected"></div>
    <?php 
      echo t('select an area by clicking on it in the map or in the table');
    ?>
  </li>
  <li>
    <div class="controlAreaCreate"></div>
    <?php
      echo t('create a new area by using the drawing symbol and drawing on the map');
    ?>
  </li>
</ul>