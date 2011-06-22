
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
		if (idType[0] == 'inventory')
			window.location.href = Drupal.settings.basePath+'inventory/'+idType[1];
		if (idType[0] == 'organism')
			window.location.href = Drupal.settings.basePath+'organism/'+idType[1];
		
	}	
	// register events
	jQuery("#inventories").live('click', this.onTableRowClicked);
	jQuery("#organisms").live('click', this.onTableRowClicked);
}

new inventoryRowSelect();