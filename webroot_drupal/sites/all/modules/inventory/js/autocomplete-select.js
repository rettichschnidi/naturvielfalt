jQuery(document).ready(
		function() {
			// bind event "autocomplete_selection"
			jQuery("input#edit-areatext").bind("autocomplete_select",
					sgroupAutocompleteSelect);
		});

function sgroupAutocompleteSelect(event) {
	var id = jQuery(this).val();
	jQuery("#edit-areatext").val('');
}