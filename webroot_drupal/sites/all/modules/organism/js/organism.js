jQuery(document).ready(function() {
	$ = jQuery;
	organism = {};
	
	/**
	 * Executed before table is populated
	 * ! must return the data !
	 */
	organism.tablePreProcess = function(data) {
		data = gallery_addon.preProcess('organisms', 'gallery_image', data);
		return data;
	};
	
	/**
	 * Go to /organism/classifiction/id if not already there
	 */
	organism.loadClassification = function(id) {
		var path = window.location.pathname.split('/');
		var currentId = path[path.length-1];
		if (parseInt(currentId) != parseInt(id))
			window.location = '/organism/classification/' + id;
	};
});