jQuery(document).ready(function() {
	$ = jQuery;
	organism = {};
	
	organism.loadClassification = function(id) {
		var path = window.location.pathname.split('/');
		var currentId = path[path.length-1];
		if (parseInt(currentId) != parseInt(id))
			window.location = '/organism/classification/' + id;
	};
});