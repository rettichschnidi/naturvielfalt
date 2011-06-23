var gallery_rating = {};

(function($) {
  
  galleryLightboxSettings.captionFactory = function(caption) {
    caption.find('form').each(gallery_rating.stars);
    return caption;
  }
  
  gallery_rating.init = function() {
    $('form.show_hide').each(gallery_rating.show_hide);
  }
  
  gallery_rating.stars = function() {
    if($(this).data('stars'))
      return;
    var caption = $('<small />');
    var avg = $('<div class="stars-avg" />')
    var stars = $('<div class="stars" />').append(avg);
    $(this).find('option[selected]').removeAttr('selected');
    $(this).stars({
      inputType: 'select',
      cancelShow: false,
      cancelValue: 'cancel',
      captionEl: caption,
      callback: gallery_rating.callback
    }).append(stars).append(caption).find('input[type="submit"]').remove();
    stars.append($(this).find('.ui-stars-star'));
    avg.css('width', $(this).data('average')/5*100+'%');
  }
  
  gallery_rating.show_hide = function() {
    $(this).find('input[type="submit"]').click(function(e) {
      e.preventDefault();
      e.stopPropagation();
      var form = $(this).parents('form');
      $.post(form.attr('action')+'?ajax=1', $.proxy(function(data) {
        if(!data)
          return;
        form.attr('action', data.action);
        form.find('input[type="submit"]').attr('value', data.label);
      }, form), 'json');
    });
  }
  
  gallery_rating.callback = function(ui, type, value) {
    var action = ui.$form.attr('action')+'?ajax=1';
    var rating = ui.$form.find('.rating');
    $.post(action, ui.$form.serializeArray(), $.proxy(function(data) {
      if(!data || !data.rating)
        return;
      var rating = $(data.rating);
      ui.$form.parents('.rating').replaceWith(rating);
      rating.find('form').each(function() {
        var average = $(this).data('average');
        var label = $(this).find('label').html();
        var parts = $(this).attr('action').split('/');
        $('.rating form[action$="\/'+parts[parts.length-1]+'"]:has(input[name="type"][value="'+$(this).find('input[name="type"]').val()+'"])')
          .attr('data-average', average)
          .find('label').html(label);
        $.proxy(gallery_rating.stars, $(this))();
      });
    }, ui), 'json');
  }
  
  $(document).ready(gallery_rating.init);
})(jQuery);