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
		observation.resetOrganism();
		observation.hideDetMethods();
		observation.hideAttributes();
		if(autocomplete.request) {
			autocomplete.request.abort();
		}
		autocomplete.request = $.ajax({
			url: Drupal.settings.basePath + "organism/search/json",
			dataType: "json",
			data: {
				artgroup: $('#artgroup').val(),
				term: actualElement.val()
			},
			// success : response
			success: function (data) {
				if(data == null || data.length == 0) {
					// change visual indicator to notfound and hide menu again
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
			$( "#species_autocomplete" ).html(ui.item.label_latin);
		}
		
		for (var i in ui.item.attributes){
			observation.showAttribute(ui.item.attributes[i]);
		}
		for (var i in ui.item.det_methods){
			observation.showDetMethod(ui.item.det_methods[i]);
		}

		if(event.keyCode == 9 || event.keyCode == 13) { // TAB, ENTER
			$( "#date" ).focus();
		}
		return false;
	}
}).keyup(function () {
	// monitor field and remove class 'notfound' if its length is less than 2
	if($(this).val().length < 2) {
		$(this).removeClass('notfound');
		$( "#organismn_id" ).val();
		$( "#species_autocomplete" ).html('');
	}
}).each(function () {
	$(this).data('autocomplete')._renderItem = function (ul, item) {
		term = this.term;
		// highlighting of matches
		spacer = '';
		var label = item.label;
		var label_latin = item.label_latin;
		var re = new RegExp($.trim(term), 'ig');
		item.langfounded = false;
		if(label.match(re)) {
			item.langfounded = true;
		}
		label = label.replace(re, '<span class="ui-state-highlight">$&</span>');
		
		// mark the search result in the sc name
		term = $.trim(term).split(' ');
		item.latinfounded = false;
		label_latin_split = label_latin.split(' ');
		
		re = new RegExp(term[0], 'ig');
		if(label_latin_split[0].match(re)) {
			item.latinfounded = true;
		}
		label_latin_split[0] = label_latin_split[0].replace(re, '<span class="ui-state-highlight">$&</span>');
		
		if(label_latin_split.length > 1 && term.length > 1){
			re = new RegExp(term[1], 'ig');
			if(label_latin_split[1].match(re)) item.latinfounded = true;
			label_latin_split[1] = label_latin_split[1].replace(re, '<span class="ui-state-highlight">$&</span>');
		}
		
		if(label_latin_split.length > 2 && term.length > 2){
			re = new RegExp(term[2], 'ig');
			if(label_latin_split[2].match(re)) item.latinfounded = true;
			label_latin_split[2] = label_latin_split[2].replace(re, '<span class="ui-state-highlight">$&</span>');
		}
		label_latin = label_latin_split.join(" ");
		
		if(label_latin != '' && label != '')  {
			spacer = '<br>';
		}
		return $('<li></li>').data('item.autocomplete', item).append('<a class="organism_autocomplete_list">' + label + spacer +'<em class="organism_autocomplete_list_s">' + label_latin + '</em></a>').appendTo(ul);
	};
});






});