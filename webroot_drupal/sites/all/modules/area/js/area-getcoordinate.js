/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-getcoordinate.js
 */

/**
 * Allow the user to set a marker and store the data about this
 * point (country, township, etc) returned by Google into hidden
 * fields.
 */

jQuery(document).ready(
		function() {
			var lastOverlay = false;

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

			var update = function() {
				jQuery('#' + coordinate_storage_id).val(
						JSON.stringify(lastOverlay.overlay
								.getJsonCoordinates()));
				updateHiddenfields(lastOverlay);
			};
			
			google.maps.event.addListener(
					drawingManager,
					'overlaycomplete',
					function(overlay) {
						if (lastOverlay) {
							lastOverlay.overlay.setMap(null);
						}
						lastOverlay = overlay;
						lastOverlay.overlay.setupGeometryChangedEvent();
						this.setDrawingMode(null);
						overlay.overlay.setEditable(true);
						update();
						
						google.maps.event.addListener(
								lastOverlay.overlay,
								'geometry_changed',
								update);
					});
		});
