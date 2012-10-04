(function($) {
	flexigridExtensions = {};
	flexigridExtension = {};
	
	flexigridExtensions.showLoading = function(o) {
		var loading = o.tableId + '_loading';
		if (!flexigridExtension[loading])
			flexigridExtension[loading] = $('<div class="datatable-loading"><img src="/sites/all/modules/commonstuff/images/loading.gif" /></div>')
				.prependTo($('#'+o.tableId).closest('.flexigrid'));
		flexigridExtension[loading].show();
	};
			
	flexigridExtensions.hideLoading = function(o) {
		var loading = o.tableId + '_loading';
		if (flexigridExtension[loading])
			flexigridExtension[loading].hide();
	};
	$.extend(true, $.addFlex, flexigridExtensions);
})(jQuery);