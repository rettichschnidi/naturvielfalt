/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-getcoordinate.js
 * 
 * Allow the user to set a marker and store the data about this
 * point (country, township, etc) returned by Google into hidden
 * fields.
 */

jQuery(document).ready(
		function() {
			areabasic.drawingManager = new google.maps.drawing.DrawingManager({
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
						JSON.stringify(areabasic.newestElement.overlay
								.getJsonCoordinates()));
				updateHiddenfields(areabasic.newestElement);
			};
			
			google.maps.event.addListener(
					areabasic.drawingManager,
					'overlaycomplete',
					function(overlay) {
						if (areabasic.newestElement) {
							areabasic.newestElement.overlay.setMap(null);
						}
						areabasic.newestElement = overlay;
						areabasic.newestElement.overlay.setupGeometryChangedEvent();
						this.setDrawingMode(null);
						overlay.overlay.setEditable(true);
						update();
						
						google.maps.event.addListener(
								areabasic.newestElement.overlay,
								'geometry_changed',
								update);
					});
			google.maps.event.addListener(
					areabasic,
					'overlaycomplete',
					function() {
						this.newestElement.overlay.setupGeometryChangedEvent();
						this.newestElement.overlay.setMap(areabasic.googlemap);
						this.drawingManager.setDrawingMode(null);
						this.newestElement.overlay.setEditable(true);
						update();
						
						google.maps.event.addListener(
								this.newestElement.overlay,
								'geometry_changed',
								update);
					});
		});
