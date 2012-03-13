<?php
/**
 * @file import_evab_cscf_basedata.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import evab's classification of the fauna to the new naturvielfalt DB
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
$dbevab = new Db(
	$config['evab']['driver'],
	$config['evab']['name'],
	$config['evab']['user'],
	$config['evab']['password'],
	$config['evab']['host']);

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
 *  evaborder  | character varying(255) |
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
 * Create classifier if not already existing,
 * set $organism_classifier_id
 */
$importTable = 'import_evab_cscf_view';

$organism_classifier_id = 0;
{
	$classifierName = 'CSCF';
	$isScientificClassification = true;
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
 * add the attribute 'NUESP',
 * set $organism_attribute_id
 */
$organism_attribute_id = 0;
{
	$attribute_name = 'NUESP';
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
 * fill hashmap $organism_classification_level_name2organism_classification_level_id
 */
{
	$classification_data = array(
			'class' => array(
					'columnname' => 'class'
			),
			// order is a reserved word so evaborder got used
			'order' => array(
					'columnname' => 'evaborder'
			),
			'family' => array(
					'columnname' => 'family'
			),
			'genus' => array(
					'columnname' => 'genus'
			),
			'subgenus' => array(
					'columnname' => 'subgenus'
			)
	);

	$organism_classification_level_id = $organism_classifier_id;
	$parent_id = $organism_classification_level_id;
	foreach ($classification_data as $classification_level_name => $moredata) {
		if (!$db->haveClassificationLevel(
				$classification_level_name,
				$organism_classifier_id,
				$parent_id)) {
			$organism_classification_level_id = $db->createClassificationLevel(
					$classification_level_name,
					$organism_classifier_id,
					$parent_id);
		} else {
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

// Add the classifier
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
print "ClassificatorId for $classifierName: $classification_id\n";
$classification_root_id = $classification_id;

/**
 * Add all classifications and connect them to their classifications_level,
 * fill hashmap $classification_name2classification_id
 */
{
	// store all level names in $classification_level_columns
	$classification_level_columns = array();
	foreach ($classification_data as $classification_level_name => $data) {
		$classification_level_columns[] = $data['columnname'];
	}

	// extract all unique organism classifications
	$columnstring = implode(',', $classification_level_columns);
	$sql = "SELECT
				DISTINCT $columnstring
			FROM 
				$importTable
			ORDER BY
			 $columnstring";
	if (false) {
		print "Query: $sql\n";
	}

	$typeArray = array();
	$typeValue = array();
	$rows = $dbevab->query($sql, $typeArray, $typeValue);
	assert($rows != false);
	print "Got " . count($rows) . " unique unique classification trees.\n";
	// for each unique classification...
	$i = 0;
	$start = microtime(true);
	foreach ($rows as $row) {
		// Print how much time per 100 entries got used
		if (++$i % 100 == 0) {
			$current = microtime(true);
			print "#: $i, ";
			print "Time: " . ($current - $start) . "s\n";
			$start = $current;
		}
		// ...do add an entry to the organism_classification table
		$db->startTransactionIfPossible();
		$classification_parent_id = $classification_root_id;
		foreach ($classification_data as $classification_level_name => $classification_level_data) {
			$classification_level_columnname = $classification_level_data['columnname'];
			$classification_level_id = $classification_level_data['classificationlevelid'];
			$classification_name = $row[$classification_level_data['columnname']];
			if (isset(
				$classification_data[$classification_level_name]['classifications'][$classification_name])) {
				$classification_parent_id = $classification_data[$classification_level_name]['classifications'][$classification_name];
				continue;
			}
			// drop empty entries...
			if (empty($classification_name)) {
				// ...but make sure that just subgenus entries can be NULL
				if ($classification_level_name != 'subgenus') {
					print 
						"$classification_level_name can not be empty ('$classification_name')\n";
					print "Result row: " . var_export($row, true) . "\n";
					assert(false);
				}
				continue;
			}

			// If not yet existing...
			if (!$db->haveClassification(
					$classification_name,
					$classification_level_id)) {
				if (false) {
					print 
						"Adding new $classification_level_name: $classification_name\n";
				}
				// ...then add entry
				assert($classification_parent_id != NULL);
				assert($organism_classifier_id != NULL);
				$classification_id = $db->createClassification(
						$classification_name,
						$classification_root_id,
						$classification_level_id,
						$classification_parent_id);
			} else {
				if (false) {
					print 
						"Getting $classification_level_name: $classification_name\n";
				}
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
			$classification_parent_id = $classification_id;
		}
		$db->stopTransactionIfPossible();
	}
}
print "Classification done...\n";

/**
 * add all organism
 * - connect with the correct classification
 * - the organism_scientific_name table,
 * - organism table,
 * - hashmap $scientific_name2organism_id
 */
{
	$scientific_name2organism_id = array();
	$columns = array(
			'nuesp',
			'genus',
			'subgenus',
			'species',
			'subspecies',
			'name_de',
			'name_fr',
			'name_it',
			'name_en',
			'name_latin'
	);
	$sql = "FROM
				$importTable
			ORDER BY
				nuesp";
	$typesArray = array();
	$valuesArray = array();
	$rows = $dbevab->select_query($columns, $sql, $typeArray, $typeValue);
	print "Make sure all " . count($rows) . " species are in DB\n";
	$i = 0;
	$db->startTransactionIfPossible();
	foreach ($rows as $row) {
		if (++$i % 100 == 0) {
			$current = microtime(true);
			print "#: $i, ";
			print "Time: " . ($current - $start) . "s\n";
			$start = $current;
		}
		if ($i % 1000 == 0) {
			$db->stopTransactionIfPossible();
			$db->startTransactionIfPossible();
		}
		$organism_id = 0;
		$organism_genus = $row['genus'];
		$organism_subgenus = $row['subgenus'];
		$organism_species = $row['species'];
		$organism_subspecies = $row['subspecies'];
		$organism_scientific_name = $row['name_latin'];
		$nuesp_nr = $row['nuesp'];
		$organism_lang_name = array(
				'de' => $row['name_de'],
				'en' => $row['name_en'],
				'fr' => $row['name_fr'],
				'it' => $row['name_it']
		);

// 		// Craft a scientific name from genus, if available subgenus, species and if available, subspecies.
// 		$organism_scientific_name = $organism_subgenus ? ucfirst(
// 					$organism_subgenus) : ucfirst($organism_genus);
// 		$organism_scientific_name .= " $organism_species";
// 		$organism_scientific_name .= $organism_subspecies ? " $organism_subspecies"
// 				: '';
		$organism_classification_id = '';
		if ($organism_subgenus) {
			$organism_classification_name = $organism_subgenus;
			// $classification_data[$classification_level_name]['classifications'][$classification_name] = $classification_id;
			$organism_classification_id = $classification_data['subgenus']['classifications'][$organism_classification_name];
			if(false)
				print("GOT SUBGENUS ID: $organism_classification_id for organism '$organism_scientific_name'\n");
		} else {
			$organism_classification_name = $organism_genus;
			$organism_classification_id = $classification_data['genus']['classifications'][$organism_classification_name];
		}
		assert($organism_classification_id);
		if (!$db->haveScientificName($organism_scientific_name)) {
			$organism_id = $db->createOrganism(NULL, NULL);
			$organism_scientific_name_id = $db->createScientificName(
					$organism_id,
					$organism_scientific_name);
			assert($organism_id != 0);
			// Create the nuesp number as attribute
			if (!$db->haveAttributeValueNumber(
					$organism_attribute_id,
					$nuesp_nr)) {
				$organism_attribute_value_id = $db->createAttributeValueNumber(
						$organism_attribute_id,
						$nuesp_nr);
				$db->createAttributeValueSubscription(
						$organism_id,
						$organism_attribute_value_id);
			}
			// Subscribe the organism to its classification
			if (!$db->haveOrganismClassificationSubscription(
					$organism_id,
					$organism_classification_id)) {
				$organism_classification_subscription_id = $db->createOrganismClassificationSubscription(
						$organism_id,
						$organism_classification_id);
			}
			if(false && $organism_subgenus)
				print "Hooked up organism_id $organism_classification_id/$organism_classification_name  to organism_classification_subscription_id $organism_classification_subscription_id\n";
			// If available: add localized texts to each organism
			foreach ($organism_lang_name as $lang => $name) {
				if (!empty($name)
						&& !$db->haveOrganismLang($organism_id, $lang, $name)) {
					$organismlangid = $db->createOrganismLang(
							$organism_id,
							$lang,
							$name);
					if (false) {
						print 
							"Created: $name with id $organismlangid for lang $lang and organismid $organism_id\n";
					}
				}
			}
		} else {
			$organism_id = $db->getScientificNameOrganismId(
					$organism_scientific_name);
		}
		if (false) {
			print 
				"Scientific Classification Name: $organism_classification_name, id: $organism_classification_id\n";
		}
		$scientific_name2organism_id[$organism_scientific_name] = $organism_id;
		assert($organism_id != 0);
	}
	$db->stopTransactionIfPossible();
}
?>
