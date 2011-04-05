<script type="text/javascript" src="http://maps.google.com/maps/api/js?key=ABQIAAAABuqUv_uyCZ4WzUTgK5G-thR8vyPbVAPvWWUSjekUdI5ADbIJJRSNaY0WIlEy744RJmMGHB5KrWGGKw&sensor=false"></script>
<script type="text/javascript" src="../inventoryServer/web_root/public/css/overlay-style.js"></script> 
<script type="text/javascript" src="../inventoryServer/web_root/public/js/lib/jquery/jquery-ui-1.8.6.custom.min.js"></script> 
<script type="text/javascript" src="../inventoryServer/web_root/public/js/lib/jquery/jquery.dataTables.js"></script> 
<script type="text/javascript" src="../inventoryServer/web_root/public/js/lib/jquery/jquery.dataTables.pluginAPI.js"></script> 
<script type="text/javascript" src="modules/inventory/js/lib/jquery/ui.geo_autocomplete.js"></script> 
<script type="text/javascript" src="modules/inventory/js/util.js"></script> 
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/Config.js"></script> 
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/MapGeometryOverlays.js"></script> 
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/MapCache.js"></script> 
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/GeometryOverlay.js"></script> 
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/Polygon.js"></script> 
<script type="text/javascript" src="modules/area/js/lib/rwo_gmaps/Marker.js"></script> 
<script type="text/javascript" src="modules/area/js/area-select.js"></script> 

<div id="content"><!-- application/views/scripts/area-select/index.phtml -->
  <h2>Gebiet auswählen</h2>
    <div>
    <table cellpadding="0" cellspacing="0" border="0" class="display" id="area_table">
   
   </table>
  </div>
 <br/>
  <div>
    <dd id="map_search-element">
      <input type="text" id= "map_search" name="map_search" value="" />
      <button id="map_search_button">Los</button>
   </dd>  
  </div>
  <div id='map'>
     <div id="map_canvas"></div>
  </div>
  <div>
     <button id="new-area">Neues Gebiet</button>
     <button id="create-inventory">Inventar erstellen</button>
  </div>
 </div> 