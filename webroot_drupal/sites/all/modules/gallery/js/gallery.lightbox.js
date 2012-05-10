var galleryLightboxSettings = {
	captionSelector : '.caption',
	captionAttr : false,
	captionHTML : true,
	imageSelector : 'a:first'
}; (function($) {
	$(document).ready(function() {
		$('a[rel^="lightbox"]').lightBox();
		$('ul.gallery:not(.presentation) li').lightBox(galleryLightboxSettings).find('.caption').hide();
		$('ul.presentation li:not(.noimage)').lightBox(galleryLightboxSettings);
		$('ul.presentation li .caption a').click(function(e) {
			e.stopPropagation();
		});
	});
})(jQuery);
