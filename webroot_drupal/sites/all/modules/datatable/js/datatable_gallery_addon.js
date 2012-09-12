/**
 * the "gallery_addon" integrates our drupal gallery into the flexigrid.
 * have a look at the datatable documentation to see how the gallery can be enabled
 */
jQuery(document).ready(function() {
	$ = jQuery;
	gallery_addon = {};
	// Amount of rows in image div
	var COL_COUNT = 4;

	/**
	 * is called by every refresh before the data is displayed
	 */
	gallery_addon.preProcess = function(data) {
		if(!gallery_addon.__isGalleryActive())
			return data;
		
		gallery_addon.__clearGalleryDiv();
		
		var table = document.createElement('table');
		table.className = 'imgGallery';
		
		var tbody = document.createElement('tbody');				
		
		
		for(i=0;i <= data.rows.length / COL_COUNT;i++){
			var tr = document.createElement('tr');
			for (j = 0; j < COL_COUNT; j++){
				var index = i * COL_COUNT + j;
				var td = document.createElement('td');			

				if (data.rows[index] != undefined){
					if(data.rows[index].cell['gallery_image'] != undefined){
						$(td).html(data.rows[index].cell['gallery_image']);
					}
				}
				$(tr).append(td);
			}			
			$(tbody).append(tr);			
			tr = null;
		}
	
		$(table).append(tbody);
		$('#gallery_images').append(table);
		
		// Reregister lightbox
		gallery_lightbox.registerLightBox();
		
		return data;
	}
	
	/**
	 * displays the gallery or the datatable. 
	 * 
	 *  @param boolean enabled
	 *  	true -> show gallery
	 *  	false -> switch back to the datatable
	 */
	gallery_addon.toggleGallery = function(tableid, enabled) {	
		$('.bDiv').toggle();
		$('.hDiv').toggle();
		$('#batch-div').toggle();
		gallery_addon.__getGalleryDiv().toggle();
		
		if(enabled){
			$('#' + tableid + '_gallery_link').attr('disabled', 'disabled');
			$('#' + tableid + '_table_link').removeAttr('disabled');
			$('#' + tableid + '_gallery_image_source').css('display', 'inline');
			$("#observations").flexigrid().flexReload();
			window.location.hash = 'gallery';
		}else {
			$('#' + tableid + '_table_link').attr('disabled', 'disabled');
			$('#' + tableid + '_gallery_link').removeAttr('disabled');
			$('#' + tableid + '_gallery_image_source').hide();
			window.location.hash = '';
		}
	}
	
	/**
	 * is called when the user has chosen another imagesource.
	 * imagesouce = "Nur Portraitbilder", "Nur Belegbilder" ....
	 * 
	 * this method adds the chosen imagesource to the flexigrid parameters
	 * the php backend can then evaluate the "imagesource" parameter during the ajax calls
	 * 
	 * @param the <select> element
	 */
	gallery_addon.sourceChanged = function(element) {
		// add the chosen imagesource to the ajax request
		$("#observations").flexOptions({params:[{name:'imagesource', value: element.value}]});
		$("#observations").flexigrid().flexReload();
	}
	
	/**
	 * Removes all content of the gallery div
	 */
	gallery_addon.__clearGalleryDiv = function () {
		$('#gallery_images').empty();		
	}
	
	/**
	 * returns or creats the div for the gallery images
	 */
	gallery_addon.__getGalleryDiv = function () {
		var div = $('#gallery_images');
		if(div.length == 0){
			$('.mDiv').after('<div id="gallery_images" style="display: none; padding-left:30px;"> ' 
					+ '</div>');	
			return $('#gallery_images');
		} else {
			return div;
		}
	}
	
	/**
	 * @returns true if the gallery is currently displayed
	 */
	gallery_addon.__isGalleryActive = function () {
		return $('#gallery_images:visible').length > 0;
	}
	
});