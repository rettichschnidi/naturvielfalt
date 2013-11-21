jQuery(document).ready(function() {
	$ = jQuery;
	organism = {};
	
	/**
	 * Executed before table is populated
	 * ! must return the data !
	 */
	organism.tablePreProcess = function(data, flexigridOptions) {
		// FIXME table id 'organisms' should not be hard-coded
		data = gallery_addon.preProcess('organisms', 'gallery_image', data, flexigridOptions);
		return data;
	};
	
	/**
	 * Go to /organism/classifiction/id if not already there
	 */
	organism.loadClassification = function(id) {
		if(id == "navigateToMenu") {
			window.location = '/organism/';
		} else {
			var path = window.location.pathname.split('/');
			var currentId = path[path.length-1];
			if (parseInt(currentId) != parseInt(id))
				window.location = '/organism/classification/' + id;
		}
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
	
	/**
	 * Initialize Diagram hoover on naturvielfalt/organism
	 */
	organism.initDiagram = function() {
		var linkColor = '#000000'; //link color if not visited
		
		$('#diagram-container area').each(function () {
			var artgroup = $(this).attr('id').replace('area_', '');
			//on mouseenter over an area, change the image to the according artgroup, and change the link color to white of the description div
		    $(this).mouseenter(function (e) {
		        $('#diagram-image').attr('src', 'sites/all/modules/organism/img/' + artgroup + '_aktiv.png');
		        $('#' + artgroup + '-link .classifier-div').css('color', '#ffffff');
		    });
		    
		    $(this).mouseout(function (e) {
		    	$('#' + artgroup + '-link .classifier-div').css('color', linkColor);
		    	$('#diagram-image').attr('src', 'sites/all/modules/organism/img/gesamt.png');
		        //change the link color back to standard
		        $('#diagram-container area').each(function () {
		        	var artgroup = $(this).attr('id').replace('area_', '');
		        	 $('#' + artgroup + '-link .classifier-div').css('color', linkColor);
		    	});
		    });
		    
			//on mouseenter over a description div: change image to the according artgroup, and change link color to white
			$('#' + artgroup + '-link .classifier-div').mouseenter(function(e) {
				if($('#diagram-image').attr('src') != 'sites/all/modules/organism/img/' + artgroup + '_aktiv.png')
				$('#diagram-image').attr('src', 'sites/all/modules/organism/img/' + artgroup + '_aktiv.png');
				$('#' + artgroup + '-link .classifier-div').css('color', '#ffffff');
			});
		});
	};
	
	//initial Diagram, if we found the diagram-container div
	if(!$.isEmptyObject($('.diagram-container area'))) {
		organism.initDiagram();
	}
});