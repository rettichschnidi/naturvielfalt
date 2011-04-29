<?php 

/**
 * 
 * Enter description here ...
 */
function inventory_entries_ajax() {
	$data = $_POST;
	$return = array();
	if(isset($data["deleteRows"]))
	{
		foreach($data["deleteRows"] as $rowId)
		{
			db_delete('inventory_entry')
			->condition('id', $rowId)
			->execute();
		}
	}
	
	// Save the changed rows
	if(isset($data["addRows"]))
	{
		foreach($data["addRows"] as $invIdKey => $invIdValue)
		{
			$inventoryId = $invIdKey;
			if(substr($invIdKey, 0, 8) == "inv_new_")
			{// New inventory
				$inventoryId = db_insert('inventory')
				->fields (
					array (
						'inventory_type_id' => $invIdValue['invTypeId'],
						'head_inventory_id' => $data["headInventoryId"],
					)
				)
				->execute();
				$return[$invIdKey] = $inventoryId;
			}

			foreach($invIdValue as $rowIdKey => $rowIdValue)
			{
				$inventoryEntryId = $rowIdKey;
				// Save entry
				if(substr($rowIdKey, 0, 8) == "row_new_") {
					// New entry
					$inventoryEntryId = db_insert('inventory_entry')
					->fields (
						array (
							'organism_id' => $rowIdValue['orgId'],
							'inventory_id' => $inventoryId,
						)
					)
					->execute();
					$return[$rowIdKey] = $inventoryEntryId;
				}
				foreach($rowIdValue as $attrIdKey => $attrIdValue) {
					if(substr($attrIdKey, 0, 4) == "col_") {
						$attrId = substr($attrIdKey, 4);
						
						// get format of attribute
						$result = db_query ('
							SELECT af.format FROM attribute_format af
							LEFT JOIN inventory_type_attribute ita ON af.id = ita.attribute_format_id
							WHERE ita.inventory_type_id = :itid AND ita.id = :iaid;',
							array(':itid' => $invIdValue['invTypeId'], ':iaid' => $attrId)
						);
						if(!empty($result)) {
							foreach ($result as $record) {
								$attrFormat = $record->format;
							}
						}
						$dbCol = 'value';
						if($attrFormat == 'dropdown') {
							$dbCol = 'inventory_type_attribute_dropdown_value_id';
							if($attrIdValue == 0)  $attrIdValue = NULL;
						}
						
						// Save entry
						if(substr($rowIdKey, 0, 8) == "row_new_") {
							// new attribute entry
							$inventoryAttributEntryId = db_insert('inventory_type_attribute_inventory_entry')
							->fields (
								array (
									'inventory_entry_id' => $inventoryEntryId,
									$dbCol => $attrIdValue,
									'inventory_type_attribute_id' => $attrId,
								)
							)
							->execute();
						}
						else {
							// modify existing attribute entry
							$affectedRows = db_update('inventory_type_attribute_inventory_entry')
							->condition(
								db_and()
								->condition('inventory_entry_id', $inventoryEntryId, '=')
								->condition('inventory_type_attribute_id', $attrId, '=')
							)
							->fields(array( $dbCol => $attrIdValue ))
							->execute();
						}
					}
				}
			}
		}
	}
	
	drupal_json_output($return);
}

?>