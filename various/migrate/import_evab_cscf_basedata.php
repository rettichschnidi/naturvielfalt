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
$importTable = 'import_evab_cscf_view';

$organism_classifier_id = 0;
{
	$crsfCode = 'CSCF';
	$isScientificClassification = TRUE;
	if (!$db->haveClassifier($crsfCode)) {
		$db->createClassifier($crsfCode, $isScientificClassification);
	} else {
		print $crsfCode . " classification already existing.\n";
	}
	$organism_classifier_id = $db->getClassifierId($crsfCode);
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
 * create organism classification level according to the CSCF
 * set $organism_classification_level_id
 * fill hashmap $$organism_classification_level_name2$organism_classification_level_id
 */

$organism_classification_level_name2organism_classification_level_id = array();
{
	$classification_data = array(
			'class' => array(
					'columnname' => 'class'
			),
			// order is a reserved word
			'order' => array(
					'columnname' => '"order"'
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

	$organism_classification_level_id = NULL;
	foreach ($classification_data as $classification_level_name => $moredata) {
		$column = $moredata['columnname'];
		if (!$db->haveClassificationLevel(
				$classification_level_name,
				$organism_classifier_id)) {
			$organism_classification_level_id = $db->createClassificationLevel(
					$classification_level_name,
					$organism_classifier_id,
					$organism_classification_level_id);
		} else {
			$organism_classification_level_id = $db->getClassificationLevelId(
					$classification_level_name,
					$organism_classifier_id,
					$organism_classification_level_id);
		}
		print
			("Classification level id for '$classification_level_name': $organism_classification_level_id\n");
		$classification_data[$classification_level_name]['classificationlevelid'] = $organism_classification_level_id;
	}
	assert($organism_classification_level_id != NULL);
}

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
	$sql = "SELECT DISTINCT ON(" . implode(',', $classification_level_columns)
			. ") " . implode(',', $classification_level_columns)
			. " FROM $importTable";
	// print "Query: $sql\n";
	$typeArray = array();
	$typeValue = array();
	$rows = $dbevab->query($sql, $typeArray, $typeValue);
	assert($rows != false);
	print "Got " . count($rows) . " unique unique classifications.\n";
	// for each unique classification...
	$i = 0;
	$start = microtime(true);
	$db->startTransactionIfPossible();
	foreach ($rows as $row) {
		if (++$i % 100 == 0) {
			$current = microtime(true);
			print "#: $i, ";
			print "Time: " . ($current - $start) . " seconds\n";
			$start = $current;
		}
		$classification_parent_id = NULL;
		// ...do add an entry to the organism_classification table
		foreach ($classification_data as $classification_level_name => $classification_level_data) {
			$classification_level_name = $classification_level_name;
			$classification_level_columnname = $classification_level_data['columnname'];
			$classification_level_id = $classification_level_data['classificationlevelid'];
			$classification_name = $row[$classification_level_name];
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
				$classification_id = $db->createClassification(
						$classification_name,
						$organism_classifier_id,
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
			assert($classification_id != NULL);
			$classification_parent_id = $classification_id;
		}
	}
	$db->stopTransactionIfPossible();
}
print "Classification done\n";
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
	$db->startTransactionIfPossible();
	foreach ($rows as $row) {
		$organism_id = 0;
		$organism_genus = $row['genus'];
		$organism_subgenus = $row['subgenus'];
		$organism_species = $row['species'];
		$organism_subspecies = $row['subspecies'];
		$organism_lang_name_de = $row['name_de'];
		$organism_lang_name_en = $row['name_en'];
		$organism_lang_name_fr = $row['name_fr'];
		$organism_lang_name_it = $row['name_it'];
		$organism_scientific_name_synonym = $row['name_latin'];
		$nuesp_nr = $row['nuesp'];

		$organism_scientific_name = $organism_subgenus ? ucfirst(
					$organism_subgenus) : ucfirst($organism_genus);
		$organism_scientific_name .= " $organism_species";
		$organism_scientific_name .= $organism_subspecies ? " $organism_subspecies"
				: '';
		$organism_classification_id = '';
		if ($organism_subgenus) {
			$organism_classification_name = $organism_subgenus;
			// $classification_data[$classification_level_name]['classifications'][$classification_name] = $classification_id;
			$organism_classification_id = $classification_data['subgenus']['classifications'][$organism_classification_name];
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
			if (!$db->haveOrganismClassificationSubscription(
					$organism_id,
					$organism_classification_id)) {
				$organism_classification_subscription_id = $db->createOrganismClassificationSubscription(
						$organism_id,
						$organism_classification_id);
			}
		} else {
			$organism_id = $db->getScientificNameOrganismId(
					$organism_scientific_name);
		}
		if (!$db->haveScientificName($organism_scientific_name_synonym)) {
			$organism_scientific_name_id = $db->createScientificName(
					$organism_id,
					$organism_scientific_name_synonym);
		}
		if (FALSE) {
			print 
				"Scientific Classification Name: $organism_classification_name, id: $organism_classification_id\n";
			print 
				"Scientific Name: $organism_scientific_name + $organism_scientific_name_synonym, id: $organism_id\n";
		}
		$scientific_name2organism_id[$organism_scientific_name] = $organism_id;
		$scientific_name2organism_id[$organism_scientific_name_synonym] = $organism_id;
		assert($organism_id != 0);
	}
	$db->stopTransactionIfPossible();
}
?>
