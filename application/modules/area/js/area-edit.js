var currentOverlays;
var currentMap;
var editTool;
var currentGeometry;
var init = false;
var activeEditingTool;

function enable_map_editing(map, overlays) {
	if (jQuery("#edit-map-button").length) {
		//editable overlay
		currentOverlays = overlays;
		
		//init controls
		controls = jQuery('<div style="margin: 5px;"></div>');
		icon = jQuery('<img />')
		switch (jQuery("div#edit-map-button #area-type").text()) {
		case "marker":
			icon.data('active', 'markerCtl.png');
			icon.data('inactive', 'markerCtl-selected.png');
			activeEditingTool = new MarkerEdit();
			break;
		case "polyline":
			icon.data('active', 'path.png');
			icon.data('inactive', 'path-selected.png');
			activeEditingTool = new PathEdit();
			break;
		case "polygon":
			icon.data('active', 'polygon.png');
			icon.data('inactive', 'polygon.png');
			activeEditingTool = new PolygonEdit();
			break;
		}
		
		icon.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + icon.data('active'));
		icon.click(edit_click);		
		controls.append(icon);
		
	    map.controls[google.maps.ControlPosition.TOP_LEFT].push(controls.get(0));
	    
	    activeEditingTool.setMap(map);
	}
};

function edit_click() {
	if (!init) {
		for (var o in currentOverlays.overlays) {
			var currentGeometry = currentOverlays.overlays[o];
			activeEditingTool.init(currentGeometry);
			init = true;
		    break;
		}
	}
	activeEditingTool.start();			
};


function abort_edit() {
	
}


function dump(obj) {
    var out = '';
    for (var i in obj) {
        out += i + ": " + obj[i] + "\n";
    }

    console.debug(out);
};