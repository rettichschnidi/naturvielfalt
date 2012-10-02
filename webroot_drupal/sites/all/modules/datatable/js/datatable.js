(function($) {
	var flexigridExtensions = {
			populate: function() {
				alert('foo');
			}
	};
	
	$.extend(true, $.addFlex.prototype, flexigridExtensions);
})(jQuery);