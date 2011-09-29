function EditGeometry () {};
EditGeometry.prototype.setMap = function (map) {
	this.map = map;
};

function MarkerEdit() {};
MarkerEdit.prototype = new EditGeometry();

MarkerEdit.prototype.init = function(geometry) {
	this.geometry = geometry;
}

MarkerEdit.prototype.start = function(latLngs) {
	this.originalPos = this.geometry.overlay.getCenter();
	this.clickListener = google.maps.event.addListener(this.map, 'click', jQuery.proxy(this.click, this));
    this.map.setOptions({disableDoubleClickZoom: true, draggableCursor: 'crosshair'});
};

MarkerEdit.prototype.stop = function(latLngs) {
	this.disable();
};

MarkerEdit.prototype.cancel = function() {
	this.geometry.overlay.setCenter(this.originalPos);
	this.disable();
}

MarkerEdit.prototype.click = function(event){
	this.geometry.overlay.setCenter(event.latLng);
};

MarkerEdit.prototype.disable = function() {
	this.clickListener.remove();
	this.map.setOptions({draggableCursor: 'hand'});
}


function PathEdit() {
	
}

PathEdit.prototype = new EditGeometry();

PathEdit.prototype.init = function(geometry) {
	this.geometry = geometry;
}


PathEdit.prototype.start = function() {
	this.points = this.geometry.overlay.getPath();
	this.markers = new google.maps.MVCArray();
	//create a marker for each edge point
	var createMarker = jQuery.proxy(this.createControlMarker, this);
	this.points.forEach (createMarker);
	
	//hide the old line
	this.geometry.overlay.setMap(null);
	
	// crate single line elements that connect the markers
	this.tempOverlayLine = new google.maps.MVCArray();
	var createLineSegment = function(marker, index) {
		if (index < this.markers.getLength()-1) {
			var line = new google.maps.Polyline();
			line.getPath().push(marker.position);
			line.getPath().push(this.markers.getAt(index+1).position);
			line.setMap(this.map);
			line.index = index;
			this.tempOverlayLine.push(line);
		}
	};
	createLineSegment = jQuery.proxy(function(line, index) {
		this.createLineSegment(index);
	}, this);
	this.markers.forEach(createLineSegment);
};


PathEdit.prototype.addControlMarker = function (index, latLng) {	
	this.createControlMarker(latLng, index+1);
	this.tempOverlayLine.getAt(index).getPath().setAt(1, latLng);
	this.createLineSegment(index+1);
	this.tempOverlayLine.forEach(function(line, index) {
		line.index = index;
	});	
};


PathEdit.prototype.stop = function() {
	var points = new google.maps.MVCArray();
	this.markers.forEach(function(marker, index) {
		points.push(marker.position);
	});
	this.geometry.overlay.setPath(points);
	this.unload();
}

PathEdit.prototype.cancel = function() {
	this.unload();
}

PathEdit.prototype.unload = function() {
	this.markers.forEach(function(marker,index){
		marker.setMap(null);
	});
	this.markers.clear();
	this.tempOverlayLine.forEach(function(line,index){
		line.setMap(null);
	});	
	this.tempOverlayLine.clear();
	this.geometry.overlay.setMap(this.map);
}


PathEdit.prototype.createLineSegment = function(index) {
	if (index < this.markers.getLength()-1) {
		var line = new google.maps.Polyline();
		line.getPath().push(this.markers.getAt(index).position);
		line.getPath().push(this.markers.getAt(index+1).position);
		line.setMap(this.map);
		line.index = index;
		this.tempOverlayLine.insertAt(index, line);
		
		var that = this;
		google.maps.event.addListener(line, "click", function(event) {
			that.addControlMarker(line.index, event.latLng);
		});
	}
}


PathEdit.prototype.createControlMarker = function(latLng, index) {
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
	
	this.markers.insertAt(index, marker);
	var that = this;
	
    // hover icon
    google.maps.event.addListener(marker, "mouseover", function () {
        marker.setIcon(imageHover);
    });
    
    google.maps.event.addListener(marker, "mouseout", function () {
        marker.setIcon(imageNormal);
    });
	
    // enable dragging the markers
    google.maps.event.addListener(marker, "drag", function () {
    	//TODO get rid of the for loop
        for (var m = 0, l = that.markers.getLength(); m < l; m++) {
            if (that.markers.getAt(m) == marker) {
            	if (m-1 >= 0) {
            		that.tempOverlayLine.getAt(m-1).getPath().setAt(1, marker.getPosition());
            	}
            	if (m < that.markers.length-1) {
            		that.tempOverlayLine.getAt(m).getPath().setAt(0, marker.getPosition());
            	}
                marker.setIcon(imageHover);
                break;
            }
        }
    });    
};








/*POLYGON EDIT TOOL*/


function PolygonEdit() {
	
}

PolygonEdit.prototype = new EditGeometry();

PolygonEdit.prototype.init = function(geometry) {
	this.geometry = geometry;
}

PolygonEdit.prototype.start = function() {
	this.points = this.geometry.overlay.getPath();
	this.markers = new google.maps.MVCArray();
	//create a marker for each edge point
	var createMarker = jQuery.proxy(function(latLng, index) {
		if (index != this.points.getLength()-1) {
			this.createControlMarker(latLng, index);
		}
	}, this);
	this.points.forEach (createMarker);
	
	//hide the old line
	this.geometry.overlay.setMap(null);
	
	// crate single line elements that connect the markers
	this.tempOverlayLine = new google.maps.MVCArray();
	var createLineSegment = function(marker, index) {
		if (index < this.markers.getLength()-1) {
			var line = new google.maps.Polyline();
			line.getPath().push(marker.position);
			line.getPath().push(this.markers.getAt(index+1).position);
			line.setMap(this.map);
			line.index = index;
			this.tempOverlayLine.push(line);
		}
	};
	createLineSegment = jQuery.proxy(function(line, index) {
		this.createLineSegment(index);
	}, this);
	this.markers.forEach(createLineSegment);
};

PolygonEdit.prototype.stop = function() {
	var points = new google.maps.MVCArray();
	this.markers.forEach(function(marker, index) {
		points.push(marker.position);
	});
	points.push(this.markers.getAt(0).position);
	this.geometry.overlay.setPath(points);
	this.unload();
}

PolygonEdit.prototype.cancel = function() {
	this.unload();
}

PolygonEdit.prototype.unload = function() {
	this.markers.forEach(function(marker,index){
		marker.setMap(null);
	});
	this.markers.clear();
	this.tempOverlayLine.forEach(function(line,index){
		line.setMap(null);
	});	
	this.tempOverlayLine.clear();
	this.geometry.overlay.setMap(this.map);
}


PolygonEdit.prototype.addControlMarker = function (index, latLng) {
	this.createControlMarker(latLng, index+1);
	this.tempOverlayLine.getAt(index).getPath().setAt(1, latLng);
	this.createLineSegment(index+1);
	this.tempOverlayLine.forEach(function(line, index) {
		line.index = index;
	});
	
};


PolygonEdit.prototype.createLineSegment = function(index) {
	//if (index < this.markers.getLength()-1) {
		var line = new google.maps.Polyline();
		line.getPath().push(this.markers.getAt(index).position);

		if (index < this.markers.getLength()-1) {
			line.getPath().push(this.markers.getAt(index+1).position);	
		}
		else {
			line.getPath().push(this.markers.getAt(0).position);	
		}

		line.setMap(this.map);
		line.index = index;
		this.tempOverlayLine.insertAt(index, line);
		
		var that = this;
		google.maps.event.addListener(line, "click", function(event) {
			that.addControlMarker(line.index, event.latLng);
		});
	//}
}

PolygonEdit.prototype.createControlMarker = function(latLng, index) {
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
	
	this.markers.insertAt(index, marker);
	var that = this;
	
    // hover icon
    google.maps.event.addListener(marker, "mouseover", function () {
        marker.setIcon(imageHover);
    });
    
    google.maps.event.addListener(marker, "mouseout", function () {
        marker.setIcon(imageNormal);
    });
	
    // enable dragging the markers
    google.maps.event.addListener(marker, "drag", function () {
    	//TODO get rid of the for loop
        for (var m = 0, l = that.markers.getLength(); m < l; m++) {
            if (that.markers.getAt(m) == marker) {
            	//adjust endpoint of previous line
            	var prevLineIndex = m-1 >= 0 ? m-1 : that.markers.getLength()-1;
            	that.tempOverlayLine.getAt(prevLineIndex).getPath().setAt(1, marker.getPosition());
            	//adjust startpoint of next line
            	var nextLineIndex = m < that.markers.getLength() - 1 ? m : 0;
            	that.tempOverlayLine.getAt(m).getPath().setAt(0, marker.getPosition());
                marker.setIcon(imageHover);
                break;
            }
        }
    });    
};