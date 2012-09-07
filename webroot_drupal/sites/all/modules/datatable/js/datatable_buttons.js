jQuery(document).ready(function() {
	$ = jQuery;
	datatable_buttons = {};

	/**
	 * is called by every refresh before the data is displayed
	 */
	datatable_buttons.preProcess = function(data) {
		if(!datatable_buttons.__isGalleryActive())
			return data;
		
		// update the pictures...
			
		
		return data;
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
		
		if(enabled)
			$("#observations").flexigrid() .flexReload();
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