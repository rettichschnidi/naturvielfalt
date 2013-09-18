jQuery(document).ready(function() {
	var data = {
			offset: 0,
			limit: 10
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
});