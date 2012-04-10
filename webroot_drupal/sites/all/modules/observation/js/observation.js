jQuery(document).ready(function() {
 observation = {};
 
	jQuery( "#date" ).datepicker({
		dateFormat: 'dd.mm.yy',
		duration: 0,
		showButtonPanel: true,
		onSelect: function (date, inst) {
			observation.last_date = date;
		}
	}).width(80);
	if(observation.last_date) jQuery( "#date" ).val(observation.last_date)
	jQuery( "#organismn_autocomplete" ).focus();
	
	
	function changeArtGroup(id){
		alert('not implemented')
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
					alert('success');
				}else{
					alert('fail');
				}
				});

				observation.ajax.fail(function(jqXHR, textStatus) {
				  alert( "Request failed: " + textStatus );
				});
			return false;
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
});