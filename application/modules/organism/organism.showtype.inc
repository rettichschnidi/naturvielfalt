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
		$header = array(t('Family'),t('Name'),t('Latin'),t('Author'),t('id'));
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
			'select organism.id, organism.organism_type, flora_organism.name, flora_organism."Familie", flora_organism."Gattung", flora_organism."Art", flora_organism."Autor", flora_organism.name_de
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
			$row[] = l($record->family,"organism/".$record->id);
			$row[] = l($record->genus,"organism/".$record->id);
			$row[] = l($record->species,"organism/".$record->id);
			$row[] = l($record->name_de,"organism/".$record->id);
			$row[] = $record->id;
			
			$rows[] = $row;
		} else {
			$row = array();
			$row[] = l($record->familie,"organism/".$record->id);
			$row[] = l($record->name_de,"organism/".$record->id);
			$row[] = l($record->gattung." ".$record->art,"organism/".$record->id);
			$row[] = $record->autor;
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