/**
* @file habitat.datatable.select.js
* @author Damian Conrad, 2011
* @author Ramon Gamma, 2012
* @copyright 2011-2012 Naturwerk, Brugg
*/

/**
 * 
 */
function customTableRowSelect() {

	/**
	 * This function is called when the user clicks on a row
	 */
	customTableRowSelect.prototype.onTableRowClicked = function(e,r) {
		if (!e) e = window.event;
		if (e.target) targ = e.target;
		else if (e.srcElement) targ = e.srcElement;
		if (targ.nodeType == 3) // defeat Safari bug
			targ = targ.parentNode;
		var habitatId = targ.parentNode.id.split('_');
		//me.selectOrganism(organismId[1], targ);
//		alert(targ.parentNode.id);
		if (habitatId[0] == 'habitats')
			window.location.href = Drupal.settings.basePath+'habitat/'+habitatId[1];
		if (habitatId[0] == 'areas')
			window.location.href = Drupal.settings.basePath+'area/'+habitatId[1];
	};
	// register events
	jQuery("#habitats").live('click', this.onTableRowClicked);
	jQuery("#areas").live('click', this.onTableRowClicked);
//	jQuery('#habitats').flexigrid({onRowSelect:function(e,r){this.onTableRowClicked();}});
//	jQuery('#areas').flexigrid({onRowSelect:function(e,r){this.onTableRowClicked();}}); 
}

new customTableRowSelect();