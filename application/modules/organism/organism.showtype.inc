<?php
function organism_showtype($typeId) {
	$inventory_type= db_query(
		"SELECT name FROM inventory_type WHERE id=:id",
		array(':id'=> $typeId)
	);
	drupal_set_title(t( $inventory_type->fetch()->name ));

	
	if( $typeId <> 16){
		$header = array(t('Genus'),t('Spezies'),t('Name'),t('id'));	
	} else {
		$header = array(t('Name'),t('id'));
	}
	
	if($typeId <> 16){
		$result = db_query(
			"select organism.id, organism.organism_type, fauna_organism.genus, fauna_organism.species, fauna_organism.name_de
				from organism
				left join fauna_organism ON organism.organism_id = fauna_organism.id
				left join inventory_type ON organism.inventory_type_id = inventory_type.id
				where inventory_type.id = :id AND organism.organism_type = 1 order by fauna_organism.genus",
				array(':id' => $typeId)
		);
	} else {
		$result = db_query(
			"select organism.id, organism.organism_type, flora_organism.name
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
			$row[] = l($record->genus,"organism/".$record->id);
			$row[] = $record->species;
			$row[] = $record->name_de;
			$row[] = $record->id;
			
			$rows[] = $row;
		} else {
			$row = array();
			$row[] = l($record->name,"organism/".$record->id);
			$row[] = $record->id;
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