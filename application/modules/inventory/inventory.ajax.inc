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
							// modify existing attribute entry, if value not yet existing insert is used instead of update	
							$result_temp = db_query ('
							SELECT inventory_type_attribute_id FROM inventory_type_attribute_inventory_entry itaie
							LEFT JOIN inventory_entry ie ON itaie.inventory_entry_id = ie.id
							WHERE ie.id = :ieid AND inventory_type_attribute_id = :iaid',
							array(':ieid' => $inventoryEntryId, ':iaid' => $attrId)
							);	
							if(isset($result_temp->inventory_type_attribute_id)) {
								$affectedRows = db_update('inventory_type_attribute_inventory_entry') 
								->condition(
									db_and()
									->condition('inventory_entry_id', $inventoryEntryId, '=')
									->condition('inventory_type_attribute_id', $attrId, '=')
								)
								->fields(array( $dbCol => $attrIdValue ))
								->execute();
							}
							else {
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
						}
					}
				}
			}
		}
	}
	
	drupal_json_output($return);
}

/**
 * 
 * Enter description here ...
 */
function inventory_add_inventory($inventory_id) {
	$result = db_query ('
		SELECT id, name FROM inventory_type it
		WHERE it.id = :iid;',
		array(':iid' => $inventory_id)
	);
	
	if(!empty($result)) {
		foreach ($result as $record) {
			$return['name'] = $record->name;
			$return['id'] = $record->id;
		}
	}
	
	$result = db_query ('
		SELECT ita.id, ita.name, af.format, itadv.id dd_id, itadv.value FROM inventory_type_attribute ita
		LEFT JOIN attribute_format af ON af.id = ita.attribute_format_id
		LEFT JOIN inventory_type_attribute_dropdown_value itadv ON itadv.inventory_type_attribute_id = ita.id
		WHERE ita.inventory_type_id = :iid;',
		array(':iid' => $inventory_id)
	);
	if(!empty($result)) {
		$col_cnt = 0;
		$dd_cnt = 0;
		$id = 0;
		foreach ($result as $record) {
			if($id != $record->id) {
				$return['cols'][$col_cnt]['name'] = $record->name;
				$id = $return['cols'][$col_cnt]['id'] = $record->id;
				$return['cols'][$col_cnt]['format'] = $record->format;
				if($record->format == 'dropdown') {
					$dd_cnt=0;
					$act_col = $col_cnt;
					$return['cols'][$col_cnt]['dropdown_values'][$dd_cnt]['id'] = $record->dd_id;
					$return['cols'][$col_cnt]['dropdown_values'][$dd_cnt]['value'] = $record->value;
					$dd_cnt++;
				}
				$col_cnt++;
			}
			else {
				$return['cols'][$col_cnt-1]['dropdown_values'][$dd_cnt]['id'] = $record->dd_id;
				$return['cols'][$col_cnt-1]['dropdown_values'][$dd_cnt]['value'] = $record->value;
				$dd_cnt++;
			}
		}
	}
	drupal_json_output($return);
}

/**
 * 
 * Enter description here ...
 */
function inventory_sgroup_autocomplete($string) {
	global $user;
	
	$result = db_query('
		SELECT sg.sgid sg_id, sgroup_name FROM sgroup sg
		LEFT JOIN sgroup_users sgu ON sgu.sgid = sg.sgid
		WHERE sg.sgid NOT IN (SELECT sguEx.sgid FROM sgroup_users sguEx WHERE sguEx.uid = :uid)
		AND sgroup_name ILIKE :term
		ORDER BY sgroup_name
		', array(':uid' => $user->uid, ':term' => '%'.$string.'%')
	);

	$items = array();
	foreach ($result as $record) {
		$value = $record->sgroup_name;
		$key = $record->sg_id."|".$record->sgroup_name;
		$items[$key]=check_plain($value);
	}

	return drupal_json_output($items);
}

/**
 * ajax request to delete a member from a group.
 * security is done by drupal (hook_menu, access callback)
 * 
 * @param unknown_type $headInventoryId
 * @param unknown_type $sgid
 */
function inventory_delete_sgroup($headInventoryId, $sgid) {
	
	db_delete('sgroup_inventory')
	->condition('sgid', $sgid)
	->condition('hiid', $headInventoryId)
	->execute();

	return drupal_json_output('success');
}

/**
 * 
 * Enter description here ...
 */
function inventory_organism_autocomplete() {
	$invId = $_REQUEST['inv_id'];
	$term = $_REQUEST['term'];
	
	$termBSpace = substr($term, 0, strpos($term, " "));
	if($termBSpace == "") {
		$termBSpace = "99999999999";	
	}
	
	$termASpace = substr($term, strpos($term, " ") +1);
	
	$fauna = ($invId == 16 ? false : true);
	
	$organismus_json = array();
	
	if($fauna) {
		$result = db_query(
		 	'SELECT "t".*, "o"."organism_id" AS "ref_organism_id", "o"."id" AS "o_id", "f".* FROM "inventory_type_organism" AS "t"
			 	INNER JOIN "organism" AS "o" ON o.id = t.organism_id
		 		INNER JOIN "fauna_organism" AS "f" ON f.id = o.organism_id AND o.organism_type = 1
	
		 		WHERE (t.inventory_type_id = :inventoryTypeId
		 			AND (f.name_de ILIKE  :term
		 				OR (f.genus ILIKE  :term)
		 				OR (f.species ILIKE  :term)
		 				OR (f.genus ILIKE :termBSpace AND f.species ILIKE :termASpace)
		 			)
		 		);'
			,array(
				":term" => "%".$term."%",
				":termBSpace" => $termBSpace."%",
				":termASpace" => $termASpace."%",
				":inventoryTypeId" => $invId
		 	)
		);
	} else {
		$result = db_query(
			'SELECT "t".*, "o"."organism_id" AS "ref_organism_id", "o"."id" AS "o_id", "f".* FROM "inventory_type_organism" AS "t"
				INNER JOIN "organism" AS "o" ON o.id = t.organism_id
				INNER JOIN "flora_organism" AS "f" ON f.id = o.organism_id AND o.organism_type = 2
 
				WHERE (t.inventory_type_id = :inventoryTypeId
					AND (f.name_de ILIKE :term
						OR ("Gattung" ILIKE :term)
						OR ("Art" ILIKE :term)
						OR ("Gattung" ILIKE :termBSpace AND "Art" ILIKE :termASpace)
					)
				);'
			,array(
				":term" => "%".$term."%",
				":termBSpace" => $termBSpace."%",
				":termASpace" => $termASpace."%",
				":inventoryTypeId" => $invId
			)
		);
	}
	
	foreach ($result as $record){
		if(!$fauna){
			// if it is not the official name it is an alias
			// this will let the user search for aliases but saves only the 'parent', the
			// one where the official_flora_organism_id matches the organism_id
			while ($record->ref_organism_id != $record->official_flora_orfganism_id){
				$official_organismn = db_query(
					'SELECT "flora_organism".* FROM "flora_organism" WHERE "flora_organism"."id" = :official_flora_org_id'
					,array(":official_flora_org_id" => $record->official_flora_orfganism_id)
				)->fetch();
				
				$org = db_query(
					'SELECT "organism".* FROM "organism" WHERE (organism_type = 2 AND organism_id = :official_org_id)'
					,array(":official_org_id" => $official_organismn->id)
				)->fetch();

				$organismus['old_name_de'] = ($record->name_de == null ? "" : $record->name_de);
				$organismus['old_genus'] = $record->gattung;
				$organismus['old_species'] = $record->art;
				
				$record->official_flora_orfganism_id = $official_organismn->official_flora_orfganism_id;
				$record->ref_organism_id = $official_organismn->id;
				$record->name_de = $official_organismn->name_de;
				$record->art = $official_organismn->art;
				$record->gattung = $official_organismn->gattung;
				$record->o_id = $org->id;
			}
		}

		$organismus['id']		=	$record->o_id;
		$organismus['genus']	=	($fauna ? $record->genus : $record->gattung);
		$organismus['genus']	=	($organismus['genus'] == null ? "" : $organismus['genus']);
		$organismus['species'] 	=	($fauna ? $record->species : $record->art);
		$organismus['species']	=	($organismus['species'] == null ? "" : $organismus['species']);
		$organismus['label']	=	$record->name_de." [".$organismus['genus']." ".$organismus['species']."]";
		$organismus['name_de']	=	($record->name_de == null ? "" : $record->name_de);
		$organismus['position']	=	1000;
		
		$organismus_json[] = $organismus;
	}

	drupal_json_output($organismus_json);
}

?>