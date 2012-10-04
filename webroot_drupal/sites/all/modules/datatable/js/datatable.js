(function($) {
	flexigridExtension = {};
	
	var flexigridExtensions = {
			showLoading: function(o) {
				var loading = 'loading_' + o.tableId;
				if (!flexigridExtension[loading])
					flexigridExtension[loading] = $('<div class="datatable-loading"><img src="/sites/all/modules/commonstuff/images/loading.gif" /></div>')
						.prependTo($('#'+o.tableId).closest('.flexigrid'));
				flexigridExtension[loading].show();
			},
			
			hideLoading: function(o) {
				var loading = 'loading_' + o.tableId;
				if (flexigridExtension[loading])
					flexigridExtension[loading].hide();
			}
	};
	
	$.extend(true, $.addFlex, flexigridExtensions);
})(jQuery);