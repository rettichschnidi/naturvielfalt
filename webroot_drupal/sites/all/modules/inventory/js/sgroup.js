/**
 * 
 * 
 */


/**
 * 
 */
(function($) {

	var sgroupIds = new Array();

	/**
	 * 
	 */
	function initializeGroupRow() {
		$(this).find('input[name^="write"]').change(
				function() {
					$(this).closest('tr').find(
							'input[name^="show_red"], input[name^="read"]')
							.attr('disabled',
									$(this).is(':checked') ? 'disabled' : '')
							.attr('checked',
									$(this).is(':checked') ? 'checked' : '');
				}).change();
		$(this).find("td.delete div").unbind('click').click(deleteSgroup);
		$(this).find("td.delete img").unbind('click').click(removeSgroup);
	}

	/**
	 * This will be called if user selects an autocompletion value
	 * @param event
	 */
	function sgroupAutocompleteSelect(event) {
		// get the new selected value
		var newSgroup = $(this).val();
		var newSgroupArray = newSgroup.split("|");

		// avoid duplicates
		if ($.inArray(newSgroupArray[0], sgroupIds) < 0) {
			sgroupIds.push(newSgroupArray[0]);
			// append it to the table
			var cnt = sgroupIds.length - 1;
			var name = "<td>" + newSgroupArray[1] + "</td>";
			var write = "<td>"
					+ "<input type='checkbox' class='form-checkbox' name='write_new_"
					+ cnt + "' value='1'/>"
					+ "<input type='hidden' name='sgroup_id_" + cnt
					+ "' value='" + newSgroupArray[0] + "'/>" + "</td>";
			var read = "<td><input type='checkbox' class='form-checkbox' name='read_new_"
					+ cnt + "' value='1'/></td>";
			var show_red = "<td><input type='checkbox' class='form-checkbox' name='show_red_new_"
					+ cnt + "' value='1'/></td>";
			var removeImg = Drupal.settings.basePath
					+ "/sites/all/modules/inventory/images/can_delete.png";
			var remove = "<td class='delete'><img src='" + removeImg
					+ "' style='cursor:pointer'/></td>";
			var row = $("<tr>" + name + write + read + show_red + remove
					+ "</tr>");
			$("#right_overview_other").append(row);
			row.each(initializeGroupRow);
		}

		// reset the autocompletion textfield
		$(this).val("");
	}

	/**
	 * This will be called if a user clicks on the remove icon if permissions
	 * are not yet saved in db
	 * @param image
	 */
	function removeSgroup(image) {
		var row = $(this).parent().parent();
		var id = row.find('input').attr('value');
		var pos = $.inArray(id, sgroupIds);
		sgroupIds.splice(pos, 1);
		row.remove();
	}

	/**
	 * This gets called if a user clicks on the delete button if permissions are
	 * already saved in db
	 * @param event
	 */
	function deleteSgroup(event) {
		var row = $(this).parent().parent();
		var id = $(this).attr('id');
		var idArray = id.split("/");
		var url = Drupal.settings.basePath + "inventory/" + idArray[0]
				+ "/delete_sgroup/" + idArray[1];
		$.getJSON(url, function(data) {
			if (data == 'success') {
				// remove row from table
				row.remove();
			}
		});
	}

	/**
	 * 
	 */
	$(document).ready(
			function() {
				// bind event "autocomplete_selection"
				$("input#edit-sgrouptext").bind("autocomplete_select",
						sgroupAutocompleteSelect);

				$('#inventory-edit-rights table tbody tr').each(
						initializeGroupRow);
			});

})(jQuery);
