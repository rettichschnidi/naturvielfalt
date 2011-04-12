/**
 * @author Roger Wolfer
 *
 * Class for handling (drawing, editing, show) polygons
 */
	Polygon.prototype = new GeometryOverlay();
	Polygon.prototype.constructor = Polygon;

/**
 * Constructor
 * @var map google.maps.Map map on which the polygon is shown
 * @var opt_opts array, optional options
 */
function Polygon(map, opt_opts){
	this.initGO(map, opt_opts);
	this.initPgon(map, opt_opts);
};

Polygon.prototype.initPgon = function(map, opts){
	this.type = 'polygon';
	this.name = this.type;
	this.id = opts && opts.id ? opts.id: null;
	this.map = map;
	this.style = '';
	this.removeListener = function(){};
	this.polygon = this.gOverlay = new google.maps.Polygon(overlayStyle.polygon);
	if(opts && opts.area_points){
	  var latLngs = [];
	  for(var i in opts.area_points){
	    latLngs[i] = new google.maps.LatLng(opts.area_points[i].lat, opts.area_points[i].lng);
	  }
	  this.setLatLngs(latLngs, false);
	}
	this.markers = []; // markers at the vertexes to edit/delete the vertex of a polygon
	this.vmarkers = []; // markers on line between vertexes
	this.tmpPolyLine = new google.maps.Polyline(); // if marker is draged a polyline is shown
	this.tmpPolyLine.setMap(this.map);
	this.polygon.setMap(this.map);

};

/**
 * Returns LatLng at vertex position
 * @var int vertex position
 * @return google.maps.LatLng at position
 */
Polygon.prototype.getLatLng = function(latLngAt){
	return this.polygon.getPath().getAt(latLngAt);
};

/**
 * Returns polygon path
 * @return google.maps.MVCArray<google.maps.LatLng>
 */
Polygon.prototype.getLatLngs = function(){
	return this.polygon.getPath();
};

/**
 * Create polygon from path
 * @var latLngs array<google.maps.LatLng>
 * @var marker_on if true, created polygon is editable otherwise not.
 */
Polygon.prototype.setLatLngs = function(latLngs, marker_on){
  var marker_on = typeof(marker_on) != 'undefined' ? marker_on : true;
  
  this.polygon.setPath(latLngs);
  if(marker_on){
    	this.createControlMarkers(latLngs);
  }
	
};

/**
 * Add vertex to polygon
 * @var latLng google.maps.LatLng new vertex
 * @var marker_on if true, vertex is editable
 */
Polygon.prototype.addLatLng = function(latLng, marker_on){
    var marker_on = typeof(marker_on) != 'undefined' ? marker_on : true;
  
    this.polygon.getPath().push(latLng);
    if(marker_on){
        this.createControlMarker(latLng);
        if(this.markers.length != 1){
            this.createVMarker(latLng);
        }
    }
};

/**
 * Create control markers for path to edit vertexes of polygon
 * @var latLngs array<google.maps.LatLng>
 */
Polygon.prototype.createControlMarkers = function(latLngs){
	this.markers.length = 0;
	for(latLng in latLngs){
		this.createControlMarker(latLng);
	}
};

/**
 * Create control marker for a vertex at specified coordinate
 * @var latLng google.maps.LatLng point at which a control marker is created
 */
Polygon.prototype.createControlMarker = function(latLng) {
	var me = this;
	
	var imageNormal = new google.maps.MarkerImage(
		Drupal.settings.basePath + "modules/area/js/lib/rwo_gmaps/images/square.png",
		new google.maps.Size(11, 11),
		new google.maps.Point(0, 0),
		new google.maps.Point(6, 6)
	);
	var imageHover = new google.maps.MarkerImage(
		Drupal.settings.basePath + "modules/area/js/lib/rwo_gmaps/images/square_over.png",
		new google.maps.Size(11, 11),
		new google.maps.Point(0, 0),
		new google.maps.Point(6, 6)
	);
	var marker = new google.maps.Marker({
		position: latLng,
		map: me.map,
		icon: imageNormal,
		draggable: true
	});
	google.maps.event.addListener(marker, "mouseover", function() {
		marker.setIcon(imageHover);
	});
	google.maps.event.addListener(marker, "mouseout", function() {
		marker.setIcon(imageNormal);
	});
	google.maps.event.addListener(marker, "drag", function() {
		for (var m = 0; m < me.markers.length; m++) {
			if (me.markers[m] == marker) {
				me.gOverlay.getPath().setAt(m, marker.getPosition());
				me.moveVMarker(m);
				break;
			}
		}
		m = null;
	});
	google.maps.event.addListener(marker, "click", function() {
     for (var m = 0; m < me.markers.length; m++) {
      if (me.markers[m] == marker) {
        marker.setMap(null);
        me.markers.splice(m, 1);
        me.gOverlay.getPath().removeAt(m);
        me.removeVMarkers(m);
        break;
      }
    }
    m = null;
    me.removeListener();
	});
	me.markers.push(marker);
	return marker;
	};

/**
 * Show/hide control markers
 * @var show true: show control markers, false: hide control markers
 */
Polygon.prototype.showControlMarkers = function(show){
	for(marker in this.markers){
		this.showControlMarker(marker, show);
	}
};

/**
 * Show/hide control marker
 * @var marker google.maps.Marker marker to show
 */
Polygon.prototype.showControlMarker = function(marker, show){
	if (typeof show == undefined) {
		var show = true;
	}
	
	if (show) {
		marker.setMap(this.map);
	}
	else {
		marker.setMap(null);
	}
};

/**
 * Create Marker between last vertex and current vertex
 * @var latLng google.maps.LatLng current vertex
 */
Polygon.prototype.createVMarker = function(latLng) {
	var me = this;
	var prevpoint = me.markers[me.markers.length-2].getPosition();
	var imageNormal = new google.maps.MarkerImage(
		Drupal.settings.basePath + "modules/area/js/lib/rwo_gmaps/images/square_transparent.png",
		new google.maps.Size(11, 11),
		new google.maps.Point(0, 0),
		new google.maps.Point(6, 6)
	);
	var imageHover = new google.maps.MarkerImage(
		Drupal.settings.basePath + "modules/area/js/lib/rwo_gmaps/images/square_transparent_over.png",
		new google.maps.Size(11, 11),
		new google.maps.Point(0, 0),
		new google.maps.Point(6, 6)
	);
	var marker = new google.maps.Marker({
		position: new google.maps.LatLng(
			latLng.lat() - (0.5 * (latLng.lat() - prevpoint.lat())),
			latLng.lng() - (0.5 * (latLng.lng() - prevpoint.lng()))
		),
		map: me.map,
		icon: imageNormal,
		draggable: true
	});
	google.maps.event.addListener(marker, "mouseover", function() {
		marker.setIcon(imageHover);
	});
	google.maps.event.addListener(marker, "mouseout", function() {
		marker.setIcon(imageNormal);
	});
	google.maps.event.addListener(marker, "dragstart", function() {
		for (var m = 0; m < me.vmarkers.length; m++) {
			if (me.vmarkers[m] == marker) {
				var tmpPath = me.tmpPolyLine.getPath();
				tmpPath.push(me.markers[m].getPosition());
				tmpPath.push(me.vmarkers[m].getPosition());
				tmpPath.push(me.markers[m+1].getPosition());
				break;
			}
		}
		m = null;
	});
	google.maps.event.addListener(marker, "drag", function() {
		for (var m = 0; m < me.vmarkers.length; m++) {
			if (me.vmarkers[m] == marker) {
				me.tmpPolyLine.getPath().setAt(1, marker.getPosition());
				break;
			}
		}
		m = null;
	});
	google.maps.event.addListener(marker, "dragend", function() {
		for (var m = 0; m < me.vmarkers.length; m++) {
			if (me.vmarkers[m] == marker) {
				var newpos = marker.getPosition();
				var startMarkerPos = me.markers[m].getPosition();
				var firstVPos = new google.maps.LatLng(
					newpos.lat() - (0.5 * (newpos.lat() - startMarkerPos.lat())),
					newpos.lng() - (0.5 * (newpos.lng() - startMarkerPos.lng()))
				);
				var endMarkerPos = me.markers[m+1].getPosition();
				var secondVPos = new google.maps.LatLng(
					newpos.lat() - (0.5 * (newpos.lat() - endMarkerPos.lat())),
					newpos.lng() - (0.5 * (newpos.lng() - endMarkerPos.lng()))
				);
				var newVMarker = me.createVMarker(secondVPos);
				newVMarker.setPosition(secondVPos);//apply the correct position to the vmarker
				var newMarker = me.createControlMarker(newpos);
				me.markers.splice(m+1, 0, newMarker);
				me.gOverlay.getPath().insertAt(m+1, newpos);
				marker.setPosition(firstVPos);
				me.vmarkers.splice(m+1, 0, newVMarker);
				me.tmpPolyLine.getPath().removeAt(2);
				me.tmpPolyLine.getPath().removeAt(1);
				me.tmpPolyLine.getPath().removeAt(0);
				newpos = null;
				startMarkerPos = null;
				firstVPos = null;
				endMarkerPos = null;
				secondVPos = null;
				newVMarker = null;
				newMarker = null;
				break;
			}
		}
	});
	me.vmarkers.push(marker);
	return marker;
};

/**
 * moves vmarker at position index and creates a new polygon vertex
 * @var index nth vmarker
 */
Polygon.prototype.moveVMarker = function(index) {
	var me = this;
	var newpos = me.markers[index].getPosition();
	if (index != 0) {
		var prevpos = me.markers[index-1].getPosition();
		me.vmarkers[index-1].setPosition(new google.maps.LatLng(
			newpos.lat() - (0.5 * (newpos.lat() - prevpos.lat())),
			newpos.lng() - (0.5 * (newpos.lng() - prevpos.lng()))
		));
		prevpos = null;
	}
	if (index != me.markers.length - 1) {
		var nextpos = me.markers[index+1].getPosition();
		me.vmarkers[index].setPosition(new google.maps.LatLng(
			newpos.lat() - (0.5 * (newpos.lat() - nextpos.lat())), 
			newpos.lng() - (0.5 * (newpos.lng() - nextpos.lng()))
		));
		nextpos = null;
	}
	newpos = null;
	index = null;
};

/**
 * Remove vmarker at index.
 * @var index of vmarker
 */
Polygon.prototype.removeVMarkers = function(index) {
	var me = this;
	if (me.markers.length > 0) {//clicked marker has already been deleted
		if (index != me.markers.length) {
			me.vmarkers[index].setMap(null);
			me.vmarkers.splice(index, 1);
		} else {
			me.vmarkers[index-1].setMap(null);
			me.vmarkers.splice(index-1, 1);
		}
	}
	if (index != 0 && index != me.markers.length) {
		var prevpos = me.markers[index-1].getPosition();
		var newpos = me.markers[index].getPosition();
		me.vmarkers[index-1].setPosition(new google.maps.LatLng(
			newpos.lat() - (0.5 * (newpos.lat() - prevpos.lat())),
			newpos.lng() - (0.5 * (newpos.lng() - prevpos.lng()))
		));
		prevpos = null;
		newpos = null;
	}
	index = null;
};

/**
 * Disable the editing of a polygon (remove all Markers)
 */
Polygon.prototype.stopEditing = function(){
	this.removeAllMarkers();
};

/**
 * Removes all edit markers. Polygon isn't editable afterwards.
 */
Polygon.prototype.removeAllMarkers = function(){
	var me = this;
	for(var i = 0; i < me.markers.length; i++){
		me.markers[i].setMap(null);
	}
	for(var i = 0; i < me.vmarkers.length; i++){
		me.vmarkers[i].setMap(null);
	}
	me.markers.length = 0;
	me.vmarkers.length = 0;
};

/**
 * Set Style of polygon
 * @var style : selected, selected-disable, highlighted, highlighted-disable
 */
Polygon.prototype.setStyle = function(style){
  var _style;

    switch(style) {
        case 'selected':
            this.style = 'polygon-selected';
            break;
        case 'selected-disable':
            if(this.style == 'polygon-selected') {
              this.style = 'polygon';
            }
            break;
        case 'highlighted':
            if(this.style != 'polygon-selected') {
              this.style = 'polygon-highlighted';
            }
            break;
        case 'highlighted-disable':
            if(this.style == 'polygon-highlighted') {
                this.style = 'polygon';
            }
            break;
        default:
            this.style = 'polygon';
    }
  this.polygon.setOptions(overlayStyle[this.style]);
}

/**
 * 
 * @param {Object} latLng_nr google.maps.LatLng / integer (nth coordinate in array)
 */
//Polygon.prototype.removeLatLng = function(latLngAt){
////	//Coordinate LatLng
////	if(typeof latLng_nr == typeof google.maps.LatLng){ 
////		for(var i = 0; i < this.latLngs.length(); i++){ //Deletes EVERY Element with the specified LatLng
////			if(this.latLngs[i].equals(latLngAt)){
////				this.latLngs.splice(i, 1);
////			}
////		}
////	}
//	// nth Coordinate
//	if(!isNaN(latLngAt)){ 
//		nr = Math.floor(latLngAt);
//		this.polygon.getPath().removeAt(nr);
//	}
//
//};

Polygon.prototype.__pathToString = function(path){
	var s = "";
	path.forEach(function(el, i){
		s+= i+": "+el + ", ";
	});
	return s;
};

/**
 * Create a json from polygon
 * @return json
 */
Polygon.prototype.toJson = function(){
	var json = {
		type: this.type,
		attr: {},
		coords: {}
	}
	var latLngs = this.polygon.getPath();
	for (var i = 0; i < latLngs.getLength(); i++){
		json.coords[i] = {
			'lat' : latLngs.getAt(i).lat(),
			'lng': latLngs.getAt(i).lng()
		};
	}
	return json;
};

/**
 * Recalculates the surface area of a polygon.
 * Without considering earth curvature.
 * @return int area in square meters
 */
Polygon.prototype.calcArea = function(){
  this._area = parseInt(this.polygon.Area() + 0.5);
  return this._area;
};

/**
 * Returns the surface area of a polygon in square meters.
 * Doesn't not automatically recalculate area, if polygon has changed!
 */
Polygon.prototype.getArea = function() {
  if(this._area == null){
    this.calcArea();
  }
  return this._area;
};

/**
 * Recalculate
 */
Polygon.prototype.calcCenter = function(){
  this._center = this.polygon.Bounds().getCenter();
  return this._center
};

Polygon.prototype.getCenter = function() {
    if(this._center == null){
    this.calcCenter();
  }
  return this._center;
};