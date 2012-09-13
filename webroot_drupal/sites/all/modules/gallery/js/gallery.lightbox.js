var galleryLightboxSettings = {
	captionSelector : '.caption',
	captionAttr : false,
	captionHTML : true,
	imageSelector : 'a:first'
}; 


(function($) {
	
	gallery_lightbox = {};
	
	$(document).ready(function() {
		gallery_lightbox.registerLightBox();
	});
	
	gallery_lightbox.registerLightBox = function() {
		$('a[rel^="lightbox"]').lightBox();
		$('ul.gallery:not(.presentation) li').lightBox(galleryLightboxSettings).find('.caption').hide();
		$('ul.presentation li:not(.noimage)').lightBox(galleryLightboxSettings);
		$('ul.presentation li .caption a').click(function(e) {
			e.stopPropagation();
		});
	};
	
})(jQuery);
