/**
 * @author Roger Wolfer
 * This object holds the map and all the geometry overlays on the map.
 * 
 */

/*
 * Events to listen to:
 * Map dragging --> load new Overlays, remove out of sight overlays.
 * Zooming --> load new overlays for the zoom level and remove old ones.
 */
function MapGeometryOverlays(map){
	this.map = map;
  this.bounds = new google.maps.LatLngBounds();
	this.overlays = []; //contains all the overlays currently shown on the map; key = overlay.id
}
/*
MapGeometryOverlays.prototype.addOverlay = function(overlay){
	this.overlays.push(overlay);
	overlay.getOverlay().setMap(this.map);
}

MapGeometryOverlays.prototype.removeOverlay = function(overlay){
	this.overlays[overlay.id] = undefined;
	overlay.getOverlay().setMap(null);
}
*/

MapGeometryOverlays.prototype.addOverlaysJson = function(overlayJson){
    var me = this;
    var overlay;
    for(var i in overlayJson) {
        if(overlayJson[i].type == 'polygon') {
            overlay = new Polygon(me.map, overlayJson[i]);
            var points = overlay.getLatLngs().getArray();
            for(var n=0; n<points.length; n++)
              me.bounds.extend(points[n]);
            me.overlays[overlayJson[i].id] = overlay;
        } else if (overlayJson[i].type == 'marker') {
            overlay = new Marker(me.map, overlayJson[i]);
            me.bounds.extend(overlay.getLatLng());
            me.overlays[overlayJson[i].id] = overlay;
        } else {
            console.error('Type of overlay is undefined!');
        }
    }
    me.map.fitBounds(me.bounds);
};

//Removes all Overlays from the Map
MapGeometryOverlays.prototype.clear = function(){
  for(id in this.overlays){
    this.overlays[id].show(false);
  }
  this.overlays = [];
};

MapGeometryOverlays.prototype.selectOverlay = function(overlayId){
  var me = this;
  console.info("select: "+overlayId);
  me.overlays[overlayId].setStyle('selected');
};

MapGeometryOverlays.prototype.highlightOverlay = function(overlayId){
  var me = this;
  console.info("highlight: "+overlayId);
  me.overlays[overlayId].setStyle('highlighted');
};
