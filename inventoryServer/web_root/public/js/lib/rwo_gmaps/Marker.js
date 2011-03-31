/**
 * @author Roger Wolfer
 */
Marker.prototype = new GeometryOverlay();
Marker.prototype.constructor = Marker;

function Marker(map, opts){
	if(map){
	  this.initGO(map, opts);
		this.initM(map, opts);
	}
};

Marker.prototype.initM = function(map, opts){
	this.type = 'marker';
	this.id = opts && opts.id ? opts.id: null;
	this.name = this.type;
	this._area = 0;
	this.removeListener = function(){};
	this.map = map;
	this.marker = this.gOverlay = this.createMarker(opts);
	this.style = 'marker';
};

Marker.prototype.createMarker = function(markerOpts){
    var me = this;
    var marker = new google.maps.Marker();
    if(markerOpts && markerOpts.area_points){
        marker.setPosition(new google.maps.LatLng(markerOpts.area_points[0].lat, markerOpts.area_points[0].lng));
        marker.setDraggable(false);
    } else {
        marker.setDraggable(true);
        google.maps.event.addListener(marker, 'click', function() {
            me.marker.setMap(null);
            me.removeListener();
        });
    }
    marker.setMap(me.map);

    console.info('Marker: created');
    return marker;
};

Marker.prototype.startEditing = function(){
	this.marker.setDraggable(true);
};

Marker.prototype.stopEditing = function(){
	this.marker.setDraggable(false);
};

/**
 * @param {Object} coord
 */
Marker.prototype.addLatLng = function(latLng){
	this.marker.setPosition(latLng);
};

/**
 * @param {Object} coords
 */
Marker.prototype.setLatLngs = function(coords){
	this.addCoord(coords[0]);
};

Marker.prototype.getLatLng = function(){
	return this.marker.getPosition();
};

Marker.prototype.setStyle = function(style){
  var _style;

    switch(style) {
        case 'selected':
            this.style = 'marker-selected';
            break;
        case 'selected-disable':
            if(this.style == 'marker-selected') {
              this.style = 'marker';
            }
            break;
        case 'highlighted':
            if(this.style != 'marker-selected') {
              this.style = 'marker-highlighted';
            }
            break;
        case 'highlighted-disable':
            if(this.style == 'marker-highlighted') {
                this.style = 'marker';
            }
            break;
        default:
            this.style = 'marker';
    }
  this.marker.setOptions(overlayStyle[this.style]);
};

Marker.prototype.getCenter = function(){
  return this.getLatLng();
};

Marker.prototype.getArea = function(){
  return this._area;
};

Marker.prototype.toJson = function(){
  var json = {
    type: this.type,
    attr: {},
    coords: {}
  };
  var latLng = this.marker.getPosition();
  json.coords[0] = {
      'lat' : latLng.lat(),
      'lng': latLng.lng()
  };
  return json;
};
