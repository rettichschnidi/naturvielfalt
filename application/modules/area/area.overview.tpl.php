<script type="text/javascript" src="http://maps.google.com/maps/api/js?key=ABQIAAAABuqUv_uyCZ4WzUTgK5G-thR8vyPbVAPvWWUSjekUdI5ADbIJJRSNaY0WIlEy744RJmMGHB5KrWGGKw&sensor=false"></script>
<script	type="text/javascript" src="../inventoryServer/web_root/public/css/overlay-style.js"></script>
<script	type="text/javascript" src="../inventoryServer/web_root/public/js/lib/jquery/jquery-ui-1.8.6.custom.min.js"></script>
<script	type="text/javascript" src="../inventoryServer/web_root/public/js/lib/jquery/jquery.dataTables.js"></script>
<script	type="text/javascript" src="../inventoryServer/web_root/public/js/lib/jquery/jquery.dataTables.pluginAPI.js"></script>
<script	type="text/javascript" src="modules/inventory/js/lib/jquery/ui.geo_autocomplete.js"></script>
<script	type="text/javascript" src="modules/inventory/js/util.js"></script>
<script	type="text/javascript" src="modules/area/js/lib/rwo_gmaps/Config.js"></script>
<script	type="text/javascript" src="modules/area/js/lib/rwo_gmaps/v3_epoly_sphericalArea.js"></script>
<script	type="text/javascript" src="modules/area/js/lib/rwo_gmaps/MapGeometryOverlays.js"></script>
<script	type="text/javascript" src="modules/area/js/lib/rwo_gmaps/MapCache.js"></script>
<script	type="text/javascript" src="modules/area/js/lib/rwo_gmaps/GeometryOverlay.js"></script>
<script	type="text/javascript" src="modules/area/js/lib/rwo_gmaps/Polygon.js"></script>
<script	type="text/javascript" src="modules/area/js/lib/rwo_gmaps/Marker.js"></script>
<script	type="text/javascript" src="modules/area/js/area-select.js"></script>

<div id="content">
<h1><?php echo t('Search by Google Maps:'); ?></h1>
	<div id="map_search-element">
		<input type="text" id="map_search" name="map_search" value="" />
		<button id="map_search_button"><?php echo t('Search map'); ?></button>
		<button id="create-inventoryByMaps"><?php echo t('create inventory'); ?></button>
	</div>
	<div id="map">
		<div id="map_canvas" style="height: 100%"></div>
	</div>
	<h1><?php echo t('Search by field name:'); ?></h1>
	<button id="create-inventoryByFieldName"><?php echo t('create inventory'); ?></button>
	<div>
		<table cellpadding="0" cellspacing="0" border="0" class="display" id="area_table">
		</table>
	</div>
</div>
