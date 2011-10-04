var currentOverlays;
var currentMap;
var editTool;
var currentGeometry;
var init = false;
var activeEditingTool;
var editIcon;
var applyButton;
var cancelButton;

function enable_map_editing() {
	if (jQuery("#edit-map-button").length) {
		
		map = createGoogleMaps("map_canvas");
		
		//editable overlay
		currentOverlays = new GeometryOverlays(map);
		
		var coords = jQuery("#area-coords-input").val().replace("&quot;", '"');
		var coordsArray = JSON.parse(jQuery("#area-coords-input").val());
		    
		area = new Object();
		area.type = jQuery("#area-type-input").val();
		var coords = jQuery("#area-coords-input").val().replace("&quot;", '"');
		area.area_points = JSON.parse(jQuery("#area-coords-input").val());
		a = new Array();
		a[0] = area;
		currentOverlays.addOverlaysJson(a);
		
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


/**
 * Creates the google maps object and attaches it to the element with the id
 * 'map_canvas'. Returns a google.maps.Map
 */
function createGoogleMaps (map_id) {
  var mapcenter = [46.77373, 8.25073];
  var canvas = jQuery('#'+ map_id);
  if(!canvas.size())
    return false;
  zoom = parseInt(canvas.data('zoom'));
  mapcenter = canvas.data('center').split(',');
  center = new google.maps.LatLng(parseFloat(mapcenter[0]), parseFloat(mapcenter[1]));
  var mapsOptions = {
      zoom : zoom,
      center : center,
      mapTypeId : google.maps.MapTypeId.ROADMAP,
      maxZoom: 18,
      minZoom: 5,
      scrollwheel: true
    };
  
  var map = new google.maps.Map(document.getElementById(map_id), mapsOptions);

  return map;
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
	activeEditingTool.stop();
	toggle_editing(false);
		
	var area_coords = new Array();
	activeEditingTool.geometry.getLatLngs().forEach(function (position) {
		area_coords.push([position.lat(), position.lng()]);
	});
	area_coords = JSON.stringify(area_coords); 
	jQuery("#area-coords-input").val(area_coords);
	jQuery("#area-type-input").val(activeEditingTool.geometry.type);
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

/*load the area*/
var areaselect = null;
jQuery(document).ready(function() {
  if(jQuery('#map_canvas').size()) {
	  enable_map_editing();
  }
});