/**
 * Handle custom autocomplete functions
 */
jQuery(document).ready( function () {
	(function($) {
		if (Drupal.jsAC !== undefined) {
			// Overwrite default Drupal function to define autocomplete_select event
			// do it only if autocomplete.js is loaded
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
		}

		/**
		 * This will be called if user selects an autocompletion value
		 * @param event
		 */
		function autocompleteInvite(event) {
			// get the new selected value
			var elemStr, elem, line, marker; // line_copy, css_class
			marker = '<span class="warning">*</span>';	
			elemStr = $(this).val();
			if (elemStr !== '') {
				try {
					elem = $.parseJSON(elemStr);
				}
				catch(e) {
					return;
				}
				
				$("#tabs-wrapper").after('<div class="messages warning"><h2 class="element-invisible">Error message</h2>'
						+ marker + Drupal.t('You need to click') + ' "' + Drupal.t('Save') + '" ' + Drupal.t('in order to add this user	') + '</div>');
				/*
				// reset input value and copy line
				$(this).val('');
				line_copy = $(this).parents('tr:first').clone();
				 */
				// set value and name
				$(this).val(elem.name);
				$(this).attr('name', $(this).attr('name') + '-' + elem.id);
				$(this).hide();
				line = $(this).parents('tr:first');
				line.find('td').each(function (i) {
					if (i === 0) {
						$(this).append(elem.name);
						$(this).append(marker);
					}
					else {
						$(this).html("<div class='emptyTableField'></div>");
						$(this).html("-");
					}
				});
				/*
				css_class = line_copy.hasClass('even') ? 'odd' : 'even';
				line_copy.attr('class', css_class);
				line_copy.find('div#autocomplete').remove();
				line_copy.appendTo($(this).parents('tbody:first'));
				 */
			}
		}

		/**
		 * This will be called if user selects an autocompletion value
		 * @param event
		 */
		function autocompleteSelect(event) {
			// get the new selected value
			var elemStr, elem, line, marker;
			elemStr = $(this).val();
			if (elemStr !== '') {
				try {
					elem = $.parseJSON(elemStr);
				}
				catch(e) {
					return;
				}
				marker = '<span class="warning">*</span>';
				if ($("#mymsgbox").length === 0) {
					$("#tabs-wrapper").after('<div id="mymsgbox" class="messages warning"><h2 class="element-invisible">Error message</h2>'
							+ marker + Drupal.t('You need to click') + ' "' + Drupal.t('Save') + '" ' + Drupal.t('in order to add this element') + '</div>');
				}
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
				$(this).parents('td:first').html(elem.name + marker);
			}
		}

		/**
		 * This will be called if user clicks on delete symbol
		 * @param event
		 */
		function deleteRow (event) {
			$(this).parents('tr:first').remove();
		}

		// bind "autocomplete_selection" event on autocomplete input fields
		jQuery("input#add-sgroup").bind("autocomplete_select",
				autocompleteSelect);
		jQuery("input#add-users").bind("autocomplete_select",
				autocompleteSelect);
		jQuery("input#add-sgroup").bind("blur",
				autocompleteSelect);
		jQuery("input#add-users").bind("blur",
				autocompleteSelect);
		jQuery("input#invite-users").bind("autocomplete_select",
				autocompleteInvite);
		jQuery("input#invite-users").bind("blur",
				autocompleteInvite);
		// bind click event on delete icons
		jQuery("div[id^='delete']").bind("click",
				deleteRow);

	})(jQuery);
});