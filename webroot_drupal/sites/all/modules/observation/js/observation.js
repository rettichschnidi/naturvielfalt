jQuery(document).ready(function() {
	
	$ = jQuery;
	observation = {};
	observation.message = $('#message');
	
	/*
	 * Functions for the observations list page
	 */
	
	/**
	 * Filter the observations list by access right
	 */
	observation.aclFilter = function(tableId, filter) {
		var url = $('#' + tableId)[0].p.url;
		// replace string after last slash and preserve query string
		url = url.replace(/(.+\/)[^?]+(.*)/, '$1' + filter + '$2');
		$('#' + tableId).flexOptions({
			url: url
		}).flexReload();
	}
	$('.acl_filter').change(function(event) {
		var tableId = $(event.target).closest('div.flexigrid').find('div.bDiv table').first().attr('id');
		var filter = $(this).val();
		observation.aclFilter(tableId, filter);
	});
	
	/**
	 * Filter the observations list by artgroup
	 */
	observation.artgroupFilter = function(tableId, filter) {
		var url = $('#' + tableId)[0].p.url;
		// if there's already a query string, append the filter
		url = url.replace(/[&?]oaid=\d+/, '');
		url += (url.indexOf('?') == -1) ? '?' : '&';
		url += 'oaid=' + filter;
		$('#' + tableId).flexOptions({
			url: url
		}).flexReload();
	}
	$('.artgroup_filter').change(function(event) {
		var tableId = $(event.target).closest('div.flexigrid').find('div.bDiv table').first().attr('id');
		var filter = $(this).val();
		observation.artgroupFilter(tableId, filter);
	});
	
	
	/**
	 * Select or deselect all rows
	 * 
	 * @param Event object event
	 * @param boolean status
	 */
	observation.toggleSelectedRows = function(event, status) {
		$flexiDiv = $(event.target).closest('div.flexigrid');
		$flexiDiv.find('input.gridSelect').each(function() {
			this.checked = status;
			observationmap.selectMultipleObservation(event, parseInt($(this).val()), !status);
		});
	};
	
	/**
	 * Export the selected table rows.
	 * If none are selected, all rows will be exported.
	 * 
	 * @param string tableId
	 */
	observation.exportSelectedRows = function(tableId) {
		url = Drupal.settings.basePath + 'observation/export?';
		var $table = $('#' + tableId);
		if ($table.length < 1)
			return false;
		var $flexidiv = $table.closest('div.flexigrid');
		
		// collect shown column header ids, ordered
		$flexidiv.find('div.nDiv').find('input.togCol').each(function() {
			if (this.checked)
				url += 'col[]=' + $(this).val() + '&';
		});
		
		// collect selected observations
		$flexidiv.find('div.bDiv input.gridSelect').each(function() {
			if (this.checked)
				url += 'oid[]=' + $(this).val() + '&';
		});
		
		// pass search, order and limit parameters
		var gridPrefs = $table[0].p;
		for(var i = 0; i < gridPrefs.query.length; i++) {
			url += 'query[]=' + gridPrefs.query[i] + '&';
		}
		for(var i = 0; i < gridPrefs.qtype.length; i++) {
			url += 'qtype[]=' + gridPrefs.qtype[i] + '&';
		}
		url = url.substring(0, url.length-1);
		
		url += '&sortname=' + gridPrefs.sortname + '&sortorder=' + gridPrefs.sortorder;
		url += '&rp=18446744073709551615';  // max limit for sql query, select all (2^64-1) rows
		
		// load export url in a hidden iframe to get a download prompt
		var $status = $flexidiv.find('.pPageStat');
		var oldStatus = $status.text();
		$status.text(gridPrefs.procmsg);
		$('<iframe />').attr('src', url).hide().appendTo('body').load(function() {
			$status.text(oldStatus);
		});
	};
	
	/**
	 * Verify the selected table rows.
	 * If none are selected, all rows will be verified.
	 * 
	 * @param string tableId
	 */
	observation.verificationSelectedRows = function(tableId) {
		var destination = '/vote/observation/';
		var table = $('#' + tableId);
		if (table.length < 1)
			return false;
		
		table.find('input.gridSelect').each(function() {
			if (this.checked) {
				var observation_id = $(this).val();
				destination += observation_id + ',';
			}
		});
		
		if (destination.indexOf(',', destination.length - 1 != -1)) {
			destination = destination.substring(0, destination.length - 1);
		}
		
		window.location.href = destination;
	};
	
	/**
	 * Adds selected observations to an inventory. Overrides observations which already belong to an inventory.
	 * If none are selected, all rows will be verified.
	 * 
	 * @param string tableId
	 */
	observation.addSelectedRowsToInventory = function(tableId) {
		var table = $('#' + tableId);
		if (table.length < 1)
			return false;
		var observations = [];
		table.find('input.gridSelect').each(function() {
			if (this.checked) {
				observations.push($(this).val());
			}
		});
		observation.showLoading();
		$.ajax({
			type: "POST",
			url: Drupal.settings.basePath + "observation/addtoinventorydata",
			success: function(result){
				var areas = result.areas;
				var areasAsOptions = result.areas_as_options;
				var inventories = null;
				
				observation.hideLoading();
				dialog = $('<div id="add-to-inventory-wrapper" title="' + Drupal.t('Add to Area/Inventory') + '" />');
				
				//area title and select
				areaTitle = $('<label>' + Drupal.t('Area') + '</label>');
				areaSelect = $('<select id="area-select" class="form-select" \>');
				var i = 0;
				$.each(areasAsOptions, function(key, value) {
					if(i++ == 0) {
						inventories = areas[key].inventories;
					}
					areaSelect.append($("<option></option>")
				     .attr("value", key).text(value));
				});
				
				//inventory title and select
				inventoryTitle = $('<label>' + Drupal.t('Inventory') + '</label>');
				inventorySelect = $('<select id="inventory-select" class="form-select"\>');
				if(inventories) {
					$.each(inventories, function(key, value) {
						inventorySelect.append($("<option></option>")
					     .attr("value", key).text(value));
					});
				}
				
				//on change to switch the inventories of the newly selected inventory
				areaSelect.off().on('change', function() {
					inventories = areas[$(this).val()].inventories;
					if(inventories) {
						inventorySelect.empty();
						$.each(inventories, function(key, value) {
							inventorySelect.append($("<option></option>")
						     .attr("value", key).text(value));
						});
					}
				});
				
				//saveButton
				saveButton = $('<input class="form-submit" type="button" value="' + Drupal.t('Save') + '" />');
				
				//saveButton onclick
				saveButton.off().on('click', function() {
					var really = confirm(Drupal.t('All selected observations will be assigned to this area/inventory. Continue?'));
					if(really) {
						var inventoryId = inventorySelect.val();
						observation.showLoading();
						//make ajax call to save the observations to the inventory
						$.ajax({
							type: "POST",
							url: Drupal.settings.basePath + "observation/addtoinventory/" + inventoryId,
							success: function(result){
								observation.hideLoading();
								//remove dialog, show response, and reload table
								dialog.remove();
								var translations = {};
								translations['@successfull'] = result.successfull;
								translations['@total'] = result.total;
								if(result.success) {
									observation.setMessage(Drupal.t('@successfull of @total observations were added to the new area/inventory.', translations), 'status', 5000);
									table.flexReload();
								}	
								else {
									observation.setMessage(Drupal.t('@successfull of @total observations were added to the new area/inventory.', translations), 'warning', 5000);
								} 
								
							},
							error: function(result) {
								observation.hideLoading();
								dialog.remove();
								observation.setMessage(Drupal.t('Could not add observations to the area/inventory.'), 'error', 5000);
							},
							data: {
								observations: observations
							}
						});
					}
				});
				
				//open dialog with selects
				dialog.append(areaTitle, areaSelect, inventoryTitle, inventorySelect, '</br></br>', saveButton);
				dialog.dialog({
					modal: true,
					resizable: false,
					closeOnEscape: false,
					closeText: '',
					close: function (event, ui) {
						$(this).remove();
					},
					width: 'auto'
				});
			},
			error: function(result) {
				observation.hideLoading();
			},
			data: {
				observations: observations
			}
		});
		
	};
	
	/**
	 * Delete the selected rows
	 * 
	 * @param string tableId
	 */
	observation.deleteSelectedRows = function(tableId) {
		var $table = $('#' + tableId);
		if ($table.length < 1)
			return false;

		var pathname = window.location.pathname;
		var observation_edit_id = pathname.match(/observation\/(\d+)\/edit/);
		if (observation_edit_id != null) {
			if (observation_edit_id.length >= 2)
				observation_edit_id = observation_edit_id[1];
		}
		else
			observation_edit_id = 0;
		
		var redirect = false;
		var transportData = '';
		$table.find('input.gridSelect').each(function() {
			if (this.checked) {
				var observation_id = $(this).val();
				transportData += observation_id + ',';
				if (observation_id == observation_edit_id)
					redirect = true;
			}
		});
		if (transportData.length == '') {
			alert(Drupal.t('No records selected'));
			return false;
		}
		transportData = '{' + transportData.substring(0,transportData.length-1) + '}';
		
		var really = confirm(Drupal.t('Do you really want to delete the selected records?'));
		if (really){
			var data = {
				observations: transportData
			};
			
			var ajaxurl = Drupal.settings.basePath + 'observation/delete';
			$.getJSON(ajaxurl, data, function(json){
				if(json.count > 0) $table.flexReload();
				observation.showDeleteResponse(json);
				if (redirect) {
					$.safetynet.suppressed(true);
					window.location.href = '/observation/show';
				}
			});
		}
	};
	
	/**
	 * Show the message returned from the deletion request
	 */
	observation.showDeleteResponse = function(responseText, statusText, xhr, $form)  { 
		if(responseText != null && responseText.success == true){
			observation.setMessage(responseText.message, responseText.type, 5000);
		} else if (responseText != null) {
			observation.setMessage('&bull;&nbsp;' + responseText.message.join("<br>&bull;&nbsp;"), 'error', 5000);
		} else {
			observation.setMessage('&bull;&nbsp;' + Drupal.t('Deletion failed due to unknown error.'), 'error', 5000);
		}
	};
	
	/**
	* Show a top message banner
	*
	* @param string message
	* @param string type
	* @param int time
	*/
	observation.setMessage = function(message, type, time) {
		if(observation.messageTimer) window.clearTimeout(observation.messageTimer);
		observation.message.children('.messages').html(message).attr('class', 'messages').addClass(type);
		observation.message.stop().css('height', 'auto').slideDown('fast');
		if(time) observation.messageTimer = window.setTimeout(function () {
			observation.message.slideUp('fast');
		}, time);
	
		// scroll to message
		$('html, body').animate({ scrollTop: 0 });
	};
	
	/**
	 * Update the map to show only overlays of the given data.
	 * 
	 * @param array(int) observation_ids
	 */
	observation.syncMapWithTable = function(observation_ids) {
		if (observationmap == undefined)
			return false;
		
		observationmap.clearOverlays();
		
		if (observation_ids.length < 1)
			return true;
		
		$.ajax({
			type: 'POST',
			url: observationmap.options.geometriesfetchurl,
			data: {
				observation_ids: observation_ids
			},
			success: function (data) {
				observationmap.loadGeometriesAndOverlaysFromJson(data);

				var mapcontains = true;
				var bounds = new google.maps.LatLngBounds();
				for (var i = 0; i < observation_ids.length; i++) {
					var id = observation_ids[i];
					
					if (!(id in observationmap.overlaysArray))
						continue;
					
					var marker = observationmap.overlaysArray[id];
					
					var position = marker.getPosition();
					bounds.extend(position);
					// don't touch mapcontains once it changed to false
					if (mapcontains)
						mapcontains = observationmap.googlemap.getBounds().contains(position);
				}
				if (!mapcontains) {
					observationmap.googlemap.fitBounds(bounds);
				}
				if(observationmap.googlemap.getZoom() < 6) {
					observationmap.googlemap.fitBounds(bounds);
					observationmap.googlemap.setZoom(6);
				}
				
				return true;
			}
		});
	};
	
	/**
	 * Display the batch div, holding the select-toggle, delete and export buttons
	 */
	observation.displayBatchArea = function(tableId) {
		$flexiDiv = $('#' + tableId).closest('div.flexigrid');
		
		if ($flexiDiv.find('.batch-div').length == 0) {
			$checkbox = $('<input type="checkbox" />');
			$btnDeleteSelected = $('<input type="button" class="btnDeleteSelected" disabled="true" value="' + Drupal.t('Delete') + '" />');
			$btnExportSelected = $('<input type="button" class="btnExportSelected" value="' + Drupal.t('Export all') + '" />');
			$btnAddToInventory = $('<input type="button" class="btnAddToInventory" disabled="true" value="' + Drupal.t('Add to Area/Inventory') + '" />');
			if(voteModuleExits) $btnVerificationSelected = $('<input type="button" class="btnVerificationSelected" value="' + Drupal.t('Verify all') + '" />');
			if(voteModuleExits) {
				$batchDiv = $('<div class="batch-div" />')
					.append($checkbox, $btnDeleteSelected, $btnExportSelected, $btnAddToInventory, $btnVerificationSelected)
					.insertBefore($flexiDiv.find('.sDiv'));
			} else {
				$batchDiv = $('<div class="batch-div" />')
				.append($checkbox, $btnDeleteSelected, $btnExportSelected, $btnAddToInventory)
				.insertBefore($flexiDiv.find('.sDiv'));
			}
			$checkbox.click(function(event) {
				observation.toggleSelectedRows(event, this.checked);
			});
			$btnDeleteSelected.click(function() {
				observation.deleteSelectedRows(tableId);
			});
			$btnExportSelected.click(function() {
				observation.exportSelectedRows(tableId);
			});
			$btnAddToInventory.click(function() {
				observation.addSelectedRowsToInventory(tableId);
			});
			if(voteModuleExits) {
				$btnVerificationSelected.click(function() {
					observation.verificationSelectedRows(tableId);
				});
			}
		}
		else {
			$flexiDiv.find('.btnDeleteSelected').attr('disabled', true);
			$flexiDiv.find('.btnAddToInventory').attr('disabled', true);
			$flexiDiv.find('.btnExportSelected').val(Drupal.t('Export all'));
			if(voteModuleExits) $flexiDiv.find('.btnVerificationSelected').val(Drupal.t('Verify all'));
		}
	};
	
	/**
	 * Executed before table is populated
	 * ! must return the data !
	 * 
	 * @param JSON object data
	 */
	observation.tablePreProcess = function(data, flexigridOptions) {
		var observation_ids = [];
		$(data.rows).each(function(idx, obs) {
			observation_ids[idx] = obs.id;
		});
		observation.syncMapWithTable(observation_ids);
		
		// FIXME table id 'observations' should not be hard-coded
		data = gallery_addon.preProcess('observations', 'gallery_image', data, flexigridOptions);
		
		return data;
	};
	
	/**
	 * Executed before table is populated
	 * ! must return the data !
	 * 
	 * @param JSON object data
	 */
	observation.tablePreProcessWithoutGallery = function(data, flexigridOptions) {
		var observation_ids = [];
		$(data.rows).each(function(idx, obs) {
			observation_ids[idx] = obs.id;
		});
		observation.syncMapWithTable(observation_ids);

		return data;
	};
	
	/**
	 * Executed after table is populated
	 * 
	 * @param jQuery object flexigrid
	 */
	observation.onTableSuccess = function(flexigrid) {
		var tableId = $(flexigrid.bDiv).find('table').first().attr('id');
		observation.displayBatchArea(tableId);
	};
	
	/**
	 * Select an observation on the map. A single selection is green (@see area.js).
	 * 
	 * Old selected observation is deselected, and the map changes only if the
	 * observation is out of the current map section.
	 * 
	 * This function does not touch observations selected by multiple select.
	 * 
	 * @param int id
	 */
	Area.prototype.selectObservation = function(id) {
		if (id == this.selectedId) return
		
		// check if marker id is available
		if (id in this.overlaysArray) {
			var marker = null;
			
			// deselect previously selected marker
			// if it is part of multiple select (z-index 3), change color to blue
			if (this.selectedId != null) {
				marker = this.overlaysArray[this.selectedId];
				var z = marker.getZIndex();
				if (z < 3)
					marker.deselect(this.pinColor.red);
				else
					marker.deselect(this.pinColor.blue, z);
			}
			
			// save reference for future use
			this.selectedId = id;
			
			// get the requested marker
			marker = this.overlaysArray[id];
			// and select it
			var z = marker.getZIndex();
			marker.select(this.pinColor.green);
			// set z-index if it was previously part of multiple select
			if (z >= 3)
				marker.setZIndex(4);

			// update map only if marker is out of view
			if (!this.googlemap.getBounds().contains(marker.getPosition())) {
				this.googlemap.fitBounds(marker.getBounds());
				this.googlemap.setZoom(this.googlemap.getZoom() - 4);
			}
		} else {
			console.error("Observation not available: " + id);
		}
	};
	
	/**
	 * De-/Select multiple observations on the map. Multiple selections are blue.
	 * 
	 * Also selects the corresponding checkbox in the table/gallery respectively and
	 * handles batch area button values/states depending on selected checkboxes.
	 * 
	 * The map is zoomed to show all selected observations, and it changes only if 
	 * one of the selected observations is out of the current map section.
	 * 
	 * This function does not touch the observation overlay changed by single select.
	 * 
	 * @param Event object event that triggered this function
	 * @param int id of the de-/selected observation
	 * @param boolean deselect whether to select or deselect, default: false
	 */
	Area.prototype.selectMultipleObservation = function(event, id, deselect) {
		// prevent bubbling to row click action (selectObservation())
		var e = event || window.event;
		if (e != undefined)	e.stopPropagation();
		
		if (deselect == undefined)
			deselect = false;
		
		$flexiDiv = $(event.target).closest('div.flexigrid');
		
		// select both checkboxes in table/gallery
		$flexiDiv.find('input.gridSelect[value="'+id+'"]').each(function() {
			$(this)[0].checked = !deselect;
		});
		
		// update batch area buttons
		if ($flexiDiv.find('input.gridSelect:checked').length == 0) {
			$flexiDiv.find('.btnDeleteSelected').attr('disabled', true);
			$flexiDiv.find('.btnAddToInventory').attr('disabled', true);
			$flexiDiv.find('.btnExportSelected').val(Drupal.t('Export all'));
			if(voteModuleExits) $flexiDiv.find('.btnVerificationSelected').val(Drupal.t('Verify all'));
		}
		else {
			$flexiDiv.find('.btnDeleteSelected').removeAttr('disabled');
			$flexiDiv.find('.btnAddToInventory').removeAttr('disabled');
			$flexiDiv.find('.btnExportSelected').val(Drupal.t('Export selected'));
			if(voteModuleExits) $flexiDiv.find('.btnVerificationSelected').val(Drupal.t('Verify selected'));
		}
		
		// check if there exists an overlay with the given id
		if (id in this.overlaysArray) {
			if (this.selectedIds == null) {
				this.selectedIds = [];
			}
			if (deselect == true) {
				// deselect
				var idx = $.inArray(id, $.unique(this.selectedIds))
				if (idx > -1) {
					this.selectedIds.splice(idx, 1);
					this.overlaysArray[id].inMultiSelection = false;
					this.overlaysArray[id].deselect();					
				}
			}
			else {
				// select
				this.selectedIds.push(id);
			}
			
			// loop through selected overlays, select them on the map, calculate the
			// bounds around them, and check if refitting map is needed
			var mapcontains = true;
			var bounds = new google.maps.LatLngBounds();
			for (var i = 0; i < this.selectedIds.length; i++) {
				var marker = this.overlaysArray[this.selectedIds[i]];

				marker.inMultiSelection = true;
				marker.select();
				
				var position = marker.getPosition();
				bounds.extend(position);
				// don't touch mapcontains once it changed to false
				if (mapcontains)
					mapcontains = this.googlemap.getBounds().contains(position);
			}
			if (!mapcontains)
				this.googlemap.fitBounds(bounds);
		}
	}
	  
	/**
	 * Show a loading indicator
	 */
	observation.showLoading = function() {
		if (!observation.loading) {
			observation.loading = $('<div><img src="' + Drupal.settings.basePath + 'sites/all/modules/commonstuff/images/loading.gif" /></div>').hide();
			$('body').append(observation.loading);
		}
		observation.loading.dialog({
			modal: true,
			draggable: false,
			resizable: false,
			closeOnEscape: false,
			closeText: '',
			dialogClass: 'loading'
		});
	};
		
	/**
	 * Hide the loading indicator
	 */
	observation.hideLoading = function() {
		if (!observation.loading) return;
		observation.loading.dialog('close');
	};
	
	/**
	 * Show a progress bar
	 */
	observation.showProgressbar = function() {
		if (!observation.progbar) {
			observation.progbar = $('<div id="progressbar" />').progressbar();
		}
		observation.progbar.progressbar('option', 'value', 0);
		observation.progbar.dialog({
			modal: true,
			draggable: false,
			resizable: false,
			closeOnEscape: false,
			closeText: '',
			dialogClass: 'progressbar'
		});
	};
	
	/**
	 * Update the progress bar
	 */
	observation.updateProgressbar = function(percent) {
		observation.progbar.progressbar('option', 'value', percent);
	};
	
	/**
	 * Hide the progress bar
	 */
	observation.hideProgressbar = function() {
		if (!observation.progbar) return;
		observation.progbar.dialog('close');
	};
	
	/**
	 * Check if files are to be uploaded, and if yes, show progress bar
	 */
	observation.conditionalProgressbar = function(data, form, options) {
		$(data).each(function() {
			if (this.name == 'files[]' && this.value != '') {
				observation.showProgressbar();
				if (options.fileAPI) {
					$.extend(options, {
						uploadProgress: function(event, position, total, percentComplete) {
							observation.updateProgressbar(percentComplete);
						},
					});
				}
				else {
					// just show full progress bar, since it's an animated gif it still
					// indicates an upload in progress
					observation.updateProgressbar(100);
				}
				$.extend(options, {
					complete: observation.hideProgressbar
				});
				return false;
			}
		});
		return true;
	};
	
	/**
	 * Opens a dialog to create an observation
	 * 
	 * @param div
	 */
	observation.createObservationDialog = function() {
		var area_id = document.getElementById('area_id').value;
		var inventory_id = document.getElementById('id').value;

		observation.showLoading();
		var data = {
			a_id: area_id,
			i_id: inventory_id,
		};
		$.getJSON(Drupal.settings.basePath + 'inventory/' +inventory_id + '/addobservation/popup', data, function (data) {
			if(data && data.form) {
				dialog = $('<div title="' + Drupal.t('Meta data for uploads') + '" />');
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
				var parform = dialog.find('input[name="uploadform"]').val();
				dialog.find('input[name="title"]').val($('#'+parform).find('input[id="meta_title"]').val());
				dialog.find('input[name="description"]').val($('#'+parform).find('input[id="meta_description"]').val());
				dialog.find('input[name="location"]').val($('#'+parform).find('input[id="meta_location"]').val());
				dialog.find('input[name="author"]').val($('#'+parform).find('input[id="meta_author"]').val());
				dialog.find('#edit-actions a').click(function (e) {
					e.preventDefault();
					$(this).closest('.ui-dialog-content').dialog('close');
				});
				dialog.find('form').submit(function (e) {
					var parform = $(this).find('input[name="uploadform"]').val();
					$('#'+parform).find('input[id="meta_title"]').val($(this).find('input[name="title"]').val());
					$('#'+parform).find('input[id="meta_description"]').val($(this).find('input[name="description"]').val());
					$('#'+parform).find('input[id="meta_location"]').val($(this).find('input[name="location"]').val());
					$('#'+parform).find('input[id="meta_author"]').val($(this).find('input[name="author"]').val());
					$(this).closest('.ui-dialog-content').dialog('close');
					return false;
				});
			}
			observation.hideLoading();
		});
	};
	
	/**
	 * Opens a dialog to add meta data to a fileupload
	 * 
	 * @param div
	 */
	observation.galleryMetaDataDialog = function(div) {
		div_id = div.attr("id");
		id = div_id.replace('picture_upload__','');
		file_name = document.getElementById('file_input' + id).value;
		observation.showLoading();
		if(file_name == ''){
			alert(Drupal.t('Please select a file'));
			observation.hideLoading();
			return;
		}
		var data = {
			fn: file_name,
			di: div_id,
		};
		$.getJSON(Drupal.settings.basePath + 'gallery/json/meta-form/', data, function (data) {
			if(data && data.form) {
				dialog = $('<div title="' + Drupal.t('Meta data for uploads') + '" />');
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
				var parform = dialog.find('input[name="uploadform"]').val();
				dialog.find('input[name="title"]').val($('#'+parform).find('input[id="meta_title"]').val());
				dialog.find('input[name="description"]').val($('#'+parform).find('input[id="meta_description"]').val());
				dialog.find('input[name="location"]').val($('#'+parform).find('input[id="meta_location"]').val());
				dialog.find('input[name="author"]').val($('#'+parform).find('input[id="meta_author"]').val());
				dialog.find('#edit-actions a').click(function (e) {
					e.preventDefault();
					$(this).closest('.ui-dialog-content').dialog('close');
				});
				dialog.find('form').submit(function (e) {
					var parform = $(this).find('input[name="uploadform"]').val();
					$('#'+parform).find('input[id="meta_title"]').val($(this).find('input[name="title"]').val());
					$('#'+parform).find('input[id="meta_description"]').val($(this).find('input[name="description"]').val());
					$('#'+parform).find('input[id="meta_location"]').val($(this).find('input[name="location"]').val());
					$('#'+parform).find('input[id="meta_author"]').val($(this).find('input[name="author"]').val());
					$(this).closest('.ui-dialog-content').dialog('close');
					return false;
				});
			}
			observation.hideLoading();
		});
	};
	
	/**
	 * Bind acl form using 'ajaxForm'
	 * TODO: return and display response message
	 */
	if ($('#c-acl-form').length > 0) $('#c-acl-form').ajaxForm({
		beforeSubmit: observation.showLoading,
		complete: observation.hideLoading
	});

});



