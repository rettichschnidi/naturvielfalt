<?php
/**
 * @file import_swisslichens_basedata.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import lichens to the new naturvielfalt DB
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

$importTable = 'import_swisslichens_basedata';
/**
 * Copy all SwissLichens into naturvielfalt DB
 *
 * $columns = array(
 * 	'familyname',
 * 	'species'
 * );
 *
 */

/**
 * Create classifier if not already existing,
 * set $organism_classifier_id
 */
$organism_classifier_id = 0;
{
	$classifierName = 'SwissLichens';
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
 * create organism classification level
 * set $organism_classification_level_id
 */

{
	$classification_data = array(
			'family' => array(
					'columnname' => 'familyname'
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
 * Add all families to the classification,
 * set $organism_classification_id and
 * hashmap $classification_name2classification_id
 */
{
	// get all familynames
	$columns = array(
			'familyname'
	);
	$sql = "FROM $importTable GROUP BY familyname ORDER BY familyname";
	$typeArray = array();
	$typeValue = array();
	$rows = $db->select_query($columns, $sql, $typeArray, $typeValue);
	print "Make sure all " . count($rows) . " classifications are in DB\n";

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
			$classification_data[$classification_level_name]['classifications'][$classification_name] = $classification_id;
			if (false) {
				print "New ID: $classification_id\n";
			}
			assert($classification_id != NULL);
			$db->stopTransactionIfPossible();
		}
	}
}
print "Classification done...\n";

/**
 * add all speciesnames to the organism_scientific_name table,
 * add an entry in the organism table for each organism,
 * connect organisms with its scientific name(s)
 * add all organisms names and organism id's to hashmap $scientific_name2organism_id
 */
{
	$scientific_name2organism_id = array();
	$columns = array(
			'species',
			'familyname'
	);
	$sql = "FROM
				$importTable
			GROUP BY
				species,
				familyname
			ORDER BY
				species";
	$typesArray = array();
	$valuesArray = array();
	$rows = $db->select_query($columns, $sql, $typeArray, $typeValue);
	print "Make sure all " . count($rows) . " species are in DB\n";
	$db->startTransactionIfPossible();
	foreach ($rows as $row) {
		$organism_id = 0;
		$scientific_name_name = $row['species'];
		$organism_classification_name = $row['familyname'];
		$organism_classification_id = $classification_data['family']['classifications'][$organism_classification_name];

		if (!$db->haveScientificName($scientific_name_name)) {
			$organism_id = $db->createOrganism(NULL, NULL);
			$organism_scientific_name_id = $db->createScientificName(
					$organism_id,
					$scientific_name_name);
			assert($organism_id != 0);
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
?>
