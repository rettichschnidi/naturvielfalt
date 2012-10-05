(function($) {
	// our extension methods
	flexigridExtensions = {};
	// to temporary save our extension objects
	flexigridExtension = {};
	
	/**
	 * Show a Ajax loading indicator
	 * 
	 * @param Object o flexigrid options
	 */
	flexigridExtensions.showLoading = function(o) {
		var loading = o.tableId + '_loading';
		if (!flexigridExtension[loading])
			flexigridExtension[loading] = $('<div class="datatable-loading"><img src="/sites/all/modules/commonstuff/images/loading.gif" /></div>')
				.prependTo($('#'+o.tableId).closest('.flexigrid'));
		flexigridExtension[loading].show();
	};
	
	/**
	 * Hide the Ajax loading indicator
	 * 
	 * @param Object o flexigrid options
	 */
	flexigridExtensions.hideLoading = function(o) {
		var loading = o.tableId + '_loading';
		if (flexigridExtension[loading])
			flexigridExtension[loading].hide();
	};
	
	/**
	 * Add pager option 'show all' with value set to max limit for sql query, i.e. 2^64-1
	 * 
	 * @param Object o flexigrid options
	 */
	flexigridExtensions.rpOptionsShowAllOnSubmit = function(o) {
		var rpShowAll = o.tableId + '_rpShowAll';
		if (o.usepager && o.useRp && !flexigridExtension[rpShowAll])
			flexigridExtension[rpShowAll] = $('<option value="18446744073709551615">' + Drupal.t('All') + '</option>')
				.appendTo($('#'+o.tableId).closest('.flexigrid').find('select[name="rp"]'));
	};
	
	/**
	 * Add our extensions to flexigrid
	 */
	$.extend(true, $.addFlex, flexigridExtensions);
})(jQuery);
