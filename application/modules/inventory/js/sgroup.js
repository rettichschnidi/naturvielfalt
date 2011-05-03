// This JavaScript is responsible for adding and removing the habitats at creation of a new area.

jQuery(document).ready(function() {
	// bind event "autocomplete_selection"
	jQuery("input#edit-sgrouptext").bind("autocomplete_select", sgroupAutocompleteSelect);
});

var sgroupIds = new Array();

// This will be called if user selects an autocompletion value
function sgroupAutocompleteSelect(event) {
	// get the new selected value
	var newSgroup = jQuery(this).val();
	var newSgroupArray = newSgroup.split("|");

	// avoid duplicates
	if (jQuery.inArray(newSgroupArray[0], sgroupIds) < 0) {
		sgroupIds.push(newSgroupArray[0]);
		// append it to the table
		var name = "<td>" + newSgroupArray[1] + "</td>";
		var checkbox = "<td>" +
							"<input type='checkbox' class='form-checkbox' name='read_new' value='1'/>" +
							"<input type='hidden' name='sgroup_id' value='" + newSgroupArray[0] + "'/>" +
						"</td>";
		var removeImg = Drupal.settings.basePath + "/modules/inventory/images/can_delete.png";
		var remove = "<td><img src='" + removeImg + "' onClick='removeSgroup(this);' style='cursor:pointer'/></td>";
		jQuery("#right_overview_other").append("<tr>" + name + checkbox + remove + "</tr>");
	}

	// reset the autocompletion textfield
	jQuery(this).val("");
}

//This will be called if a user clicks on the remove icon if permissions are not yet saved in db
function removeSgroup(image) {
	var row = jQuery(image).parent().parent();
	var id = row.find('input').attr('value');
	var pos = jQuery.inArray(id, sgroupIds);
	sgroupIds.splice(pos, 1);
	row.remove();
}


//This JavaScript is responsible for adding and removing the habitats at creation of a new area.

jQuery(document).ready(function() {
	// bind event "click" on all delete buttons
	jQuery("td.delete div").bind("click", deleteSgroup);
});

//This gets called if a user clicks on the delete button if permissions are already saved in db
function deleteSgroup(event) {
	var row = jQuery(this).parent().parent();
	var id = jQuery(this).attr('id');
	var idArray = id.split("/");
	var url = Drupal.settings.basePath + "inventory/" + idArray[0] + "/delete_sgroup/" + idArray[1];
	jQuery.getJSON(url, function(data) {
		if (data == 'success') {
			// remove row from table
			row.remove();
		}
	});
}