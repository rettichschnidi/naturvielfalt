<?php 
function inventory_show ($head_inventory_id) {
	
	drupal_set_title(getInventoryTitle($head_inventory_id)." (".$head_inventory_id.")");
	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');
	
	//Display Inventory Basics
	$inventories = db_query('
		SELECT head_inventory.name,description,shared,create_time,modify_time, u.name "owner"
		FROM head_inventory
		LEFT JOIN users u on head_inventory.owner_id = u.uid
		WHERE id = :id;', array(':id' => $head_inventory_id)
	);

	foreach($inventories as $inventory) {
		$row = array();
		$row[] = $inventory->owner;
		$row[] = $inventory->description;
		$row[] = date("j.m.Y", strtotime($inventory->modify_time));
		$row[] = date("j.m.Y", strtotime($inventory->create_time));
		If($inventory->shared == "t") {
			$row[] = t('Yes');
		}
		else {
			$row[] = t('No');
		}
		$rows[] = $row;
	}
	$header = array(t('Owner'),t('Description'),t('Modified on'),t('Created on'),t('Public'));
	$render_array['inventories'] = array(
	    '#theme' => 'table',
		'#header' => $header,
	    '#rows' => $rows,
	);
	
	$rows = '';	
	//Display Area Details	
	$areas = db_query('
		SELECT field_name,locality,zip, area.id
		FROM area
		LEFT JOIN head_inventory h on area.id = h.area_id
		WHERE h.id = :id;', array('id' => $head_inventory_id)
	);	
	
	foreach($areas as $area) {
		$row = array();
		$row[] = $area->field_name;
		$row[] = $area->locality;
		$row[] = $area->zip;
		$rows[] = $row;
	}
	$header = array(t('Area'),t('City'),t('ZIP'));
	$render_array['areas'] = array(
	    '#theme' => 'table',
		'#header' => $header,
	    '#rows' => $rows,
	);
	$rows = '';
	$row = '';	

	// Get inventory organism entries
	$inventoryEntries = db_query('
		SELECT e.id, e.organism_id, fauna.name_de faunanamede, fauna.genus, fauna.species, o.organism_type, flora.name_de floranamede, flora."Gattung" floragattung, flora."Art" floraart, itaie.value, af.name af_type, ita.name value_name, itadv.value dvalue, fauna.protection_ch, frl.red_list_ch
		FROM inventory_entry e
		LEFT JOIN inventory_type_attribute_inventory_entry itaie on e.id = itaie.inventory_entry_id
		LEFT JOIN inventory_type_attribute ita on itaie.inventory_type_attribute_id = ita.id
		LEFT JOIN attribute_format af on ita.attribute_format_id = af.id
		LEFT JOIN inventory_type_attribute_dropdown_value itadv on itaie.inventory_type_attribute_dropdown_value_id = itadv.id
		LEFT JOIN inventory i on e.inventory_id = i.id
		LEFT JOIN organism o on e.organism_id = o.id
		LEFT JOIN fauna_organism fauna ON o.organism_id = fauna.id
		LEFT JOIN flora_organism flora ON o.organism_id = flora.id
		LEFT JOIN flora_red_list frl ON flora.id = frl.flora_organism_id
		WHERE i.head_inventory_id = :id ORDER BY fauna.name_de, e.id ,af.name ASC;'
		,array(':id' => $head_inventory_id)
	);
	

	foreach($inventoryEntries as $inventoryEntry) {
		//check organism red list state and set variable
		if($inventoryEntry->protection_ch == '0' OR $inventoryEntry->protection_ch == '') {
			$fauna_red_list = FALSE;
		}
		else {
			$fauna_red_list = TRUE;
		}
		if($inventoryEntry->red_list_ch=='0' OR $inventoryEntry->red_list_ch=='') {
			$flora_red_list = FALSE;
		}
		else {
			$flora_red_list = TRUE;
		}
		
		if($inventoryEntry->organism_type==1) {
			// it's organism type 1, hence fauna
			if($recordid!=$inventoryEntry->id) {
				$strCount = '';
				$strStatus = '';
				$recordid = $inventoryEntry->id;
				$strName=$inventoryEntry->faunanamede;
				$strLat=$inventoryEntry->genus." ".$inventoryEntry->species;
				if($inventoryEntry->value_name='Funddatum') {
					$strDate = $inventoryEntry->value; 
				}
				//if organism is protected only show details if permissions are granted
				if($fauna_red_list == FALSE OR $fauna_red_list == TRUE AND hasRedListPermission($head_inventory_id) == TRUE) {
					$row = array(
					'Art' => $strName,
					'Lateinisch' => $strLat,
					'Anzahl' => $strCount,
					'Status' => $strStatus,
					'Funddatum' => $strDate,
					);
					$rows[] = array('data' => $row, 'id' => 'organism_'. $inventoryEntry->organism_id);	
				}
				else {
					$row = array(
					'Art' => t('Protected organism'),
					'Lateinisch' => '',
					'Anzahl' => '',
					'Status' => '',
					'Funddatum' => '',
					);
					$rows[] = array('data' => $row, 'id' => 'protected');	
				}	
			} else {
				if($fauna_red_list == FALSE OR $fauna_red_list == TRUE AND hasRedListPermission($head_inventory_id) == TRUE) {
					if($inventoryEntry->value_name=='Anzahl') {
						// inventory id same as the last one => modify count field of last element in list 
						$strCount = $inventoryEntry->value;
						$rows[count($rows)-1]['data']['Anzahl'] = $rows[count($rows)-1]['data']['Anzahl'].$strCount;
					}
					if($inventoryEntry->value_name=='Status') {
						$strStatus = $inventoryEntry->dvalue;
						$rows[count($rows)-1]['data']['Status'] = $rows[count($rows)-1]['data']['Status'].$strStatus;
					}
				}
			} 
		}
		
		if($inventoryEntry->organism_type==2) {
			// it's organism type 2, hence flora
			if($recordid!=$inventoryEntry->id) {	
				$strPopulation = '';
				$strPhase = '';
				$strVitality = '';
				$strAsize = '';
				$strOrigin = '';
				$recordid = $inventoryEntry->id;
				$strName=$inventoryEntry->floranamede;
				$strLat=$inventoryEntry->floragattung." ".$inventoryEntry->floraart;
				if($inventoryEntry->value_name='Funddatum') {
					$strDate = $inventoryEntry->value; 
				}
				//if organism is protected only show details if permissions are granted
				if($flora_red_list == FALSE OR $flora_red_list == TRUE AND hasRedListPermission($head_inventory_id) == TRUE) {
					$row = array(
					'Art' => $strName,
					'Lateinisch' => $strLat,
					'Population' => $strPopulation,
					'Area size' => $strAsize,
					'Origin' => $strOrigin,
					'Phenological Phase' => $strPhase,
					'Vitality' => $strVitality,
					'Funddatum' => $strDate,
					);
					$rows2[] = array('data' => $row, 'id' => 'organism_'. $inventoryEntry->organism_id);
				}
				else {
					$row = array(
					'Art' => t('Protected Organism'),
					'Lateinisch' => '',
					'Population' => '',
					'Area size' => '',
					'Origin' => '',
					'Phenological Phase' => '',
					'Vitality' => '',
					'Funddatum' => '',
					);
					$rows2[] = array('data' => $row, 'id' => 'protected');
				}
				
			} else {
				if($flora_red_list == FALSE OR $flora_red_list == TRUE AND hasRedListPermission($head_inventory_id) == TRUE) {
					// inventory id same as the last one => modify count field of last element in list 
					if($inventoryEntry->value_name=='Artmächtigkeit') {
						$strPopulation = $inventoryEntry->dvalue;
						$rows2[count($rows2)-1]['data']['Population'] = $rows2[count($rows2)-1]['data']['Population'].$strPopulation;
					}
					if($inventoryEntry->value_name=='Phänologische Phasen') {
						$strPhase = $inventoryEntry->dvalue;
						$rows2[count($rows2)-1]['data']['Phenological Phase'] = $rows2[count($rows2)-1]['data']['Phenological Phase'].$strPhase;
					}
					if($inventoryEntry->value_name=='Vitalität') {
						$strVitality = $inventoryEntry->dvalue;
						$rows2[count($rows2)-1]['data']['Vitality'] = $rows2[count($rows2)-1]['data']['Vitality'].$strVitality;
					}					
					if($inventoryEntry->value_name=='Flächengrösse') {
						$strAsize = $inventoryEntry->dvalue;
						$rows2[count($rows2)-1]['data']['Area size'] = $rows2[count($rows2)-1]['data']['Area size'].$strAsize;
					}	
					if($inventoryEntry->value_name=='Herkunft der Population') {
						$strOrigin = $inventoryEntry->dvalue;
						$rows2[count($rows2)-1]['data']['Origin'] = $rows2[count($rows2)-1]['data']['Origin'].$strOrigin;
					}
				}
			}	
		}
	}

	//Define tables only if not empty
	if($rows!='') {
		$render_array['Fauna'] = array(
	  		'#type' => 'fieldset', 
	  		'#title' => t('Fauna'), 
	  		'#weight' => 3, 
	  		'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array (
				'js' => array (
					'misc/form.js',
					'misc/collapse.js',
				),
			),
			'#attributes' => array(
				'class' => array('collapsible'),
	        ),
		);
		
		$header = array(t('Name'),t('Latin name'),t('Amount'),t('State'),t('Date'));
		$render_array['Fauna']['Art'] = array(
   			'#theme' => 'datatable',
			'#header' => $header,
    		'#rows' => $rows,
			'#id_table' => 'fauna',
		);
	}
	if($rows2!='') {
		$render_array['Flora'] = array(
			'#type' => 'fieldset', 
			'#title' => t('Flora'), 
			'#weight' => 3, 
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array (
				'js' => array (
					'misc/form.js',
					'misc/collapse.js',
				),
			),
			'#attributes' => array(
				'class' => array('collapsible'),
	        ),
		);

		$header = array(t('Name'),t('Latin name'),t('Population'),t('Area size'),t('Origin'),t('Phenological Phase'),t('Vitality'),t('Date'));
		$render_array['Flora']['Art'] = array(
			'#theme' => 'datatable',
			'#header' => $header,
			'#rows' => $rows2,
			'#id_table' => 'flora',
		);		
	}
	if(hasRedListPermission($head_inventory_id) == FALSE) {
		$render_array['RedList'] = array(
			'#markup' => t('You do not have permission to display protected organisms from this inventory'));
	}
	return $render_array;
}

function getInventoryTitle($head_inventory_id) {
	$inventory_title = '-';

	$inventories = db_query('	SELECT name
								FROM head_inventory
								WHERE id = :id;', array(':id' => $head_inventory_id));
	foreach($inventories as $inventory) {
		if(!is_null($inventory->name))
		$inventory_title =  $inventory->name;
	}

	return $inventory_title;
}

//check if user is allowed to display red list organisms -> either by owning the inventory or group permissions
function hasRedListPermission($head_inventory_id) {
	global $user;
	//query group membership of user
	$permissions = db_query(
		'SELECT sgi.read, sgi.write, sgi.show_red, hi.owner_id
		FROM sgroup_inventory sgi
		LEFT JOIN head_inventory hi ON sgi.hiid = hi.id
		LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
		WHERE sgi.hiid = :invid AND sgu.uid = :userid',
		array(':invid' => $head_inventory_id, ':userid' => $user->uid)
	);
	
	//query inventory owner
	$owner = db_query('
		SELECT owner_id
		FROM head_inventory
		WHERE id = :hiid',
		array(':hiid' => $head_inventory_id)
	);
	$owner = $owner->fetch()->owner_id;
	
	if($owner == $user->uid) {
		return TRUE;
	}
	else {
		foreach($permissions as $permission) {
			if($permission->show_red == '1') {
				return TRUE;
			}
		}	
	}

	
}

?>