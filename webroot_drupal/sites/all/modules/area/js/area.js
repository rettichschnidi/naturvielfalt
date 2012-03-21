/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area.js
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

function Area(map_id) {
	/**
	 * Member variable initialisation
	 */
	// save the html canvas element id (usually '#map_canvas')
	this.map_id = map_id;
	// map holds the google maps object
	this.googlemap = undefined;
	// Switzerland almostly fits at this level
	this.zoom = 8;
	// points to the currently selected element
	this.selectedElement = undefined;
	// points to last created element
	this.lastCreatedElement = undefined;
	// contains all the overlays currently shown on the map
	this.overlays = [];
	/**
	 * Creates the google maps object and attaches it to the element with the id
	 * this.map_id.
	 */
	this.createGoogleMaps = function() {
		// set mapcenter to Switzerland
		var center = new google.maps.LatLng(46.77373, 8.25073);
		// extract the html-element where the map should be stuffed into
		var canvas = jQuery('#' + this.map_id);
		// return false if no such element available
		if (!canvas.length) {
			console.error("HTML element with id #'" + this.map_id
					+ "' is not available.");
			return false;
		}

		// set the options for a map
		var mapsOptions = {
			zoom : this.zoom,
			center : center,
			streetViewControl : false,
			overviewMapControl : true,
			scaleControl : false,
			// Select ROADMAP out of HYBRID, ROADMAP, SATELLITE, TERRAIN;
			mapTypeId : google.maps.MapTypeId.ROADMAP,
			scrollwheel : true
		};

		this.googlemap = new google.maps.Map(document
				.getElementById(this.map_id), mapsOptions);
	};

	/**
	 * Load the last saved location
	 * 
	 * @see automaticallySaveLocation(boolean)
	 */
	this.initLocation = function() {
		if (window.localStorage) {
			var bounds = window.localStorage.getItem('naturvielfalt_ne_lat');
			if (bounds != null) {
				var ls = window.localStorage;
				var ne = new google.maps.LatLng(ls
						.getItem('naturvielfalt_ne_lat'), ls
						.getItem('naturvielfalt_ne_lng'));
				var sw = new google.maps.LatLng(ls
						.getItem('naturvielfalt_sw_lat'), ls
						.getItem('naturvielfalt_sw_lng'));
				bounds = new google.maps.LatLngBounds(sw, ne);
				this.googlemap.fitBounds(bounds);
			}
		}
	};

	/**
	 * At each map change, save the current position in the users localStorage
	 * if possible. The used localStorage keys are: 
	 * - naturvielfalt_ne_lat
	 * - naturvielfalt_ne_lng 
	 * - naturvielfalt_sw_lat
	 * - naturvielfalt_sw_lng
	 * 
	 * @param enable
	 *            If true, enable automatically saving. Otherwise disable it.
	 */
	this.automaticallySaveLocation = function(enable) {
		if (arguments.length == 0 || enable) {
			// store location whenever bounds change
			if (window.localStorage) {
				var googlemap = this.googlemap;
				automaticallySaveLocationListener = google.maps.event
						.addListener(googlemap, 'bounds_changed', function() {
							var b = googlemap.getBounds();
							var ne = b.getNorthEast();
							var sw = b.getSouthWest();
							window.localStorage.setItem('naturvielfalt_ne_lat',
									ne.lat());
							window.localStorage.setItem('naturvielfalt_ne_lng',
									ne.lng());
							window.localStorage.setItem('naturvielfalt_sw_lat',
									sw.lat());
							window.localStorage.setItem('naturvielfalt_sw_lng',
									sw.lng());
						});
			}
		} else {
			// remove bounds change listener
			if (automaticallySaveLocationListener) {
				google.maps.event
						.removeListener(automaticallySaveLocationListener);
			}
		}
	};

	/**
	 * Update the last selected element
	 * @param e Selected element
	 */
	this.setSelectedElement = function(e) {
		this.selectedElement = e;
	};

	/**
	 * Return the last selected element
	 * @returns selected Element
	 */
	this.getSelectedElement = function() {
		return this.selectedElement;
	};

	/**
	 * Update the last created element
	 * @param e Newly created element
	 */
	this.setSelectedElement = function(e) {
		this.lastCreatedElement = e;
	};

	/**
	 * Return the last created element
	 * @returns last created element
	 */
	this.getSelectedElement = function() {
		return this.lastCreatedElement;
	};

	this.overlayComplete = function(overlay) {
		console.debug(this);
		if(overlay.type == 'marker') {
			overlay.overlay.setDraggable(true);
			overlay.overlay.setClickable(true);
		} else if (overlay.overlay.type == 'polyline' || overlay.overlay.type == 'polygon'){
			overlay.overlay.setEditable(false);
		} else {
			console.debug('Overlay type is unknown.');
			console.debug('Type: ' + overlay.type);
		}
		contentString = "<h1>You created an overlay.</h1>";
		var infowindow = new google.maps.InfoWindow({
			content : contentString
		});
		google.maps.event.addListener(infowindow, 'closeclick', function(){
			overlay.overlay.setMap(null);
			overlay = null;
		});
		this.setDrawingMode(null);
		var map = this.getMap();
		infowindow.open(map, overlay);
	};
	
	this.createGoogleMaps();
	// if something bad happened during creation, just return false
	if (!this.googlemap)
		return false;
};

jQuery(document).ready(
		function() {
			console.debug("area.js");
			canvasid = 'map_canvas';
			if (jQuery('#' + canvasid).length) {
				areabasic = new Area(canvasid);
				// areabasic.initLocation();
				areabasic.automaticallySaveLocation();
			} else {
				// display errormessage to console log
				errormsg = "HTML element with id '" + canvasid
						+ "' not found. No google maps will be displayed.";
				alert(errormsg);
			}
});