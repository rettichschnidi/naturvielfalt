<?php
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