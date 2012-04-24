jQuery(document).ready(function() {

		
$ = jQuery;
var autocomplete = {};

// Organism field
jQuery( "#organismn_autocomplete" ).autocomplete({
	minLength: 2,
	autoFocus: true,
	source: function (request, response) {
		//new search, so we change the indicator to searching
		actualElement = this.element;
		actualElement.removeClass('notfound');
		actualElement.addClass('searching');
		$( "#organismn_id" ).val('');
		$( "#species_autocomplete" ).html('');
		$( "#observation_found_as_latin" ).val('false');
		$( "#observation_found_as_lang" ).val('false');
		observation.hideDetMethods();
		observation.hideAttributes();
		if(autocomplete.request) {
			autocomplete.request.abort();
		}
		autocomplete.request = $.ajax({
			url: Drupal.settings.basePath + "/organism/search/json",
			dataType: "json",
			data: {
				artgroup: $('#artgroup').val(),
				term: actualElement.val()
			},
			// success : response,
			success: function (data) {
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
//	focus: function (event, ui) {
//		inventory.scrollToAutocompleteItem(this);
//		return false;
//	},
	select: function (event, ui) {
		$(this).val(ui.item.label || ui.item.label_latin_short);
		lan = ui.item.langfounded;
		lat = ui.item.latinfounded;
		$( "#observation_found_as_latin" ).val(lat);
		$( "#observation_found_as_lang" ).val(lan);
		
		if(!lan && lat){
			$( "#organismn_id" ).val(ui.item.id);
			$( "#organismn_autocomplete" ).val(ui.item.label_latin);
			$( "#species_autocomplete" ).html(ui.item.label);
		}else{
			$( "#organismn_id" ).val(ui.item.id);
//			$( "#organismn_autocomplete" ).val(ui.item.label_latin);
			$( "#species_autocomplete" ).html(ui.item.label_latin);
		}
		
		for (var i in ui.item.attributes){
			observation.showAttribute(ui.item.attributes[i]);
		}
		for (var i in ui.item.det_methods){
			observation.showDetMethod(ui.item.det_methods[i]);
		}
		
//		alert('lang:' + ui.item.langfounded + ' latin:' + ui.item.latinfounded);
//		$(this).closest('tr').find('td > em').html(ui.item.label_latin);
//		inventory.enableDisable($(this).closest('tr'));
		if(event.keyCode == 9 || event.keyCode == 13) { // TAB, ENTER
//			$( "#date" ).focus();
			$( "#date" ).focus();
//			event.preventDefault();
		}
//		if($(this).closest('tr:last-child').size()) inventory.duplicateRow($(this).closest('tr'));
//		inventory.getImage($(this).closest('tr'));
		return false;
	}
}).keyup(function () {
	//monitor field and remove class 'notfound' if its length is less than 2
	if($(this).val().length < 2) {
		$(this).removeClass('notfound');
		$( "#organismn_id" ).val();
		$( "#species_autocomplete" ).html('');
//		$(this).closest('td').find('input.organism_id').val('');
//		$(this).closest('tr').find('td > em').html('');
//		$(this).closest('tr').find('.image').html('');
//		inventory.enableDisable($(this).closest('tr'));
	}
}).each(function () {
	$(this).data('autocomplete')._renderItem = function (ul, item) {
		term = this.term;
//		term = this.term.replace(/[aou]/, function (m) {
//			// to find with term 'wasser' -> 'wasser' and 'gewasesser'
//			var hash = {
//				'a': '(ä|a)',
//				'o': '(ö|o)',
//				'u': '(ä|u)'
//			};
//			return hash[m];
//		});
		// highlighting of matches
		spacer = '';
		var label = item.label;
		var label_latin = item.label_latin;
		var re = new RegExp($.trim(term), 'ig');
		item.langfounded = false;
		if(label.match(re)) item.langfounded = true;
		label = label.replace(re, '<span class="ui-state-highlight">$&</span>');
		// seach result mark is not perfect, should be the same as the search...
		term = $.trim(term).split(' ');
		item.latinfounded = false;
		while(term.length) {
			re = new RegExp(term.pop(), 'ig');
			if(label_latin.match(re)) item.latinfounded = true;
			label_latin = label_latin.replace(re, '<span class="ui-state-highlight">$&</span>');
		}
//		
//		var old_label = item.old_label;
//		var old_label_latin = item.old_label_latin;
//		var re = new RegExp($.trim(term), 'ig');
//		if(old_label) old_label = old_label.replace(re, '<span class="ui-state-highlight">$&</span>');
//		else label = label.replace(re, '<span class="ui-state-highlight">$&</span>');
//		term = $.trim(term).split(' ');
//		while(term.length) {
//			re = new RegExp(term.pop(), 'ig');
//			if(old_label_latin) old_label = old_label.replace(re, '<span class="ui-state-highlight">$&</span>');
//			else label_latin = label_latin.replace(re, '<span class="ui-state-highlight">$&</span>');
//		}
//		var old = '';
//		if(old_label || old_label_latin) old = '<small>' + old_label + '<em>' + old_label_latin + '</em></small>';
		if(label_latin != '' && label != '') spacer = '<br>';
		return $('<li></li>').data('item.autocomplete', item).append('<a class="organism_autocomplete_list">' + label + spacer +'<em class="organism_autocomplete_list_s">' + label_latin + '</em></a>').appendTo(ul);
	};
});






});