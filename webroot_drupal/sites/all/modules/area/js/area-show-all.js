/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-show-all.js
 */

/**
 * Create a Searchbar using autocomplete-feature by google places api
 */

jQuery(document).ready(function() {
	if (!areabasic) {
		console.error("area-show-all.js: areabasic does not exist.");
		return;
	}

	// areabasic.loadOverlaysFromJson
	var jsonurl = Drupal.settings.basePath + 'area/json';
	jQuery.getJSON(jsonurl, function(json, textStatus) {
		if (textStatus == 'success') {
			areabasic.loadOverlaysFromJson(json);
		} else {
			console.error("Could not load json from '" + jsonurl + "'");
		}
	});
});