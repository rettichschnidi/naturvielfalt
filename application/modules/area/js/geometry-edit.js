/*basic class for all tools to edit geometry*/
function EditGeometry () {};
EditGeometry.prototype.setMap = function (map) {
	this.map = map;
};
/*edit marker in map*/
function MarkerEdit() {};
MarkerEdit.prototype = new EditGeometry();
/*init this tool. geometry must be of type Geometry (see geometry.js)*/
MarkerEdit.prototype.init = function(geometry) {
	this.geometry = geometry;
	this.originalPos = this.geometry.overlay.getCenter();
	this.clickListener = google.maps.event.addListener(this.map, 'click', jQuery.proxy(this.click, this));
    this.map.setOptions({disableDoubleClickZoom: true, draggableCursor: 'crosshair'});
}
MarkerEdit.prototype.apply = function(latLngs) {
};
/*set back to old position*/
MarkerEdit.prototype.reset = function() {
	this.geometry.overlay.setCenter(this.originalPos);
}
/*change center on click*/
MarkerEdit.prototype.click = function(event){
	this.geometry.overlay.setCenter(event.latLng);
};
/*Tool to edit Paths*/
function PathEdit() {}
PathEdit.prototype = new EditGeometry();
/*init path tool. geometry must be of type Geometry (see geometry.js)*/
PathEdit.prototype.init = function(geometry, controls) {
	this.geometry = geometry;
	this.points = this.geometry.overlay.getPath();
	this.geometry.overlay.setMap(null);
	this.initMarkersLines();
	if (controls != undefined){
		this.initControls(controls);
	}
}
/*creates markers at the edge-points and connects them with lines.
  called during init process*/
PathEdit.prototype.initMarkersLines = function(){
	this.markers = new google.maps.MVCArray();
	//create a marker for each edge point
	var createMarker = jQuery.proxy(this.createControlMarker, this);
	this.points.forEach (createMarker);
	// crate single line elements that connect the markers
	this.lines = new google.maps.MVCArray();
	var createLineSegment = jQuery.proxy(function(line, index) {
		if (index < this.markers.getLength()-1) {
			this.createLineSegment(index);	
		}
	}, this);
	this.markers.forEach(createLineSegment);
}
/*applies the changes from the UI to the geometry instance*/
PathEdit.prototype.apply = function() {
	var points = new google.maps.MVCArray();
	this.markers.forEach(function(marker, index) {
		points.push(marker.position);
	});
	this.geometry.overlay.setPath(points);
}
/*resets the path, as it was at load time*/
PathEdit.prototype.reset = function() {
	//clear lines and marker
	this.markers.forEach(function(marker,index){
		marker.setMap(null);
	});
	this.markers.clear();
	this.lines.forEach(function(line,index){
		line.setMap(null);
	});	
	this.lines.clear();
	//init again
	this.init(this.geometry);
}
/*eventhandler for adding a controll marker*/
PathEdit.prototype.addControlMarker = function (index, latLng) {	
	this.createControlMarker(latLng, index+1);
	this.lines.getAt(index).getPath().setAt(1, latLng);
	this.createLineSegment(index+1);
	this.lines.forEach(function(line, index) {
		line.index = index;
	});
	this.markers.forEach(function(marker, index) {
		marker.index = index;
	});	
};
/*eventhandler for deleting a control marker */
PathEdit.prototype.deleteControlMarker = function (index) {
	this.removeControlMarker(index);
	this.lines.forEach(function(line, index) {
		line.index = index;
	});
	this.markers.forEach(function(marker, index) {
		marker.index = index;
	});	
}
/*procedure to actually remove the control marker*/
PathEdit.prototype.removeControlMarker = function (index) {
	if (this.markers.getLength() > 2) {
		console.debug(index);
		this.markers.getAt(index).setMap(null);
		this.markers.removeAt(index);
		var deleteLineIndex = index < this.lines.getLength() ? index : index - 1;
		this.lines.getAt(deleteLineIndex).setMap(null);
		if (index > 0 && index < this.lines.getLength()) {
			this.lines.getAt(index-1).getPath().setAt(1, this.markers.getAt(index).getPosition());
		}	
		this.lines.removeAt(deleteLineIndex);
	}
}
/*create a line segment that connects two markers*/
PathEdit.prototype.createLineSegment = function(index) {
	if (index < this.markers.getLength()-1) {
		var line = new google.maps.Polyline();
		line.getPath().push(this.markers.getAt(index).position);
		line.getPath().push(this.markers.getAt(index+1).position);
		line.setMap(this.map);
		line.index = index;
		this.lines.insertAt(index, line);	
		var that = this;
		google.maps.event.addListener(line, "click", function(event) {
			that.addControlMarker(line.index, event.latLng);
		});
	}
}
/*create the marker, that is a google markerimage*/
PathEdit.prototype.createGoogleMapsMarker = function (latLng, index) {
    var imageNormal = new google.maps.MarkerImage(Drupal.settings.basePath
			+ "modules/area/js/lib/rwo_gmaps/images/square.png",
			new google.maps.Size(11, 11), new google.maps.Point(0, 0),
			new google.maps.Point(6, 6));
    var imageHover = new google.maps.MarkerImage(Drupal.settings.basePath
			+ "modules/area/js/lib/rwo_gmaps/images/square_over.png",
			new google.maps.Size(11, 11), new google.maps.Point(0, 0),
			new google.maps.Point(6, 6));
	var marker = new google.maps.Marker( {
		position : latLng,
		map : this.map,
		icon : imageNormal,
		draggable : true
	});
	marker.imgNormal = imageNormal;
	marker.imgHover = imageHover;
	marker.index = index;
	this.markers.insertAt(index, marker);
    //hover icon
    google.maps.event.addListener(marker, "mouseover", function () {
        marker.setIcon(imageHover);
    });
    google.maps.event.addListener(marker, "mouseout", function () {
        marker.setIcon(imageNormal);
    });
    //delete marker on click
    google.maps.event.addListener(marker, "click", jQuery.proxy(function () {
    	if (this.deleteMarkers) {
    		this.deleteControlMarker(marker.index);
    	}
    }, this));
    return marker;
}
/*creates a control marker for line edge points*/
PathEdit.prototype.createControlMarker = function(latLng, index) {
    var marker = this.createGoogleMapsMarker(latLng, index);
	var that = this;
    // enable dragging the markers
    google.maps.event.addListener(marker, "drag", function () {
    	if (marker.index >= 0) {
    		that.lines.getAt(marker.index-1).getPath().setAt(1, marker.getPosition());
    	}
    	if (marker.index < that.markers.length-1) {
    		that.lines.getAt(marker.index).getPath().setAt(0, marker.getPosition());
    	}
        marker.setIcon(marker.imgHover);
    });
};
/*init additional controls for adding and deleting markers*/
PathEdit.prototype.initControls = function (controls) {
	//init controls
	addIcon = jQuery('<img style="display:inline;"/>');
	removeIcon = jQuery('<img style="display:inline;"/>');
	addIcon.data('inactive', 'addmarker.png');
	addIcon.data('selected', 'addmarker-selected.png');
	removeIcon.data('inactive', 'removemarker.png');
	removeIcon.data('selected', 'removemarker-selected.png');
	
	var handler = jQuery.proxy(function() {
		if (this.deleteMarkers) {
			this.deleteMarkers = false;
			addIcon.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + addIcon.data('selected'));
			removeIcon.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + removeIcon.data('inactive'));
		}
	}, this);
	var toolTip = jQuery("div#edit-map-button #add-marker-caption").text();
	addIcon.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + addIcon.data('selected'))
	.attr('alt', toolTip)
	.attr('title', toolTip)
	.click(handler);

	handler = jQuery.proxy(function() {
		if (!this.deleteMarkers) {
			this.deleteMarkers = true;
			addIcon.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + addIcon.data('inactive'));
			removeIcon.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + removeIcon.data('selected'));
		}
	}, this);
	toolTip = jQuery("div#edit-map-button #remove-marker-caption").text();
	removeIcon.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + removeIcon.data('inactive'))
	.attr('alt', toolTip)
	.attr('title', toolTip)
	.click(handler);
	
	controls.append(addIcon).append(removeIcon);
}
/*Tool for editing polygons. Extends Path-Editing tool.
  It overrides some of the methods of PathEdit since
  it behaves slightly different.*/
function PolygonEdit() {}
PolygonEdit.prototype = new PathEdit();
/*init markers and lines*/
PolygonEdit.prototype.initMarkersLines = function(){
	this.markers = new google.maps.MVCArray();
	//create a marker for each edge point
	var createMarker = jQuery.proxy(function(latLng, index) {
		if (index != this.points.getLength()-1) {
			this.createControlMarker(latLng, index);
		}
	}, this);
	this.points.forEach (createMarker);	
	// crate single line elements that connect the markers
	this.lines = new google.maps.MVCArray();
	var createLineSegment = jQuery.proxy(function(line, index) {
		this.createLineSegment(index);
	}, this);
	this.markers.forEach(createLineSegment);
}

PolygonEdit.prototype.apply = function() {
	var points = new google.maps.MVCArray();
	this.markers.forEach(function(marker, index) {
		points.push(marker.position);
	});
	points.push(this.markers.getAt(0).position);
	this.geometry.overlay.setPath(points);
}

PolygonEdit.prototype.createLineSegment = function(index) {
	var line = new google.maps.Polyline();
	//start point of segment
	line.getPath().push(this.markers.getAt(index).position);
	//endpoint of segment
	if (index < this.markers.getLength() - 1) {
		line.getPath().push(this.markers.getAt(index + 1).position);
	} else {
		line.getPath().push(this.markers.getAt(0).position);
	}
	line.setMap(this.map);
	line.index = index;
	this.lines.insertAt(index, line);
	var that = this;
	google.maps.event.addListener(line, "click", function(event) {
		that.addControlMarker(line.index, event.latLng);
	});
}

PolygonEdit.prototype.createControlMarker = function(latLng, index) {
    var m = this.createGoogleMapsMarker(latLng, index);
	var that = this;
    // enable dragging the markers
    google.maps.event.addListener(m, "drag", function () {
    	var prevLineIndex = m.index-1 >= 0 ? m.index-1 : that.markers.getLength()-1;
    	that.lines.getAt(prevLineIndex).getPath().setAt(1, m.getPosition());
    	//adjust startpoint of next line
    	var nextLineIndex = m.index < that.markers.getLength() ? m.index : 0;
    	that.lines.getAt(nextLineIndex).getPath().setAt(0, m.getPosition());
        m.setIcon(m.imgHover);
    });
};

PolygonEdit.prototype.removeControlMarker = function (index) {
	if (this.markers.getLength()>3) {
		var l = this.markers.getLength();
		this.markers.getAt(index).setMap(null);
		this.lines.getAt(index).setMap(null);
		var prevLineIndex = index > 0 ? index-1 : l-1;  
		this.lines.getAt(prevLineIndex).getPath().setAt(1, this.markers.getAt ((index+1) % l).getPosition());
		this.markers.removeAt(index);
		this.lines.removeAt(index);
	}
}
