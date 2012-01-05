var area = {};
		
		
area.init = function() {
		var parcels = jQuery("#parcels");
		if(!parcels.size())
			return;
		parcels.find('tr:last td:first input').blur(area.addField);
}
	
area.addField = function() {
	if(!jQuery(this).val())
		return;
	var row = jQuery(this).parents('tr');
	var clone = jQuery('<tr>'+row.html()+'</tr>');
	clone.addClass(row.hasClass('odd') ? 'even' : 'odd');
	row.parents('tbody').append(clone);
	clone.find('td:first input').blur(area.addField);
	clone.find('input, select').each(function() {
		var name = jQuery(this).attr('name');
		var i = parseInt(name.replace(/[^\d]+(\d+)[^\d]+/, '$1'));
		jQuery(this).attr('name', name.replace('['+i+']', '['+(i+1)+']'));
	});
	jQuery(this).unbind('blur');
}

jQuery(document).ready(area.init);
