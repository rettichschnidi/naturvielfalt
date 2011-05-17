<?php 
function inventory_show ($head_inventory_id) {
	
	drupal_set_title(getInventoryTitle($head_inventory_id)." (".$head_inventory_id.")");
	
	//Display Inventory Basics
	$inventories = db_query('	SELECT name,description,shared,create_time,modify_time
									FROM head_inventory
									WHERE id = :id;', array(':id' => $head_inventory_id));

	foreach($inventories as $inventory) {
		$row = array();
		$row[] = $inventory->name;
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

	$header = array(t('Inventory name'),t('Description'),t('Modified on'),t('Created on'),t('Public'));
	
	$render_array['inventories'] = array(
    '#theme' => 'table',
	'#header' => $header,
    '#rows' => $rows,
	);
	
	$rows = '';	
	
	//Display Area Details	
	$areas = db_query('SELECT field_name,locality,zip, area.id
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

	// Get Fauna inventory entries
	$inventoryEntries = db_query('
		SELECT e.id, e.organism_id, fauna.name_de faunanamede, fauna.genus, fauna.species, o.organism_type, flora.name_de floranamede, flora."Gattung" floragattung, flora."Art" floraart, itaie.value, af.name af_type, ita.name value_name, itadv.value dvalue
			FROM inventory_entry e
			LEFT JOIN inventory_type_attribute_inventory_entry itaie on e.id = itaie.inventory_entry_id
			LEFT JOIN inventory_type_attribute ita on itaie.inventory_type_attribute_id = ita.id
			LEFT JOIN attribute_format af on ita.attribute_format_id = af.id
			LEFT JOIN inventory_type_attribute_dropdown_value itadv on itaie.inventory_type_attribute_dropdown_value_id = itadv.id
			LEFT JOIN inventory i on e.inventory_id = i.id
			LEFT JOIN organism o on e.organism_id = o.id
			LEFT JOIN fauna_organism fauna ON o.organism_id = fauna.id
			LEFT JOIN flora_organism flora ON o.organism_id = flora.id
		WHERE i.head_inventory_id = :id ORDER BY fauna.name_de, e.id ,af.name ASC;'
		,array(':id' => $head_inventory_id)
	);

	foreach($inventoryEntries as $inventoryEntry) {
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
				$row = array(
				'Art' => l($strName, "organism/".$inventoryEntry->organism_id),
				'Lateinisch' => $strLat,
				'Anzahl' => $strCount,
				'Status' => $strStatus,
				'Funddatum' => $strDate,
				);
				$rows[] = $row;	
			} else {
				if($inventoryEntry->value_name=='Anzahl') {
					// inventory id same as the last one => modify count field of last element in list 
					$strCount = $inventoryEntry->value;
					$rows[count($rows)-1]['Anzahl'] = $rows[count($rows)-1]['Anzahl'].$strCount;
				}
				if($inventoryEntry->value_name=='Status') {
					$strStatus = $inventoryEntry->dvalue;
					$rows[count($rows)-1]['Status'] = $rows[count($rows)-1]['Status'].$strStatus;
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
				$row = array(
				'Art' => l($strName, "organism/".$inventoryEntry->organism_id),
				'Lateinisch' => $strLat,
				'Population' => $strPopulation,
				'Area size' => $strAsize,
				'Origin' => $strOrigin,
				'Phenological Phase' => $strPhase,
				'Vitality' => $strVitality,
				'Funddatum' => $strDate,
				);
				$rows2[] = $row;
			} else {
				// inventory id same as the last one => modify count field of last element in list 
				if($inventoryEntry->value_name=='Artmächtigkeit') {
					$strPopulation = $inventoryEntry->dvalue;
					$rows2[count($rows2)-1]['Population'] = $rows2[count($rows2)-1]['Population'].$strPopulation;
				}
				if($inventoryEntry->value_name=='Phänologische Phasen') {
					$strPhase = $inventoryEntry->dvalue;
					$rows2[count($rows2)-1]['Phenological Phase'] = $rows2[count($rows2)-1]['Phenological Phase'].$strPhase;
				}
				if($inventoryEntry->value_name=='Vitalität') {
					$strVitality = $inventoryEntry->dvalue;
					$rows2[count($rows2)-1]['Vitality'] = $rows2[count($rows2)-1]['Vitality'].$strVitality;
				}					
			}
			if($inventoryEntry->value_name=='Flächengrösse') {
				$strAsize = $inventoryEntry->dvalue;
				$rows2[count($rows2)-1]['Area size'] = $rows2[count($rows2)-1]['Area size'].$strAsize;
			}	
			if($inventoryEntry->value_name=='Herkunft der Population') {
				$strOrigin = $inventoryEntry->dvalue;
				$rows2[count($rows2)-1]['Origin'] = $rows2[count($rows2)-1]['Origin'].$strOrigin;
			}
		}
	}

	//Define tables only if not empty
	if($rows!='') {
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
		
		$header = array(t('Name'),t('Latin name'),t('Amount'),t('State'),t('Date'));
		$render_array['Flora']['Art'] = array(
   			'#theme' => 'datatable',
			'#header' => $header,
    		'#rows' => $rows,
			'#id_table' => 'flora',
		);
	}
	if($rows2!='') {
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

		$header = array(t('Name'),t('Latin name'),t('Population'),t('Area size'),t('Origin'),t('Phenological Phase'),t('Vitality'),t('Date'));
		$render_array['Fauna']['Art'] = array(
			'#theme' => 'datatable',
			'#header' => $header,
			'#rows' => $rows2,
			'#id_table' => 'fauna',
		);		
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

?>