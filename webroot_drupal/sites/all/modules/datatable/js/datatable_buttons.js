jQuery(document).ready(function() {
	$ = jQuery;
	datatable_buttons = {};

	// add our own div to the datatable
	
	
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
	}
});