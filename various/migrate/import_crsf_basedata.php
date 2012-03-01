<?php
/**
 * @file import_crsf_basedata.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import CRFS's classification of the flora to the new naturvielfalt DB
 */

require_once(dirname(__FILE__) . '/lib/bootstrap.php');
require_once($libdir . '/NaturvielfaltDb.php');
global $drupalprefix;
global $errors;

$db = new NaturvielfaltDb(
	$config['naturvielfalt_dev']['driver'],
	$config['naturvielfalt_dev']['name'],
	$config['naturvielfalt_dev']['user'],
	$config['naturvielfalt_dev']['password'],
	$config['naturvielfalt_dev']['host']);

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
 * Create classifier if not already existing,
 * set $organism_classifier_id
 */
$organism_classifier_id = 0;
{
	$classifierName = 'CRSF';
	$isScientificClassification = TRUE;
	if (!$db->haveClassifier($classifierName)) {
		print "Classifier $classifierName not found. Create it.\n";
		$organism_classifier_id = $db->createClassifier(
				$classifierName,
				$isScientificClassification);
	} else {
		print $classifierName . " classification already existing.\n";
		$organism_classifier_id = $db->getClassifierId($classifierName);
	}
	assert($organism_classifier_id != 0);
}

/**
 * add the attribute 'SISF-Nr.',
 * set $organism_attribute_id
 */
$organism_attribute_id = 0;
{
	$attribute_name = 'SISF-Nr.';
	if (!$db->haveAttributeName($attribute_name)) {
		$db->createAttribute($attribute_name, 'n');
	}
	$organism_attribute_id = $db->getAttributeId($attribute_name);
	print "Attribute id for '$attribute_name': $organism_attribute_id\n";
	assert($organism_attribute_id != 0);
}

/**
 * create organism classification level
 * set $organism_classification_level_id
 */
{
	$classification_data = array(
			'family' => array(
					'columnname' => 'familienname'
			)
	);

	$organism_classification_level_id = $organism_classifier_id;
	$parent_id = $organism_classification_level_id;
	foreach ($classification_data as $classification_level_name => $moredata) {
		if (!$db->haveClassificationLevel(
				$classification_level_name,
				$organism_classifier_id,
				$parent_id)) {
			print "create\n";
			$organism_classification_level_id = $db->createClassificationLevel(
					$classification_level_name,
					$organism_classifier_id,
					$parent_id);
		} else {
			print "got it\n";
			$organism_classification_level_id = $db->getClassificationLevelId(
					$classification_level_name,
					$organism_classifier_id,
					$parent_id);
		}
		print
			("Classification level id for '$classification_level_name': $organism_classification_level_id\n");
		$classification_data[$classification_level_name]['classificationlevelid'] = $organism_classification_level_id;
		$parent_id = $organism_classification_level_id;
	}
	assert($organism_classification_level_id != NULL);
}

{
	if (!$db->haveClassification($classifierName, $organism_classifier_id)) {
		print "Adding new classificator: $classifierName\n";
		// ...then add entry
		$classification_id = $db->createClassification(
				$classifierName,
				NULL,
				$organism_classifier_id,
				NULL);
	} else {
		print 
			"Getting classificator $classification_level_name: $classifierName\n";
		// ...or just get its id
		$classification_id = $db->getClassificationId(
				$classifierName,
				$organism_classifier_id);
	}
}
$classification_root_id = $classification_id;
print "ClassificatorId for $classifierName: $classification_id\n";

/**
 * Add all classifications and connect them to their classifications_level,
 * fill hashmap $classification_name2classification_id
 */
{
	$classification_name2classification_id = array();
	// get all familynames
	$columns = array(
			'familienname'
	);
	$sql = "FROM import_crsf_basedata WHERE familienname <> '#N/A' GROUP BY familienname ORDER BY familienname";
	$typeArray = array();
	$typeValue = array();
	$rows = $db->select_query($columns, $sql, $typeArray, $typeValue);
	print "Make sure all " . count($rows) . " classifications are in DB\n";
	// for each unique classification...
	$i = 0;
	$start = microtime(true);
	foreach ($rows as $row) {
		if (++$i % 100 == 0) {
			$current = microtime(true);
			print "#: $i, ";
			print "Time: " . ($current - $start) . " s\n";
			$start = $current;
		}
		$classification_parent_id = $classification_root_id;
		// ...do add an entry to the organism_classification table
		$db->startTransactionIfPossible();
		foreach ($classification_data as $classification_level_name => $classification_level_data) {
			$classification_level_name = $classification_level_name;
			$classification_level_columnname = $classification_level_data['columnname'];
			$classification_level_id = $classification_level_data['classificationlevelid'];
			$classification_name = $row[$classification_level_columnname];
			if (isset(
				$classification_data[$classification_level_name]['classifications'][$classification_name])) {
				$classification_parent_id = $classification_data[$classification_level_name]['classifications'][$classification_name];
				continue;
			}
			// doge emtpy entries...
			if (!$classification_name) {
				// ...but make sure that just subgenus entries can be NULL
				if ($classification_level_name != 'subgenus') {
					print 
						"$classification_level_name can not be NULL('$classification_name')\n";
					print "Result row: " . var_export($row, TRUE) . "\n";
					assert(false);
				}
				continue;
			}

			// If not yet existing...
			if (!$db->haveClassification(
					$classification_name,
					$classification_level_id)) {
				print 
					"Adding new $classification_level_name: $classification_name\n";
				// ...then add entry
				assert($classification_parent_id != NULL);
				assert($organism_classifier_id != NULL);
				$classification_id = $db->createClassification(
						$classification_name,
						$classification_root_id,
						$classification_level_id,
						$classification_parent_id);
			} else {
				print 
					"Getting $classification_level_name: $classification_name\n";
				// ...or just get its id
				$classification_id = $db->getClassificationId(
						$classification_name,
						$classification_level_id);
			}
			$classification_name2classification_id[$classification_name] = $classification_id;
			$classification_data[$classification_level_name]['classifications'][$classification_name] = $classification_id;
			if (false) {
				print "New ID: $classification_id\n";
			}
			assert($classification_id != NULL);
		}
		$db->stopTransactionIfPossible();
	}
}
print "Classification done...\n";

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
	$rows = $db->select_query($columns, $sql, $typeArray, $typeValue);
	print "Make sure all " . count($rows) . " species are in DB\n";

	$db->startTransactionIfPossible();
	$i = 0;
	$start = microtime(true);
	foreach ($rows as $row) {
		if (++$i % 100 == 0) {
			$current = microtime(true);
			print "#: $i, ";
			print "Time: " . ($current - $start) . " s\n";
			$start = $current;
		}
		$organism_id = 0;
		$scientific_name_name = $row['artname'];
		$organism_classification_name = $row['familienname'];
		$sisf_nr = $row['sisf_nr'];
		$organism_classification_id = $classification_data['family']['classifications'][$organism_classification_name];

		if (!$db->haveScientificName($scientific_name_name)) {
			$organism_id = $db->createOrganism(NULL, NULL);
			$organism_scientific_name_id = $db->createScientificName(
					$organism_id,
					$scientific_name_name);
			assert($organism_id != 0);
			if (!$db->haveAttributeValueNumber($organism_attribute_id, $sisf_nr)) {
				$organism_attribute_value_id = $db->createAttributeValueNumber(
						$organism_attribute_id,
						$sisf_nr);
				$db->createAttributeValueSubscription(
						$organism_id,
						$organism_attribute_value_id);
			}
			if (!$db->haveOrganismClassificationSubscription(
					$organism_id,
					$organism_classification_id)) {
				$organism_classification_subscription_id = $db->createOrganismClassificationSubscription(
						$organism_id,
						$organism_classification_id);
			}
		} else {
			$organism_id = $db->getScientificNameOrganismId(
					$scientific_name_name);
		}
		$scientific_name2organism_id[$scientific_name_name] = $organism_id;
		assert($organism_id != 0);
	}
	$db->stopTransactionIfPossible();
}

/**
 * Import all "included" species ('I')
 */
{
	$columns = array(
			'artname',
			'eingeschlossen_in_artname',
			'familienname',
			'sisf_nr'
	);
	$sql = " FROM import_crsf_basedata WHERE status IN ('I')";
	$typesArray = array();
	$valuesArray = array();
	$synonyms = $db->select_query(
			$columns,
			$sql,
			$typesArray,
			$valuesArray,
			true);
	print 
		"Got " . count($synonyms)
				. " species which are subspecies for already imported ones.\n";
	$db->startTransactionIfPossible();
	foreach ($synonyms as $synonym) {
		$subspecies_name = $synonym['artname'];
		$metaspecies_name = $synonym['eingeschlossen_in_artname'];
		$familyname = $synonym['familienname'];
		$sisf_nr = $synonym['sisf_nr'];

		if (!isset($scientific_name2organism_id[$metaspecies_name])) {
			global $errors;
			$errors[] = "Undefined metaspecies: $metaspecies_name";
			assert(false);
		}
		$metaspecies_id = $scientific_name2organism_id[$metaspecies_name];

		if (!$db->haveScientificName($subspecies_name)) {
			if ($db->haveScientificName($metaspecies_name)) {
				$prime_father_id = $db->getPrimeFatherId($metaspecies_id);
				$organism_id = $db->createOrganism(
						$prime_father_id,
						$metaspecies_id);
				$organism_scientific_name_id = $db->createScientificName(
						$organism_id,
						$subspecies_name);
				$organism_classification_id = $classification_name2classification_id[$familyname];

				assert($organism_classification_id != NULL);

				if (!$db->haveAttributeValueNumber(
						$organism_attribute_id,
						$sisf_nr)) {
					$organism_attribute_value_id = $db->createAttributeValueNumber(
							$organism_attribute_id,
							$sisf_nr);
					$db->createAttributeValueSubscription(
							$organism_id,
							$organism_attribute_value_id);
				}
				if (!$db->haveOrganismClassificationSubscription(
						$organism_id,
						$organism_classification_id)) {
					$organism_classification_subscription_id = $db->createOrganismClassificationSubscription(
							$organism_id,
							$organism_classification_id);
				}
			} else {
				global $errors;
				$errors[] = "$subspecies_name is subspecies of $metaspecies_name, but $metaspecies_name is not available in DB.";
				assert(false);
			}
		} else {
			$organism_id = $db->getScientificNameOrganismId($subspecies_name);
		}
		$scientific_name2organism_id[$subspecies_name] = $organism_id;
	}
	$db->stopTransactionIfPossible();
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
	$synonyms = $db->select_query(
			$columns,
			$sql,
			$typesArray,
			$valuesArray,
			true);
	print 
		"Got " . count($synonyms)
				. " species which are synonyms for already imported ones.\n";
	$db->startTransactionIfPossible();
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

		if (!$db->haveScientificName($synonymspecies_name)) {
			if ($db->haveScientificName($species_name)) {
				$organism_id = $db->getScientificNameOrganismId($species_name);
				$organism_scientific_name_id = $db->createScientificName(
						$organism_id,
						$synonymspecies_name);
				assert($organism_scientific_name_id != NULL);
			} else {
				global $errors;
				$errors[] = "$subspecies_name is subspecies of $metaspecies_name, but $metaspecies_name is not available in DB.";
				assert(false);
			}
		} else {
			$organism_id = $db->getScientificNameOrganismId($subspecies_name);
		}
		$scientific_name2organism_id[$synonymspecies_name] = $organism_id;
	}
	$db->stopTransactionIfPossible();
}
?>
