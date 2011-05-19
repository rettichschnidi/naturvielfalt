
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
	// register events
	jQuery("#flora").live('click', this.onTableRowClicked);
	jQuery("#fauna").live('click', this.onTableRowClicked);
	jQuery("#inventories_share").live('click', this.onTableRowClicked);
	jQuery("#inventories_group").live('click', this.onTableRowClicked);
	jQuery("#inventories_own").live('click', this.onTableRowClicked);	
}

new inventoryRowSelect();