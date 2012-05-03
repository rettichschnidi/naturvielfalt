<?php
/**
 * @file organism.inc
 * @author Reto Schneider, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

/**
 * Return an organism, just lat&lang name
 * maybe later we need more fields
 *
 * @return array
 */
function organism_get($organismid) {
	global $user;
	$resultarray = array();
	return db_query(
			'SELECT
				osn.name name_lat,
				ol.name name_lang
			FROM
				{organism_scientific_name} osn
				LEFT JOIN {organism_lang} ol ON ol.organism_id=:oid AND ol.languages_language=:lang_code
			WHERE
				osn.organism_id = :oid',
			array(':oid' => $organismid, ':lang_code' => $user->language))->fetchAssoc();
}

/**
 * Return all classification levels of a given organism.
 * 
 * @param $classificationid
 * 	Id of the organism in question.
 */
function organism_get_available_classification_levels($classificationid) {
	$classificationdata = array();
	$attributevalueresult = db_query(
		'SELECT
			ocl2.id,
			ocl2.left_value,
			ocl2.name
		FROM
			{organism_classification} oc
			JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.id
			JOIN {organism_classification_level} ocl2 ON ocl2.prime_father_id = ocl.prime_father_id
		WHERE
			oc.id = :organismid
		ORDER BY
			left_value;',
		array(':organismid' => $classificationid));
	return $attributevalueresult->fetchAll();
}
?>
