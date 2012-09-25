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
	gallery_addon.preProcess = function(gridid, json_item_name, data) {
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
					if(data.rows[index].cell[json_item_name] != undefined){
						$(td).html(data.rows[index].cell[json_item_name]);
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
		var div = $('#' + gridid).closest('div.flexigrid');
		
		div.children('.bDiv').toggle();
		div.children('.hDiv').toggle();
		gallery_addon.__getGalleryDiv(gridid).toggle();
	
		if(enabled) {
			$('#' + gridid + '_gallery_link').attr('disabled', 'disabled');
			$('#' + gridid + '_table_link').removeAttr('disabled');
			$('#' + gridid + '_gallery_image_source').closest('div').show();
			window.location.hash = 'gallery';
		}
		else {
			$('#' + gridid + '_table_link').attr('disabled', 'disabled');
			$('#' + gridid + '_gallery_link').removeAttr('disabled');
			$('#' + gridid + '_gallery_image_source').closest('div').hide();
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
	$('.form-item-image-source select').change(function() {
		var gridid = this.id.substring(0, this.id.length - "_gallery_image_source".length);
		gallery_addon.sourceChanged(gridid, this);
	});
	
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
		var flexiDiv = $('#' + gridid).closest('div.flexigrid');
		if(div.length == 0) {
			div = $('<div />').attr('id', gridid+'_gallery_images').addClass('datatable_gallery');
			flexiDiv.children('.mDiv').after(div);
		}
		return div;
	}
	
	/**
	 * @returns true if the gallery is currently displayed
	 */
	gallery_addon.__isGalleryActive = function (gridid) {
		return $('#'+gridid+'_gallery_images:visible').length > 0;
	}
	
});