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
		if(responseText.success == true){
			observation.setMessage('<br><br>'+Drupal.t('Observation saved successfully'), 'status', 3000);
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
			}
		}else{
			observation.setMessage('<br><br>&bull;&nbsp;'+responseText.message.join("<br>&bull;&nbsp;"),'error', 5000);
		}
	};
	  var ajaxurl = Drupal.settings.basePath + 'observation/save';
	  if($('#observation_id').val() != ''){
		  ajaxurl = Drupal.settings.basePath + 'observation/'+ $('#observation_id').val() +'/save';
	  }
	var options = { 
//	        target:        '#message',   // target element(s) to be updated with server response 
	        success:       observation.showResponse,  // post-submit callback 
	 
	        // other available options: 
	        url:       ajaxurl,         // override for form's 'action' attribute 
	        type:      'post',        // 'get' or 'post', override for form's 'method' attribute 
	        dataType:  'json',        // 'xml', 'script', or 'json' (expected server response type) 
	        //clearForm: true        // clear all form fields after successful submit 
	        //resetForm: true        // reset the form after successful submit 
	 
	        // $.ajax options can be used here too, for example: 
	        //timeout:   3000 
	    };
	    // bind form using 'ajaxForm'
		if($('#observation_form').length > 0) $('#observation_form').ajaxForm(options); 
		observation.setMessage = function (message, type, time) {
			if(observation.messageTimer) window.clearTimeout(observation.messageTimer);
			observation.message.children('.messages').html(message).attr('class', 'messages').addClass(type);
			observation.message.stop().css('height', 'auto').slideDown('fast');
			if(time) observation.messageTimer = window.setTimeout(function () {
				observation.message.slideUp('fast');
			}, time);
		};
	  
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
		
		observation.hideLoading = function () {
			if(!observation.loading) return;
			observation.loading.dialog('close');
		};

		observation.hideDetMethods = function(){
			$("#determination_method_id option").each(function () {
                $(this).css('display','none');
              });
			observation.showDetMethod('0');
		};
		observation.showDetMethod = function(id){
			$('#artgroup_detmethod_value_'+id).css('display','block');
		};
		observation.hideAttributes = function(){
			$("tr[id^='attributes_tr_']").each(function () {
                $(this).css('display','none');
              });
		};
		observation.showAttribute = function(id){
			$('#attributes_tr_'+id).css('display','table-row');
		};
		
		observation.addCustomAttribute = function(){
			$('#attributes_table > tbody:last').append(
					'<tr><td><input type="text" name="attributes_custom_names[]" onFocus="javascript:if($(this).val()==\''
					+Drupal.t('Please enter a name')+'\'){$(this).val(\'\');}" maxlength="40" value="'+ Drupal.t('Please enter a name')
					+'"></td>'+'<td><input type="text" name="attributes_custom_values[]" onFocus="javascript:if($(this).val()==\''
					+Drupal.t('Please enter a value')+'\'){$(this).val(\'\');}" maxlength="40" value="'+ Drupal.t('Please enter a value')
					+'"></td></tr>');
		};
		
		observation.resetOrganism = function(){
			$( "#organismn_id" ).val('');
			$( "#species_autocomplete" ).html('');
			$( "#observation_found_as_latin" ).val('false');
			$( "#observation_found_as_lang" ).val('false');
			observation.hideAttributes();
			observation.hideDetMethods();
		};
		
		observation.resetOrganismAutomcomplete = function(){
			$( "#organismn_autocomplete" ).val('');
			observation.resetOrganism();
		};

	customAttributeDelete = function (attribute_id) {
		tmp = attribute_id.split('_');
		var id = tmp[2];
//		alert(id); return;
		if (confirm(Drupal.t('This attribute will be deleted in all existing observations, are you sure?'))==false) return false;
		  var ajaxurl = Drupal.settings.basePath + '/observation/deleteCustomAttribute/'+id;
		  observation.attrAjax = $.ajax({
				  type: 'POST',
				  url: ajaxurl,
				  dataType: 'json',
				  type: 'POST',
				});
			observation.attrAjax.done(function(msg) {
				if(msg.success == true){
					observation.setMessage('<br><br>'+Drupal.t('Custom attribute deleted'), 'status', 5000);
					$('#attributes_tr_'+id).remove();
				}else{
					observation.setMessage('<br><br>&bull;&nbsp;'+Drupal.t('Custom attribute not deleted'),'error', 15000);
				}
				});

				observation.attrAjax.fail(function(jqXHR, textStatus) {
//					  alert( "Request failed: " +  );
				  observation.setMessage(Drupal.t('Request failed: ')+textStatus,'error', 15000);
				});
			return false;
		};
		
		addUploadSlot = function(form){
			$('#picture_upload__0').clone().appendTo('#picture').css('display','block').css('height','auto');
			elemets_ids=0;
			$("div[id^='picture_upload__']").each(function () {
				$(this).attr("id", 'picture_upload__'+(elemets_ids++));
//            	$(this).attr("id");
			});
		};
		
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

		observation.galleryMetaDataDialog = function (div) {
//			e.preventDefault();
//			tmphref = div; return;
			file_name = div.children('input#file_input.form-file').val();
			div_id = div.attr("id");
//			observation.showLoading();
//			alert(form.files["0"].type);
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
						
//						var entry_id = $(this).attr('action').split('/').pop().replace(/\?.*$/, '');
//						var row = inventory.container.find('input.entry_id[value="' + entry_id + '"]').closest('tr');
//						var base_name = row.find('input.entry_id').attr('name');
//form = $(this);
//						inventory.storePrevPos(inventory.location.position);
//
//						inventory.addInput(row, base_name, $(this), 'lat');
//						inventory.addInput(row, base_name, $(this), 'lng');
//						inventory.addInput(row, base_name, $(this), 'zip');
//						inventory.addInput(row, base_name, $(this), 'locality');
//						inventory.addInput(row, base_name, $(this), 'township');
//						inventory.addInput(row, base_name, $(this), 'canton');
//						inventory.addInput(row, base_name, $(this), 'country');

//						row.find('a.location img').attr('src', row.find('a.location img').attr('src').replace('_unset', ''));
					
//						inventory.setMessage(Drupal.t('The location will be stored only after saving the whole form by pressing the <em>Save</em> button in the lower right.'), 'warning', 15000);
						$(this).closest('.ui-dialog-content').dialog('close');
						return false;
					});
				}
//				observation.hideLoading();
			});
		};
		
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
		 * Show a info window for a given, existing area
		 * 
		 * @param id
		 *            integer area id
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