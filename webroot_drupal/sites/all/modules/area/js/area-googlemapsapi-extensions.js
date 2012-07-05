/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-googlemapsapi-extensions.js
 * 
 * Add some functions to the google maps overlays to
 * allow them to be handled in a more generic way.
 */

/**
 * Add getPosition to google.maps.Polygon - required to set a
 * google.maps.InfoWindow on a polygon
 * 
 * @returns google.maps.LatLng Arithmetic center of polygon
 */
google.maps.Polygon.prototype.getPosition = function() {
	var lng = 0;
	var lat = 0;
	var i = this.getPath().length;
	this.getPath().forEach(function(p) {
		lat = lat + p.lat();
		lng = lng + p.lng();
	});
	point = new google.maps.LatLng(lat / i, lng / i);
	return point;
};

/**
 * Add getPosition to google.maps.Polyline - required to set a
 * google.maps.InfoWindow on a polyline
 * 
 * @returns google.maps.LatLng Last point of polyline
 */
google.maps.Polyline.prototype.getPosition = function() {
	var point = this.getPath().getAt(this.getPath().length - 1);
	return point;
};

/**
 * Return the MVC Array of the LatLng
 * 
 * @return
 */
google.maps.Marker.prototype.getAllCoordinates = function() {
	return new google.maps.MVCArray([ this.getPosition() ]);
};

/**
 * Returns polygon path
 * 
 * @return google.maps.MVCArray<google.maps.LatLng>
 */
google.maps.Polygon.prototype.getAllCoordinates = function() {
	return this.getPath();
};

/**
 * Returns polyline path
 * 
 * @return google.maps.MVCArray<google.maps.LatLng>
 */
google.maps.Polyline.prototype.getAllCoordinates = function() {
	return this.getPath();
};

/**
 * Emulates setPath (Polyline and Polygon offer this method)
 * 
 * @return
 */
google.maps.Marker.prototype.setPath = function(points) {
	this.setPosition(points[0]);
};

/**
 * Emulates Area() for markers
 * 
 * @return
 */
google.maps.Marker.prototype.Area = function() {
	return 0;
};

google.maps.Marker.prototype.deselect = function() {
};

google.maps.Marker.prototype.select = function() {
};

google.maps.Polyline.prototype.deselect = function() {
	this.setOptions({
		strokeColor : "#FF0000",
		strokeWeight : 3,
	});
};

google.maps.Polyline.prototype.select = function() {
	this.setOptions({
		strokeColor : "#0000FF",
		strokeWeight : 3,
	});
};

google.maps.Polygon.prototype.deselect = function() {
	this.setOptions({
		strokeColor : "#FF0000",
		strokeWeight : 1,
		strokeOpacity : 0.75,
		fillColor : "#AA0000",
		fillOpacity : 0.25
	});
};

google.maps.Polygon.prototype.select = function() {
	this.setOptions({
		strokeColor : "#0000FF",
		strokeWeight : 1,
		strokeOpacity : 0.25,
		fillColor : "#0000AA",
		fillOpacity : 0.4
	});
};

google.maps.Marker.prototype.getBounds = function() {
	var bound = new google.maps.LatLngBounds();
	this.getAllCoordinates().forEach(function(e) {
		bound.extend(e);
	});
	return bound;
};

google.maps.Polyline.prototype.getBounds = google.maps.Marker.prototype.getBounds;
google.maps.Polygon.prototype.getBounds = google.maps.Marker.prototype.getBounds;

google.maps.Marker.prototype.setEditable = function(e) {
	this.setDraggable(e);
	this.setClickable(e);
};

/**
 * Fire the even 'geometry_changed' every time the marker got moved
 */
google.maps.Marker.prototype.setupGeometryChangedEvent = function() {
	var me = this;
	google.maps.event.addListener(me, 'dragend', function() {
		google.maps.event.trigger(me, "geometry_changed");
	});
};

google.maps.Marker.prototype.deleteClosestVertex = function() {
	// just for compatibilty with polygon and polyline
};

google.maps.Polyline.prototype.deleteClosestVertex = function(pointToCompareTo) {
	var points = this.getPath();
	if (points.getLength() < 3) {
		console.log("Element has just 2 elements- will not delete any more.");
		return;
	} else {
		console.log("Number of elements: " + points.getLength());
	}
	var smallestDistance = Infinity;
	var smallestDistanceIndex = 0;
	for ( var i = 0; i < points.getLength(); i++) {
		var currentPoint = points.getAt(i);
		var currentDistance = google.maps.geometry.spherical
				.computeDistanceBetween(currentPoint, pointToCompareTo);
		console.log(currentDistance);
		if (currentDistance < smallestDistance) {
			smallestDistance = currentDistance;
			smallestDistanceIndex = i;
		}
	}
	points.removeAt(smallestDistanceIndex);
	this.setPath(points);
};

google.maps.Polygon.prototype.deleteClosestVertex = function(pointToCompareTo) {
	var points = this.getPath();
	if ((points.getAt(0).equals(points.getAt(points.getLength() - 1)) && points
			.getLength() < 5)) {
		console
				.log("Element has just 3 different elements - will not delete any more.");
		return;
	} else if (points.getLength() < 4) {
		console.log("Element has just 3 elements- will not delete any more.");
		return;
	} else {
		console.log("Number of elements: " + points.getLength());
	}
	var smallestDistance = Infinity;
	var smallestDistanceIndex = 0;
	for ( var i = 0; i < points.getLength(); i++) {
		var currentPoint = points.getAt(i);
		var currentDistance = google.maps.geometry.spherical
				.computeDistanceBetween(currentPoint, pointToCompareTo);
		console.log(currentDistance);
		if (currentDistance < smallestDistance) {
			smallestDistance = currentDistance;
			smallestDistanceIndex = i;
		}
	}
	points.removeAt(smallestDistanceIndex);
	this.setPath(points);
};

/**
 * Fire the even 'geometry_changed' every time the path of this line gets
 * changed.
 * 
 * @note If you use setPath(), you have to call this function again, as it
 *       actually just hooks up to the path array.
 */
google.maps.Polyline.prototype.setupGeometryChangedEvent = function() {
	var me = this;
	google.maps.event.addListener(me.getPath(), 'insert_at', function() {
		google.maps.event.trigger(me, "geometry_changed");
		console.debug("insert_at triggered.");
	});
	google.maps.event.addListener(me.getPath(), 'remove_at', function() {
		google.maps.event.trigger(me, "geometry_changed");
		console.debug("remove_at triggered.");
	});
	google.maps.event.addListener(me.getPath(), 'set_at', function() {
		google.maps.event.trigger(me, "geometry_changed");
		console.debug("set_at triggered.");
	});
};

google.maps.Polygon.prototype.setupGeometryChangedEvent = google.maps.Polyline.prototype.setupGeometryChangedEvent;

google.maps.Marker.prototype.setup = function() {
	this.setupGeometryChangedEvent();
	this.deselect();
};

google.maps.Polygon.prototype.setup = google.maps.Marker.prototype.setup;
google.maps.Polyline.prototype.setup = google.maps.Marker.prototype.setup;

google.maps.Marker.prototype.getJsonCoordinates = function() {
	var coordinates = new Array();
	this.getAllCoordinates().forEach(
			function(position) {
				coordinates.push([ position.lat(),
						position.lng() ]);
			});
	return coordinates;
};

google.maps.Polyline.prototype.getJsonCoordinates = google.maps.Marker.prototype.getJsonCoordinates;
google.maps.Polygon.prototype.getJsonCoordinates = google.maps.Marker.prototype.getJsonCoordinates;