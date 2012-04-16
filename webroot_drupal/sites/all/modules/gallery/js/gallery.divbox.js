(function($) {
	$(document).ready(function() {
		$('a.divbox_sound').divbox({
			caption : false,
			width : 640,
			height : 22
		});
		$('a.divbox_video').divbox({
			caption : false,
			width : 640,
			height : 480
		});
	});
})(jQuery);
