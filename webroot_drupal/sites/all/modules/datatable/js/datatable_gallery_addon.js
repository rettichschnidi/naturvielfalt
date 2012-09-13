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
	gallery_addon.preProcess = function(gridid, data) {
		if(!gallery_addon.__isGalleryActive(gridid))
			return data;
		
		gallery_addon.__clearGalleryDiv(gridid);
		
		var table = document.createElement('table');
		table.className = 'imgGallery';
		
		var tbody = document.createElement('tbody');				
		
		
		for(i=0;i <= data.rows.length / COL_COUNT;i++){
			var tr = document.createElement('tr');
			for (j = 0; j < COL_COUNT; j++){
				var index = i * COL_COUNT + j;
				var td = document.createElement('td');
				$(td).addClass('datatable_gallery_row');

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
		gallery_addon.__getGalleryDiv(gridid).append(table);
		
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
	gallery_addon.toggleGallery = function(gridid, enabled) {
		// find the div of this flexigrid
		var div = $('#' + gridid).parent().parent();
		
		div.children('.bDiv').toggle();
		div.children('.hDiv').toggle();
		gallery_addon.__getGalleryDiv(gridid).toggle();
		
		// remove the batch-command-div (observation) if available
		if($('#batch-div').length > 0){
			if(enabled){
				$('#batch-div').css('display', 'none');
			} else {
				$('#batch-div').css('display', 'block');
			}
		}
	
		if(enabled){
			$('#' + gridid + '_gallery_link').attr('disabled', 'disabled');
			$('#' + gridid + '_table_link').removeAttr('disabled');
			$('#' + gridid + '_gallery_image_source').css('display', 'inline');
			$("#" + gridid).flexigrid().flexReload();
			window.location.hash = 'gallery';
		}else {
			$('#' + gridid + '_table_link').attr('disabled', 'disabled');
			$('#' + gridid + '_gallery_link').removeAttr('disabled');
			$('#' + gridid + '_gallery_image_source').hide();
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
	gallery_addon.sourceChanged = function(gridid, element) {
		// add the chosen imagesource to the ajax request
		$("#" + gridid).flexOptions({params:[{name:'imagesource', value: element.value}]});
		$("#" + gridid).flexigrid().flexReload();
	}
	
	/**
	 * Removes all content of the gallery div
	 */
	gallery_addon.__clearGalleryDiv = function (gridid) {
		gallery_addon.__getGalleryDiv(gridid).empty();		
	}
	
	/**
	 * returns or creates the div for the gallery images
	 */
	gallery_addon.__getGalleryDiv = function (gridid) {
		var div = $('#' + gridid + '_gallery_images');
		var flexiDiv = $('#' + gridid).parent().parent();
		if(div.length == 0){
			flexiDiv.children('.mDiv').after(
					'<div id="'+gridid+'_gallery_images" class="datatable_gallery"> ' 
					+ '</div>');	
			return $('#' + gridid + '_gallery_images');
		} else {
			return div;
		}
	}
	
	/**
	 * @returns true if the gallery is currently displayed
	 */
	gallery_addon.__isGalleryActive = function (gridid) {
		return $('#'+gridid+'_gallery_images:visible').length > 0;
	}
	
});