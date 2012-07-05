/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-show-allareas.js
 * 
 * Show all accessible areas on a google map
 */

jQuery(document).ready(function() {
	if (!areabasic) {
		console.error("area-show-allareas.js: areabasic does not exist.");
		return;
	}

	var jsonurl = Drupal.settings.basePath + 'areas/json';
	jQuery.getJSON(jsonurl, function(json, textStatus) {
		if (textStatus == 'success') {
			areabasic.loadOverlaysFromJson(json);
		} else {
			console.error("Could not load json from '" + jsonurl + "'");
		}
	});
});
