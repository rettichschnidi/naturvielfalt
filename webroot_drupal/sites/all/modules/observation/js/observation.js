jQuery(document).ready(function() {
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
	jQuery( "#organismn_autocomplete" ).focus();
	
	
	function changeArtGroup(id){
		alert('not implemented');
	}
	  
	  
//	  $('observation_form').submit(function (e) {
//				observation.save(e, $.proxy(function () {
//					$(this).unbind('submit').submit();
//				}, this));
//		});

	  observation.ajax = '';
	  observation.save = function (event, callback) {
//			observation.showLoading();
//			observation.callback = callback;
		  observation.ajax = $.ajax({
				  type: 'POST',
				  url: Drupal.settings.basePath + '/observation/save',
				  data: $("#observation_form").serialize(),
				  dataType: 'json',
				  type: 'POST',
				});
			observation.ajax.done(function(msg) {
//				  $("#log").html( msg );
				
				if(msg.success == true){
//					alert('success');
					observation.setMessage('<br><br>'+Drupal.t('Observation saved successfully'), 'status', 5000);
					$('#observation_form').trigger('reset');
					$( "#species_autocomplete" ).html('');
				}else{
//					alert('fail');
					observation.setMessage('<br><br>&bull;&nbsp;'+msg.message.join("<br>&bull;&nbsp;"),'error', 15000);
				}
				});

				observation.ajax.fail(function(jqXHR, textStatus) {
//				  alert( "Request failed: " +  );
				  observation.setMessage(Drupal.t('Request failed: ')+textStatus,'error', 15000);
				});
			return false;
		};
		
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


});