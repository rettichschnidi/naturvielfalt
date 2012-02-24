/**
 * Handle custom autocomplete functions
 */
(function($) {
	$(document).ready( function () {
		// Overwrite default Drupal function to define autocomplete_select event
		// START
		/**
		 * Puts the currently highlighted suggestion into the autocomplete field.
		 */
		Drupal.jsAC.prototype.select = function (node) {
			this.input.value = $(node).data('autocompleteValue');
			// !! INSERTED BY SWISSMON PROJECT!! START
			$(this.input).trigger('autocomplete_select', [node]);
			// !! INSERTED BY SWISSMON PROJECT!! END
		};

		/**
		 * Handler for the "keydown" event.
		 */
		Drupal.jsAC.prototype.onkeydown = function (input, e) {
			if (!e) {
				e = window.event;
			}
			switch (e.keyCode) {
			// !! INSERTED BY SWISSMON PROJECT!! START
			case 13: // Enter.
				this.hidePopup(e.keyCode);
				$(this.input).trigger('autocomplete_select');
				return true;
				// !! INSERTED BY SWISSMON PROJECT!! END
			case 40: // down arrow.
				this.selectDown();
				return false;
			case 38: // up arrow.
				this.selectUp();
				return false;
			default: // All other keys.
				return true;
			}
		};
		// END

		/**
		 * This will be called if user selects an autocompletion value
		 * @param event
		 */
		function autocompleteSelect(event) {
			// get the new selected value
			var elemStr, elem, line;
			elemStr = $(this).val();
			elem = $.parseJSON(elemStr);
			// replace "new" by [acl_id] in all radio elements
			line = $(this).parents('tr:first');
			line.find('div[class$="new"]').each(function () {
				var input_class;
				input_class = $(this).attr('class');
				input_class = input_class.replace("new", elem.id);
				$(this).attr('class', input_class);
			});
			line.find('input[name$="new"]').each(function () {
				var name;
				name = $(this).attr('name');
				name = name.replace("new", elem.id);
				$(this).attr('name', name);
			});
			// replace inputfield by new name
			$(this).parents('td:first').html(elem.name);
		}

		/**
		 * This will be called if user clicks on delete symbol
		 */
		function deleteRow (event) {
			$(this).parents('tr:first').remove();
		}

		// bind "autocomplete_selection" event on autocomplete input fields
		jQuery("input#add-sgroup").bind("autocomplete_select",
				autocompleteSelect);
		jQuery("input#add-users").bind("autocomplete_select",
				autocompleteSelect);
		// bind click event on delete icons
		jQuery("div[id^='delete']").bind("click",
				deleteRow);
	});

})(jQuery);