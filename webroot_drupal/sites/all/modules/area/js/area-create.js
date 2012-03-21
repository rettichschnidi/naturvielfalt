/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-create.js
 */

Area.prototype.createDrawingTools = function() {
	var map = this.googlemap;
	var me = this;
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
		map: map
	});
	
	var drawingManager = this.drawingManager;
	// set a listener which fires for every created shape
	google.maps.event.addListener(drawingManager, 'overlaycomplete', function(overlay) {
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
		me.setSelectedElement(overlay);
		contentString = "<h1>You created a " + overlay.type + "</h1>";
		var infowindow = new google.maps.InfoWindow({
			content : contentString
		});
		google.maps.event.addListener(infowindow, 'closeclick', function(){
			overlay.overlay.setMap(null);
			overlay = null;
		});
		this.setDrawingMode(null);
		var map = this.getMap();
		infowindow.open(map, overlay.overlay);
	});
};

jQuery(document).ready(function() {
	areabasic.createDrawingTools();
	console.debug('Created drawing tools on "#' + canvasid + '"');
});