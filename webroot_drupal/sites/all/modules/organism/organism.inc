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
 * Example:
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

/**
 * Return an array with all classifiers of this organism
 * 
 * @return array
 * Example:
 * 	array ( 
 * 		0 => array (
 * 				'name' => 'Fungus-Nr.',
 * 				'valuetype' => 'n',
 * 				'id' => '1'
 * 			)
 * 	)
 */
function organism_get_attributes_of_organism($organismid) {
	$attributes = array();
	$attributeresult = db_query(
		'SELECT
				DISTINCT
					oa.name,
					oa.valuetype,
					oa.id
			FROM
				drupal_organism_attribute oa
				JOIN {organism_attribute_value} oav ON oav.organism_attribute_id = oa.id
				JOIN {organism_attribute_value_subscription} oavs ON oavs.organism_attribute_value_id = oav.id
				JOIN {organism_classification_subscription} ocs ON oavs.organism_id = ocs.organism_id
				JOIN {organism_classification} oc ON oc.id = ocs.organism_classification_id
			WHERE
				oavs.organism_id = :organismid
			GROUP BY
				oa.name,
				oa.valuetype,
				oa.id;',
		array(':organismid' => $organismid));

	foreach ($attributeresult as $attribute) {
		$attributes[] = array(
				'name' => $attribute->name,
				'valuetype' => $attribute->valuetype,
				'id' => $attribute->id
		);
	}
	return $attributes;
}

/**
 * 
 * Enter description here ...
 * @param integer $organismid
 * @param integer $attributeid
 * @param integer $attributetype
 * @return array
 * Example:
 * 	array(
 * 		0 => '295',
 * 		1 => '351
 * 	)
 */
function organism_get_attribute_values_of_organism($organismid, $attributeid,
		$attributetype) {
	$attributevalues = array();
	$attributevalueresult = db_query(
		'SELECT
				oav.text_value,
				oav.boolean_value,
				oav.number_value
			FROM
				drupal_organism_attribute oa
				JOIN {organism_attribute_value} oav ON oav.organism_attribute_id = oa.id
				JOIN {organism_attribute_value_subscription} oavs ON oavs.organism_attribute_value_id = oav.id
				JOIN {organism_classification_subscription} ocs ON oavs.organism_id = ocs.organism_id
				JOIN {organism_classification} oc ON oc.id = ocs.organism_classification_id
			WHERE
				oavs.organism_id = :organismid AND oa.id = :attributeid',
		array(':organismid' => $organismid,
				':attributeid' => $attributeid
		));
	foreach ($attributevalueresult as $attributevalue) {
		if ($attributetype == 'n') {
			$attributevalues[] = $attributevalue->number_value;
		} else if ($attributetype == 'b') {
			$attributevalues[] = $attributevalue->number_boolean;
		} else if ($attributetype == 't') {
			$attributevalues[] = $attributevalue->number_value;
		}
	}
	return $attributevalues;
}
?>
