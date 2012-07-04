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

	observation.showResponse = function(responseText, statusText, xhr, $form)  { 
		if(responseText != null && responseText.success == true){
			observation.setMessage(Drupal.t('Observation saved successfully'), 'status', 3000);
			$('#observation_form').trigger('reset');
			$('#species_autocomplete').html('');
			if(responseText.update) {
				window.location = window.location.toString().replace("edit", "show");
			}else{
				observation.hideAttributes();
				observation.hideDetMethods();
				areabasic.newestElement.overlay.setMap(null);
				areabasic.drawingManager.setDrawingMode(google.maps.drawing.OverlayType.MARKER);
				areabasic.initLocation();
				$('#recent_observations').flexReload();
			}
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
	        success:   observation.showResponse,
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
				observation.loading = $('<div><img src="' + Drupal.settings.basePath + 'sites/all/modules/inventory/images/loading.gif" /></div>').hide();
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
		 * get a observation on the map and zoom.. by id
		 * @param id
		 */
		observation.selectObservation = function(id){
			if (id in areabasic.overlayElements) {
				if (areabasic.currentElement != null) {
					areabasic.currentElement.deselect();
				}
				areabasic.currentElement = areabasic.overlayElements[id];
				areabasic.currentData = areabasic.overlayData[id];

				areabasic.currentElement.select();
				var bounds = areabasic.currentElement.getBounds();
				areabasic.googlemap.fitBounds(bounds);
				areabasic.googlemap.setZoom(areabasic.googlemap.getZoom() - 2);
				observation.showInfoWindow(id);
			} else {
				console.error("Observation not available: " + id);
			}
		};
		
		/**
		 * Show a info window for a given, existing observation
		 * @param id
		 */
		observation.showInfoWindow = function(id) {
			var url = Drupal.settings.basePath + 'observation/' + id
					+ '/overview/ajaxform';

			if (areabasic.visibleInfoWindow != null) {
				areabasic.visibleInfoWindow.close();
			}
			var infowindow = areabasic.visibleInfoWindow = new google.maps.InfoWindow({
				content : Drupal.t('Loading...'),
			});

			jQuery.get(url, function(data) {
				infowindow.setContent(data);
			});
			// move marker a little bit down, (approximately)
			// centers the infowindow
			areabasic.googlemap.panBy(0, -250);
			infowindow.open(areabasic.googlemap, areabasic.currentElement);
		};

});
