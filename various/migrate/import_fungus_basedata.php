<?php
/**
 * @file import_fungus_basedata.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import fungus to the new naturvielfalt DB
 */

require_once (dirname(__FILE__) . '/lib/bootstrap.php');
require_once ($libdir . '/NaturvielfaltDb.php');
global $drupalprefix;
global $errors;

$db = new NaturvielfaltDb($config['naturvielfalt_dev']['driver'], $config['naturvielfalt_dev']['name'], $config['naturvielfalt_dev']['user'], $config['naturvielfalt_dev']['password'], $config['naturvielfalt_dev']['host']);

/**
 * Copy all fungus including metadata into naturvielfalt DB
 *
 * $columns = array(
 * 	'fungus_artnr',
 * 	'pilzname',
 * 	'author',
 * 	'klasse',
 * 	'erstnachweis',
 * 	'anzahl_funde'
 * );
 *
 */

/**
 * Create classifier Fungus if not already existing,
 * set $organism_classifier_id
 */
$organism_classifier_id = 0;
{
	$crsfCode = 'Fungus';
	$isScientificClassification = TRUE;
	if (!$db -> haveClassifier($crsfCode)) {
		$db -> createClassifier($crsfCode, $isScientificClassification);
	} else {
		print $crsfCode . " classification already existing.\n";
	}
	$organism_classifier_id = $db -> getClassifierId($crsfCode);
	assert($organism_classifier_id != 0);
}

/**
 * add the attribute 'Fungus-Nr.',
 * set $organism_attribute_id
 */
$organism_attribute_id = 0;
{
	$attribute_name = 'Fungus-Nr.';
	if (!$db -> haveAttributeName($attribute_name)) {
		$db -> createAttribute($attribute_name, 'n');
	}
	$organism_attribute_id = $db -> getAttributeId($attribute_name);
	print "Attribute id for '$attribute_name': $organism_attribute_id\n";
	assert($organism_attribute_id != 0);
}

/**
 * create organism classification level according to the Fungus-DB
 * (there is just one: class)
 * set $organism_classification_level_id
 */
$organism_classification_level_id = 0;
{
	$classification_level_name = 'class';
	if (!$db -> haveClassificationLevel($classification_level_name, $organism_classifier_id)) {
		$db -> createClassificationLevel($classification_level_name, NULL, $organism_classifier_id);
	}
	$organism_classification_level_id = $db -> getClassificationLevelId($classification_level_name, $organism_classifier_id);
	print "Classification level id for '$classification_level_name': $organism_classification_level_id\n";
	assert($organism_classification_level_id != 0);
}

/**
 * Add all families to the classification_level family,
 * set $organism_classification_id,
 * set $classifiercolumn
 * hashmap $classification_name2classification_id
 */
{
	// get all familynames
	$classification_name2classification_id = array();
	$classifiercolumn = 'klasse';
	$columns = array($classifiercolumn);
	$sql = "FROM import_fungus_basedata GROUP BY $classifiercolumn ORDER BY $classifiercolumn";
	$typeArray = array();
	$typeValue = array();
	$rows = $db -> select_query($columns, $sql, $typeArray, $typeValue);
	print "Make sure all " . count($rows) . " classifications are in DB\n";
	$db -> startTransactionIfPossible();
	foreach ($rows as $row) {
		$organism_classification_id = 0;
		$organism_classification_name = $row[$classifiercolumn];
		$table = $drupalprefix . 'organism_classification';
		if ($db -> haveClassification($organism_classification_name, $organism_classification_level_id)) {
			$organism_classification_id = $db -> getClassificationId($organism_classification_name, $organism_classification_level_id);
		} else {
			$organism_classification_id = $db -> createClassification($organism_classification_name, $organism_classification_level_id);
		}
		$classification_name2classification_id[$organism_classification_name] = $organism_classification_id;
		assert($organism_classification_id != 0);
	}
	$db -> stopTransactionIfPossible();
}

/**
 * add all speciesnames to the organism_scientific_name table,
 * add an entry in the organism table,
 * add all organisms to hashmap $scientific_name2organism_id
 */
{
	$scientific_name2organism_id = array();
	$columns = array(
			'pilzname',
			'klasse',
			'fungus_artnr'
	);
	$sql = "FROM
				import_fungus_basedata
			GROUP BY
				pilzname,
				klasse,
				fungus_artnr
			ORDER BY
				pilzname";
	$typesArray = array();
	$valuesArray = array();
	$rows = $db -> select_query($columns, $sql, $typeArray, $typeValue);
	print "Make sure all " . count($rows) . " species are in DB\n";

	$db -> startTransactionIfPossible();
	foreach ($rows as $row) {
		$organism_id = 0;
		$scientific_name_name = $row['pilzname'];
		$organism_classification_name = $row['klasse'];
		$sisf_nr = $row['fungus_artnr'];
		$organism_classification_id = $classification_name2classification_id[$organism_classification_name];

		if (!$db -> haveScientificName($scientific_name_name)) {
			$organism_id = $db -> createOrganism(NULL, NULL);
			$organism_scientific_name_id = $db -> createScientificName($organism_id, $scientific_name_name);
			assert($organism_id != 0);
			if (!$db -> haveAttributeValueNumber($organism_attribute_id, $sisf_nr)) {
				$organism_attribute_value_id = $db -> createAttributeValueNumber($organism_attribute_id, $sisf_nr);
				$db -> createAttributeValueSubscription($organism_id, $organism_attribute_value_id);
			}
			if (!$db -> haveOrganismClassificationSubscription($organism_id, $organism_classification_id)) {
				$organism_classification_subscription_id = $db -> createOrganismClassificationSubscription($organism_id, $organism_classification_id);
			}
		} else {
			$organism_id = $db -> getScientificNameOrganismId($scientific_name_name);
		}
		$scientific_name2organism_id[$scientific_name_name] = $organism_id;
		assert($organism_id != 0);
	}
	$db -> stopTransactionIfPossible();
}
?>
