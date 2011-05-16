<?php
function organism_showtype($typeId) {
	$inventory_type= db_query(
		"SELECT name FROM inventory_type WHERE id=:id",
		array(':id'=> $typeId)
	);
	drupal_set_title(t( $inventory_type->fetch()->name ));

	
	if( $typeId <> 16){
		$header = array(t('Family'),t('Genus'),t('Species'),t('Name'),t('id'));	
	} else {
		$header = array(t('Family'),t('Name'),t('id'));
	}
	
	if($typeId <> 16){
		$result = db_query(
			"select organism.id, organism.organism_type, fauna_organism.genus, fauna_organism.species, fauna_organism.name_de, fauna_organism.family
				from organism
				left join fauna_organism ON organism.organism_id = fauna_organism.id
				left join inventory_type ON organism.inventory_type_id = inventory_type.id
				where inventory_type.id = :id AND organism.organism_type = 1 order by fauna_organism.genus",
				array(':id' => $typeId)
		);
	} else {
		$result = db_query(
			'select organism.id, organism.organism_type, flora_organism.name, flora_organism."Familie"
				from organism
				left join flora_organism ON organism.organism_id = flora_organism.id
				left join inventory_type ON organism.inventory_type_id = inventory_type.id
				where inventory_type.id = :id AND organism.organism_type = 2 order by flora_organism.name',
				array(':id' => $typeId)
		);
	}

foreach ($result as $record) {
		if ($typeId <> 16){			
			$row = array();
			$row[] = $record->family;
			$row[] = l($record->genus,"organism/".$record->id);
			$row[] = $record->species;
			$row[] = $record->name_de;
			$row[] = $record->id;
			
			$rows[] = $row;
		} else {
			$row = array();
			$row[] = $record->familie;
			$row[] = l($record->name,"organism/".$record->id);
			$row[] = $record->id;
			$rows[] = $row;
		}
	}
	
	$render_array['organisms'] = array(
    '#theme' => 'datatable', 
	'#header' => $header,
    '#rows' => $rows,
	'#id_table' => 'organisms',
	);
	return $render_array;
}
?>