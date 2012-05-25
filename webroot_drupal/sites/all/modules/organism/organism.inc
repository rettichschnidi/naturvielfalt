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
 * @param int $organismid
 * @return array
 */
function organism_get($organismid) {
	global $user;
	$organismid = intval(filter_xss($organismid));
	if($organismid < 1 || $organismid > 1000000) return false;
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
		array(':oid' => $organismid,
				':lang_code' => $user->language
		))->fetchAssoc();
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

function organism_get_available_attributes($classificationid) {
	$sqlGetAttributes = "WITH subclassification_constraints AS (
	-----------------------------------------------
	-- Get the left_value and right_value
	-- Those values will allow us to recieve the subtree
	-- Example result:
	--  prime_father_id | left_value | right_value
	-- -----------------+------------+-------------
	--                1 |          4 |           7
	-----------------------------------------------
	SELECT
		--id,
		--name,
		prime_father_id,
		left_value,
		right_value
		FROM
		{organism_classification} oc
	WHERE
		oc.id = :classificationid
	), included_organisms AS (
	-----------------------------------------------------
	-- Get all organisms within those subclassifications
	-- Example result:
	--  organism_id
	-- -------------
	--        20706
	--        20707
	--        20708
	--        20709
	--        20710
	--        20757
	--        20755
	-----------------------------------------------------
	SELECT
		ocs.organism_id AS organism_id
	FROM
		{organism_classification} oc
		JOIN subclassification_constraints scs ON oc.left_value >= scs.left_value AND oc.right_value <= scs.right_value AND oc.prime_father_id = scs.prime_father_id
		JOIN {organism_classification_subscription} ocs ON ocs.organism_classification_id = oc.id
	ORDER BY
		oc.left_value ASC
	)
	SELECT
		DISTINCT ON (oa.id)
		oa.id,
		oa.name
	FROM
		included_organisms io
		JOIN drupal_organism_attribute_value_subscription oavs ON oavs.organism_id = io.organism_id
		JOIN drupal_organism_attribute_value oav ON oav.id = oavs.organism_attribute_value_id
		JOIN drupal_organism_attribute oa ON oa.id = oav.organism_attribute_id
	ORDER BY oa.id";

	$rows= db_query(
			$sqlGetAttributes,
			array(':classificationid' => $classificationid),
			array('fetch' => PDO::FETCH_ASSOC))->fetchAll();

	$levelnamesstring = '';
	foreach($rows as $row) {

	}
}
?>
