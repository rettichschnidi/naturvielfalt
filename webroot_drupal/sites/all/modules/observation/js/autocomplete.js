jQuery(document).ready(function() {

		jQuery( "#organismn_autocomplete" ).autocomplete({
			source: Drupal.settings.basePath + "/organism/search/json",
			minLength: 2,
			select: function( event, ui ) {
				log( ui.item ?
					"Selected: " + ui.item.value + " aka " + ui.item.id :
					"Nothing selected, input was " + this.value );
			}
		});
		
	});
