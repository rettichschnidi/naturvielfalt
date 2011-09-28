/**
 * Equal Heights Plugin
 * Equalize the heights of elements. Great for columns or any elements
 * that need to be the same size (floats, etc).
 * 
 *
 * Usage: $(object).equalHeights([minHeight], [maxHeight]);
 * 
 * Example 1: $(".cols").equalHeights(); Sets all columns to the same height.
 * Example 2: $(".cols").equalHeights(400); Sets all cols to at least 400px tall.
 * Example 3: $(".cols").equalHeights(100,300); Cols are at least 100 but no more
 * than 300 pixels tall. Elements with too much content will gain a scrollbar.
 * 
 */

(function($) {
	$.fn.equalHeights = function(minHeight, maxHeight) {
		tallest = (minHeight) ? minHeight : 0;
		this.each(function() {
			if($(this).height() > tallest) {
				tallest = $(this).height();
			}
		});
		if((maxHeight) && tallest > maxHeight) tallest = maxHeight;
		return this.each(function() {
			$(this).height(tallest).css("overflow:hidden","auto");
		});
	}
})(jQuery);
 
(function ($) {
	 Drupal.behaviors.whatever = {
	 attach: function(context, settings) {
		 
		// equalize sidebar and content height
		//$(".column").equalHeights(487);
		
		// set page title width to content witdh
		contentwidth = $("#content").width();
		if($(".region-sidebar-first").length == 0) {
			$("#title-wrapper").css("margin-left","7px");
			$("#title-wrapper").width(contentwidth-14);
		} else {
			$("#title-wrapper").width(contentwidth-7)
		}
		
		/*
		// set page title height
		if($(".tabs").length == 0 && $(".messages").length == 0) {
			$("#title-wrapper").height(33);
			$(".region-content .block.first .content").css("margin-top", "35px;");
		}*/
     }
     };
})(jQuery);