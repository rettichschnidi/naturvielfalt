(function($) {
	var image_attributes = ['id', 'class', 'for', 'name'];
	$(document).ready(function() {
		var form = $('#gallery-image-form');
		var editing = !form.find('#edit-actions input').data('add');
		if(editing) {
			initializeImageForm(form);
		} else {
			var add = $('<a id="edit-cancel" href="javascript://">' + form.find('#edit-actions input').data('add') + '</a>');
			var actions = form.find('#edit-actions');
			actions.append(add);
			var i = 1;
			while(form.find('.form-item-files-images-' + i + '-file').size()) {
				var wrapper = $('<div id="image' + i + '" class="image" />');
				wrapper.insertBefore(actions);
				wrapper.append($('#gallery-image-form > div > .form-item-images-' + i + '-item-search, #gallery-image-form > div > input.item:[type="hidden"], #gallery-image-form > div > .form-item-images-' + i + '-item, #gallery-image-form > div > .form-item-images-' + i + '-subtypes, #gallery-image-form > div > .form-item-files-images-' + i + '-file, #gallery-image-form > div > .categories:first, #gallery-image-form > div > .metadata:first'));
				if(i++ > 1)
					add_remove_button(wrapper);
				initializeImageForm(wrapper);
			}
			add.click(function() {
				var last = $('#gallery-image-form .image:last');
				var num = parseInt(last.attr('id').replace('image', '')) + 1;
				var clone = last.clone(false);
				clone.attr('id', 'image' + num);
				clone.find('input, label, select, textarea, .form-item, .form-checkboxes').each(function() {
					for(var i = 0; i < image_attributes.length; i++) {
						var attr = $(this).attr(image_attributes[i]);
						if(attr && (attr.indexOf('images-' + (num - 1)) > -1 || attr.indexOf('images_' + (num - 1)) > -1))
							$(this).attr(image_attributes[i], attr.replace('images-' + (num - 1), 'images-' + num).replace('images_' + (num - 1), 'images_' + num));
					}
				});
				clone.find('input[type="file"]').val('');
				var item = clone.find('select.item');
				if(item.size()) {
					item.find('option:selected').attr('selected', '');
					item.find('option:eq(' + wrapper.find('select.item option').index(wrapper.find('select.item option:selected')) + ')').attr('selected', 'selected');
				}
				clone.find('.categories input').attr('checked', '');
				initializeImageForm(clone);
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
		}
	});
	function initializeImageForm(wrapper) {
		wrapper.find('input[type="file"]').change(file_changed);
		wrapper.find('select.item').change(function() {
			update_categories($(this).closest('.image, form'), $(this).find('option:selected').val());
		});
		if(!wrapper.find('input.search').size())
			return;
		wrapper.find('input.search').autocomplete({
			minLength : 2,
			autoFocus : true,
			source : function(request, response) {
				//new search, so we change the indicator to searching
				this.element.removeClass('notfound');
				this.element.addClass('searching');
				actualElement = this.element;
				$.ajax({
					url : $(actualElement).attr('rel'),
					dataType : "json",
					data : {
						term : this.element.val()
					},
					// success : response,
					success : function(data) {
						if(data.length == 0) {
							//change visual indicator to notfound and hide menu again
							actualElement.removeClass("searching");
							actualElement.addClass("notfound");
							$('.ui-autocomplete').hide();
						} else {
							// Remove search symbol
							actualElement.removeClass("searching");
							response(data);
						}
					}
				});
			},
			focus : function(event, ui) {
				var widget = $(this).autocomplete('widget');
				var item = widget.find('#ui-active-menuitem');
				var offset = item.offset();
				var height = item.outerHeight();
				var wheight = $(window).height();
				var scrolltop = $(document).scrollTop();
				if((offset.top + height - scrolltop <= wheight) && offset.top >= scrolltop)
					return false;
				var cover = $('#ui-autocomplete-cover');
				if(!cover.size())
					var cover = $('<div id="ui-autocomplete-cover" />').appendTo($('body')).mousemove(function(e) {
						$(this).remove();
					});
				cover.css({
					'position' : 'absolute',
					'left' : widget.position().left,
					'top' : widget.position().top,
					'height' : widget.outerHeight(),
					'width' : widget.outerWidth(),
					'z-index' : 100000
				});
				if(offset.top + height - scrolltop > wheight)
					$(document).scrollTop(offset.top + height - wheight + 2);
				else if(offset.top < scrolltop)
					$(document).scrollTop(offset.top - 2);
				return false;
			},
			select : function(event, ui) {
				$(this).val(ui.item.name);
				$(this).closest('.image, form').find('input.item').val(ui.item.item_type + ':' + ui.item.item_id);
				update_categories($(this).closest('.image, form'), ui.item.item_type + ':' + ui.item.item_id);
				return false;
			}
		}).keyup(function() {
			//monitor field and remove class 'notfound' if its length is less than 2
			if($(this).val().length < 2) {
				$(this).removeClass('notfound');
				$(this).closest('.image, form').find('input.item').val('');
			}
		}).each(function() {
			$(this).data('autocomplete')._renderItem = function(ul, item) {
				return $('<li></li>').data('item.autocomplete', item).append('<a>' + item.label + '</a>').appendTo(ul);
			};
		});
	}

	function update_categories(wrapper, item_value) {
		var item = item_value.split(':');
		var data = wrapper.find('.categories input, .categories select').serializeArray();
		data.push({
			name : 'item_type',
			value : item[0]
		});
		data.push({
			name : 'item_id',
			value : item[1]
		});
		data.push({
			name : 'prefix',
			value : wrapper.find('input[type="file"]').attr('name').replace('files[', '').replace('_file]', '')
		});
		$.post(wrapper.closest('form').attr('action').split('edit')[0] + 'edit_categories', data, $.proxy(function(data) {
			if(!data)
				return;
			this.closest('form').find('input[name="form_token"]').val(data.form_token);
			this.closest('form').find('input[name="form_build_id"]').val(data.form_build_id);
			this.find('.categories .fieldset-wrapper').html(data.categories);
			if(!data.categories)
				this.find('.categories').fadeOut('fast');
			else
				this.find('.categories').fadeIn('fast');
		}, wrapper), 'json');
	}

	function file_changed() {
		var fieldset = $(this).closest('.image').find('.categories');
		if(fieldset.is('.collapsed'))
			Drupal.toggleFieldset(fieldset.get(0));
		fieldset = $(this).closest('.image').find('.metadata');
		if(fieldset.is('.collapsed'))
			Drupal.toggleFieldset(fieldset.get(0));
	}

	function add_remove_button(wrapper) {
		var remove = $('<a class="remove-image" href="javascript://">' + $('#gallery-image-form #edit-actions input').data('remove') + '</a>');
		remove.click(function() {
			$(this).closest('.image').remove();
		});
		wrapper.append(remove);
	}

})(jQuery);
