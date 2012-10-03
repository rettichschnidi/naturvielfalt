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
		var filter = $(this).val()
		observation.aclFilter(tableId, filter);
	});
	
	/**
	 * Filter the observations list by artgroup
	 */
	observation.artgroupFilter = function(tableId, filter) {
		var url = $('#' + tableId)[0].p.url;
		// if there's already a query string, append the filter
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
	}
	
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
		url = url.substring(0, url.length-1);
		
		// pass search, order and limit parameters
		var gridPrefs = $table[0].p;
		url += '&query=' + gridPrefs.query + '&qtype=' + gridPrefs.qtype;
		url += '&sortname=' + gridPrefs.sortname + '&sortorder=' + gridPrefs.sortorder;
		url += '&rp=18446744073709551615';  // max limit for sql query, select all (2^64-1) rows
		
		// load export url in a hidden iframe to get a download prompt
		var $status = $flexidiv.find('.pPageStat');
		var oldStatus = $status.text();
		$status.text(gridPrefs.procmsg);
		$('<iframe />').attr('src', url).hide().appendTo('body').load(function() {
			$status.text(oldStatus);
		});
	}
	
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
				$table.flexReload();
				observation.showDeleteResponse(json);
				if (redirect) {
					$.safetynet.suppressed(true);
					window.location.href = '/observation/show';
				}
			});
		}
	}
	
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
		
		$.getJSON(
			observationmap.options.geometriesfetchurl,
			{
				observation_ids: observation_ids
			},
			function(data) {
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
				if (!mapcontains)
					observationmap.googlemap.fitBounds(bounds);
				
				return true;
			}
		);
	}
	
	/**
	 * Display the batch div, holding the select-toggle, delete and export buttons
	 */
	observation.displayBatchArea = function(tableId) {
		$flexiDiv = $('#' + tableId).closest('div.flexigrid');
		
		if ($flexiDiv.find('.batch-div').length == 0) {
			$batchDiv = $('<div class="batch-div" />');
			$checkbox = $('<input type="checkbox" />');
			$btnDeleteSelected = $('<input type="button" class="btnDeleteSelected" disabled="true" value="' + Drupal.t('Delete') + '" />');
			$btnExportSelected = $('<input type="button" class="btnExportSelected" value="' + Drupal.t('Export all') + '" />');
			$batchDiv.append($checkbox, $btnDeleteSelected, $btnExportSelected);
			$flexiDiv.find('.bDiv').after($batchDiv);
			
			$checkbox.click(function(event) {
				observation.toggleSelectedRows(event, this.checked);
			});
			$btnDeleteSelected.click(function() {
				observation.deleteSelectedRows(tableId);
			});
			$btnExportSelected.click(function() {
				observation.exportSelectedRows(tableId);
			});
		}
		else {
			$flexiDiv.find('.btnDeleteSelected').attr('disabled', true);
			$flexiDiv.find('.btnExportSelected').val(Drupal.t('Export all'));
		}
	};
	
	/**
	 * Executed before table is populated
	 * ! must return the data !
	 * 
	 * @param JSON object data
	 */
	observation.tablePreProcess = function(data) {
		var observation_ids = [];
		$(data.rows).each(function(idx, obs) {
			observation_ids[idx] = obs.id;
		});
		observation.syncMapWithTable(observation_ids);
		
		// FIXME table id 'observations' should not be hard-coded
		data = gallery_addon.preProcess('observations', 'gallery_image', data);
		
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
			$flexiDiv.find('.btnExportSelected').val(Drupal.t('Export all'));
		}
		else {
			$flexiDiv.find('.btnDeleteSelected').removeAttr('disabled');
			$flexiDiv.find('.btnExportSelected').val(Drupal.t('Export selected'));
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
					this.overlaysArray[id].deselect(this.pinColor.red);
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

				// set z-index to mark as selected by multiple select
				marker.select(this.pinColor.blue, 3);
				
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
	 * Override default Google Maps Marker select() method to define color and
	 * z-index
	 * 
	 * @param string color
	 * @param int zindex
	 */
	google.maps.Marker.prototype.select = function(color, zindex) {
		if (color != undefined)
			this.setIcon(getMarkerImage(color));
		
		if (zindex == undefined)
			zindex = 2;
		this.setZIndex(zindex);
	}

	/**
	 * Override default Google Maps Marker deselect() method to define color and
	 * z-index
	 * 
	 * @param string color
	 * @param int zindex
	 */
	google.maps.Marker.prototype.deselect = function(color, zindex) {
		if (color != undefined)
			this.setIcon(getMarkerImage(color));
		
		if (zindex == undefined)
			zindex = 1;
		this.setZIndex(zindex);
	}

	/**
	 * Get a custom colored Google marker
	 * 
	 * @param string color
	 */
	getMarkerImage = function(color) {
		if (color == undefined)
			return
		
		var pinImage = new google.maps.MarkerImage('http://chart.apis.google.com/chart?chst=d_map_pin_letter&chld=%E2%80%A2|' + color,
		        new google.maps.Size(21, 34),
		        new google.maps.Point(0,0),
		        new google.maps.Point(10, 34));
		
		return pinImage;
	};
	
	
 
	/*
	 * Functions for the observation edit page
	 */
	
	/**
	 * Change the determination methods according to the selected art group
	 * TODO implementation
	 */
	observation.changeArtGroup = function(id) {
		console.log('implement me!');
	}

	/**
	 * Show the message returned from the save request
	 * 
	 * @param responseText
	 * @param statusText
	 * @param xhr
	 * @param $form
	 */
	observation.showSaveResponse = function(responseText, statusText, xhr, $form)  { 
		if(responseText != null && responseText.success == true){
			observation.setMessage(Drupal.t('Observation saved successfully'), 'status', 5000);
			
			// on add, clear form 
			if (!responseText.update) {
				$('#observation_form').trigger('reset');
				$('#species_autocomplete').html('');
				observation.hideAttributes();
				observation.hideDetMethods();
				observationmap.newOverlay.overlay.setMap(null);
				observationmap.drawingManager.setDrawingMode(google.maps.drawing.OverlayType.MARKER);
			}
			
			// reload table
			$('#recent_observations').flexReload();
		} else if (responseText != null) {
			observation.setMessage('&bull;&nbsp;' + responseText.message.join("<br>&bull;&nbsp;"), 'error', 5000);
		} else {
			observation.setMessage('&bull;&nbsp;' + Drupal.t('Saving failed due to unknown error.'), 'error', 5000);
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
		$('body,html').animate({
			scrollTop: observation.message.offset().top
		});
	};
	
	observation.onFormSuccess = function(responseText, statusText, xhr, $form) {
		if (responseText.success == true)
			resetUploadSlots();
		
		observation.showSaveResponse(responseText, statusText, xhr, $form);
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
				$.extend(options, {
					uploadProgress: function(event, position, total, percentComplete) {
						observation.updateProgressbar(percentComplete);
					},
					complete: observation.hideProgressbar
				});
				return false;
			}
		});
		return true;
	};

	/**
	 * Hide all determination methods
	 */
	observation.hideDetMethods = function() {
		$("#determination_method_id option").each(function () {
            $(this).css('display','none');
          });
		observation.showDetMethod('0');
	};
	
	/**
	 * Show a determination method by id
	 * 
	 * @param int id
	 */
	observation.showDetMethod = function(id) {
		$('#artgroup_detmethod_value_'+id).css('display','block');
	};
	
	/**
	 * Hide all attributes
	 */
	observation.hideAttributes = function() {
		$("tr[id^='attributes_tr_']").each(function () {
            $(this).css('display','none');
          });
	};
	
	/**
	 * Show an attribute by id
	 * 
	 * @param int id
	 */
	observation.showAttribute = function(id) {
		$('#attributes_tr_'+id).css('display','table-row');
	};
	
	/**
	 * Reset the input's specially the organism related
	 */
	observation.resetOrganism = function() {
		$('#organismn_id').val('');
		$('#species_autocomplete').html('');
		$('#observation_found_as_latin').val('false');
		$('#observation_found_as_lang').val('false');
		observation.hideAttributes();
		observation.hideDetMethods();
	};
	
	/**
	 * Reset the autocomplete field, and the organism related
	 */
	observation.resetOrganismAutomcomplete = function() {
		$('#organismn_autocomplete').val('');
		observation.resetOrganism();
	};
	
	/**
	 * Add another upload slot for files
	 * 
	 * @param form //unused
	 */
	addUploadSlot = function(form) {
		$('#picture_upload__0').clone().appendTo($('#picture_upload__0').parent()).css('display','block').css('height','auto');
		var slot_id = 0;
		var empty_slots = 0;
		$('div[id^="picture_upload__"]').each(function() {
			var $div = $(this);
			if ($div.find('input:file').first().val() == '') {
				// we need 2 empty elements because of picture_upload__0 field
				if(empty_slots >= 2)
					$div.remove();
				else
					empty_slots++;
			}
			if ($div)
				$div.attr('id', 'picture_upload__'+(slot_id++));
		});
	};
	
	/**
	 * Reset upload slots
	 */
	resetUploadSlots = function() {
		var element_id = 0;
		var slots = 0;
		$('div[id^="picture_upload__"]').each(function() {
			if(slots >= 2)
				$(this).remove();
			else
				slots++;
		});
	};
	
	/**
	 * Check the type of the selected file
	 * 
	 * @param form
	 */
	checkMimeType_metaData = function(form) {
		mimeType = form.files["0"].type;
		re = new RegExp('image/', 'ig');
		re2 = new RegExp('video/mp4', 'ig');
		re3 = new RegExp('audio/mpeg', 'ig');
		if(mimeType.match(re) || mimeType.match(re2) || mimeType.match(re3)) {
			return 'media';
		}else{
			return 'file';
		}
	};

	/**
	 * Opens a dialog to add meta data to a fileupload
	 * 
	 * @param div
	 */
	observation.galleryMetaDataDialog = function(div) {
		file_name = div.children('input#file_input.form-file').val();
		div_id = div.attr("id");
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
	 * Bind the form for observations to the ajax form
	 */
	var ajaxurl = Drupal.settings.basePath + 'observation/save';
	if ($('#observation_id').val() != '') {
		ajaxurl = Drupal.settings.basePath + 'observation/'+ $('#observation_id').val() +'/save';
	}
	
	// bind form using 'ajaxForm'
	if($('#observation_form').length > 0) $('#observation_form').ajaxForm({
		beforeSubmit: observation.conditionalProgressbar,
		success:      observation.onFormSuccess,
		url:          ajaxurl,
		type:         'post',
		dataType:     'json',
	});
	
	// if files are to be uploaded, show progress bar
	
	// automatically add upload slot
	$('.form-file').live('change', addUploadSlot);
	
	/**
	 * Bind the date picker to the date field
	 */
	$('#date').datepicker({
		dateFormat: 'dd.mm.yy',
		duration: 0,
		showButtonPanel: true,
		onSelect: function (date, inst) {
			observation.last_date = date;
		}
	}).width(80);
	if(observation.last_date) $('#date').val(observation.last_date);
	
	if($('#organismn_autocomplete').val() == '') $('#organismn_autocomplete').focus();
});



