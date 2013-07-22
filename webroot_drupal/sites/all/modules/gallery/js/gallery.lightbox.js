var galleryLightboxSettings = {
	captionSelector : '.caption',
	captionAttr : false,
	captionHTML : true,
	imageSelector : 'a:first'
}; 

var galleryOpenLightboxSettings = {
		captionSelector : '.caption',
		captionAttr : false,
		captionHTML : true,
}; 

(function($) {
	
	gallery_lightbox = {};
	
	$(document).ready(function() {
		gallery_lightbox.registerLightBox();
	});
	
	//lightbox for iteration over multiple entries
	gallery_lightbox.registerLightBox = function() {
		$('a[rel^="lightbox"]').lightBox();
		$('ul.gallery:not(.presentation) li').lightBox(galleryLightboxSettings).find('.caption').hide();
		$('ul.presentation li:not(.noimage)').lightBox(galleryLightboxSettings);
		$('ul.presentation li .caption a').click(function(e) {
			e.stopPropagation();
		});
	};
	
	//lightbox for iteration over images of one entry
	gallery_lightbox.openLightBoxEntry = function(entryId) {
		$('div[name=lightbox_entry_' + entryId + '] a[class=]"').lightBox(galleryOpenLightboxSettings);
		$('div[name=lightbox_entry_' + entryId + '] a[class=]"').first().trigger('click');
	};
	
})(jQuery);
