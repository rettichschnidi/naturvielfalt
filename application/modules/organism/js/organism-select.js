
// @author Damian Conrad

function organismSelect() {
	/**
	 * This function is called when the user clicks on a row
	 */
	
	organismSelect.prototype.onTableRowClicked = function(e) {
		//console.info("hallo schnuzz");
		if (!e) var e = window.event;
		if (e.target) targ = e.target;
		else if (e.srcElement) targ = e.srcElement;
		if (targ.nodeType == 3) // defeat Safari bug
			targ = targ.parentNode;
		var organismId = targ.parentNode.id.split('_');
		//me.selectOrganism(organismId[1], targ);
		window.location.href = '../'+organismId[1];
	}	
	// register events
	jQuery("#organisms").live('click', this.onTableRowClicked);
}



new organismSelect();