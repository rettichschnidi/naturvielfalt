<?php
/**
 * @file migrate_organism.php
 * @author Reto Schneider, 2011, github@reto-schneider.ch
 *
 * Migrate the organisms to the new naturvielfalt DB
 */

require_once (dirname(__FILE__) . '/lib/bootstrap.php');
require_once ($libdir . '/NaturvielfaltDb.php');
global $drupalprefix;
global $errors;

$db = new NaturvielfaltDb($config['naturvielfalt_dev']['driver'], $config['naturvielfalt_dev']['name'], $config['naturvielfalt_dev']['user'], $config['naturvielfalt_dev']['password'], $config['naturvielfalt_dev']['host']);

/**
 * Copy all crsf organisms including metadata into naturvielfalt DB
 *
 * $columns = array(
 * 	'sisf_nr',
 * 	'status',
 * 	'artname',
 * 	'familienname',
 * 	'eingeschlossen_in_sisf_nr',
 * 	'synonym_von_sisf_nr',
 * 	'synonym_von_artname',
 * 	'quellen',
 * 	'fh_nr',
 * 	'xenophyt',
 * 	'angrenzendes_gebiet'
 * );
 *
 */

/**
 * Create classifier CRSF if not already existing,
 * set $organism_classifier_id
 */
$organism_classifier_id = 0;
{
	$crsfCode = 'CRSF';
	if (!$db -> haveClassifier($crsfCode)) {
		$db -> createClassifier($crsfCode, 1);
	} else {
		print "CRSF classification already existing.\n";
	}
	$organism_classifier_id = $db -> getClassifierId($crsfCode);
	assert($organism_classifier_id != 0);
}

/**
 * add the attribute 'SISF-Nr.',
 * set $organism_attribute_id
 */
$organism_attribute_id = 0;
{
	$attribute_name = 'SISF-Nr.';
	if (!$db -> haveAttributeName($attribute_name)) {
		$db -> createAttribute('SISF-Nr.', 'n');
	}
	$organism_attribute_id = $db -> getAttributeId($attribute_name);
	print "Attribute id for 'SIFS-Nr.': $organism_attribute_id\n";
	assert($organism_attribute_id != 0);
}

/**
 * create organism classification level according to the CRSF
 * (there is just one: family)
 * set $organism_classification_level_id
 */
$organism_classification_level_id = 0;
{
	if (!$db -> haveClassificationLevel('family', $organism_classifier_id)) {
		$db -> createClassificationLevel('family', NULL, $organism_classifier_id);
	}
	$organism_classification_level_id = $db -> getClassificationLevelId('family', $organism_classifier_id);
	print "Classification level id for 'family': $organism_classification_level_id\n";
	assert($organism_classification_level_id != 0);
}

/**
 * Add all families to the classification_level family,
 * set $organism_classification_id and
 * hashmap $classification_name2classification_id
 */
{
	// get all familynames
	$classification_name2classification_id = array();
	$columns = array('familienname');
	$sql = "FROM import_crsf_basedata WHERE familienname <> '#N/A' GROUP BY familienname ORDER BY familienname";
	$typeArray = array();
	$typeValue = array();
	$rows = $db -> select_query($columns, $sql, $typeArray, $typeValue);
	print "Make sure all " . count($rows) . " classifications are in DB\n";
	$db -> startTransactionIfPossible();
	foreach ($rows as $row) {
		$organism_classification_id = 0;
		$organism_classification_name = $row['familienname'];
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
 * add all speciesnames ('A' and 'C') to the organism_scientific_name table,
 * add an entry in the organism table,
 * add all organisms to hashmap $scientific_name2organism_id
 */
{
	$scientific_name2organism_id = array();
	$columns = array(
			'artname',
			'familienname',
			'sisf_nr'
	);
	$sql = "FROM
				import_crsf_basedata
			WHERE
				status IN ('A', 'C')
			GROUP BY
				artname,
				familienname,
				sisf_nr
			ORDER BY
				artname";
	$typesArray = array();
	$valuesArray = array();
	$rows = $db -> select_query($columns, $sql, $typeArray, $typeValue);
	print "Make sure all " . count($rows) . " species are in DB\n";

	$db -> startTransactionIfPossible();
	foreach ($rows as $row) {
		$organism_id = 0;
		$scientific_name_name = $row['artname'];
		$organism_classification_name = $row['familienname'];
		$sisf_nr = $row['sisf_nr'];
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

/**
 * Import all "included" species ('I')
 */
{
	$columns = array(
			'artname',
			'eingeschlossen_in_artname',
			'sisf_nr'
	);
	$sql = " FROM import_crsf_basedata WHERE status IN ('I')";
	$typesArray = array();
	$valuesArray = array();
	$synonyms = $db -> select_query($columns, $sql, $typesArray, $valuesArray, true);
	print "Got " . count($synonyms) . " species which are subspecies for already imported ones.\n";
	$db -> startTransactionIfPossible();
	foreach ($synonyms as $synonym) {
		$subspecies_name = $synonym['artname'];
		$metaspecies_name = $synonym['eingeschlossen_in_artname'];
		$sisf_nr = $synonym['sisf_nr'];

		if (!isset($scientific_name2organism_id[$metaspecies_name])) {
			global $errors;
			$errors[] = "Undefined metaspecies: $metaspecies_name";
			assert(false);
		}
		$metaspecies_id = $scientific_name2organism_id[$metaspecies_name];

		if (!$db -> haveScientificName($subspecies_name)) {
			if ($db -> haveScientificName($metaspecies_name)) {
				$prime_father_id = $db -> getPrimeFatherId($metaspecies_id);
				$organism_id = $db -> createOrganism($prime_father_id, $metaspecies_id);
				$organism_scientific_name_id = $db -> createScientificName($organism_id, $subspecies_name);
				$organism_classification_id = $classification_name2classification_id[$organism_classification_name];

				if (!$db -> haveAttributeValueNumber($organism_attribute_id, $sisf_nr)) {
					$organism_attribute_value_id = $db -> createAttributeValueNumber($organism_attribute_id, $sisf_nr);
					$db -> createAttributeValueSubscription($organism_id, $organism_attribute_value_id);
				}
				if (!$db -> haveOrganismClassificationSubscription($organism_id, $organism_classification_id)) {
					$organism_classification_subscription_id = $db -> createOrganismClassificationSubscription($organism_id, $organism_classification_id);
				}
			} else {
				global $errors;
				$errors[] = "$subspecies_name is subspecies of $metaspecies_name, but $metaspecies_name is not available in DB.";
				assert(false);
			}
		} else {
			$organism_id = $db -> getScientificNameOrganismId($subspecies_name);
		}
		$scientific_name2organism_id[$subspecies_name] = $organism_id;
	}
	$db -> stopTransactionIfPossible();
}

/**
 * Import all "synonym" species ('S')
 * @note SIFS-Nr. of synonyms get dropped as synonyms will have the SIFS-Nr. of the already importet speciesname
 */
{
	$columns = array(
			'artname',
			'synonym_von_artname'
	);
	$sql = " FROM import_crsf_basedata WHERE status IN ('S')";
	$typesArray = array();
	$valuesArray = array();
	$synonyms = $db -> select_query($columns, $sql, $typesArray, $valuesArray, true);
	print "Got " . count($synonyms) . " species which are synonyms for already imported ones.\n";
	$db -> startTransactionIfPossible();
	foreach ($synonyms as $synonym) {
		$organism_scientific_name_id = NULL;
		$synonymspecies_name = $synonym['artname'];
		$species_name = $synonym['synonym_von_artname'];

		if (!isset($scientific_name2organism_id[$species_name])) {
			global $errors;
			$errors[] = "Undefined metaspecies: $species_name";
			assert(false);
		}
		$species_id = $scientific_name2organism_id[$species_name];

		if (!$db -> haveScientificName($synonymspecies_name)) {
			if ($db -> haveScientificName($species_name)) {
				$organism_id = $db -> getScientificNameOrganismId($species_name);
				$organism_scientific_name_id = $db -> createScientificName($organism_id, $synonymspecies_name);
				assert($organism_scientific_name_id != NULL);
			} else {
				global $errors;
				$errors[] = "$subspecies_name is subspecies of $metaspecies_name, but $metaspecies_name is not available in DB.";
				assert(false);
			}
		} else {
			$organism_id = $db -> getScientificNameOrganismId($subspecies_name);
		}
		$scientific_name2organism_id[$synonymspecies_name] = $organism_id;
	}
	$db -> stopTransactionIfPossible();
}
?>
