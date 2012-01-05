jQuery(document).ready(function() {
	// bind event "autocomplete_selection"
	jQuery("input#edit-areatext").bind("autocomplete_select", areaAutocompleteSelect);
});

function areaAutocompleteSelect(event){
	var id = jQuery(this).val();
	areaselect.selectArea(id);
	jQuery("#edit-areatext").val('');
}