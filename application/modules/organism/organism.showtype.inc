<?php
function organism_showtype($typeId) {
	drupal_set_title(t('Show Organism'));
/*	$areaResult = db_query("SELECT * FROM area WHERE id = :id", array(':id' => $areaId));
	$area = $areaResult->fetch();
	drupal_set_title($area->field_name);
	$output = theme('item_list', array('title' => $area->field_name)); */
		
	/*
	$result = db_query("SELECT name FROM inventory_type where id=:id",
		array(':id' => $typeId)
	);
	*/
	
	if( $typeId <> 16){
		$header = array(t('Genus'),t('Spezies'),t('Name'),t('id'));	
	} else {
		$header = array(t('Name'),t('id'));
	}
	
	if($typeId <> 16){
		$result = db_query(
			"select organism.id, organism.organism_type, inventory_type.name, fauna_organism.genus, fauna_organism.species, fauna_organism.name_de
				from organism
				left join fauna_organism ON organism.organism_id = fauna_organism.id
				left join inventory_type ON organism.inventory_type_id = inventory_type.id
				where inventory_type.id = :id AND organism.organism_type = 1 order by fauna_organism.genus",
				array(':id' => $typeId)
		);
	} else {
		$result = db_query(
			"select organism.id, organism.organism_type, inventory_type.name, flora_organism.name
				from organism
				left join flora_organism ON organism.organism_id = flora_organism.id
				left join inventory_type ON organism.inventory_type_id = inventory_type.id
				where inventory_type.id = :id AND organism.organism_type = 2 order by flora_organism.name",
				array(':id' => $typeId)
		);
	}

foreach ($result as $record) {
		if ($typeId <> 16){			
			$row = array();
			//$row[] = l($record->name,"organism/type/".$record->id);
			$row[] = $record->genus;
			$row[] = $record->species;
			$row[] = $record->name_de;
			$row[] = $record->id;
			//$link = l(t('new inventory'), 'inventory/new');
			
			$rows[] = $row;
		} else {
			$row = array();
			//$row[] = l($record->name,"organism/type/".$record->id);
			$row[] = $record->name;
			$row[] = $record->id;
			//$link = l(t('new inventory'), 'inventory/new');
			
			$rows[] = $row;			
		}
	}
	
	$render_array['organisms'] = array(
    '#theme' => 'table', 
	'#header' => $header,
    '#rows' => $rows,
	);
	return $render_array;
}
?>