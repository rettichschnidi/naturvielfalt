/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-show-myareas.js
 * 
 * Show all my areas on a google map.
 */

jQuery(document).ready(function() {
	if (!areabasic) {
		console.error("area-show-myareas.js: areabasic does not exist.");
		return;
	}

	// areabasic.loadOverlaysFromJson
	var jsonurl = Drupal.settings.basePath + 'areas/own/json';
	jQuery.getJSON(jsonurl, function(json, textStatus) {
		if (textStatus == 'success') {
			areabasic.loadOverlaysFromJson(json);
		} else {
			console.error("Could not load json from '" + jsonurl + "'");
		}
	});
});
