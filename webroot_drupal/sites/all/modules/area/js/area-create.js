/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-create.js
 */

jQuery(document).ready(
		function() {
			var drawingManager = new google.maps.drawing.DrawingManager({
				// do not select a tool yet
				drawingMode : false,
				// show the tools
				drawingControl : true,
				drawingControlOptions : {
					// show the toolbox on the right, middle
					position : google.maps.ControlPosition.TOP_LEFT,
					// enable marker, polyline and polygon as drawing primitves
					drawingModes :  [ google.maps.drawing.OverlayType.MARKER,
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
				map : areabasic.googlemap
			});

			google.maps.event.addListener(drawingManager, 'overlaycomplete', function(overlay) {
				overlay.overlay.setEditable(false);

				var url = Drupal.settings.basePath
						+ 'area/getnewareanameajaxform';
				jQuery.get(url, function(data) {
					areabasic.showInfoWindowToCreateNewArea(overlay.overlay, data);
					updateHiddenfields(overlay);
				});
				this.setDrawingMode(null);
			});
			console.debug('Created drawing tools on "#' + canvasid + '"');
		});
