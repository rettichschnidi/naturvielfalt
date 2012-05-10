/**
 * @author Damian Conrad
 */

/**
 * 
 */
function inventoryRowSelect() {
	
	/**
	 * This function is called when the user clicks on a row
	 * @param 
	 */
	inventoryRowSelect.prototype.onTableRowClicked = function(e) {
		if (!e)
			e = window.event;
		if (e.target)
			targ = e.target;
		else if (e.srcElement)
			targ = e.srcElement;
		if (targ.nodeType == 3) // defeat Safari bug
			targ = targ.parentNode;
		var idType = targ.parentNode.id.split('_');
		if (idType[0] == 'organism')
			window.location.href = Drupal.settings.basePath + 'organism/'
					+ idType[1];
		if (idType[0] == 'inventory')
			window.location.href = Drupal.settings.basePath + 'inventory/'
					+ idType[1];
		if (idType[0] == 'template')
			window.location.href = Drupal.settings.basePath
					+ 'inventorytemplate/' + idType[1];
		if (idType[0] == 'protected')
			window.location.href = Drupal.settings.basePath + 'user/'
					+ idType[1] + '/contact';
		if (idType[0] == 'protection')
			window.location.href = Drupal.settings.basePath
					+ 'protection_lists/' + idType[1];
	};

	// register events
	jQuery("#flora").live('click', this.onTableRowClicked);
	jQuery("#fauna").live('click', this.onTableRowClicked);
	jQuery("#inventories_public").live('click', this.onTableRowClicked);
	jQuery("#inventories_shared").live('click', this.onTableRowClicked);
	jQuery("#inventories_own").live('click', this.onTableRowClicked);
	jQuery("#templates_own").live('click', this.onTableRowClicked);
	jQuery("#templates_public").live('click', this.onTableRowClicked);
	jQuery("#protection_lists").live('click', this.onTableRowClicked);
	jQuery(".invgroup tr").live('click', this.onTableRowClicked);
}

new inventoryRowSelect();