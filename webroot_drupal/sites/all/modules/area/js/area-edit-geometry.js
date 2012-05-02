/**
 * @author Ramon Gamma, 2012
 * @copyright Naturwerk
 * @file area-edit-geometry.js
 */

/**
 * 
 */

jQuery(document).ready(
		function() {
			if (typeof json_url == 'undefined') {
				var errormsg = "No url provided";
				console.log(errormsg);
				alert(errormsg);
				return;
			}
			var url = Drupal.settings.basePath + json_url;
			jQuery.get(url, function(data) {
				data[0]['id'] = 0;
				areabasic.loadOverlaysFromJson(data);
				var overlayElement = areabasic.overlayElements[0];
				var overlayData = areabasic.overlayData[0];
				var map = areabasic.googlemap;
				var bounds = overlayElement.getBounds();
				map.fitBounds(bounds);
				overlayElement.setEditable(true);
				jQuery('#' + coordinate_storage_id).val(
						JSON.stringify(overlayElement.getJsonCoordinates()));
				updateHiddenfields(overlayElement);
				google.maps.event.addListener(areabasic.googlemap,
						'rightclick', function(mouseevent) {
							console.log(mouseevent.latLng);
							overlayElement
									.deleteClosestVertex(mouseevent.latLng);
						});

				google.maps.event.addListener(overlayElement,
						'geometry_changed', function() {
					jQuery('#' + coordinate_storage_id).val(
							JSON.stringify(overlayElement.getJsonCoordinates()));
					updateHiddenfields(overlayElement);
						});
			});
		});