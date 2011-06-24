
(function($) {
  var image_attributes = ['id', 'class', 'for', 'name'];
  $(document).ready(function() {
    var form = $('#gallery-image-form');
    var add = $('<a id="edit-cancel" href="javascript://">'+form.find('#edit-actions input').data('add')+'</a>');
    var actions = form.find('#edit-actions');
    actions.append(add);
    var i=1;
    while(form.find('.form-item-files-images-'+i+'-file').size()) {
      var wrapper = $('<div id="image'+i+'" class="image" />');
      wrapper.insertBefore(actions);
      wrapper.append($('#gallery-image-form > div > .form-item-images-'+i+'-subtype, #gallery-image-form > div > .form-item-files-images-'+i+'-file, #gallery-image-form > div > .categories:first, #gallery-image-form > div > .metadata:first'));
      if(i++ > 1)
        add_remove_button(wrapper);
    }
    $('#gallery-image-form input[type="file"]').change(file_changed);
    add.click(function() {
      var last = $('#gallery-image-form .image:last');
      var num = parseInt(last.attr('id').replace('image', ''))+1;
      var clone = last.clone(false);
      clone.attr('id', 'image'+num);
      clone.find('input[type="file"]').val('').change(file_changed);
      clone.find('input, label, select, textarea, .form-item, .form-checkboxes').each(function() {
        for(var i=0; i<image_attributes.length; i++) {
          var attr = $(this).attr(image_attributes[i]);
          if(attr && (attr.indexOf('images-'+(num-1)) > -1 || attr.indexOf('images_'+(num-1)) > -1))
            $(this).attr(image_attributes[i], attr.replace('images-'+(num-1), 'images-'+num).replace('images_'+(num-1), 'images_'+num));
        }
      });
      clone.find('.error').removeClass('error');
      clone.find('.collapsible').addClass('collapsed');
      clone.find('.edit-cancel').remove();
      add_remove_button(clone);
      clone.insertAfter(last);
      Drupal.detachBehaviors(clone.get(0));
      clone.find('.resizable .grippie').remove();
      clone.find('*[class*="-processed"]').each(function() {
        $(this).attr('class', $(this).attr('class').replace(/\s[a-zA-Z]+-processed/, ''));
      });
      Drupal.attachBehaviors(clone.get(0));
    });
  });
  
  function file_changed() {
    var fieldset = $(this).parents('.image').find('.categories');
    if(fieldset.is('.collapsed'))
      Drupal.toggleFieldset(fieldset.get(0));
    fieldset = $(this).parents('.image').find('.metadata');
    if(fieldset.is('.collapsed'))
      Drupal.toggleFieldset(fieldset.get(0));
  }
  
  function add_remove_button(wrapper) {
    var remove = $('<a class="remove-image" href="javascript://">'+$('#gallery-image-form #edit-actions input').data('remove')+'</a>');
    remove.click(function() {
      $(this).parents('.image').remove();
    });
    wrapper.append(remove);
  }
  
})(jQuery);
