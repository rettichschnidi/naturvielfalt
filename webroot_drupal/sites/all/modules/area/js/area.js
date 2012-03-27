/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area.js
 */

/**
 * @Class Contains all the logic to handle a map
 */
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
	this.newestElement = undefined;
	// contains all the overlays currently shown on the map
	this.overlays = [];
	// there is just one at a time
	this.visibleInfoWindow = undefined;
	this.drawingManager = undefined;
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
	
	this.createDrawingTools = function() {
		var area = this;
		// initialize drawing overlay and tools
		this.drawingManager = new google.maps.drawing.DrawingManager({
			// do not select a tool yet
			drawingMode : false,
			// show the tools
			drawingControl : true,
			drawingControlOptions : {
				// show the toolbox on the right, middle
				position : google.maps.ControlPosition.TOP_LEFT,
				// enable marker, polyline and polygon as drawing primitves
				drawingModes : [ google.maps.drawing.OverlayType.MARKER,
						google.maps.drawing.OverlayType.POLYLINE,
						google.maps.drawing.OverlayType.POLYGON ]
			},
			// set options for the 3 elements
			makerOptions : {
				draggable : true
			},
			polylineOptions : {
				strokeWeight : 3,
				strokeColor : '#ff0000',
				strokeOpacity : 0.5,
				clickable : true,
				editable : true,
			},
			polygonOptions : {
				fillColor : '#ff0000',
				fillOpacity : 0.5,
				strokeWeight : 0,
				clickable : true,
				editable : true,
			},
			map : this.googlemap
		});

		google.maps.event.addListener(this.drawingManager, 'overlaycomplete', function(
				overlay) {
			if (overlay.type == 'marker') {
				overlay.overlay.setDraggable(true);
				overlay.overlay.setClickable(true);
			} else if (overlay.overlay.type == 'polyline'
					|| overlay.overlay.type == 'polygon') {
				overlay.overlay.setEditable(false);
			} else {
				console.debug('Overlay type is unknown.');
				console.debug('Type: ' + overlay.type);
			}
			area.setSelectedElement(overlay);

			var contentString = "<div id='infowindowcontent'><h1>You created a " + overlay.type + "</h1></div>";
			var infowindow = new google.maps.InfoWindow({
				content : contentString
			});
			google.maps.event.addListener(infowindow, 'closeclick', function() {
				overlay.overlay.setMap(null);
				overlay = null;
			});

			// FIXME: Refactor this into own function
			// -----------
			var xhrFetcher = false;
			var url = 'http://localhost/naturvielfalt_dev/area/getnewareanameajaxform';

			if (!!window.XMLHttpRequest) {
				xhrFetcher = new window.XMLHttpRequest(); // Most browsers
			} else if (!!window.ActiveXObject) {
				alert("XMLHttpRequest not supportet by your browser");
			}

			if (!xhrFetcher) {
				console.log('Unable to create XHR object');
			} else {
				xhrFetcher.open('GET', url, true);
				xhrFetcher.onreadystatechange = function() {
					if (xhrFetcher.readyState === 4) {
						// Retrieval complete
						if (!!xhrFetcher.timeout)
							clearTimeout(xhrFetcher.timeout);
						if (xhrFetcher.status >= 400) {
							console.log('HTTP error ' + xhrFetcher.status
									+ ' retrieving ' + url);
						} else {
							// Returned successfully
							infowindow.setContent(xhrFetcher.response);
							getAddress(overlay, function(address) {
								console.log("Address:");
								console.log(address);
								jQuery('#edit-canton').val(address.canton);
							    jQuery('#edit-township').val(address.township);
							    jQuery('#edit-locality').val(address.locality);
							    jQuery('#edit-zip').val(address.zip);
							    jQuery('#edit-country').val(address.country);    
							});
							getAltitude(overlay, function(altitude) {
								console.log("Altitude:");
								console.log(altitude);
							    jQuery('#edit-altitude').val(altitude);
							});
						    jQuery('#edit-latitude').val(overlay.overlay.getPosition().lat());
						    jQuery('#edit-longitude').val(overlay.overlay.getPosition().lng());
						    jQuery('#edit-area-type').val(overlay.type);
						    
							var area_coords = new Array();
							overlay.overlay.getAllCoordinates().forEach(function (position) {
								area_coords.push([position.lat(), position.lng()]);
							});
							area_coords = JSON.stringify(area_coords);
						    jQuery('#edit-area-coords').val(area_coords);
						}
					} else {
						console.log("FORM NOT YET READY");
					}
				};
				xhrFetcher.send(null);
			}
			// -----------
			this.setDrawingMode(null);
			var map = this.getMap();
			// move marker a little bit down, (approximately) center the infowindow
			map.panBy(0, -150);
			infowindow.open(map, overlay.overlay);
		});
	};
	
	/**
	 * 
	 */
	this.clearOverlays = function() {
		this.overlays = [];
	};

	this.createOverlayFromJson = function(currentjsonoverlay) {
		if (currentjsonoverlay.type == 'polygon') {
			newoverlay = new google.maps.Polygon(overlayStyle['polygon']);
        } else if (currentjsonoverlay.type == 'polyline') {
        	newoverlay = new google.maps.Polyline(overlayStyle['polyline']);
        } else if (currentjsonoverlay.type == 'marker') {
        	newoverlay = new google.maps.Marker(overlayStyle['marker']);
        } else {
            console.error('Unknown type of overlay!');
            return;
        }
		return newoverlay;
	};
	
	this.addOverlayFromJsonToGoogleMap = function(currentjsonoverlay) {
		newoverlay = this.createOverlayFromJson(currentjsonoverlay);

        var latLngs = [];
        for (var k in currentjsonoverlay.area_points) {
        	var currentpoint = currentjsonoverlay.area_points[k];
            latLngs[k] = new google.maps.LatLng(currentpoint.lat, currentpoint.lng);
        }

       	newoverlay.setPath(latLngs);
        newoverlay.setMap(this.googlemap);

        this.addWindowsListener(currentjsonoverlay.id, newoverlay);
       };

    this.addWindowsListener = function(area_id, overlay) {
		console.debug("This:");
		console.debug(this);
		var area = this;
		var contentString = "<div id='infowindowcontent'><h1>You clicked area: " + area.overlays[area_id].name + "</h1></div>";
		var infowindow = new google.maps.InfoWindow({
			content : contentString
		});
		google.maps.event.addListener(overlay, 'click', function() {
			infowindow.setPosition(this.getPosition());
        	map = this.getMap();
        	map.setCenter(this.getPosition());
        	map.panBy(0, -150);
        	if(area.visibleInfoWindow)
        		area.visibleInfoWindow.close();
        	area.visibleInfoWindow = infowindow;
			infowindow.open(this.getMap(), this);
		});
    };

	this.addOverlayFromJsonToArea = function(currentjsonoverlay) {
		this.overlays[currentjsonoverlay.id] = currentjsonoverlay;
	};
	
	/**
	 * @param json A json object
	 */
	this.loadOverlaysFromJson = function(json) {
	    for (var i in json) {
	    	this.addOverlayFromJsonToArea(json[i]);
	    	this.addOverlayFromJsonToGoogleMap(json[i]);
	    }
	    console.debug("Addded overlays from json");
	};

	this.createSearchbar = function() {
		var googlemap = this.googlemap;
		// create a new div element to hold everything needed for the searchbar
		var searchdiv = document.createElement('div');
		// create new input field
		var searchinput = document.createElement('input');
		// add searchinput to searchdiv
		searchdiv.appendChild(searchinput);
		// set id's for both elements
		searchdiv.setAttribute('id', 'search_container');
		searchinput.setAttribute('id', 'search_input');

		// push the search element on the google map in the top left corner
		googlemap.controls[google.maps.ControlPosition.TOP_LEFT].push(searchdiv);

		var autocomplete = new google.maps.places.Autocomplete(searchinput, {
			// available types: 'geocode' for places, 'establishment' for
			// companies
			types : [ 'geocode' ]
		});

		// listen do pressed suggestions and center map on them
		autocomplete.bindTo('bounds', googlemap);
		
		google.maps.event.addListener(autocomplete, 'place_changed', function() {
			var place = autocomplete.getPlace();
			console.log(place);
			if (typeof place.geometry !== 'undefined') {
				if (place.geometry.viewport) {
					googlemap.fitBounds(place.geometry.viewport);
				} else {
					console.log(googlemap);
					googlemap.setCenter(place.geometry.location);
					googlemap.setZoom(17);
				}
			} else {
				console.error("Search by pressing enter not yet implemented.");
			}
		});

		// remove focus from searchinput when map moved
		google.maps.event.addListener(googlemap, 'bounds_changed', function() {
			searchinput.blur();
		});

		// select all text when user focus the searchinput field
		searchinput.onfocus = function() {
			searchinput.select();
		};
	};

	/**
	 * Update the last selected element
	 * @param e Selected element
	 */
	this.setSelectedElement = function(e) {
		this.googlemap.setCenter(e.overlay.getPosition());
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
	this.setNewestElement = function(e) {
		overlays.push(e);
		console.print("Have " + overlays.length + " areas.");
		this.newestElement = e;
	};

	/**
	 * Return the last created element
	 * @returns last created element
	 */
	this.getNewestElement = function() {
		return this.newestElement;
	};

	/**
	 * Get the address for the submitted element
	 * @param overlay overlay
	 * @param callback callback function
	 */
	getAddress = function (overlay, callback) {
	    var geocoder = new google.maps.Geocoder();
	    var latlng = overlay.overlay.getPosition();
	    
	    geocoder.geocode({'latLng': latlng}, function(results, status) {
            var address = {};
	        if (status == google.maps.GeocoderStatus.OK) {
	            jQuery.each(results, function(index, result) {
	            	console.debug("Result from google:");
	            	console.debug(result);
	                if(result.types == 'postal_code') {
	                    var length = result.address_components.length;
	                    address.zip = result.address_components[0].long_name;
	                    address.locality = result.address_components[1].long_name;
	                    address.canton = result.address_components[length-2].short_name;
	                    address.country = result.address_components[length-1].long_name;
	                }
	                if(result.types == 'locality,political') {
	                    address.township = result.address_components[0].long_name;
	                }
	            });
	            console.debug(address);
		        callback(address);
	        } else {
	            console.error("Geocoder failed due to: " + status);
	        }
	    });
	};

	/**
	 * Get the altitude for the submitted overlay
	 * @param overlay overlay
	 * @param callback callback function
	 */
	getAltitude = function (element, callback) {
	    var elevator = new google.maps.ElevationService();
	    var request = {locations: [element.overlay.getPosition()]};

	    elevator.getElevationForLocations(request, function(results, status) {
	        if (status == google.maps.ElevationStatus.OK) {
	            // Retrieve the first result
	            if (results[0]) {
	                var altitude = parseInt(results[0].elevation + 0.5);
	    	        callback(altitude);
	            }
	        } else {
	        	console.debug("Could not get altitute. Errorstatus: " + status);
	        }
	    });
	};

	// Initialize a basic map (no search functionality, no creation tools, etc)
	this.createGoogleMaps();
	// if something bad happened during creation, just return false
	if (!this.googlemap)
		return false;
};

jQuery(document).ready(
		function() {
			console.debug("Executing area.js");
			canvasid = 'map_canvas';
			if (jQuery('#' + canvasid).length) {
				areabasic = new Area(canvasid);
				// areabasic.initLocation();
				// areabasic.automaticallySaveLocation();
			} else {
				// display errormessage to console log
				errormsg = "HTML element with id '" + canvasid
						+ "' not found. No google maps will be displayed.";
				alert(errormsg);
			}
});