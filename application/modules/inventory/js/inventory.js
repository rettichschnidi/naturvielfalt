/**
 * @author Janosch Rohdewald
 */
var changedRows = [];
var saveTimerRunning = false;
var rowTrackingEnabled = true;
var saveTimeout = 0;
var newRowId = 0;
var invNum = 0;

var INVENTORY_SERVER_PATH = '../../inventoryServer/web_root/public/';

/*
 * Inventory initialisiern. Die bereits bestehenden Inventories werden hier aus
 * der JSON Variable exInvs geladen.
 * 
 */
function initInventory() {

	var inventories = jQuery('<div id="inventories">');

	jQuery("#Inventory").prepend(inventories);

	// Disable autosave
	rowTrackingEnabled = false;
	// Go through each existing inventory
	jQuery.each(exInvs, function(invId, inv) {
		invTbody = addInventory(inv["invDesc"], invId);
		// Go through each inventory entry
		jQuery.each(inv, function(entryId, entry) {
			if (entryId == "invDesc")
				return;
			// Add the row
			row = addRow(invTbody, inv["invDesc"]["cols"], entryId, entry);
			// And activate it
			activateRow(row, invTbody, inv["invDesc"]["cols"]);
		});
		addRow(invTbody, inv["invDesc"]["cols"]);
	});

	// Enable autosave
	rowTrackingEnabled = true;
}

// Called when the button 'Inventar hinzufügen' is clicked.
// Gets the attributes of the inventory and passes them to addInventory()
function onAddInventory() {
	jQuery.getJSON(INVENTORY_SERVER_PATH + "inventory/new-inventory?inv_id=" + jQuery('#edit-inventory-types').val(), function(json) {
		tbody = addInventory(json);
		addRow(tbody, json["cols"]);
	});
}

// Adds a new inventory to the page.
// Params: - json with the inventory specifications (attributes)
// - The inventory id (null when new inventory)
function addInventory(json, id) {
	var inventories = jQuery('#inventories');
	id = (id == null ? "inv_new_" + (invNum++) : id);

	jQuery('<b>').text(json["name"]).appendTo(inventories);

	var table = jQuery("<table cellspacing=0 cellpadding=0 class='invTable'>");
	var invDef = jQuery("<input type='hidden' name='invId' value='" + id + "'>");
	var invTypeDef = jQuery("<input type='hidden' name='invTypeId' value='" + json["id"] + "'>");

	var thead = jQuery("<thead>");
	var trow = jQuery("<tr>");
	jQuery("<td>").text("Art").appendTo(trow);
	
	jQuery.each(json["cols"], function(key, value) {
		jQuery("<td>").text(value["name"]).appendTo(trow);
	});
	trow.appendTo(thead);

	table.append(invDef);
	table.append(invTypeDef);
	table.append(thead);
	
	var tbody = jQuery("<tbody>");
	table.append(tbody);
	inventories.append(table);
	inventories.append('<br>');

	return tbody;
}

// Activate a row. This means disable the 'Art' input field and enable the other
// fields.
// Params: The jQuery row object
// The tbody of the inventory
// The columns (attributes) of the inventory (JSON)
function activateRow(row, tbody, cols) {
	var i = 0;
	row.find('td').each(function() {
		// Activate custom attributes
		if (i > 0) {
			// Copy dates from last row
			/*
			 * if (cols[i] == null && cols[i]["format"] == "date") {
			 * $(this).children().attr( "value", row.prev().find('td:eq(' + i +
			 * ')').children() .attr("value")); rowChanged(row); }
			 */
			jQuery(this).children().attr("disabled", false);
		} else
		// Deactivate the 'nature' field
		{
			jQuery(this).children().attr("readonly", true);
		}
		i++;
	});

	// Add remove icon
	jQuery("<td>").append("<img src='../modules/inventory/images/can_delete.png' onclick='javascript:deleteRow(jQuery(this));' class='a'>")
			.appendTo(row);
	// Add insert icon
	jQuery("<td>").append("<img src='../modules/inventory/images/insert.png' onclick='javascript:insert(jQuery(this));' class='a'>")
			.appendTo(row);
}

// Delete a row
// Params: The jQuery row object
function deleteRow(img) {
	var row = img.parent().parent();
	// Row must also be deleted in database
	if (rowTrackingEnabled) {
		rowId = row.find("input[name='rowId']").attr("value").substr(0, 8);
		if (rowId != "row_new_") {
			changedRows.push({
				"action" : "delete",
				"rowId" : rowId
			});
		}
	}
	row.remove();

	if (!saveTimerRunning) {
		saveTimerRunning = true;
		setTimeout("saveRows()", saveTimeout);
	}
}

// Inserts a new row below the old one
// Params: The jQuery row object
function insert(img) {

	// var trow = $("<tr>");

	var inventories = jQuery('<div id="inventories">');
	inv = exInvs["38"];
	tbody = addInventory(inv["invDesc"], inv);
	cols = inv["invDesc"]["cols"];

	// activateRow(trow, tbody, cols);
	addRow(tbody, cols);
}

// Add a row, which should be saved at the next save
// Params: The jQuery row object
function rowChanged(row) {
	if (!rowTrackingEnabled)
		return;

	var newRow = true;
	for (key in changedRows) {
		if (changedRows[key]["action"] == "save") {
			value = changedRows[key]["row"];
			if (value.get(0) === row.get(0)) {
				newRow = false;
				return;
			}
		}
	}

	if (newRow) {
		changedRows.push({
			"action" : "save",
			"row" : row
		});
	}

	if (!saveTimerRunning) {
		saveTimerRunning = true;
		setTimeout("saveRows()", saveTimeout);
	}
}

// Save all rows which have changed (collected by rowChanged)
function saveRows() {
	saveTimerRunning = false;
	var rowsToSave = changedRows;
	changedRows = [];

	var saveArray = {};
	var newColCnt = 0;

	saveArray["headInventoryId"] = jQuery('input[name="head_inventory_id"]').attr("value");
	for (key in rowsToSave) {
		if (rowsToSave[key]["action"] == "save") {
			row = rowsToSave[key]["row"];
			if (saveArray["addRows"] == undefined)
				saveArray["addRows"] = {};

			var rowId = row.find("input[name='rowId']").attr("value");
			var invId = row.parent().parent().find("input[name='invId']").attr("value");

			if (saveArray["addRows"][invId] == undefined)
				saveArray["addRows"][invId] = {};
			saveArray["addRows"][invId]['invTypeId'] = row.parent().parent().find("input[name='invTypeId']").attr("value");

			saveArray["addRows"][invId][rowId] = {};
			saveArray["addRows"][invId][rowId]['orgId'] = row.find("input[name='orgId']").attr("value");

			row.find("td").each(function() {
				cell = jQuery(this).children();
				// Jump over non-input cells
				if (cell.attr("name") == null || cell.attr("name").substr(0, 4) != "col_")
					return true;
				saveArray["addRows"][invId][rowId][cell.attr("name")] = cell.attr("value");
			});

			newColCnt++;
		} else if (rowsToSave[key]["action"] == "delete") {
			if (saveArray["deleteRows"] == undefined)
				saveArray["deleteRows"] = [];
			saveArray["deleteRows"].push(rowsToSave[key]["rowId"]);
		}
	}

	jQuery.post(INVENTORY_SERVER_PATH + "inventory/save-ajax", saveArray, function(ids) {
		jQuery.each(ids, function(key, value) {
			var name = "";
			if (key.substr(0, 8) == "row_new_")
				name = "rowId";
			else if (key.substr(0, 8) == "inv_new_")
				name = "invId";

			jQuery("input[type=hidden][name='" + name + "'][value='" + key + "']").attr("value", value);
		});
		
		jQuery('#last_saved').removeClass("messages error");
		
		var now = new Date();
		var displayTime = now.getHours()+':'+now.getMinutes();
		var displayDate = (now.getDate()) + '.' + (now.getMonth()+1) + '.' + now.getFullYear();
		jQuery('#last_saved').text('Zuletzt gespeichert am ' + displayDate + ' um ' + displayTime);
		jQuery('#last_saved').addClass('messages status');		
	}, "json");
	
	jQuery('#last_saved').ajaxError(function() {
		jQuery('#last_saved').removeClass('messages status');
		jQuery(this).addClass("messages error");
		jQuery(this).text( "Fehler beim speichern!" );
	});

	// TODO: overwrite exInvs with the actual inventorys
	// var exInvs = $this->inventorys;
}

// Add a new row to an inventory
// Params: The jQuery tbody of the inventory
// The inventory attribute columns
// The row id (mysql id of inventoryentry) (on a new row: null)
// The values of the cells (if it is no a new row) otherwise null
function addRow(tbody, cols, rowId, cellValues) {
	var trow = jQuery("<tr>");
	organismField = jQuery("<input>").attr("type", "text");
	organismIdField = jQuery("<input>").attr("type", "hidden").attr("name", "orgId");
	if (cellValues != null)
		organismIdField = organismIdField.attr("value", cellValues["orgId"])

	rowId = (rowId == null ? "row_new_" + (newRowId++) : rowId);
	var invDef = jQuery("<input type='hidden' name='rowId' value='" + rowId + "'>");

	organismField = organismField.width(300);
	if (cellValues != null)
		organismField = organismField.attr("value", cellValues["label"]);

	jQuery("<td>").append(organismField).appendTo(trow);
	trow.append(organismIdField);
	trow.append(invDef);

	jQuery.each(cols, function(key, value) {
		var input = jQuery("<input>").attr("type", "text");

		switch (value["format"]) {
		case "date":
			input = input.datepicker({
				dateFormat : 'dd.mm.yy'
			});
			input = input.width(80);
			break;
		case "int":
			input = input.numeric();
			input = input.width(70);
			break;
		case "dropdown":
			input = jQuery('<select>').width(180);
			input = input.append(jQuery('<option>').attr("value", "0").text(""));
			jQuery.each(value["dropdown_values"], function(key, value) {
				input = input.append(jQuery('<option>').attr("value", value["id"]).text(value["value"]));
			});
			break;
		default:
			break;
		}

		jQuery("<td>").append(input).appendTo(trow);
		input.attr("name", "col_" + value['id']).attr("disabled", true);
		if (cellValues != null)
			input.attr("value", cellValues["col_" + value['id']]);

		input.change(function() {
			rowChanged(jQuery(this).parent().parent());
		});
	});
	// TODO: Thats exactly where the magic happens
	trow.appendTo(tbody);

	organismField.autocomplete(

			{
				minLength : 2,
				noCache : true,
				matchSubset : false,
				cacheLength : 0,
				source : function(request, response) {
					//new search, so we change the indicator to searching
					this.element.removeClass("notfound");
					this.element.addClass("searching");
					actualElement = this.element;
					jQuery.ajax({
						url : INVENTORY_SERVER_PATH + 'inventory/get-organisms',
						dataType : "json",
						data : {
							inv_id : this.element.parent().parent().parent().parent().find('input[name|=invTypeId]').val(),
							term : this.element.val()
						},
						// success : response,
						success : function(data){
							if(data.length==0){
								//change visual indicator to notfound and hide menu again
								actualElement.removeClass("searching");
								actualElement.addClass("notfound");
								jQuery('.ui-widget-content').css("display","none");
								
							} else {
								// Remove search symbol
								actualElement.removeClass("searching");
								response(data);
							}
							},
						
					});
				},
				focus : function(event, ui) {
					jQuery(this).val(ui.item.label);
					return false;
				},
				select : function(event, ui) {
					jQuery(this).val(ui.item.label);
					jQuery(this).parent().next().val(ui.item.id);
					activateRow(trow, tbody, cols);
					addRow(tbody, cols);
					return false;
				},
				change : function(event, ui) {
					// if the value of the textbox does not match a
					// suggestion, clear its value
					if (jQuery(
							".ui-menu-item-label:textEquals('"
									+ jQuery(this).val().replace(/([{}\(\)\^$&.\*\?\/\+\|\[\\\\]|\]|\-)/g, '\\$1') + "')").size() == 0) {
					}
				}
			}).live(
			'keydown',
			function(e) {
				var keyCode = e.keyCode || e.which;
				// if TAB or RETURN is pressed and the text in the
				// textbox does not match a suggestion, set the value of
				// the textbox to the text of the first suggestion
				if ((keyCode == 9 || keyCode == 13)){
					if (jQuery(
							".ui-menu-item-label:textEquals('"
									+ jQuery(this).val().replace(/([{}\(\)\^$&.\*\?\/\+\|\[\\\\]|\]|\-)/g, '\\$1') + "')").size() == 0) {

					} else {
						// Magic
						var item = jQuery(this).data('autocomplete').selectedItem;
						
						activateRow(trow, tbody, cols);
						addRow(tbody, cols);
						jQuery(this).focus();
					}

				}
			}).focus(function() {
		// jQuery(this).autocomplete("search");
	}).keyup(function(){
		//monitor field and remove class 'notfound' if its length is less than 2
		if(this.value.length<2){
			jQuery(this).removeClass("notfound");
		}
	}).data("autocomplete")._renderItem = function(ul, item) {
		
		var term = this.term.replace(/[aou]/, function(m) {
			// to find with term 'wasser' -> 'wasser' and 'gewasesser'
			var hash = {
				'a' : '(Ã¤|a)',
				'o' : '(Ã¶|o)',
				'u' : '(Ã¤|u)'
			};
			return hash[m];
		})

		// highlighting of matches
		var label = item.name_de.replace(new RegExp(term, 'ig'), "<span class='ui-term-match'>$&</span>");

		var buffer = this.term.split(" ");
		label += " [" + item.genus.replace(new RegExp(buffer[0], 'ig'), "<span class='ui-term-match'>$&</span>");
		label += " " + item.species.replace(new RegExp(buffer[1], 'ig'), "<span class='ui-term-match'>$&</span>") + "]";

		// If there is a old name (synonym)
		var old_label = "";
		if (item.old_name_de != undefined) {
			old_label = item.old_name_de.replace(new RegExp(term, 'ig'), "<span class='ui-term-match'>$&</span>");

			old_label += " [" + item.old_genus.replace(new RegExp(buffer[0], 'ig'), "<span class='ui-term-match'>$&</span>");
			old_label += " " + item.old_species.replace(new RegExp(buffer[1], 'ig'), "<span class='ui-term-match'>$&</span>") + "]";
			old_label = "<span>&nbsp;&nbsp;&nbsp;&nbsp; Synonym: " + old_label + "</span>";
		}

		return jQuery("<li></li>").data("item.autocomplete", item).append(
				"<a><div class='ui-menu-item-label'>" + label + "</div>" + old_label + "<div class='ui-menu-item-name'>" + name
						+ "</div></a>").appendTo(ul);
	};
	return trow;
}
