
// @author Damian Conrad

function inventoryOrganismSelect() {
	/**
	 * This function is called when the user clicks on a row
	 */
	
	inventoryOrganismSelect.prototype.onTableRowClicked = function(e) {
		//console.info("hallo schnuzz");
		if (!e) var e = window.event;
		if (e.target) targ = e.target;
		else if (e.srcElement) targ = e.srcElement;
		if (targ.nodeType == 3) // defeat Safari bug
			targ = targ.parentNode;
		var organismId = targ.parentNode.id.split('_');
		//me.selectOrganism(organismId[1], targ);
		window.location.href = '../organism/'+organismId[1];
	}	
	// register events
	jQuery("#flora").live('click', this.onTableRowClicked);
	jQuery("#fauna").live('click', this.onTableRowClicked);
}



new inventoryOrganismSelect();