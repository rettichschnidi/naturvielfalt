
// @author Damian Conrad

function inventoryRowSelect() {
	/**
	 * This function is called when the user clicks on a row
	 */
	
	inventoryRowSelect.prototype.onTableRowClicked = function(e) {
		if (!e) var e = window.event;
		if (e.target) targ = e.target;
		else if (e.srcElement) targ = e.srcElement;
		if (targ.nodeType == 3) // defeat Safari bug
			targ = targ.parentNode;
		var idType = targ.parentNode.id.split('_');
		if (idType[0] == 'organism')
			window.location.href = '../organism/'+idType[1];
		if (idType[0] == 'inventory')
			window.location.href = 'inventory/'+idType[1];
	}
	
	var lastHoverId = 0;
	inventoryRowSelect.prototype.showStaticImage = function(event) {
		var posleft = jQuery(event.target.parentNode).position().left+40;
		var hoverId = event.target.parentNode.id.split('_')[1];
		var postop = jQuery(event.target.parentNode.parentNode).offset().top - jQuery('#header').height() + jQuery('tbody > tr').height()+5;

		if(event.type == 'mouseover'){
			jQuery('#static_image').css('display','block');
			jQuery('#static_image').css('left', posleft);
			jQuery('#static_image').css('top', postop);
			
			if(event.currentTarget.parentNode.id!=lastHoverId){
				jQuery('#static_image').find('img').attr('src', 'modules/area/images/ajax-loader.gif');
				jQuery('#static_image').find('img').attr('src', 'area/gmap_image_redirect/'+hoverId);
			}
			lastHoverId = hoverId;
		} else if (event.type == 'mouseout'){
			jQuery('#static_image').css('display', 'none');
		}
	}
	
	// register events
	jQuery("#flora").live('click', this.onTableRowClicked);
	jQuery("#fauna").live('click', this.onTableRowClicked);
	jQuery("#inventories_share").live('click', this.onTableRowClicked);
	jQuery("#inventories_group").live('click', this.onTableRowClicked);
	jQuery("#inventories_own").live('click', this.onTableRowClicked);
	jQuery('.show_static_image').live('mouseover mouseout', this.showStaticImage);
}

new inventoryRowSelect();