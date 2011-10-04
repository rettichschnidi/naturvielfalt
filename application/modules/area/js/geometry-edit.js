function EditGeometry () {};
EditGeometry.prototype.setMap = function (map) {
	this.map = map;
};

function MarkerEdit() {};
MarkerEdit.prototype = new EditGeometry();

MarkerEdit.prototype.init = function(geometry) {
	this.geometry = geometry;
	this.originalPos = this.geometry.overlay.getCenter();
	this.clickListener = google.maps.event.addListener(this.map, 'click', jQuery.proxy(this.click, this));
    this.map.setOptions({disableDoubleClickZoom: true, draggableCursor: 'crosshair'});
}

MarkerEdit.prototype.apply = function(latLngs) {
};

MarkerEdit.prototype.reset = function() {
	this.geometry.overlay.setCenter(this.originalPos);
}

MarkerEdit.prototype.click = function(event){
	this.geometry.overlay.setCenter(event.latLng);
};



function PathEdit() {}
PathEdit.prototype = new EditGeometry();

PathEdit.prototype.init = function(geometry) {
	this.geometry = geometry;
	this.points = this.geometry.overlay.getPath();
	this.markers = new google.maps.MVCArray();
	//create a marker for each edge point
	var createMarker = jQuery.proxy(this.createControlMarker, this);
	this.points.forEach (createMarker);
	
	//hide the old line
	this.geometry.overlay.setMap(null);
	
	// crate single line elements that connect the markers
	this.tempOverlayLine = new google.maps.MVCArray();
	var createLineSegment = jQuery.proxy(function(line, index) {
		if (index < this.markers.getLength()-1) {
			this.createLineSegment(index);	
		}
	}, this);
	this.markers.forEach(createLineSegment);
}

PathEdit.prototype.apply = function() {
	var points = new google.maps.MVCArray();
	this.markers.forEach(function(marker, index) {
		points.push(marker.position);
	});
	this.geometry.overlay.setPath(points);
}

PathEdit.prototype.reset = function() {
	//clear lines and marker
	this.markers.forEach(function(marker,index){
		marker.setMap(null);
	});
	this.markers.clear();
	this.tempOverlayLine.forEach(function(line,index){
		line.setMap(null);
	});	
	this.tempOverlayLine.clear();
	//init again
	this.init(this.geometry);
}

PathEdit.prototype.addControlMarker = function (index, latLng) {	
	this.createControlMarker(latLng, index+1);
	this.tempOverlayLine.getAt(index).getPath().setAt(1, latLng);
	this.createLineSegment(index+1);
	this.tempOverlayLine.forEach(function(line, index) {
		line.index = index;
	});	
};

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
	
    // hover icon
    google.maps.event.addListener(marker, "mouseover", function () {
        marker.setIcon(imageHover);
    });
    
    google.maps.event.addListener(marker, "mouseout", function () {
        marker.setIcon(imageNormal);
    });
    
    return marker;
}

PathEdit.prototype.createControlMarker = function(latLng, index) {
    var marker = this.createGoogleMapsMarker(latLng, index);
	var that = this;
    // enable dragging the markers
    google.maps.event.addListener(marker, "drag", function () {
    	if (marker.index >= 0) {
    		that.tempOverlayLine.getAt(marker.index-1).getPath().setAt(1, marker.getPosition());
    	}
    	if (marker.index < that.markers.length-1) {
    		that.tempOverlayLine.getAt(marker.index).getPath().setAt(0, marker.getPosition());
    	}
        marker.setIcon(marker.imgHover);
    });
};





/*POLYGON EDIT TOOL*/


function PolygonEdit() {}
PolygonEdit.prototype = new PathEdit();

PolygonEdit.prototype.init = function(geometry) {
	this.geometry = geometry;
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
	line.getPath().push(this.markers.getAt(index).position);

	if (index < this.markers.getLength() - 1) {
		line.getPath().push(this.markers.getAt(index + 1).position);
	} else {
		line.getPath().push(this.markers.getAt(0).position);
	}

	line.setMap(this.map);
	line.index = index;
	this.tempOverlayLine.insertAt(index, line);

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
    	console.debug(m.index);
    	var prevLineIndex = m.index-1 >= 0 ? m.index-1 : that.markers.getLength()-1;
    	that.tempOverlayLine.getAt(prevLineIndex).getPath().setAt(1, m.getPosition());
    	//adjust startpoint of next line
    	var nextLineIndex = m.index < that.markers.getLength() ? m.index : 0;
    	that.tempOverlayLine.getAt(nextLineIndex).getPath().setAt(0, m.getPosition());
        m.setIcon(m.imgHover);
    });
};
