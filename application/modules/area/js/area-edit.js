var currentOverlays;
var currentMap;
var editTool;
var currentGeometry;
var init = false;
var activeEditingTool;
var editIcon;
var applyButton;
var cancelButton;

function enable_map_editing(map, overlays) {
	if (jQuery("#edit-map-button").length) {
		//editable overlay
		currentOverlays = overlays;
		
		//init controls
		controls = jQuery('<div style="margin: 5px;"></div>');
		editIcon = jQuery('<img />')
		switch (jQuery("div#edit-map-button #area-type").text()) {
		case "marker":
			editIcon.data('active', 'markerCtl.png');
			editIcon.data('inactive', 'markerCtl-selected.png');
			activeEditingTool = new MarkerEdit();
			break;
		case "polyline":
			editIcon.data('active', 'path.png');
			editIcon.data('inactive', 'path-selected.png');
			activeEditingTool = new PathEdit();
			break;
		case "polygon":
			editIcon.data('active', 'polygon.png');
			editIcon.data('inactive', 'polygon-selected.png');
			activeEditingTool = new PolygonEdit();
			break;
		}
		editIcon.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + editIcon.data('active'))
		.attr('alt', jQuery("div#edit-map-button #edit-area-caption").text())
		.attr('title', jQuery("div#edit-map-button #edit-area-caption").text())
		.click(edit_click);

		//apply and cancel button
		applyButton = jQuery('<div id="applyAreaButton" style="background-color:white; border: 1px solid black; padding:2px;"></div>').hide()
		.text(jQuery('div#edit-map-button #apply-area-caption').text())
		.click(apply_edit);
		cancelButton = jQuery('<div id="cancelAreaButton" style="background-color:white; border: 1px solid black; padding:2px;"></div>').hide()
		.text(jQuery('div#edit-map-button #cancel-area-caption').text())
		.click(abort_edit);		
		
		controls.append(editIcon).append(applyButton).append(cancelButton);
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
	toggle_editing(true);
	activeEditingTool.start();
	
};

function apply_edit() {
	console.debug("test");
	activeEditingTool.stop();
	toggle_editing(false);
};

function abort_edit() {
	activeEditingTool.cancel();
	toggle_editing(false);
};

function toggle_editing(enabled) {
	if (enabled) {
		editIcon.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + editIcon.data('inactive'));
		applyButton.show();
		cancelButton.show();
		editIcon.unbind('click');	
	}
	else {
		editIcon.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + editIcon.data('active'));
		applyButton.hide();
		cancelButton.hide();
		editIcon.click(edit_click);
	}
}


function dump(obj) {
    var out = '';
    for (var i in obj) {
        out += i + ": " + obj[i] + "\n";
    }

    console.debug(out);
};