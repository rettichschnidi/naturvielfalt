/**
 * @file organism.datatable.select.js
 * @author Damian Conrad, 2011
 * @author Reto Schneider, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

/**
 * This function is called when the user clicks on a row
 */
function organismRowSelect() {
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
		if (organismId[0] == 'organism')
			window.location.href = Drupal.settings.basePath + 'organism/'
					+ organismId[1];
		if (organismId[0] == 'classifier')
			window.location.href = Drupal.settings.basePath + 'organism/classifier/'
					+ organismId[1];
		if (organismId[0] == 'classifierclassification')
			window.location.href = Drupal.settings.basePath + 'organism/classifier/'
					+ organismId[1] + '/classification/' + organismId[2];
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