/**
 * @author Damian Conrad
 */

/**
 * 
 */
function organismRowSelect() {
	/**
	 * This function is called when the user clicks on a row
	 */

	organismRowSelect.prototype.onTableRowClicked = function(e) {
		if (!e)
			e = window.event;
		if (e.target)
			targ = e.target;
		else if (e.srcElement)
			targ = e.srcElement;
		if (targ.nodeType == 3) // defeat Safari bug
			targ = targ.parentNode;
		var organismId = targ.parentNode.id.split('_');
		// me.selectOrganism(organismId[1], targ);
		if (organismId[0] == 'organism')
			window.location.href = Drupal.settings.basePath + 'organism/'
					+ organismId[1];
		if (organismId[0] == 'organismtype')
			window.location.href = Drupal.settings.basePath + 'organism/type/'
					+ organismId[1];
		if (organismId[0] == 'inventory')
			window.location.href = Drupal.settings.basePath + 'inventory/'
					+ organismId[1];
		if (organismId[0] == 'inventoryprotected')
			window.location.href = Drupal.settings.basePath + 'user/'
					+ organismId[1] + '/contact';
	};
	// register events
	jQuery("#organisms").live('click', this.onTableRowClicked);
	jQuery("#areas").live('click', this.onTableRowClicked);
	jQuery("#inventories").live('click', this.onTableRowClicked);
}

new organismRowSelect();