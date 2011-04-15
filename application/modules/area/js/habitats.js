// This JavaScript is responsible for adding and removing the habitats at creation of a new area.

jQuery(document).ready(function() {
	// bind event "autocomplete_selection"
	jQuery("input#edit-habitattext").bind("autocomplete_select", habitatAutocompleteSelect);
});

// This will be called if user selects an autocompletion value
function habitatAutocompleteSelect(event) {
	// get the new selected value
	var newHabitat = jQuery(this).val();

	// append it to the table
	var newHabitatArray = newHabitat.split("|");
	var id = "<input type='hidden' name='habitat[" + newHabitatArray[0] + "]' value='" + newHabitatArray[0] + "'/>";
	var label = "<td>" + newHabitatArray[1] + "</td>";
	var name = "<td>" + newHabitatArray[2] + "</td>";
	var removeImg = Drupal.settings.basePath + "/modules/inventory/images/can_delete.png";
	var remove = "<td><img src='" + removeImg + "' onClick='removeHabitat(this);' style='cursor:pointer'/>";
	jQuery("#habitatSelected table").append("<tr>" + id + label + name + remove + "</tr>");
	
	// reset the autocompletion textfield
	jQuery(this).val("");
}

// This will be called if a user clicks on the remove icon
function removeHabitat(image) {
	var row = jQuery(image.parentElement.parentElement);
	row.remove();
}