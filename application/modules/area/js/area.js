/**
 * @author Roger Wolfer This class is for creating an Area and handles the form
 *         input
 */
var map;

function Area() {
	var me = this;
	me.ajax = new MapAjaxProxy();
	google.maps.event.addDomListener(window, "load", function() {
		// create map
		var bruggCenter = new google.maps.LatLng(47.487084, 8.207273);
		// google maps options
		var myOptions = {
			zoom : 15,
			center : bruggCenter,
			mapTypeId : google.maps.MapTypeId.ROADMAP
		};
		var map = new google.maps.Map(document.getElementById('map_canvas'), myOptions);

		jQuery("#habitat_id").attr('id', "habitat_id_0");
		me.habitatsCnt = 0;
		me.curHabitatId = jQuery("#habitat_id_0");

		me.overlayControl = new GeometryOverlayControl(map);
		me.overlayControl.startDigitizing();

		// set continue script
		var gotAltitude = false;
		var gotAddress = false;
		jQuery('#continue').click(function() {
			if (me.overlayControl.overlay != null) {
				jQuery('#surface_area').val(me.overlayControl.overlay.getArea());
				me.overlayControl.overlay.getAltitude(function(altitude) {
					jQuery('#altitude').val(altitude);
					gotAltitude = true;
					if (gotAddress) {
						jQuery('#Area').submit();
					}
				});
				me.overlayControl.overlay.getAddress(function(address) {
					jQuery('#canton').val(address.canton);
					jQuery('#township').val(address.township);
					jQuery('#locality').val(address.locality);
					jQuery('#zip').val(address.zip);
					gotAddress = true;
					if (gotAltitude) {
						jQuery('#Area').submit();
					}
				});
				var centGLatLng = me.overlayControl.overlay.getCenter();
				jQuery('#latitude').val(centGLatLng.lat());
				jQuery('#longitude').val(centGLatLng.lng());
				var area_coords = new Array();
				var markers = me.overlayControl.overlay.markers;
				for (var i in markers) {
					area_coords.push(new Array(markers[i].position.lat(), markers[i].position.lng()));
				}
				area_coords = JSON.stringify(area_coords);
				
				jQuery('#area_coords').val(area_coords);
				jQuery('#area_type').val(me.overlayControl.overlay.type);
				
				return false;
			} else {
				alert("Bitte zuerst ein Gebiet eintragen.");
			}

		});

		util.geocode('map_search', 'map_search_button', map);
	});
};

new Area();