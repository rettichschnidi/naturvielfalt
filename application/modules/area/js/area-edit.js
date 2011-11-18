var currentOverlays;
var currentMap;
var editTool;
var currentGeometry;
var activeEditingTool;
var applyButton;
var resetButton;

function enable_map_editing() {
	if (jQuery("#area-coords-input").length) {
		map = createGoogleMaps("map_canvas");
		//editable GeometryOverlays
		currentOverlays = new GeometryOverlays(map);
		//retrieve coords from hidden input
		var coords = jQuery("#area-coords-input").val().replace("&quot;", '"');
		var coordsArray = JSON.parse(jQuery("#area-coords-input").val());
		area = new Object();
		area.type = jQuery("#area-type-input").val();
		var coords = jQuery("#area-coords-input").val().replace("&quot;", '"');
		area.area_points = JSON.parse(jQuery("#area-coords-input").val());
		a = new Array();
		a[0] = area;
		currentOverlays.addOverlaysJson(a);
		if (jQuery("#edit-map-button").length) {	
			controls = jQuery('<div style="margin: 5px;"></div>');
			//create undo button
			var undo = jQuery('<img style="display:inline;"/>');
			undo.data('inactive', 'undo.png');
			undo.data('selected', 'undo-selected.png');
			var confirmMessage = jQuery("div#edit-map-button #reset-area-confirmation").text();
			var handler = function() {
				undo.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + undo.data('selected'));
				if (confirm(jQuery("div#edit-map-button #reset-area-confirmation").text())) {
					activeEditingTool.reset();
				}
				undo.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + undo.data('inactive'));
			};
			var toolTip = jQuery("div#edit-map-button #add-marker-caption").text();
			undo.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + undo.data('inactive'))
			.attr('alt', toolTip)
			.attr('title', toolTip)
			.click(handler).mouseout(function(){
				undo.attr('src',  Drupal.settings.basePath + 'modules/area/images/map_controls/' + undo.data('inactive'));
			});
			controls.append(undo);
			//init tool for editing area
			switch (jQuery("div#edit-map-button #area-type").text()) {
			case "marker":
				activeEditingTool = new MarkerEdit();
				break;
			case "polyline":
				activeEditingTool = new PathEdit();
				break;
			case "polygon":
				activeEditingTool = new PolygonEdit();
				break;
			}
		    activeEditingTool.setMap(map);
			for (var o in currentOverlays.overlays) {
				currentGeometry = currentOverlays.overlays[o];
				activeEditingTool.init(currentGeometry, controls);
			    break;
			}
		    map.controls[google.maps.ControlPosition.TOP_LEFT].push(controls.get(0));
		    //submit event handler
		    jQuery("#area-edit-form").submit(apply_edit);
		}
	}
};


/*Creates the google maps object and attaches it to the element with the given id.
  The google map object is returned*/
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

/*Pull changes from active editing tool. store them to hidden field*/
function apply_edit() {		
	var area_coords = new Array();
	activeEditingTool.apply();
	activeEditingTool.geometry.getLatLngs().forEach(function (position) {
		area_coords.push([position.lat(), position.lng()]);
	});
	area_coords = JSON.stringify(area_coords); 
	jQuery("#area-coords-input").val(area_coords);
	jQuery("#area-type-input").val(activeEditingTool.geometry.type);
	jQuery("#area-surface").val(activeEditingTool.geometry.getArea());
};

/*load the area*/
var areaselect = null;
jQuery(document).ready(function() {
  if(jQuery('#map_canvas').size()) {
	  enable_map_editing();
  }
});