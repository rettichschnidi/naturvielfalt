/**
 * Create a bar which shows the current coordinates in the ch1903 system.
 * Also enables the user to center the map to such a coordinate.
 * 
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-ch1903.js
 */

/**
 * Create a searchbar on top left of the google maps
 */
jQuery(document)
		.ready(
				function() {
					Area.prototype.createSearchbarCH1903 = function() {
						var googlemap = this.googlemap;
						var area = this;
						// create a new div element to hold everything needed
						// for the
						// searchbarch1903
						var searchdivch1903 = document.createElement('div');
						// create new input field
						var searchinputch1903 = document.createElement('input');
						// regex to extract values
						var regexch1903 = /^[yY:\ ]*[+-]?([0-9]+\.?[0-9]*)[\ ,]+[xX:\ ]*[+-]?([0-9]+\.?[0-9]*)$/;

						// add searchdivch1903 to searchdiv
						searchdivch1903.appendChild(searchinputch1903);
						// set id's for both elements
						searchdivch1903.setAttribute(
								'id',
								'searchch1903_container');
						searchinputch1903.setAttribute(
								'id',
								'searchch1903_input');

						// push the search element on the google map in the top
						// left corner
						googlemap.controls[google.maps.ControlPosition.TOP_RIGHT]
								.push(searchdivch1903);

						/**
						 * When map moved: a) remove focus from
						 * searchinputch1930 b) set value of searchinputch1903
						 * to new coordinates
						 * 
						 * @param enable
						 *            boolean If true, update searchinputch1903
						 *            when map moved If false, remove listener
						 *            (if existing)
						 */
						var boundsChangeListender = function(enable) {
							if (arguments.length == 0 || enable) {
								this.ch1903MapChangeListener = google.maps.event
										.addListener(
												googlemap,
												'bounds_changed',
												function() {
													searchinputch1903.blur();
													var center = googlemap
															.getCenter();
													var lng = center.lng();
													var lat = center.lat();
													var x = WGStoCHx(lat, lng);
													var y = WGStoCHy(lat, lng);
													// round to two digits
													searchinputch1903.value = "Y: "
															+ y.toFixed(2)
															+ ", X: "
															+ x.toFixed(2);
												});
							} else {
								if (this.ch1903MapChangeListener != undefined) {
									google.maps.event
											.removeListener(this.ch1903MapChangeListener);
								}
							}
						};

						boundsChangeListender(true);

						searchinputch1903.onkeypress = function(event) {
							if (event.keyCode != 13) {
								return; // Was not enter
							}
							var text = searchinputch1903.value;
							var match = regexch1903.exec(text);
							if (match != null) {
								var y = match[1];
								var x = match[2];
								wgsLat = CHtoWGSlat(y, x);
								wgsLng = CHtoWGSlng(y, x);
								console.log("CH1903: " + y + " / " + x);
								console
										.log("WGS84: " + wgsLat + " / "
												+ wgsLng);
								var newCenter = new google.maps.LatLng(wgsLat,
										wgsLng);
								boundsChangeListender(false);
								googlemap.setCenter(newCenter);
								searchinputch1903.value = text;
								/**
								 * @todo Redo this in a sane way.
								 */
								setTimeout(function() {
									boundsChangeListender(true);
								}, 500);
								if (areabasic.newestElement) {
									areabasic.newestElement.overlay
											.setMap(null);
								}
								areabasic.newestElement = [];
								areabasic.newestElement.type = 'maker';
								areabasic.newestElement.overlay = new google.maps.Marker( {
									position: newCenter
								});
								google.maps.event.trigger(area, 'overlaycomplete');
							}
						};
					};

					areabasic.createSearchbarCH1903();
				});
