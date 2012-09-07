jQuery(document).ready(function() {
	$ = jQuery;
	datatable_buttons = {};
	// Amount of rows in image div
	var COL_COUNT = 4;

	/**
	 * is called by every refresh before the data is displayed
	 */
	datatable_buttons.preProcess = function(data) {
		if(!datatable_buttons.__isGalleryActive())
			return data;
		
		datatable_buttons.__clearGalleryDiv();
		
		var table = document.createElement('table');
		table.className = 'îmgGallery';
		
		var tbody = document.createElement('tbody');				
		
		
		for(i=0;i <= data.rows.length / COL_COUNT;i++){
			var tr = document.createElement('tr');
			for (j = 0; j < COL_COUNT; j++){
				var index = i * COL_COUNT + j;
				var td = document.createElement('td');			

				if (data.rows[index] != undefined)
					if(data.rows[index].cell['gallery_image'] != undefined
							&& data.rows[index].cell['gallery_image'].length > 0 ){
						$(td).html(data.rows[index].cell['gallery_image']);
					} else {
						$(td).html('<img src="/sites/all/modules/datatable/images/no_photo.jpg" />');
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
	 * Removes all content of the gallery div
	 */
	datatable_buttons.__clearGalleryDiv = function () {
		$('#gallery_images').empty();		
	}		
	
	/**
	 * displays the gallery or the datatable. 
	 * 
	 *  @param boolean enabled
	 *  	true -> show gallery
	 *  	false -> switch back to the datatable
	 */
	datatable_buttons.toggleGallery = function(enabled) {	
		$('.bDiv').toggle();
		$('.hDiv').toggle();
		$('#batch-div').toggle();
		datatable_buttons.__getGalleryDiv().toggle();
		
		
		if(enabled){
			$("#observations").flexigrid().flexReload();			
		}
	}
	
	datatable_buttons.__getGalleryDiv = function () {
		var div = $('#gallery_images');
		if(div.length == 0){
			$('.mDiv').after('<div id="gallery_images" style="display: none;"> ' 
					+ '</div>');	
			return $('#gallery_images');
		} else {
			return div;
		}
	}
	
	datatable_buttons.__isGalleryActive = function () {
		return $('#gallery_images:visible').length > 0;
	}
	
});