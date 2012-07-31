/**
 * @author Simon Käser
 */
var inventory = {

	id: false,
	container: false,
	add: false,
	message: false,
	form: false,
	template: false,
	loading: false,
	request: false,
	count: 1,
	group_count: 1,
	templates: false,
	base: false,
	map: false,
	entry_links: new Array('.additional', '.images', '.location'),
	last_date: false

};

/**
 * 
 */ (function ($) {

	/**
	 * 
	 */
	inventory.init = function () {
		inventory.initCustomFields();
		inventory.initAdditionalFields();
		inventory.initTemplates();
		inventory.initLocation();
		inventory.container = $('#inventories');
		// Add dialog for location form
		$('td a.location').unbind('click').click(inventory.locationDialog);
		if(!inventory.container.size()) return;
		$('form').each(function () {
			if($(this).find('#inventories').size()) return;
			$(this).submit(function (e) {
				e.preventDefault();
				e.stopPropagation();
				inventory.saveEntries(e, $.proxy(function () {
					$(this).unbind('submit').submit();
				}, this));
			});
			$(this).find('a:not(a[href^="javascript"])').click(function (e) {
				e.preventDefault();
				e.stopPropagation();
				inventory.saveEntries(e, $.proxy(function () {
					document.location = $(this).attr('href');
				}, this));
			});
		});
		inventory.form = inventory.container.closest('form');
		inventory.base = inventory.form.attr('action').replace(/\/e?d?i?t?$/, '');
		inventory.id = inventory.form.find('#invId').val();
		inventory.form.find('#edit-actions').append(inventory.form.find('.form-item-add'));
		inventory.form.find('input[name^="form_"]').remove();
		inventory.form.submit(inventory.saveEntries);
		inventory.add = inventory.form.find('#add-inventory-group');
		inventory.add.change(inventory.addInventory);
		inventory.message = inventory.form.find('#message');
		if(!inventory.message.size()) {
			inventory.message = $('<div id="message"><div class="messages" /></div>').hide();
			inventory.form.find('#edit-actions').append(inventory.message);
		}
		inventory.message.ajaxError(inventory.ajaxError);
		inventory.container.find('.invTable').each(function () {
			inventory.initializeTable($(this));
		});
		inventory.template = inventory.form.find('#template');
		if(inventory.template.size()) inventory.template.click(inventory.templatesDialog);
	};

	/**
	 * 
	 * @param message
	 * @param type
	 * @param time
	 */
	inventory.setMessage = function (message, type, time) {
		if(inventory.messageTimer) window.clearTimeout(inventory.messageTimer);
		inventory.message.children('.messages').html(message).attr('class', 'messages').addClass(type);
		inventory.message.stop().css('height', 'auto').slideDown('fast');
		if(time) inventory.messageTimer = window.setTimeout(function () {
			inventory.message.slideUp('fast');
		}, time);
	};

	/**
	 * 
	 * @param event
	 * @param jqXHR
	 * @param settings
	 */
	inventory.ajaxError = function (event, jqXHR, settings) {
		if(settings.url.indexOf('autocomplete') == -1) {
			// show error for failed requests (ignore autocomplete)
			inventory.hideLoading();
			inventory.setMessage(Drupal.t('An AJAX HTTP request terminated abnormally.'), 'error', 15000);
		}
	};

	/**
	 * 
	 */
	inventory.showLoading = function () {
		if(!inventory.loading) {
			inventory.loading = $('<div><img src="' + Drupal.settings.basePath + 'sites/all/modules/inventory/images/loading.gif" /></div>').hide();
			$('body').append(inventory.loading);
		}
		inventory.loading.dialog({
			modal: true,
			draggable: false,
			resizable: false,
			closeOnEscape: false,
			closeText: '',
			dialogClass: 'inventory-edit-loading',
			width: 42,
			height: 42,
			minWidth: 42,
			minHeight: 42
		});
	};

	/**
	 * 
	 */
	inventory.hideLoading = function () {
		if(!inventory.loading) return;
		inventory.loading.dialog('close');
	};

	/**
	 * 
	 * @param event
	 * @param callback
	 */
	inventory.saveEntries = function (event, callback) {
		inventory.showLoading();
		inventory.callback = callback;
		$.post(
		inventory.base + '/save_entries?ajax=1', inventory.form.serializeArray(), function (data) {
			inventory.hideLoading();
			if(!data) return;
			for(var i = 0; i < data.inventories.length; i++) {
				inventory.container.find('#invTitle' + data.inventories[i]['id_old']).attr('id', 'invTitle' + data.inventories[i]['id_new']);
				inventory.container.find('#invTable' + data.inventories[i]['id_old']).attr('id', 'invTable' + data.inventories[i]['id_new']);
				inventory.container.find('input.inventory[name="inventories[' + data.inventories[i]['id_old'] + ']"]').attr('name', 'inventories[' + data.inventories[i]['id_new'] + ']');
				inventory.container.find('input[name^="entries[' + data.inventories[i]['id_old'] + ']"]').each(function () {
					$(this).attr('name', $(this).attr('name').replace('entries[' + data.inventories[i]['id_old'] + ']', 'entries[' + data.inventories[i]['id_new'] + ']'));
				});
			}
			for(var i = 0; i < data.entries.length; i++) {
				var entry = inventory.container.find('input.entry_id[value="' + data.entries[i]['id_old'] + '"]');
				entry.val(data.entries[i]['id_new']);
				inventory.rewriteLinks(entry.closest('tr'), inventory.entry_links, data.entries[i]['id_new']);
			}
			inventory.group_count = data.group_count;
			if(inventory.callback) inventory.callback();
			if(data.message) inventory.setMessage(data.message, data.result == 1 ? 'status' : 'error', 15000);
		}, 'json');
		return false;
	};

	/**
	 * 
	 */
	inventory.resetOddEven = function () {
		inventory.container.find('.invTable tbody tr:odd').removeClass('odd').addClass('even');
		inventory.container.find('.invTable tbody tr:even').removeClass('even').addClass('odd');
	};

	/**
	 * 
	 */
	inventory.deleteEntry = function () {
		var row = $(this).closest('tr');
		var id = row.find('input.entry_id').val();
		if(id.substr(0, 4) == 'new_') { // The entry has not yet been stored, therefor we just delete the row
			inventory.removeEntry(row);
			return;
		}
		inventory.showLoading();
		$.getJSON(
		inventory.base + '/delete_entry/' + id, {
			ajax: 1
		}, $.proxy(function (data) {
			if(!data) return;
			if(data.result == 1) inventory.removeEntry(row);
			if(data.message) inventory.setMessage(data.message, data.result == 1 ? 'status' : 'error', 15000);
			inventory.hideLoading();
		}, row));
	};

	/**
	 * 
	 * @param row
	 */
	inventory.removeEntry = function (row) {
		var table = row.closest('table');
		row.remove();
		inventory.updateGroupCount(table);
		inventory.resetOddEven();
		inventory.updateRowPositions();
		inventory.initializeSortable(table);
	};

	/**
	 * 
	 * @param table
	 */
	inventory.updateGroupCount = function (table) {
		var orgs = [];
		var count = table.find('tbody tr:not(.disabled)').size();
		table.find('tbody tr:not(.disabled) input.organism_id').each(function () {
			if(orgs.indexOf($(this).val()) < 0) orgs.push($(this).val());
		});
		var prev = table.prev();
		while(!prev.hasClass('invTitle'))
		prev = prev.prev();
		prev.find('small').html('(' + count + '/' + orgs.length + ')');
	};

	/**
	 * 
	 * @param row
	 */
	inventory.enableDisable = function (row) {
		if(row.find == undefined) row = $(row);
		if(!row.find('input.organism_id').val()) {
			row.addClass('disabled');
			row.find('td input:not(input[type="hidden"], .organism)').attr('disabled', 'disabled');
		} else {
			if(inventory.last_date && row.hasClass('disabled')) row.find('input.date').val(inventory.last_date);
			row.removeClass('disabled');
			row.find('td input:not(input[type="hidden"], .organism)').attr('disabled', '');
		}
		inventory.updateGroupCount(row.closest('table'));
	};

	/**
	 * 
	 */
	inventory.updateRowPositions = function () {
		inventory.container.find('tbody tr').each(function () {
			inventory.updateRowPosition($(this));
		});
	};

	/**
	 * 
	 * @param row
	 */
	inventory.updateRowPosition = function (row) {
		var position = row.closest('tbody').find('tr').index(row);
		row.find('*[name^="entries["]').each(function () {
			var name = $(this).attr('name').split('[');
			name[2] = position + ']';
			$(this).attr('name', name.join('['));
		});
	};

	/**
	 * 
	 * @param row
	 */
	inventory.duplicateRow = function (row) {
		var clone = $('<tr>' + row.html() + '</tr>');
		clone.addClass(row.hasClass('odd') ? 'even' : 'odd');
		clone.find('a.delete').remove();
		clone.find('em').html('');
		clone.find('td.image').html('');
		clone.find('input:not(.identifier, .date)').val('');
		clone.find('select option').attr('selected', '');
		clone.find('input.date').attr('id', '').removeClass('hasDatepicker');
		row.closest('tbody').append(clone);
		inventory.initializeRow(clone);
		inventory.initializeSortable(clone.closest('.invTable'));
		inventory.updateRowPosition(clone);
	};

	/**
	 * 
	 * @param row
	 */
	inventory.getImage = function (row) {
		var id = row.find('input.organism_id').val();
		row.find('td.image').html('');
		$.getJSON(
		Drupal.settings.basePath + 'inventory/get_entry_image/' + id, $.proxy(function (data) {
			if(!data) return;
			this.find('td.image').html(data.data);
		}, row));
	};

	/**
	 * 
	 */
	inventory.addInventory = function () {
		inventory.showLoading();
		$.getJSON(
		inventory.base + '/add_inventory_group/' + $(this).val() + '/' + inventory.group_count++, function (data) {
			inventory.hideLoading();
			if(!data) return;
			if(data.data) {
				data = $(data.data);
				inventory.container.append(data);
				inventory.initializeTable(data);
				data.find('.organism').focus();
				inventory.form.find('p.empty').fadeOut();
			}
			inventory.add.find('option[value=""]').attr('selected', true);
		});
	};

	/**
	 * 
	 * @param e
	 */
	inventory.templatesDialog = function (e) {
		e.preventDefault();
		inventory.showLoading();
		var data = {
			ajax: 1
		};
		$.post(
		$(this).attr('href'), data, inventory.templatesCreateDialog, 'json');
	};

	/**
	 * 
	 * @param data
	 */
	inventory.templatesCreateDialog = function (data) {
		if(data && data.form) {
			var dialog = $('<div title="' + inventory.template.html() + '" />');
			dialog.append($(data.form));
			dialog.dialog({
				modal: true,
				resizable: false,
				closeOnEscape: false,
				closeText: '',
				close: function (event, ui) {
					$(this).remove();
				},
				width: 700
			});
			inventory.initTemplates();
			dialog.find('#edit-actions a').click(function (e) {
				e.preventDefault();
				$(this).closest('.ui-dialog-content').dialog('close');
			});
			dialog.find('form').submit(function (e) {
				var data = $(this).serializeArray();
				inventory.form.find('.inventory').each(function () {
					data.push({
						name: $(this).attr('name'),
						value: $(this).val()
					});
				});
				inventory.form.find('.organism_id').each(function () {
					data.push({
						name: 'organisms[]',
						value: $(this).val()
					});
				});
				$.post(
				$(this).attr('action') + '?ajax=1', data, function (data) {
					inventory.hideLoading();
					if(!data.result && data.form) {
						inventory.templatesCreateDialog(data);
						return;
					}
					if(!data) return;
					inventory.group_count = data.group_count;
					for(var id in data.new_inventories) {
						inv = $(data.new_inventories[id]);
						inventory.container.append(inv);
						inventory.initializeTable(inv);
						inventory.form.find('p.empty').fadeOut();
					}
					for(var inventory_id in data.new_entries) {
						var entries = data.new_entries[inventory_id];
						var table = inventory.container.find('#invTable' + inventory_id);
						var lastrow = table.find('tr:last');
						for(var i = 0; i < entries.length; i++) {
							if(!entries[i]) continue;
							var entry = $(entries[i]);
							entry.insertBefore(lastrow);
							inventory.initializeRow(entry);
						}
						inventory.initializeSortable(table);
					}
					inventory.updateRowPositions();
					inventory.resetOddEven();
					if(data.message) inventory.setMessage(data.message, data.result == 1 ? 'status' : 'error', 15000);
				}, 'json');
				inventory.showLoading();
				$(this).closest('.ui-dialog-content').dialog('close');
				return false;
			});
		}
		inventory.hideLoading();
	};

	/**
	 * 
	 * @param e
	 * @param url
	 * @param callback
	 */
	inventory.customFieldsDialog = function (e, url, callback) {
		e.preventDefault();
		inventory.showLoading();

		if(url == null) {
			url = $(this).attr('href');
		}

		$.getJSON(url, {
			ajax: 1
		}, function (data) {
			if(data && data.form) {
				var dialog = $('<div title="' + inventory.form.find('.custom').attr('title') + '" />');
				dialog.append($(data.form));
				dialog.dialog({
					modal: true,
					resizable: false,
					closeOnEscape: false,
					closeText: '',
					close: function (event, ui) {
						$(this).remove();
					},
					width: 500
				});
				inventory.initCustomFields();
				dialog.find('#edit-actions a').click(function (e) {
					e.preventDefault();
					$(this).closest('.ui-dialog-content').dialog('close');
				});
				dialog.find('form').submit(function (e) {
					$.post(
					$(this).attr('action'), $(this).serializeArray(), function (data) {
						inventory.hideLoading();
						if(!data) return;
						if(data.message) inventory.setMessage(data.message, data.result == 1 ? 'status' : 'error', 15000);
						if(callback != null && data.result == 1) {
							callback();
						}
					}, 'json');
					inventory.showLoading();
					$(this).closest('.ui-dialog-content').dialog('close');

					return false;
				});
			}
			inventory.hideLoading();
		});
	};

	/**
	 * 
	 * @param e
	 */
	inventory.additionalFieldsDialog = function (e) {
		e.preventDefault();
		inventory.showLoading();
		var data = {
			ajax: 1
		};
		var url = $(this).attr('href');
		var customFieldLink = $(this).parents(".invTable").prev().find("a.custom");
		$(this).closest('tr').find('td:last input[type="hidden"]').each(function () {
			var name = $(this).attr('name');
			data[name.substring(name.indexOf('col_'), name.length - 1)] = $(this).val();
		});
		$.getJSON(url, data, function (data) {
			if(data && data.form) {
				var dialog = $('<div title="' + inventory.form.find('.additional').attr('title') + '" />');
				dialog.append($(data.form));
				dialog.dialog({
					modal: true,
					resizable: false,
					closeOnEscape: false,
					closeText: '',
					close: function (event, ui) {
						$(this).remove();
					}
				});
				inventory.initAdditionalFields();
				dialog.find('#edit-actions a').click(function (e) {
					e.preventDefault();
					$(this).closest('.ui-dialog-content').dialog('close');
				});
				dialog.find('legend').css('background-color', 'white');

				var width = dialog.find('fieldset').width();
				dialog.dialog("option", "width", width + 130);
				dialog.dialog("option", "position", "center");

				var additionalFields = jQuery("<a>" + Drupal.t('edit') + " </a>");
				additionalFields.attr('href', customFieldLink.attr('href')).css('margin-left', '10px');
				dialog.find('legend span').last().append(additionalFields);
				additionalFields.click(function (e) {
					e.preventDefault();
					var dat = {
						ajax: 1
					};
					inventory.customFieldsDialog(e, $(this).attr('href'), function () {
						$.getJSON(url, dat, function (dat) {
							if(dat && dat.form) {
								var newForm = jQuery(dat.form);
								var newFields = newForm.find("div.fieldset-wrapper").last();
								if(newFields.children().length > 0) {
									var oldFields = dialog.find("div.fieldset-wrapper").last();
									if(newFields.children().length > 0 && oldFields.children().length > 0) {
										//Copy values
										newFields.find('input').each(function () {
											var oldField = oldFields.find('input#' + $(this).attr('id'));
											if(oldField.length == 1) {
												$(this).val(oldField.val());
											}
										});
									}
									dialog.find("div.fieldset-wrapper").last().replaceWith(newFields).show();
								} else {
									dialog.find("div.fieldset-wrapper").last().html("");
								}
							}
						});
					});
				});


				dialog.find('form').submit(function (e) {
					var values = $(this).serializeArray();
					var entry_id = $(this).attr('action').split('/').pop().replace(/\?.*$/, '');
					var row = inventory.container.find('input.entry_id[value="' + entry_id + '"]').closest('tr');
					var base_name = row.find('input.entry_id').attr('name');
					var inventory_id = row.closest('table').find('.inventory').attr('name');
					inventory_id = inventory_id.substring(12, inventory_id.length - 1);
					for(var i = 0; i < values.length; i++) {
						var name = values[i]['name'];
						if(name.substr(0, 4) != 'col_') continue;
						var value = values[i]['value'];
						var iname = base_name.replace('[id]', '[' + name + ']');
						var input = row.find('input[name="' + iname + '"]');
						if(!input.size()) { // create the hidden field if it is not yet present
							input = $('<input type="hidden" name="' + iname + '" value="" />');
							row.find('td:last').append(input);
						}
						input.val(value);
					}
					inventory.setMessage(Drupal.t('The additional fields data will be stored only after saving the whole form by pressing the <em>Save</em> button in the lower right.'), 'warning', 15000);
					$(this).closest('.ui-dialog-content').dialog('close');
					return false;
				});
			}
			inventory.hideLoading();
		});
	};

	/**
	 * 
	 * @param row
	 * @param base_name
	 * @param element
	 * @param field
	 */
	inventory.addInput = function (row, base_name, element, field) {
		var iname = base_name.replace('[id]', '[' + field + ']');
		var input = row.find('input[name="' + iname + '"]');
		if(!input.size()) { // create the hidden field if it is not yet present
			input = $('<input type="hidden" class="' + field + '" name="' + iname + '" value="" />');
			row.find('td:last').append(input);
		}
		input.val(element.find('input[name="' + field + '"]').val());
	};

	/**
	 * 
	 * @param e
	 */
	inventory.locationDialog = function (e) {
		e.preventDefault();
		inventory.showLoading();
		var data = {
			ajax: 1
		};
		var lat = $(this).closest('tr').find('input.lat');
		if(lat.size()) data['lat'] = lat.val();
		var lng = $(this).closest('tr').find('input.lng');
		if(lng.size()) data['lng'] = lng.val();
		$.getJSON($(this).attr('href'), data, function (data) {
			if(data && data.form) {
				var dialog = $('<div title="' + data.title + '" />');
				dialog.append($(data.form));
				dialog.dialog({
					modal: true,
					resizable: false,
					closeOnEscape: false,
					closeText: '',
					close: function (event, ui) {
						inventory.location = false;
						inventory.tmp_location = false;
						$(this).remove();
					},
					width: 700
				});
				inventory.initLocation();
				dialog.find('#edit-actions a').click(function (e) {
					e.preventDefault();
					$(this).closest('.ui-dialog-content').dialog('close');
				});
				dialog.find('form').submit(function (e) {
					var entry_id = $(this).attr('action').split('/').pop().replace(/\?.*$/, '');
					var row = inventory.container.find('input.entry_id[value="' + entry_id + '"]').closest('tr');
					var base_name = row.find('input.entry_id').attr('name');

					inventory.storePrevPos(inventory.location.position);

					inventory.addInput(row, base_name, $(this), 'lat');
					inventory.addInput(row, base_name, $(this), 'lng');
					inventory.addInput(row, base_name, $(this), 'zip');
					inventory.addInput(row, base_name, $(this), 'locality');
					inventory.addInput(row, base_name, $(this), 'township');
					inventory.addInput(row, base_name, $(this), 'canton');
					inventory.addInput(row, base_name, $(this), 'country');

					row.find('a.location img').attr('src', row.find('a.location img').attr('src').replace('_unset', ''));

					inventory.setMessage(Drupal.t('The location will be stored only after saving the whole form by pressing the <em>Save</em> button in the lower right.'), 'warning', 15000);
					$(this).closest('.ui-dialog-content').dialog('close');
					return false;
				});
			}
			inventory.hideLoading();
		});
	};

	/**
	 * 
	 * @param context
	 */
	inventory.scrollToAutocompleteItem = function (context) {
		var widget = $(context).autocomplete('widget');
		var item = widget.find('#ui-active-menuitem');
		var offset = item.offset();
		var height = item.outerHeight();
		var wheight = $(window).height();
		var scrolltop = $(document).scrollTop();
		if((offset.top + height - scrolltop <= wheight) && offset.top >= scrolltop) return false;
		cover = $('#ui-autocomplete-cover');
		if(!cover.size()) cover = $('<div id="ui-autocomplete-cover" />').appendTo($('body')).mousemove(function (e) {
			$(this).remove();
		});
		cover.css({
			'position': 'absolute',
			'left': widget.position().left,
			'top': widget.position().top,
			'height': widget.outerHeight(),
			'width': widget.outerWidth(),
			'z-index': 100000
		});
		if(offset.top + height - scrolltop > wheight) $(document).scrollTop(offset.top + height - wheight + 2);
		else if(offset.top < scrolltop) $(document).scrollTop(offset.top - 2);
	};

	/**
	 * 
	 * @param table
	 */
	inventory.initializeTable = function (table) {
		table.find('tbody tr').each(function () {
			inventory.initializeRow($(this));
		});
		table.find('tr th:last-child').html('');

		table.find('thead tr').prepend('<th>&nbsp;</th>');
		inventory.initializeSortable(table);

		var prev = table.prev();
		while(!prev.hasClass('invTitle'))
		prev = prev.prev();
		prev.find('.custom').click(inventory.customFieldsDialog);
	};

	/**
	 * 
	 * @param table
	 */
	inventory.initializeSortable = function (table) {
		table.sortable('destroy');
		table.sortable({
			items: 'tbody tr:not(.disabled)',
			handle: '.handler',
			axis: 'y',
			tolerance: 'pointer',
			update: function (event, ui) {
				inventory.resetOddEven();
				inventory.updateRowPositions();
			}
		});
	};

	/**
	 * 
	 * @param container
	 */
	inventory.initializeFields = function (container) {
		// Date fields
		container.find('input.date').datepicker({
			dateFormat: 'dd.mm.yy',
			duration: 0,
			showButtonPanel: true,
			onSelect: function (date, inst) {
				if($(this).closest('.invTable').size()) inventory.last_date = date;
			}
		}).width(80);

		// Numeric fields
		container.find('input.int').numeric().width(70);
	};

	/**
	 * 
	 * @param context
	 * @param links
	 * @param id
	 */
	inventory.rewriteLinks = function (context, links, id) {
		for(var link in links) {
			var c = links[link]; //css class
			var element = context.find(c);
			if(!element.size()) return;
			var href = element.attr('href').split('/');

			switch(c) {
			case '.location':
			case '.additional':
				href[href.length - 1] = id;
				break;
			case '.images':
				href[href.length - 3] = id;
				break;
			}

			element.attr('href', href.join('/'));
		}
	};

	/**
	 * 
	 * @param row
	 */
	inventory.initializeRow = function (row) {
		if(!row.find('.handler').size()) row.prepend('<td><a class="handler"><img src="' + Drupal.settings.basePath + 'sites/all/modules/inventory/images/sort.gif" /></a></td>');

		row.find('td.image a').lightBox();

		row.find('input.entry_id[value=""]').each(function () {
			$(this).val('new_' + inventory.count++);
			inventory.rewriteLinks($(this).closest('tr'), inventory.entry_links, $(this).val());
		});

		// Replace delete checkbox
		row.find('input.delete').attr('checked', '').hide();
		row.find('td:last-child').append('<a class="delete" href="javascript://" title="' + row.closest('table').find('tr th:last').html() + '">' + '<img src="' + Drupal.settings.basePath + 'sites/all/modules/inventory/images/delete.gif" />' + '</a>');
		row.find('a.delete').unbind('click').click(inventory.deleteEntry);

		// Add dialog for additional fields form
		row.find('a.additional').unbind('click').click(inventory.additionalFieldsDialog);

		// Add dialog for location form
		row.find('a.location').unbind('click').click(inventory.locationDialog);

		// Save entries before going to the manage images page
		row.find('a.images').unbind('click').click(function (e) {
			e.preventDefault();
			inventory.saveEntries(e, $.proxy(function () {
				document.location = $(this).attr('href');
			}, this));
		});

		// Disable input fields if no organism is selected
		inventory.enableDisable(row);

		// Initialize fields
		inventory.initializeFields(row);

		// Organism field
		row.find('input.organism').autocomplete({
			minLength: 2,
			autoFocus: true,
			source: function (request, response) {
				//new search, so we change the indicator to searching
				actualElement = this.element;
				actualElement.removeClass('notfound');
				actualElement.addClass('searching');
				if(inventory.request) {
					inventory.request.abort();
				}
				inventory.request = $.ajax({
					url: Drupal.settings.basePath + 'inventory/organism_autocomplete',
					dataType: "json",
					data: {
						inv_id: actualElement.closest('table').find('input.inventory').val(),
						term: actualElement.val()
					},
					// success : response,
					success: function (data) {
						if(data.length == 0) {
							//change visual indicator to notfound and hide menu again
							actualElement.removeClass("searching");
							actualElement.addClass("notfound");
							$('.ui-autocomplete').hide();
						} else {
							// Remove search symbol
							actualElement.removeClass("searching");
							response(data);
						}
					}
				});
			},
			focus: function (event, ui) {
				inventory.scrollToAutocompleteItem(this);
				return false;
			},
			select: function (event, ui) {
				$(this).val(ui.item.label || ui.item.label_latin_short);
				$(this).closest('td').find('input.organism_id').val(ui.item.id);
				$(this).closest('tr').find('td > em').html(ui.item.label_latin);
				inventory.enableDisable($(this).closest('tr'));
				if(event.keyCode == 9 || event.keyCode == 13) { // TAB, ENTER
					$(this).closest('tr').find('input.date').focus();
					event.preventDefault();
				}
				if($(this).closest('tr:last-child').size()) inventory.duplicateRow($(this).closest('tr'));
				inventory.getImage($(this).closest('tr'));
				return false;
			}
		}).keyup(function () {
			//monitor field and remove class 'notfound' if its length is less than 2
			if($(this).val().length < 2) {
				$(this).removeClass('notfound');
				$(this).closest('td').find('input.organism_id').val('');
				$(this).closest('tr').find('td > em').html('');
				$(this).closest('tr').find('.image').html('');
				inventory.enableDisable($(this).closest('tr'));
			}
		}).blur(function () {
			inventory.enableDisable($(this).closest('tr'));
		}).each(function () {
			$(this).data('autocomplete')._renderItem = function (ul, item) {
				term = this.term.replace(/[aou]/, function (m) {
					// to find with term 'wasser' -> 'wasser' and 'gewasesser'
					var hash = {
						'a': '(ä|a)',
						'o': '(ö|o)',
						'u': '(ä|u)'
					};
					return hash[m];
				});
				// highlighting of matches
				var label = item.label;
				var label_latin = item.label_latin;
				var old_label = item.old_label;
				var old_label_latin = item.old_label_latin;
				var re = new RegExp($.trim(term), 'ig');
				if(old_label) old_label = old_label.replace(re, '<span class="ui-state-highlight">$&</span>');
				else label = label.replace(re, '<span class="ui-state-highlight">$&</span>');
				term = $.trim(term).split(' ');
				while(term.length) {
					re = new RegExp(term.pop(), 'ig');
					if(old_label_latin) old_label = old_label.replace(re, '<span class="ui-state-highlight">$&</span>');
					else label_latin = label_latin.replace(re, '<span class="ui-state-highlight">$&</span>');
				}
				var old = '';
				if(old_label || old_label_latin) old = '<small>' + old_label + '<em>' + old_label_latin + '</em></small>';
				return $('<li></li>').data('item.autocomplete', item).append('<a>' + label + '<em>' + label_latin + '</em>' + old + '</a>').appendTo(ul);
			};
		});
	};

	/**
	 * Templates form
	 */
	inventory.initTemplates = function () {
		inventory.templates = $('#inventory-templates-form');
		if(!inventory.templates.size()) return;
		inventory.templates.find('input[name="source"]').change(function () {
			inventory.templates.find('input.id, input.term').val('');
		});
		inventory.templates.find('input.term').autocomplete({
			minLength: 0,
			autoFocus: true,
			source: function (request, response) {
				//new search, so we change the indicator to searching
				this.element.removeClass('notfound');
				this.element.addClass('searching');
				var source = inventory.templates.find('input[name="source"]:checked').val();
				$.getJSON(
				inventory.templates.attr('action').replace('templates', source + '_autocomplete'), {
					term: this.element.val()
				}, function (data) {
					if(data.length == 0) {
						//change visual indicator to notfound and hide menu again
						inventory.templates.find('input.term').removeClass("searching");
						inventory.templates.find('input.term').addClass("notfound");
						$('.ui-autocomplete').hide();
					} else {
						// Remove search symbol
						inventory.templates.find('input.term').removeClass("searching");
						response(data);
					}
				});
			},
			focus: function (event, ui) {
				inventory.scrollToAutocompleteItem(this);
				return false;
			},
			select: function (event, ui) {
				$(this).val(ui.item.name);
				inventory.templates.find('input.id').val(ui.item.id);
				inventory.templates.find('input[type="submit"]').focus();
				return false;
			}
		}).focus(function () {
			$(this).autocomplete('search', $(this).val());
		}).keyup(function () {
			//monitor field and remove class 'notfound' if its length is less than 2
			if($(this).val().length < 2) {
				$(this).removeClass('notfound');
				inventory.templates.find('input.id').val('');
			}
		}).data('autocomplete')._renderItem = function (ul, item) {
			var term = this.term.replace(/[aou]/, function (m) {
				// to find with term 'wasser' -> 'wasser' and 'gewasesser'
				var hash = {
					'a': '(ä|a)',
					'o': '(ö|o)',
					'u': '(ä|u)'
				};
				return hash[m];
			});
			// highlighting of matches
			var name = item.name;
			if($.trim(term)) {
				var re = new RegExp($.trim(term), 'ig');
				name = name.replace(re, '<span class="ui-state-highlight">$&</span>');
			}
			return $('<li></li>').data('item.autocomplete', item).append('<a>' + (item.owner ? '<em class="owner">' + item.owner + '</em> ' : '') + name + '</a>').appendTo(ul);
		};
	};

	/**
	 * Additional fields form
	 */
	inventory.initAdditionalFields = function () {
		var additionalfields = $('#inventory-edit-entry-form');
		if(!additionalfields.size()) return;
		inventory.initializeFields(additionalfields);
	};

	/**
	 * Custom fields form
	 */
	inventory.initCustomFields = function () {
		var customfields = $('#customfields');
		if(!customfields.size()) return;
		customfields.find('tr:last td:first input').blur(inventory.addCustomField);
	};

	/**
	 * 
	 */
	inventory.addCustomField = function () {
		if(!$(this).val()) return;
		var row = $(this).closest('tr');
		var clone = $('<tr>' + row.html() + '</tr>');
		clone.addClass(row.hasClass('odd') ? 'even' : 'odd');
		row.closest('tbody').append(clone);
		clone.find('td:first input').blur(inventory.addCustomField);
		clone.find('input, select').each(function () {
			var name = $(this).attr('name');
			var i = parseInt(name.replace(/[^\d]+(\d+)[^\d]+/, '$1'));
			$(this).attr('name', name.replace('[' + i + ']', '[' + (i + 1) + ']'));
		});
		$(this).unbind('blur');
	};

	/**
	 * 
	 */
	inventory.initLocation = function () {
		if(!$('#map_location').size()) return;
		inventory.map = new AreaSelect('map_location');
		inventory.locationform = $('#inventory-edit-entry-location-form');
		if(!inventory.locationform.size()) inventory.locationform = $('body');
		var position = false;
		if(inventory.locationform.find('input[name="lat"]').val() !== '' || inventory.locationform.find('input[name="lng"]').val() !== '') position = new google.maps.LatLng(parseFloat(inventory.locationform.find('input[name="lat"]').val()), parseFloat(inventory.locationform.find('input[name="lng"]').val()));
		//if we have a position and we are not in singleobservations
		//then we only place the marker
		if(position && window.location.href.search("singleobservations") == -1) {
			inventory.placeMarker(position);
			inventory.map.map.panTo(position);
			inventory.map.map.setOptions({
				zoom: 14
			});
			inventory.map.mapOverlays.fitBoundsOnLoad = false;
		} else if(window.location.href.search("singleobservations") != -1) {
			if(position) {
				// if we have
				inventory.placeMarker(position);
				inventory.map.map.panTo(position);
				inventory.map.map.setOptions({
					zoom: 14
				});
				if(inventory.getPrevPos() == null) {
					inventory.storePrevPos(position);
				}
			} else if(inventory.getPrevPos() != null) {
				inventory.map.map.panTo(inventory.getPrevPos());
				inventory.map.map.setOptions({
					zoom: 14
				});
			} else {
				inventory.map.initLocation();
				console.debug("enable de shizzle");
			}
		}



		if(!inventory.locationform.is('form')) return;
		inventory.map_pan = document.createElement('div');
		inventory.map_pan.className = 'map-control';
		var pan = document.createElement('img');
		pan.src = Drupal.settings.basePath + 'sites/all/modules/area/images/map_controls/hand-selected.png';
		inventory.map_pan.appendChild(pan);

		inventory.map_place = document.createElement('div');
		inventory.map_place.className = 'map-control';
		var place = document.createElement('img');
		place.src = Drupal.settings.basePath + 'sites/all/modules/area/images/map_controls/markerCtl.png';
		inventory.map_place.appendChild(place);

		inventory.map.map.controls[google.maps.ControlPosition.TOP_LEFT].push(inventory.map_pan);
		inventory.map.map.controls[google.maps.ControlPosition.TOP_LEFT].push(inventory.map_place);

		google.maps.event.addDomListener(inventory.map_pan, 'click', inventory.activatePan);
		google.maps.event.addDomListener(inventory.map_place, 'click', inventory.activatePlace);

		inventory.map.createSearchbar();
	};

	/**
	 * 
	 * @param p 
	 */
	inventory.storePrevPos = function (p) {
		if(window.sessionStorage) {
			window.sessionStorage.setItem('inventory_prev_lat', p.lat());
			window.sessionStorage.setItem('inventory_prev_lng', p.lng());
		}
	};

	/**
	 * 
	 */
	inventory.getPrevPos = function () {
		if(window.sessionStorage && window.sessionStorage.getItem('inventory_prev_lat') != null) {
			var lat = window.sessionStorage.getItem('inventory_prev_lat');
			var lng = window.sessionStorage.getItem('inventory_prev_lng');
			return new google.maps.LatLng(lat, lng);
		}
		return null;
	};

	/**
	 * 
	 * @param e 
	 */
	inventory.activatePan = function (e) {
		$(inventory.map_pan).find('img').attr('src', $(inventory.map_pan).find('img').attr('src').replace('-selected', '').replace('.png', '-selected.png'));
		$(inventory.map_place).find('img').attr('src', $(inventory.map_place).find('img').attr('src').replace('-selected', ''));
		if(inventory.tmp_location) {
			inventory.tmp_location.setVisible(false);
			for(var listener in inventory.listeners)
			google.maps.event.removeListener(listener);
		}
	};

	/**
	 * 
	 * @param e
	 */
	inventory.activatePlace = function (e) {
		$(inventory.map_place).find('img').attr('src', $(inventory.map_place).find('img').attr('src').replace('-selected', '').replace('.png', '-selected.png'));
		$(inventory.map_pan).find('img').attr('src', $(inventory.map_pan).find('img').attr('src').replace('-selected', ''));
		if(!inventory.tmp_location) inventory.tmp_location = new google.maps.Marker({
			map: inventory.map.map,
			draggable: false,
			position: e.latLng
		});
		else {
			inventory.tmp_location.setVisible(true);
			inventory.tmp_location.setPosition(e.latLng);
		}
		inventory.listeners = [];
		inventory.listeners.push(google.maps.event.addDomListener(inventory.map.map, 'mousemove', inventory.moveHandler, true));
		inventory.listeners.push(google.maps.event.addDomListener(inventory.map.map, 'click', inventory.clickHandler, true));
		inventory.listeners.push(google.maps.event.addDomListener(inventory.tmp_location, 'mousemove', inventory.moveHandler, true));
		inventory.listeners.push(google.maps.event.addDomListener(inventory.tmp_location, 'click', inventory.clickHandler, true));
		for(var i in inventory.map.mapOverlays.overlays) {
			var overlay = inventory.map.mapOverlays.overlays[i];
			overlay.overlay.setOptions({
				clickable: false
			});
		}
	};

	/**
	 * 
	 * @param e
	 */
	inventory.clickHandler = function (e) {
		inventory.placeMarker(e.latLng);
		inventory.activatePan();
	};

	/**
	 * 
	 * @param e
	 */
	inventory.moveHandler = function (e) {
		inventory.tmp_location.setPosition(e.latLng);
	};

	/**
	 * 
	 * @param position
	 */
	inventory.placeMarker = function (position) {
		var gOverlay = null;
		for(var o in inventory.map.mapOverlays.overlays) {
			gOverlay = inventory.map.mapOverlays.overlays[o];
			break;
		}
		if(gOverlay != null) {
			position = gOverlay.closestPoint(position);
		}
		if(!inventory.location) {
			inventory.location = new google.maps.Marker({
				map: inventory.map.map,
				draggable: inventory.locationform.is('form'),
				animation: google.maps.Animation.DROP,
				position: position
			});
			var geocoder = new google.maps.Geocoder();
			google.maps.event.addListener(inventory.location, 'position_changed', function () {
				var pos = inventory.location.getPosition();
				inventory.locationform.find('input[name="lat"]').val(pos.lat());
				inventory.locationform.find('input[name="lng"]').val(pos.lng());
			});
			google.maps.event.addListener(inventory.location, 'dragstart', function () {
				inventory.map.map.setOptions({
					draggable: false
				});
			});
			google.maps.event.addListener(inventory.location, 'drag', function (event) {
				if(gOverlay == null) {
					for(var o in inventory.map.mapOverlays.overlays) {
						gOverlay = inventory.map.mapOverlays.overlays[o];
						break;
					}
				}
				if(gOverlay != null) {
					var pos = gOverlay.closestPoint(inventory.location.getPosition());
					inventory.location.setPosition(pos);
				}
			});
			google.maps.event.addListener(inventory.location, 'dragend', function () {
				var pos = inventory.location.getPosition();
				geocoder.geocode({
					'latLng': pos,
					language: 'de'
				}, function (results, status) {
					if(status == google.maps.GeocoderStatus.OK) {
						var address = {};
						jQuery.each(results, function (index, result) {

							if(result.types == 'postal_code') {
								var length = result.address_components.length;
								address.locality = result.address_components[1].long_name;
								address.zip = result.address_components[0].long_name;
								address.canton = result.address_components[length - 2].short_name;
								address.country = result.address_components[length - 1].long_name;
							}
							if(result.types == 'locality,political') {
								address.township = result.address_components[0].long_name;
							}
						});
						inventory.locationform.find('input[name="locality"]').val(address.locality);
						inventory.locationform.find('input[name="zip"]').val(address.zip);
						inventory.locationform.find('input[name="canton"]').val(address.canton);
						inventory.locationform.find('input[name="country"]').val(address.country);
						inventory.locationform.find('input[name="township"]').val(address.township);

						console.log(address);
					} else {
						console.error("Geocoder failed due to: " + status);
					}
				});
				inventory.map.map.setOptions({
					draggable: true
				});
			});
		}
		inventory.location.setPosition(position);
	};

	$(document).ready(inventory.init);
})(jQuery);