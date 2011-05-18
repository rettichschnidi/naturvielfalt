
// @author Damian Conrad

function selectTableRow() {
	/**
	 * This function is called when the user clicks on a row
	 */
	
	selectTableRow.prototype.onTableRowClicked = function(e) {
		//console.info("hallo schnuzz");
		if (!e) var e = window.event;
		if (e.target) targ = e.target;
		else if (e.srcElement) targ = e.srcElement;
		if (targ.nodeType == 3) // defeat Safari bug
			targ = targ.parentNode;
		var organismId = targ.parentNode.id.split('_');
		//me.selectOrganism(organismId[1], targ);
		if (organismId[0] == 'organism')
			window.location.href = '../'+organismId[1];
		if (organismId[0] == 'organismtype')
			window.location.href = 'organism/type/'+organismId[1];
	}	
	// register events
	jQuery("#organisms").live('click', this.onTableRowClicked);
}



new selectTableRow();