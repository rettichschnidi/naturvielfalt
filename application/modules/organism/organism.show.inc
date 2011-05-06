<?php
function organism_show($organismId) {
	$result = db_query(
		"select organism.id, organism.organism_type, flora_organism.name flora_name, fauna_organism.genus fauna_genus, fauna_organism.species fauna_species, fauna_organism.name_de fauna_namede
			from organism
			left join flora_organism ON organism.organism_id = flora_organism.id
			left join fauna_organism ON organism.organism_id = fauna_organism.id
			where organism.id = :id limit 1",
			array(":id" => $organismId)
	);

	$row = array();
	$record = $result->fetch();

	if($record->organism_type==1){
		// yiiiiiipeee, it is Fauna
		$row[] = $record->fauna_genus;
		$row[] = $record->fauna_species;
		$row[] = $record->fauna_namede;
		$row[] = $record->id;

		$header = array(t('Genus'),t('Species'),t('Name'),t('Id'));
		drupal_set_title( $record->fauna_genus." ".$record->fauna_species);
	} else {
		$row[] = $record->flora_name;
		$row[] = $record->id;

		$header = array(t('Name'),t('id'));
		drupal_set_title( $record->flora_name );
	}
	$rows[] = $row;
	
	$render_array['organisms'] = array(
    '#theme' => 'table', 
	'#header' => $header,
    '#rows' => $rows, 
	); 
	return $render_array;
}
?>