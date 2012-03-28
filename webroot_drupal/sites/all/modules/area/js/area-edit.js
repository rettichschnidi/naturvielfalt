/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-search.js
 */

/**
 * Create a Searchbar using autocomplete-feature by google places api
 */

jQuery(document).ready(
		function() {
			if (typeof areaid == 'undefined') {
				var errormsg = "Area id is not defined!";
				console.log(errormsg);
				alert(errormsg);
				return;
			}
			console.debug("Area Id: " + areaid);
			console.debug("Using element: " + canvasid);
			var url = Drupal.settings.basePath + 'area/' + areaid + '/json';
			jQuery.get(url, function(data) {
				areabasic.loadOverlaysFromJson(data);
				var overlayElement = areabasic.overlayElements[areaid];
				var overlayData = areabasic.overlayData[areaid];
				overlayElement.setEditable(true);
				google.maps.event.addListener(areabasic.googlemap,
						'rightclick', function(mouseevent) {
							console.log(mouseevent.latLng);
							overlayElement
									.deleteClosestVertex(mouseevent.latLng);
						});

				google.maps.event.addListener(overlayElement,
						'geometry_changed', function() {
							var url = Drupal.settings.basePath + 'area/'
									+ overlayData.id + '/updategeometry/json';
							var position = overlayElement.getPosition();

							console.log("Changed position to: "
									+ position);

							getAddress(position, function(address) {
								console.log("Canton:");
								console.log(address.canton);
								overlayData.canton = address.canton;
								overlayData.township = address.township;
								overlayData.zip = address.zip;
								overlayData.country = address.country;
							});

							getAltitude(position, function(altitude) {
								console.log("Altitude:");
								console.log(altitude);
								overlayData.altitude = altitude;
							});

							var area_coords = new Array();
							overlayElement.getAllCoordinates().forEach(
									function(position) {
										area_coords.push([ position.lat(),
												position.lng() ]);
									});
							area_coords = JSON.stringify(area_coords);
							overlayData.area_points = area_coords;
							
							// Ugly hack...
							setTimeout(function() {jQuery.post(url, overlayData);}, 1500);
						});
			});
		});