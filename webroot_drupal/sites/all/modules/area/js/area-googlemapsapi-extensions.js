/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-googlemapsapi-extensions.js
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
 * @return
 */
google.maps.Marker.prototype.getAllCoordinates = function() {
    return new google.maps.MVCArray([this.getPosition()]);
};

/**
 * Returns polygon path
 * @return google.maps.MVCArray<google.maps.LatLng>
 */
google.maps.Polygon.prototype.getAllCoordinates = function(){
    return this.getPath();
};

/**
 * Returns polyline path
 * @return google.maps.MVCArray<google.maps.LatLng>
 */
google.maps.Polyline.prototype.getAllCoordinates = function(){
    return this.getPath();
};

/**
 * Emulates setPath (Polyline and Polygon offer this method)
 * @return
 */
google.maps.Marker.prototype.setPath = function(points) {
    this.setPosition(points[0]);
};

/**
 * Emulates Area()
 * @return
 */
google.maps.Marker.prototype.Area = function() {
    return 0;
};