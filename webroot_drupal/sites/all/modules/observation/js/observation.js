jQuery(document).ready(function() {
	$ = jQuery;
 observation = {};
 observation.message = $('#message');
 
	jQuery( "#date" ).datepicker({
		dateFormat: 'dd.mm.yy',
		duration: 0,
		showButtonPanel: true,
		onSelect: function (date, inst) {
			observation.last_date = date;
		}
	}).width(80);
	if(observation.last_date) jQuery( "#date" ).val(observation.last_date);
	if($( "#organismn_autocomplete" ).val() == '') $( "#organismn_autocomplete" ).focus();
	
	function changeArtGroup(id){
		alert('not implemented');
	}

	observation.showSaveResponse = function(responseText, statusText, xhr, $form)  { 
		if(responseText != null && responseText.success == true){
			observation.setMessage(Drupal.t('Observation saved successfully'), 'status', 3000);
			
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
		
			// scroll to top of form
			$('body,html').animate({
				scrollTop: $('#observation_form').offset().top
			});
		} else if (responseText != null) {
			observation.setMessage('&bull;&nbsp;' + responseText.message.join("<br>&bull;&nbsp;"),'error', 5000);
		} else {
			observation.setMessage('&bull;&nbsp;' + Drupal.t('Saving failed due to unknown error.'),'error', 5000);
		}
	};
	
	
	  
	  /**
	   * Bind the form for observations to the ajax form
	   */
	  var ajaxurl = Drupal.settings.basePath + 'observation/save';
	  if($('#observation_id').val() != ''){
		  ajaxurl = Drupal.settings.basePath + 'observation/'+ $('#observation_id').val() +'/save';
	  }

	  	var options = {
	        success:   observation.showSaveResponse,
	        url:       ajaxurl,
	        type:      'post',
	        dataType:  'json',
	    };
	    // bind form using 'ajaxForm'
		if($('#observation_form').length > 0) $('#observation_form').ajaxForm(options);
		
		/**
		 * Show a top message banner
		 * @param message
		 * @param type
		 * @param time
		 */
		observation.setMessage = function (message, type, time) {
			if(observation.messageTimer) window.clearTimeout(observation.messageTimer);
			observation.message.children('.messages').html(message).attr('class', 'messages').addClass(type);
			observation.message.stop().css('height', 'auto').slideDown('fast');
			if(time) observation.messageTimer = window.setTimeout(function () {
				observation.message.slideUp('fast');
			}, time);
		};
	  
		/**
		 * Show a loading indicator
		 */
		observation.showLoading = function () {
			if(!observation.loading) {
				observation.loading = $('<div><img src="' + Drupal.settings.basePath + 'sites/all/modules/commonstuff/images/loading.gif" /></div>').hide();
				$('body').append(observation.loading);
			}
			observation.loading.dialog({
				modal: true,
				draggable: false,
				resizable: false,
				closeOnEscape: false,
				closeText: '',
				dialogClass: 'loading',
				width: 42,
				height: 42,
				minWidth: 42,
				minHeight: 42
			});
		};
		
		/**
		 * Hide the loading indicator
		 */
		observation.hideLoading = function () {
			if(!observation.loading) return;
			observation.loading.dialog('close');
		};

		/**
		 * Hide all determination methods
		 */
		observation.hideDetMethods = function(){
			$("#determination_method_id option").each(function () {
                $(this).css('display','none');
              });
			observation.showDetMethod('0');
		};
		
		/**
		 * Show a determination method by id
		 * @param id
		 */
		observation.showDetMethod = function(id){
			$('#artgroup_detmethod_value_'+id).css('display','block');
		};
		
		/**
		 * Hide all attributes
		 */
		observation.hideAttributes = function(){
			$("tr[id^='attributes_tr_']").each(function () {
                $(this).css('display','none');
              });
		};
		
		/**
		 * Show a attribute by id
		 * @param id
		 */
		observation.showAttribute = function(id){
			$('#attributes_tr_'+id).css('display','table-row');
		};
		
		/**
		 * Reset the input's specially the organism related
		 */
		observation.resetOrganism = function(){
			$( "#organismn_id" ).val('');
			$( "#species_autocomplete" ).html('');
			$( "#observation_found_as_latin" ).val('false');
			$( "#observation_found_as_lang" ).val('false');
			observation.hideAttributes();
			observation.hideDetMethods();
		};
		
		/**
		 * Reset the autocomplete field, and the organism related
		 */
		observation.resetOrganismAutomcomplete = function(){
			$( "#organismn_autocomplete" ).val('');
			observation.resetOrganism();
		};
		
		/**
		 * Add another upload slot for files
		 * @param form //unused
		 */
		addUploadSlot = function(form){
			$('#picture_upload__0').clone().appendTo('#picture').css('display','block').css('height','auto');
			elemets_ids=0;
			$("div[id^='picture_upload__']").each(function () {
				$(this).attr("id", 'picture_upload__'+(elemets_ids++));
			});
		};
		
		/**
		 * Check the type of the selected file
		 * @param form
		 */
		checkMimeType_metaData = function(form){
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
		 * @param div
		 */
		observation.galleryMetaDataDialog = function (div) {
			file_name = div.children('input#file_input.form-file').val();
			div_id = div.attr("id");
			observation.showLoading();
			if(file_name == ''){
				alert(Drupal.t('Please select a file'));
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
		 * Reload the table.
		 */
		observation.reload = function() {
			jQuery("#observations").flexReload();
		}
		
		/**
		 * Select or deselect all rows
		 */
		observation.toggleSelectedRows = function(status){
			$('input.gridSelect').each(function() {
				$(this).attr('checked', status);
				observationmap.selectMultipleObservation(null, parseInt($(this).val()), !status);
			});
		}
		
		/**
		 * Export the selected table rows.
		 * If none are selected, all rows will be exported.
		 * 
		 * @param table id
		 */
		observation.exportSelectedRows = function(table) {
			url = Drupal.settings.basePath + 'observation/export?';
			
			// collect shown column header ids, ordered
			var $flexidiv = $(table).closest('div.flexigrid');
			$flexidiv.find('div.nDiv:first').find('input.togCol').each(function() {
				if ($(this).attr('checked'))
					url += 'columns[]=' + $(this).val() + '&';
			});
			
			// collect selected observations
			$flexidiv.find('input.gridSelect').each(function() {
				if ($(this).attr('checked'))
					url += 'observations[]=' + $(this).val() + '&';
			});
			url = url.substring(0, url.length-1);
			
			// pass search, order and limit parameters
			var gridPrefs = $(table)[0].p;
			url += '&query=' + gridPrefs.query + '&qtype=' + gridPrefs.qtype;
			url += '&sortname=' + gridPrefs.sortname + '&sortorder=' + gridPrefs.sortorder;
			url += '&rp=18446744073709551615';  // max limit for sql query, select all (2^64-1) rows
			
			// load export url in a hidden iframe to get a download prompt
			var $status = $flexidiv.find('.pPageStat:first');
			var oldStatus = $status.text();
			$status.text(gridPrefs.procmsg);
			$('<iframe />').attr('src', url).hide().appendTo('body').load(function() {
				$status.text(oldStatus);
			});
		}
		
		/**
		 * Show the message returned from the deletion request
		 */
		observation.showDeleteResponse = function(responseText, statusText, xhr, $form)  { 
			if(responseText != null && responseText.success == true){
				observation.setMessage(responseText.message, responseText.type, 3000);
			} else if (responseText != null) {
				observation.setMessage('&bull;&nbsp;' + responseText.message.join("<br>&bull;&nbsp;"), 'error', 5000);
			} else {
				observation.setMessage('&bull;&nbsp;' + Drupal.t('Deletion failed due to unknown error.'), 'error', 5000);
			}
		};
		
		/**
		 * Delete the selected rows
		 */
		observation.deleteSelectedRows = function(){
			var transportData = "";
			$("input.gridSelect").each( function() {
				if ($(this).attr('checked') == true){
					transportData += $(this).val() + ",";
				}
			});
			transportData = "{" + transportData.substring(0,transportData.length-1) + "}";
			
			if (transportData.length == 2){
				alert(Drupal.t('No records selected'));
				return;
			}
				
			var really = confirm(Drupal.t('Do you really want to delete the selected records?'));
			if (really){
									
				var data = {
					observations: transportData
				};
				
				var ajaxurl = Drupal.settings.basePath + 'observation/delete';
				$.getJSON(ajaxurl, data, function(json){
					jQuery("#observations").flexReload();
					observation.showDeleteResponse(json);
				});
			}
		}
		
		/**
		 * Update the map to show only overlays of the given data.
		 * 
		 * @param data
		 */
		observation.updateMap = function(data) {
			if (observationmap == undefined)
				return data;
			
			observationmap.clearOverlays();
			
			var mapcontains = true;
			var bounds = new google.maps.LatLngBounds();
			for (var i in data.rows) {
				var marker = observationmap.overlaysArray[data.rows[i].id];
				
				marker.setMap(observationmap.googlemap);
				
				var position = marker.getPosition();
				bounds.extend(position);
				// don't touch mapcontains once it changed to false
				if (mapcontains)
					mapcontains = observationmap.googlemap.getBounds().contains(position);
			}
			if (!mapcontains)
				observationmap.googlemap.fitBounds(bounds);
			
			return data;
		}
		
		/**
		 * Display the batch div, holding the select-toggle, delete and export buttons
		 */
		observation.displayBatchArea = function(){
			if ($('#batch-div').length == 0) {
				$('.bDiv').after('<div id="batch-div" style="display: block;"> ' +
					'	<input type="checkbox" onClick="javascript:observation.toggleSelectedRows(this.checked)"> ' + 
					'	<input type="button" id="btnDeleteSelected" disabled="true" value="' + Drupal.t('Delete') + '"> ' +
					'   <input type="button" id="btnExportSelected" value="' + Drupal.t('Export all') + '"> ' +
					'</div>');
				$('#btnDeleteSelected').click(function() {
					observation.deleteSelectedRows();
				});
				$('#btnExportSelected').click(function() {
					observation.exportSelectedRows('#observations');
				});
			}
			else {
				$('#btnDeleteSelected').attr('disabled', true);
				$('#btnExportSelected').val(Drupal.t('Export all'));
			}
		};
		
		/**
		 * Select an observation on the map. A single selection is green (@see area.js).
		 * 
		 * Old selected observation is deselected, and the map changes only if the
		 * observation is out of the current map section.
		 * 
		 * This function does not touch observations selected by multiple select.
		 * 
		 * @param id
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
		 * @param Event event that triggered this function
		 * @param int id of the de-/selected observation
		 * @param boolean deselect whether to select or deselect, default: false
		 */
		Area.prototype.selectMultipleObservation = function(event, id, deselect) {
			// prevent bubbling to row click action (selectObservation())
			var e = event || window.event;
			if (e != undefined)	e.stopPropagation();
			
			if (deselect == undefined)
				deselect = false;
			
			// select both checkboxes in table/gallery
			$('input.gridSelect[value="'+id+'"]').each(function() {
				$(this)[0].checked = !deselect;
			});
			
			// update batch area buttons
			if ($('input.gridSelect:checked').length == 0) {
				$('#btnDeleteSelected').attr('disabled', true);
				$('#btnExportSelected').val(Drupal.t('Export all'));
			}
			else {
				$('#btnDeleteSelected').removeAttr('disabled');
				$('#btnExportSelected').val(Drupal.t('Export selected'));
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
		 * @param color
		 * @param zindex
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
		 * @param color
		 * @param zindex
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
		 * @param color
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
});



