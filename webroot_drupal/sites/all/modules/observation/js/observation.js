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
		
		observation.toggleSelectedRows = function(status){
			$("input.gridSelect").each( function() {
				$(this).attr("checked",status);
			});
		}
		
		observation.showDeleteResponse = function(responseText, statusText, xhr, $form)  { 
			if(responseText != null && responseText.success == true){
				observation.setMessage(responseText.message, responseText.type, 3000);
			} else if (responseText != null) {
				observation.setMessage('&bull;&nbsp;' + responseText.message.join("<br>&bull;&nbsp;"), 'error', 5000);
			} else {
				observation.setMessage('&bull;&nbsp;' + Drupal.t('Deletion failed due to unknown error.'), 'error', 5000);
			}
		};
		
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
					var orginalText = $('.pPageStat').html();
					jQuery("#observations").flexReload();
					observation.showDeleteResponse(json);
				});
			}
		}
		
		
		observation.displayBatchArea = function(){
			if ($('#batch-div').length == 0){
				$('.bDiv').after('<div id="batch-div" style="display: block;"> ' +
					'	<input type="checkbox" onClick="javascript:observation.toggleSelectedRows(this.checked)"> ' + 
					'	<input type="button" id="btnDeleteSelected" disabled="true" value="' + Drupal.t('Delete') + '" onClick="javascript:observation.deleteSelectedRows()"> ' +
					'</div>');		
				// hide the batch div, if we are in gallery mode
				if (window.location.hash.indexOf('gallery') != -1){
					$('#batch-div').css('display', 'none');
				}
			}
			
			$('.bDiv :checkbox').click(function(){
				if ($('.bDiv :checked').length == 0)
					$('#btnDeleteSelected').attr("disabled",true);
				else
					$('#btnDeleteSelected').removeAttr("disabled");	
			});	
			

		};
		
		/**
		 * get a observation on the map and zoom.. by id
		 * @param id
		 */
		Area.prototype.selectObservation = function(id) {
			if (id in this.overlaysArray) {
				if (this.selectedId != null) {
					this.overlaysArray[this.selectedId].deselect();
					this.selectedId = null;
				}
				this.selectedId = id;
				var currentOverlays = this.overlaysArray[this.selectedId];
				currentOverlays.select();
				
				var bounds = currentOverlays.getBounds();
				this.googlemap.fitBounds(bounds);
				this.googlemap.setZoom(this.googlemap.getZoom() - 2);
				this.showInfoWindow(id);
			} else {
				console.error("Observation not available: " + id);
			}
		};
				
});



