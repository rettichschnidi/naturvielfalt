// This JavaScript is responsible for adding and removing the habitats at creation of a new area.

jQuery(document).ready(function() {
	// bind event "click" on all delete buttons
	jQuery("td.delete div").bind("click", deleteMember);
});

// This gets called if a user clicks on the delete button
function deleteMember(event) {
	var row = jQuery(this.parentElement.parentElement);
	var id = jQuery(this).attr('id');
	var url = Drupal.settings.basePath + "sgroup/delete_member/" + id;
	jQuery.getJSON(url, function(data) {
		if (data == 'success') {
			// remove row from table
			row.remove();
		}
	});
}