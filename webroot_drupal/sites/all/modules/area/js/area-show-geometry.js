/**
 * @author Ramon Gamma, 2012
 * @copyright Naturwerk
 * @file area-show-geometry.js
 */

/**
 * Show custom geometry on a google map
 */

jQuery(document).ready(function() {
	if (!areabasic) {
		console.error("area-show-geometry.js: areabasic does not exist.");
		return;
	}

	// areabasic.loadOverlaysFromJson
	if (typeof json_url == 'undefined') {
		var errormsg = "No url provided";
		console.log(errormsg);
		alert(errormsg);
		return;
	}
	jQuery.getJSON(json_url, function(json, textStatus) {
		if (textStatus == 'success') {
			areabasic.loadOverlaysFromJson(json);
		} else {
			console.error("Could not load json from '" + json_url + "'");
		}
	});
});