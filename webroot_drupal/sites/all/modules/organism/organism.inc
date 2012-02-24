<?php
/**
 * @file organism.module
 * @author Marco Zimmerli, 2011
 * @author Reto Schneider, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */
/**********************************
 *  Hook implementations
 *********************************/

/**
 * Return all scientific names of an organism
 *
 * @return array
 * 	array(
 * 		[0] => array(
 * 				'name' => "NAME1",
 * 				'id' => "ID1",
 * 				'organism_id' => 'ORGANISM_ID1"
 * 			),
 * 		[1] => array(
 * 				'name' => "NAME2",
 * 				'id' => "ID2",
 * 				'organism_id' => 'ORGANISM_ID2"
 * 			)
 * 		)
 */
function organism_get_names_of_organism($organismid) {
	$resultarray = array();
	$result = db_query(
		'SELECT
			id,
			name
		FROM
			{organism_scientific_name} osn
		WHERE
			osn.organism_id = :oid',
		array(':oid' => $organismid));

	foreach ($result as $record) {
		$resultarray[] = array(
				'id' => $record->id,
				'name' => $record->name,
				'organism_id' => $organismid
		);
	}
	return $resultarray;
}

/**
 * Return a single scientific name of an organism, even if there are many synonyms
 *
 * @return array
 * 	array(
 * 			'name' => "NAME1",
 * 			'id' => "ID1",
 * 			'organism_id' => 'ORGANISM_ID1"
 * 			)
 */
function organism_get_single_name_of_organism($organismid) {
	$namesarray = organism_get_names_of_organism($organismid);
	if (empty($namesarray)) {
		return array();
	}
	$shortestNameArray = $namesarray[0];
	foreach ($namesarray as $currentnamearray) {
		if (strlen($shortestNameArray['name'])
				> strlen($currentnamearray['name'])) {
			$shortestNameArray = $currentnamearray;
		}
	}
	return $shortestNameArray;
}

?>
