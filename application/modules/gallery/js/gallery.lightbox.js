var galleryLightboxSettings = {
  captionSelector: '.caption',
  captionAttr: false,
  captionHTML: true,
  imageSelector: 'a:first'
};

(function($) {
  $(document).ready(function() {
    $('a[rel^="lightbox"]').lightBox();
    $('ul.gallery li').lightBox(galleryLightboxSettings).find('.caption').hide();
  });
})(jQuery);