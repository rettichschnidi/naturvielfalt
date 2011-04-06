<?php
/**
 * Menu callback. This will be called if someone visits for example drupal/inventory/3.
 */
function inventory_form() {
	
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-ui-1.8.6.custom.min.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-texotela-numeric.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/ext/ext-jquery-adapter-debug.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/ext/ext-base-debug.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/JSON.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/util.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/inventory.js');

	drupal_add_css(drupal_get_path('module', 'inventory') . '/css/main.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));

	// inventories ajax form
	$form['inventories']['#markup'] = '<div id="inventories"></div>';
		 
	$form['head_inventory_id'] = array('#type' => 'hidden', '#value' => arg(1));

	$options = array();

	$result = db_query("SELECT it.id, it.name
		FROM inventory_type it
		ORDER BY it.id desc;");

	foreach ($result as $record) {
		$options[$record->id] = $record->name;
	}

	$form['inventory_types'] = array(
    '#type' => 'select',
    '#title' => t('Inventargruppe'),
    '#default_value' => variable_get('name', 259200),
    '#options' => $options,
	);

	$form['add_inventory']['#markup'] = '<div id="add_inventory"></div><input onclick="onAddInventory();" type="button" id="add-inventory" name="button" value="add inventory">';

	return $form;
}

function inventory_show($head_inventory_id) {

	if(inventory_permission($head_inventory_id)==true)
	{

		drupal_set_title(t(getInventoryTitle($head_inventory_id)." (".$head_inventory_id.")"));
		$json = getInitialInventoryJSON($head_inventory_id);

		drupal_add_js('var exInvs = ' . $json, 'inline');
		drupal_add_js('jQuery(document).ready(function(){initInventory();});', 'inline');
		return drupal_get_form('inventory_form');
	}
	else drupal_set_title(t("no permission"));
}

function inventory_permission($head_inventory_id)
{
	$result = db_query("	SELECT * FROM head_inventory hi 
								WHERE ((hi.shared='t') OR  (hi.owner_id=1))
								AND hi.id = :id"
								, array(':id' => $head_inventory_id)
	);
	
	/*
 	if (db_num_rows($result) == 0) {
    	return false;
  	}
  	*/
  	return true;
}

function getInventoryTitle($head_inventory_id) {
	$inventory_title = t("no name");
	
	$inventories = db_query('	SELECT name 
								FROM head_inventory
								WHERE id = :id;', array(':id' => $head_inventory_id));
	foreach($inventories as $inventory) {
		if(!is_null($inventory->name))
			$inventory_title =  $inventory->name;
	}
	
	return $inventory_title;
}

function getInitialInventoryJSON($head_inventory_id) {

	$invArr = array();

	$inventories = db_query('SELECT id
							FROM inventory 
							WHERE head_inventory_id = :id;', array(':id' => $head_inventory_id));
	foreach($inventories as $inventory) {

		$inventory_id = $inventory->id;
		$invArr[$inventory_id] = array();

		// Get the inventory column description
		$invArr[$inventory_id]["invDesc"] = getInventoryColumnDescription($inventory_id);

		// Get inventory entries
		$inventoryEntries = db_query('SELECT e.id, e.organism_id, fo.name_de
		FROM inventory_entry e
		LEFT JOIN fauna_organism fo ON e.organism_id = fo.id
		WHERE e.inventory_id = :id;', array(':id' => $inventory_id));
		foreach($inventoryEntries as $inventoryEntry) {
			$inventoryEntryId = $inventoryEntry->id;
			$invArr[$inventory_id][$inventoryEntryId] = array();
			$invArr[$inventory_id][$inventoryEntryId]["orgId"] = $inventoryEntry->organism_id;
			$invArr[$inventory_id][$inventoryEntryId]["label"] = $inventoryEntry->name_de;

			// Get inventory columns
			$entries = db_query('SELECT * FROM inventory_type_attribute_inventory_entry WHERE inventory_entry_id = :id', array(':id' => $inventoryEntryId));
			foreach ($entries as $entry) {
				// Dropdown
				$colVal = "";
				if($entry->inventory_type_attribute_dropdown_value_id > 0) {
					$colVal = $entry->inventory_type_attribute_dropdown_value_id;
				} else {
					$colVal = $entry->value;
				}
				$invArr[$inventory_id][$inventoryEntryId]["col_".$entry->inventory_type_attribute_id] = isset($colVal) ? $colVal : "";
			}
		}
	}
	
	$result = drupal_json_encode($invArr);

	return $result;
}

function getInventoryColumnDescription($inventoryId){
	$invJson = array();

	$resInventory = db_query_range("SELECT t.name, i.inventory_type_id FROM inventory i LEFT JOIN inventory_type t ON i.inventory_type_id = t.id WHERE i.id = :id", 0, 1, array(':id' => $inventoryId));
	foreach ($resInventory as $inventory) {
		$typeId = $inventory->inventory_type_id;
		$invJson["name"] = $inventory->name;
		$invJson["id"] = $inventory->inventory_type_id;
	}

	$resAttributes = db_query('SELECT a.id, a.name, f.format
		FROM inventory_type_attribute a 
		LEFT JOIN attribute_format f ON a.attribute_format_id = f.id 
		WHERE inventory_type_id = :typeId', array(':typeId' => $typeId));

	$i = 0;
	foreach ($resAttributes as $attribute) {
		$i++;
		$invJson["cols"][$i] = array(
			"name" => $attribute->name,
			"id" => $attribute->id,
			"format" => $attribute->format,
		);

		// If it is a dropdown list
		if($attribute->format == "dropdown") {
			$invJson["cols"][$i]["dropdown_values"] = array();
			$dropdownValues = db_query('SELECT * FROM inventory_type_attribute_dropdown_value WHERE inventory_type_attribute_id = :id', array(':id' => $attribute->id));
			foreach($dropdownValues as $dropdownValue) {
				array_push($invJson["cols"][$i]["dropdown_values"], array("id" =>$dropdownValue->id, "value" => $dropdownValue->value));
			}
		}

	}
	return $invJson;
}
?>