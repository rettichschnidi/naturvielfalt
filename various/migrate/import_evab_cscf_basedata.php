<?php
/**
 * @file import_evab_cscf_basedata.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import evab's classification of the fauna to the new naturvielfalt DB
 */

require_once (dirname(__FILE__) . '/lib/bootstrap.php');
require_once ($libdir . '/NaturvielfaltDb.php');
global $drupalprefix;
global $errors;

$db = new NaturvielfaltDb($config['naturvielfalt_dev']['driver'], $config['naturvielfalt_dev']['name'], $config['naturvielfalt_dev']['user'], $config['naturvielfalt_dev']['password'], $config['naturvielfalt_dev']['host']);
$dbevab = new Db($config['evab']['driver'], $config['evab']['name'], $config['evab']['user'], $config['evab']['password'], $config['evab']['host']);

/**
 * Copy all crsf organisms including metadata into naturvielfalt DB
 *
 * relevant view:
 *  - 'public.import_evab_cscf_view'
 *
 *    Column   |          Type          | Modifiers 
 * ------------+------------------------+-----------
 *  nuesp      | integer                | 
 *  class      | character varying(255) | 
 *  order      | character varying(255) | 
 *  family     | character varying(255) | 
 *  genus      | character varying(255) | 
 *  subgenus   | character varying(255) | 
 *  species    | character varying(255) | 
 *  subspecies | character varying(255) | 
 *  name_de    | character varying(255) | 
 *  name_fr    | character varying(255) | 
 *  name_it    | character varying(255) | 
 *  name_rm    | character varying(255) | 
 *  name_en    | character varying(255) | 
 *  name_latin | character varying(255) | 
 */

/**
 * Create classifier CRSF if not already existing,
 * set $organism_classifier_id
 */
$importTable = '"import_evab_cscf_view"';

$organism_classifier_id = 0;
{
	$crsfCode = 'CSCF';
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
 * add the attribute 'NUESP',
 * set $organism_attribute_id
 */
$organism_attribute_id = 0;
{
	$attribute_name = 'NUESP';
	if (!$db -> haveAttributeName($attribute_name)) {
		$db -> createAttribute($attribute_name, 'n');
	}
	$organism_attribute_id = $db -> getAttributeId($attribute_name);
	print "Attribute id for '$attribute_name': $organism_attribute_id\n";
	assert($organism_attribute_id != 0);
}

/**
 * create organism classification level according to the CSCF
 * set $organism_classification_level_id
 * set $$organism_classification_level_name2$organism_classification_level_id
 */
$organism_classification_level_id = NULL;
$organism_classification_level_name2organism_classification_level_id = array();
{
	$classification_level_name_array = array(
			'class',
			'order',
			'family',
			'genus',
			'subgenus'
	);

	foreach ($classification_level_name_array as $classification_level_name) {
		if (!$db -> haveClassificationLevel($classification_level_name, $organism_classifier_id)) {
			$organism_classification_level_id = $db -> createClassificationLevel($classification_level_name, $organism_classification_level_id, $organism_classifier_id);
		} else {
			$organism_classification_level_id = $db -> getClassificationLevelId($classification_level_name, $organism_classifier_id);
			print "Classification level id for '$classification_level_name': $organism_classification_level_id\n";
		}
		$organism_classification_level_name2organism_classification_level_id[$classification_level_name] = $organism_classification_level_id;
	}
	$organism_classification_level_id = $db -> getClassificationLevelId($classification_level_name, $organism_classifier_id);
	assert($organism_classification_level_id != NULL);
}

/**
 * Add all families to the classification_level family,
 * set $organism_classification_id and
 * hashmap $classification_name2classification_id
 */
{
	// get all familynames
	$classification_name2classification_id = array();
	$allClassifications = array(
			'class' => 'class',
			'order' => '"order"',
			'family' => 'family',
			'genus' => 'genus',
			'subgenus' => 'subgenus'
	);
	foreach ($allClassifications as $name => $column) {
		$columns = array();
		$ownselect = 'SELECT DISTINCT(' . $column . ') ';
		$sql = 'FROM ' . $importTable . ' WHERE ' . $column . ' IS NOT NULL AND ' . $column . " != ''";
		$typeArray = array();
		$typeValue = array();
		$rows = $dbevab -> select_query($columns, $sql, $typeArray, $typeValue, TRUE, $ownselect);
		print "Make sure all " . count($rows) . " $name's are in DB\n";
		print "Its classification level id will be: " . $organism_classification_level_name2organism_classification_level_id[$name] . "\n";
		$organism_classification_id = $organism_classification_level_name2organism_classification_level_id[$name];
		$db -> startTransactionIfPossible();
		foreach ($rows as $row) {
			$organism_classification_id = 0;
			$organism_classification_name = $row[$name];
			$table = $drupalprefix . 'organism_classification';
			if ($db -> haveClassification($organism_classification_name, $organism_classification_level_id)) {
				$organism_classification_id = $db -> getClassificationId($organism_classification_name, $organism_classification_level_id);
			} else {
				$organism_classification_id = $db -> createClassification($organism_classification_name, $organism_classification_level_id);
			}
			$classification_name2classification_id[$name . $organism_classification_name] = $organism_classification_id;
			assert($organism_classification_id != 0);
		}
		$db -> stopTransactionIfPossible();
	}
}
exit();
/**
 * add all species which have no subspecies to:
 * - the organism_scientific_name table,
 * - organism table,
 * - hashmap $scientific_name2organism_id
 */
{
	$scientific_name2organism_id = array();
	$columns = array(
			'genus',
			'subgenus',
			'species',
			'name_de',
			'name_fr',
			'name_it',
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
exit();

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
