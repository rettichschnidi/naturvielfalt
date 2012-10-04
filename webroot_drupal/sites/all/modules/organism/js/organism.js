jQuery(document).ready(function() {
	$ = jQuery;
	organism = {};
	
	/**
	 * Executed before table is populated
	 * ! must return the data !
	 */
	organism.tablePreProcess = function(datay) {
		// FIXME table id 'organisms' should not be hard-coded
		data = gallery_addon.preProcess('organisms', 'gallery_image', data, flexigridOptions);
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
	
	/**
	 * onSubmit handler for the classificators datatable
	 * ! has to return true to continue populating the table !
	 */
	organism.onClassificatorsSubmit = function() {
		$('.mTop, .mUp').click(function() {
			organism.loadClassification($(this).data('cid'));
		});
		return true;
	};
});