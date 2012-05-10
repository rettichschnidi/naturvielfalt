(function($) {
	$(document).ready(function() {
		var form = $('#open-identification-details');
		var dropdown = $('#edit-organismgroupid');
		if(dropdown) {
			dropdown.change(function() {
				var optionSelectedValue = $('#edit-organismgroupid option:selected').val();
				if(optionSelectedValue > 0) {
					// $('.search').attr('rel',
					// 'http://localhost/naturvielfalt/application/gallery/organism_type_gallery_autocomplete/inventory_type/'+optionSelectedValue);
					$('.search').attr('rel', Drupal.settings.basePath + '/open_identification/autocomplete/organisms/' + optionSelectedValue);
					$('.search').removeAttr('style');
					form = $('#open-identification-details');
					if(form) {
						initializeAutocomplete(form);
					}
				} else {
					$('.search').removeAttr('rel');
					$('.search').attr('style', 'display:none;');
				}
				$('.search').val("");
				// alert(optionSelectedValue);
			});
		}
		if(form) {
			initializeAutocomplete(form);
		}
	});
	function initializeAutocomplete(wrapper) {
		if(!wrapper.find('input.search').size())
			return;
		wrapper.find('input.search').autocomplete({
			minLength : 2,
			autoFocus : true,
			source : function(request, response) {
				// new search, so we change the indicator to searching
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
							// change visual indicator to notfound and
							// hide menu again
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
				return false;
			}
		}).keyup(function() {
			// monitor field and remove class 'notfound' if its length is less
			// than 2
			if($(this).val().length < 2) {
				$(this).removeClass('notfound');
			}
		}).each(function() {
			$(this).data('autocomplete')._renderItem = function(ul, item) {
				return $('<li></li>').data('item.autocomplete', item).append('<a>' + item.label + '</a>').appendTo(ul);
			};
		});
	}

})(jQuery);
