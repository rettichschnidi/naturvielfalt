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
	this.defaultZoom = 8;
	// points to the currently selected element
	this.selectedElement = undefined;
	// points to last created element
	this.newestElement = undefined;
	// contains all the overlays currently shown on the map
	this.overlayData = [];
	this.overlayElements = [];

	this.currentData = undefined;
	this.currentElement = undefined;

	// there is just one at a time
	this.visibleInfoWindow = undefined;
	this.drawingManager = undefined;

	this.automaticSaveLocationListener = undefined;
	this.ch1903MapChangeListener = undefined;
	this.mapTypeSwitchListener = null;
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
			zoom : this.defaultZoom,
			center : center,
			streetViewControl : false,
			overviewMapControl : true,
			scaleControl : scalecontrol,
			// Select ROADMAP out of HYBRID, ROADMAP, SATELLITE, TERRAIN;
			mapTypeId : google.maps.MapTypeId.ROADMAP,
			scrollwheel : true
		};

		this.googlemap = new google.maps.Map(document
				.getElementById(this.map_id), mapsOptions);
	};

	/**
	 * As requested by Albert:
	 * 	Automatically switch to ROADMAP/HYBRID map when below/above zoomlevel 12
	 */
	this.mapTypeSwitch = function(enable) {
		var googlemap = this.googlemap;
		if (arguments.length == 0 || enable) {
			if(this.mapTypeSwitchListener != null) {
				this.mapTypeSwitch(false);
			}
			this.mapTypeSwitchListener = google.maps.event.addListener(this.googlemap, 'zoom_changed', function() {
				if(googlemap.getZoom() >= 12) {
					googlemap.setMapTypeId(google.maps.MapTypeId.HYBRID);
				}
				if(googlemap.getZoom() < 12) {
					googlemap.setMapTypeId(google.maps.MapTypeId.ROADMAP);
				}
			});
		} else {
			google.maps.event.removeListener(this.mapTypeSwitchListener);
			this.mapTypeSwitchListener = null;
		}
	};

	/**
	 * Load the last saved location
	 * 
	 * @see automaticallySaveLocation(boolean)
	 */
	this.initLocation = function() {
		var googlemap = this.googlemap;
		if (window.localStorage) {
			var bounds = window.localStorage.getItem('naturvielfalt_ne_lat');
			if (bounds != null) {
				var ls = window.localStorage;
				var ne_lat = ls.getItem('naturvielfalt_ne_lat');
				var ne_lng = ls.getItem('naturvielfalt_ne_lng');
				var sw_lat = ls.getItem('naturvielfalt_sw_lat');
				var sw_lng = ls.getItem('naturvielfalt_sw_lng');

				var ne = new google.maps.LatLng(ne_lat, ne_lng);
				var sw = new google.maps.LatLng(sw_lat,	sw_lng);
				
				/**
				 * This is an ugly hack, but is needed due to the fact that Google
				 * adds some space to fitBounds and because of this the map would
				 * zoom out at each refresh otherwise.
				 * 
				 * This will probably not work in any case, but it is by far better
				 * than nothing.
				 */
				var factor =  0.25;
				var new_ne = google.maps.geometry.spherical.interpolate(ne, sw, factor);
				var new_sw = google.maps.geometry.spherical.interpolate(ne, sw, 1 - factor);
				
				bounds = new google.maps.LatLngBounds(new_sw, new_ne);
				googlemap.fitBounds(bounds);
			}
		}
	};

	/**
	 * At each map change, save the current position in the users localStorage
	 * if possible. The used localStorage keys are: - naturvielfalt_ne_lat -
	 * naturvielfalt_ne_lng - naturvielfalt_sw_lat - naturvielfalt_sw_lng
	 * 
	 * @param enable
	 *            If true, enable automatically saving. Otherwise disable it.
	 */
	this.automaticallySaveLocation = function(enable) {
		if (arguments.length == 0 || enable) {
			// store location whenever bounds change
			if (window.localStorage) {
				var googlemap = this.googlemap;
				this.automaticSaveLocationListener = google.maps.event
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
			if (this.automaticSaveLocationListener != 'undefined') {
				google.maps.event
						.removeListener(this.automaticSaveLocationListener);
			}
		}
	};

	/**
	 * Select the area with the given id. - set style to "selected" - set map to
	 * show the given element - open up an info window
	 * 
	 * @param id
	 *            integer area id
	 */
	this.selectArea = function(id) {
		if (id in this.overlayElements) {
			if (this.currentElement != null) {
				this.currentElement.deselect();
			}
			this.currentElement = this.overlayElements[id];
			this.currentData = this.overlayData[id];

			this.currentElement.select();
			var bounds = this.currentElement.getBounds();
			this.googlemap.fitBounds(bounds);
			this.googlemap.setZoom(this.googlemap.getZoom() - 2);
			this.showInfoWindow(id);
		} else {
			console.error("Area not available: " + id);
		}
	};

	/**
	 * Show a info window for a given, existing area
	 * 
	 * @param id
	 *            integer area id
	 */
	this.showInfoWindow = function(id) {
		var url = Drupal.settings.basePath + 'area/' + id
				+ '/areaoverview/ajaxform';

		if (this.visibleInfoWindow != null) {
			this.visibleInfoWindow.close();
		}
		var infowindow = this.visibleInfoWindow = new google.maps.InfoWindow({
			content : Drupal.t('Loading...')
		});

		jQuery.get(url, function(data) {
			infowindow.setContent(data);
		});
		// move marker a little bit down, (approximately)
		// centers the infowindow
		this.googlemap.panBy(0, -150);
		infowindow.open(this.googlemap, this.currentElement);
	};

	/**
	 * Show a info window for a given area
	 * 
	 * @param id
	 *            integer area id
	 */
	this.showInfoWindowToCreateNewArea = function(overlayElement, html) {
		if (this.visibleInfoWindow != null) {
			this.visibleInfoWindow.close();
		}
		var infowindow = this.visibleInfoWindow = new google.maps.InfoWindow({
			content : html
		});

		// Delete overlayElement if window closed
		google.maps.event.addListener(infowindow, 'closeclick', function() {
			overlayElement.setMap(null);
			overlayElement = null;
			overlayElement.setVisible(false);
		});

		// move marker a little bit down, (approximately)
		// centers the infowindow
		this.googlemap.panBy(0, -200);
		infowindow.open(this.googlemap, overlayElement);
	};

	/**
	 * Create a single overlayElement from a JSON
	 */
	this.createOverlayElementFromJson = function(currentjsonoverlay) {
		var newoverlay;
		if (currentjsonoverlay.type == 'polygon') {
			newoverlay = new google.maps.Polygon();
		} else if (currentjsonoverlay.type == 'polyline') {
			newoverlay = new google.maps.Polyline();
		} else if (currentjsonoverlay.type == 'marker') {
			newoverlay = new google.maps.Marker();
		} else {
			console.error('Unknown type of overlay!');
			return;
		}
		return newoverlay;
	};

	/**
	 * Add an overlayElement to the google map
	 * 
	 * @param currentjsonoverlay
	 *            overlayElement to add
	 */
	this.addOverlayFromJsonToGoogleMap = function(currentjsonoverlay) {
		var newoverlay = this.createOverlayElementFromJson(currentjsonoverlay);
		this.overlayElements[currentjsonoverlay.id] = newoverlay;

		var latLngs = [];
		for ( var k in currentjsonoverlay.area_points) {
			var currentpoint = currentjsonoverlay.area_points[k];
			var newlatlng = new google.maps.LatLng(currentpoint.lat,
					currentpoint.lng);
			/**
			 * PostGIS requires that the first and last point are the same value
			 * for polygons - Google closes the map automatically.
			 * Solution: Drop points which are equal to the first one
			 */
			if(k == 0 || !latLngs[0].equals(newlatlng)) {
				latLngs[k] = newlatlng;
			}
		}

		newoverlay.setPath(latLngs);
		newoverlay.setMap(this.googlemap);
		newoverlay.setup();

		this.addWindowsListenerForNewElement(currentjsonoverlay.id, newoverlay);
	};

	this.addWindowsListenerForNewElement = function(area_id, overlay) {
		var area = this;
		var infowindow = new google.maps.InfoWindow({
			content : Drupal.t("Loading...")
		});
		google.maps.event.addListener(overlay, 'click', function() {
			infowindow.setPosition(this.getPosition());
			map = this.getMap();
			map.setCenter(this.getPosition());
			map.panBy(0, -250);
			if (area.visibleInfoWindow) {
				area.visibleInfoWindow.close();
			}
			area.visibleInfoWindow = infowindow;
			var url = Drupal.settings.basePath + 'area/' + area_id
					+ '/areaoverview/ajaxform';
			jQuery.get(url, function(data) {
				area.visibleInfoWindow.setContent(data);
				area.visibleInfoWindow.open(map, overlay);
			});
		});
	};

	/**
	 * Add overlayData to this map.
	 * 
	 * @param currentjsonoverlay
	 *            overlayData to add
	 */
	this.addOverlayFromJsonToArea = function(currentjsonoverlay) {
		this.overlayData[currentjsonoverlay.id] = currentjsonoverlay;
	};

	/**
	 * Load all elements from a given json object to this object
	 * 
	 * @param json
	 *            a json object
	 */
	this.loadOverlaysFromJson = function(json) {
		for ( var i in json) {
			this.addOverlayFromJsonToArea(json[i]);
			this.addOverlayFromJsonToGoogleMap(json[i]);
		}
	};

	/**
	 * Create a searchbar on top left of the google maps
	 */
	this.createSearchbar = function() {
		var googlemap = this.googlemap;
		// create a new div element to hold everything needed for the searchbar
		var searchdiv = document.createElement('div');
		// create new input field
		var searchinput = document.createElement('input');
		// create new text field
		// add searchinput to searchdiv
		searchdiv.appendChild(searchinput);
		// set id's for both elements
		searchdiv.setAttribute('id', 'search_container');
		searchinput.setAttribute('id', 'search_input');

		// push the search element on the google map in the top left corner
		googlemap.controls[google.maps.ControlPosition.TOP_LEFT]
				.push(searchdiv);

		var autocomplete = new google.maps.places.Autocomplete(searchinput, {
			// available types: 'geocode' for places, 'establishment' for
			// companies
			types : [ 'geocode' ]
		});

		// listen do pressed suggestions and center map on them
		autocomplete.bindTo('bounds', googlemap);

		google.maps.event.addListener(autocomplete,	'place_changed', function() {
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
				console.error("Query was: " + searchinput.value);
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
	 * Create a searchbar on top left of the google maps
	 */
	this.createSearchbarCH1903 = function() {
		var googlemap = this.googlemap;
		// create a new div element to hold everything needed for the searchbarch1903
		var searchdivch1903 = document.createElement('div');
		// create new input field
		var searchinputch1903 = document.createElement('input');
		// regex to extract values
		var regexch1903 = /^[yY:\ ]*[+-]?([0-9]+\.?[0-9]*)[\ ,]+[xX:\ ]*[+-]?([0-9]+\.?[0-9]*)$/;

		// add searchdivch1903 to searchdiv
		searchdivch1903.appendChild(searchinputch1903);
		// set id's for both elements
		searchdivch1903.setAttribute('id', 'searchch1903_container');
		searchinputch1903.setAttribute('id', 'searchch1903_input');

		// push the search element on the google map in the top left corner
		googlemap.controls[google.maps.ControlPosition.TOP_RIGHT]
				.push(searchdivch1903);

		/**
		 * When map moved:
		 *  a) remove focus from searchinputch1930
		 *  b) set value of searchinputch1903 to new coordinates
		 *  
		 *  @param enable boolean
		 *  	If true, update searchinputch1903 when map moved
		 *  	If false, remove listener (if existing)
		 */ 
		var boundsChangeListender = function(enable) {
			if (arguments.length == 0 || enable) {
				this.ch1903MapChangeListener = google.maps.event.addListener(googlemap, 'bounds_changed', function() {
					searchinputch1903.blur();
					var center = googlemap.getCenter();
					var lng = center.lng();
					var lat = center.lat();
					var x = WGStoCHx(lat, lng);
					var y = WGStoCHy(lat, lng);
					// round to two digits
					searchinputch1903.value = "Y: " + y.toFixed(2) + ", X: " + x.toFixed(2);
				});
			} else {
				if(this.ch1903MapChangeListener != undefined) {
					google.maps.event.removeListener(this.ch1903MapChangeListener);
				}
			}
		};
		
		boundsChangeListender(true);
		
		searchinputch1903.onkeypress =  function(event) {
			if(event.keyCode != 13) {
				console.log("KeyCode: " + event.keyCode);
				return; // Was not enter
			}
			var text = searchinputch1903.value;
			var match = regexch1903.exec(text);
			if(match != null) {
				var y = match[1];
				var x = match[2];
				wgsLat = CHtoWGSlat(y, x);
				wgsLng = CHtoWGSlng(y, x);
				console.log("CH1903: " + y + " / " + x);
				console.log("WGS84: " + wgsLat + " / " + wgsLng);
				boundsChangeListender(false);
				googlemap.setCenter(new google.maps.LatLng(wgsLat, wgsLng));
				searchinputch1903.value = text;
				/**
				 * @todo Redo this in a sane way.
				 */
				setTimeout(function() { boundsChangeListender(true);}, 500); 
			}
		};
	};
	
	/**
	 * Get the address for the submitted element
	 * 
	 * @param overlay
	 *            overlay
	 * @param callback
	 *            callback function
	 */
	getAddress = function(latlng, callback) {
		var geocoder = new google.maps.Geocoder();

		geocoder.geocode({'latLng' : latlng }, function(results, status) {
			var address = {};
			if (status == google.maps.GeocoderStatus.OK) {
				jQuery.each(results, function(index, result) {
					if (false) {
						console.debug("Address from google:");
						console.debug(result);
					}
					if (result.types == 'postal_code') {
						var length = result.address_components.length;
						address.zip = result.address_components[0].long_name;
						address.locality = result.address_components[1].long_name;
						address.canton = result.address_components[length - 2].short_name;
						address.country = result.address_components[length - 1].long_name;
					}
					if (result.types == 'locality,political') {
						address.township = result.address_components[0].long_name;
					}
				});
				callback(address);
			} else {
				console.error("Geocoder failed due to: "
						+ status);
			}
		});
	};

	/**
	 * Get the altitude for the submitted overlay
	 * 
	 * @param overlay
	 *            overlay
	 * @param callback
	 *            callback function
	 */
	getAltitude = function(latlng, callback) {
		var elevator = new google.maps.ElevationService();
		var request = {
			locations : [ latlng ]
		};

		elevator.getElevationForLocations(request, function(results, status) {
			if (status == google.maps.ElevationStatus.OK) {
				// Retrieve the first result
				if (results[0]) {
					var altitude = parseInt(results[0].elevation + 0.5);
					callback(altitude);
				}
			} else {
				console.debug("Could not get altitute. Errorstatus: "
								+ status);
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
			canvasid = 'map_canvas';
			if (jQuery('#' + canvasid).length) {
				/**
				 * @todo This is ugly (global variable), but works for now.
				 */
				areabasic = new Area(canvasid);
				areabasic.mapTypeSwitch(true);
				areabasic.initLocation();
				areabasic.automaticallySaveLocation();
			} else {
				// display errormessage to console log
				errormsg = "HTML element with id '" + canvasid
						+ "' not found. No google maps will be displayed.";
				alert(errormsg);
			}
		});