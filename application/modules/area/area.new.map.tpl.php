<script type="text/javascript" src="http://maps.google.com/maps/api/js?key=ABQIAAAABuqUv_uyCZ4WzUTgK5G-thR8vyPbVAPvWWUSjekUdI5ADbIJJRSNaY0WIlEy744RJmMGHB5KrWGGKw&sensor=false"></script>
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/v3_epoly_sphericalArea.js"></script>
<script type="text/javascript" src="../inventoryServer/web_root/public/css/overlay-style.js"></script>
<script type="text/javascript" src="../inventoryServer/web_root/public/js/lib/jquery/jquery-ui-1.8.6.custom.min.js"></script>
<script type="text/javascript" src="modules/inventory/js/lib/jquery/ui.geo_autocomplete.js"></script>
<script type="text/javascript" src="modules/inventory/js/util.js"></script>
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/Config.js"></script>
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/MapCache.js"></script>
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/MapAjaxProxy.js"></script>
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/GeometryOverlayControl.js"></script>
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/GeometryOverlay.js"></script>
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/PolygonControl.js"></script>
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/Polygon.js"></script>
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/MarkerControl.js"></script>
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/Marker.js"></script>
<script type="text/javascript" src="modules/area/js/area.js"></script>

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
	<button id="continue" class="form-submit"><?php echo t('Continue...'); ?></button>
</div>

<div style="display: none">
	<form id="Area" enctype="application/x-www-form-urlencoded" method="post" action="area_new_prop">
		<input type="hidden" name="locality" id="locality" value="" /> 
		<input type="hidden" name="zip" id="zip" value="" />
		<input type="hidden" name="township" id="township" value="" />
		<input type="hidden" name="canton" id="canton" value="" />
		<input type="hidden" name="altitude" id="altitude" value="" />
		<input type="hidden" name="surface_area" id="surface_area" value="" />
		<input type="hidden" name="latitude" id="latitude" value="" />
		<input type="hidden" name="longitude" id="longitude" value="" />
		<input type="hidden" name="area_coords" id="area_coords" value="" />
		<input type="hidden" name="area_type" id="area_type" value="" />
	</form>
</div>