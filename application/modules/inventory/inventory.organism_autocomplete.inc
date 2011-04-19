<?php
function inventory_organism_autocomplete($invId,$term) {
	$termBSpace = substr($term, 0, strpos($term, " "));
	if($termBSpace == "") {
		$termBSpace = "99999999999";	
	}
	
	$termASpace = substr($term, strpos($term, " ") +1);
	
	$fauna = ($invId == 16 ? false : true);
	
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
      
		
	$items = array();
	foreach ($result as $record){
		$organismus['id']		=	$record->o_id;
		$organismus['genus']	=	($fauna ? $record->genus : $record->Gattung);
		$organismus['genus']	=	($organismus['genus'] == null ? "" : $organismus['genus']);
		$organismus['species'] 	=	($fauna ? $record->species : $record->Art);
		$organismus['species']	=	($organismus['species'] == null ? "" : $organismus['species']);
		$organismus['label']	=	$record->name_de." [".$organismus['genus']." ".$organismus['species']."]";
		$organismus['name_de']	=	($record->name_de == null ? "" : $record->name_de);
		$organismus['position']	=	1000;
		
		$organismsus_json[] = $organismus;
	}

	echo drupal_json_encode($organismsus_json);
	// [{"id":20806,"genus":"Trachemys","species":"scripta","label":"Rotwangen-Schmuckschildkr\u00f6te [Trachemys scripta]","name_de":"Rotwangen-Schmuckschildkr\u00f6te","position":1000}]


	$render_array['organisms'] = array(
    '#theme' => 'table', 
	'#header' => $header,
    '#rows' => $rows, 
	);
	return $render_array; 
}
?>