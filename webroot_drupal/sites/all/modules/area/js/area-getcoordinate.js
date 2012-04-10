/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-getcoordinate.js
 */

/**
 * Create a Searchbar using autocomplete-feature by google places api
 */

jQuery(document).ready(
		function() {
			var elementId = coordinate_storage_id;
			var lastOverlay = false;

			console.debug("Id: " + coordinate_storage_id);

			var drawingManager = new google.maps.drawing.DrawingManager({
				drawingMode : google.maps.drawing.OverlayType.MARKER,
				// show the tools
				drawingControl : true,
				drawingControlOptions : {
					// show the toolbox on the right, middle
					position : google.maps.ControlPosition.TOP_LEFT,
					// enable marker, polyline and polygon as drawing primitves
					drawingModes : [ google.maps.drawing.OverlayType.MARKER ]
				},
				// set options for the 3 elements
				makerOptions : {
					draggable : true
				},
				map : areabasic.googlemap
			});

			google.maps.event.addListener(
					drawingManager,
					'overlaycomplete',
					function(overlay) {
						if (lastOverlay) {
							lastOverlay.overlay.setMap(null);
						}
						lastOverlay = overlay;
						lastOverlay.overlay.setupGeometryChangedEvent();
						jQuery('#' + elementId).val(
								JSON.stringify(overlay.overlay
										.getJsonCoordinates()));

						this.setDrawingMode(null);
						overlay.overlay.setEditable(true);

						google.maps.event.addListener(
								lastOverlay.overlay,
								'geometry_changed',
								function() {
									jQuery('#' + elementId).val(
											JSON.stringify(overlay.overlay
													.getJsonCoordinates()));
								});
					});

		});
