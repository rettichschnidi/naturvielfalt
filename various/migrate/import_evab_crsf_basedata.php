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
 *  - 'public.import_evab_crsf_view'
 *
 *     Column     |          Type          | Modifiers 
 *----------------+------------------------+-----------
 * nr             | integer                | 
 * status         | character varying(1)   | 
 * familie        | character varying(255) | 
 * gattung        | character varying(100) | 
 * art            | character varying(100) | 
 * autor          | character varying(100) | 
 * rang           | character varying(50)  | 
 * nameunterart   | character varying(100) | 
 * gueltigenamen  | character varying(255) | 
 * offizielleart  | integer                | 
 * deutsch        | character varying(255) | 
 * franzoesisch   | character varying(255) | 
 * florahelvetica | character varying(50)  | 
 * xenophyte      | integer                | 
 */

/**
 * Create classifier if not already existing,
 * set $organism_classifier_id
 */
$importTable = 'import_evab_crsf_view';

$organism_classifier_id = 0;
{
	$classifierName = 'CRSF';
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
 * add the attribute 'SISF Number',
 * set $organism_attribute_sisfnr_id
 */
$organism_attribute_sisfnr_id = 0;
{
	$attribute_name = 'SISF Number';
	if (!$db->haveAttributeName($attribute_name)) {
		$db->createAttribute($attribute_name, 'n');
	}
	$organism_attribute_sisfnr_id = $db->getAttributeId($attribute_name);
	print "Attribute id for '$attribute_name': $organism_attribute_sisfnr_id\n";
	assert($organism_attribute_sisfnr_id != 0);
}

/**
 * add the attribute 'Flora Helvetica Number',
 * set $organism_attribute_florahelvetica_id
 */
$organism_attribute_florahelvetica_id = 0;
{
	$attribute_name = 'Flora Helvetica Number';
	if (!$db->haveAttributeName($attribute_name)) {
		$db->createAttribute($attribute_name, 'n');
	}
	$organism_attribute_florahelvetica_id = $db->getAttributeId($attribute_name);
	print 
		"Attribute id for '$attribute_name': $organism_attribute_florahelvetica_id\n";
	assert($organism_attribute_florahelvetica_id != 0);
}

/**
 * add the attribute 'Xenophyte',
 * set $organism_attribute_xenophyte_id
 */
$organism_attribute_xenophyte_id = 0;
{
	$attribute_name = 'Xenophyte';
	if (!$db->haveAttributeName($attribute_name)) {
		$db->createAttribute($attribute_name, 'b');
	}
	$organism_attribute_xenophyte_id = $db->getAttributeId($attribute_name);
	print 
		"Attribute id for '$attribute_name': $organism_attribute_xenophyte_id\n";
	assert($organism_attribute_xenophyte_id != 0);
}

/**
 * create organism classification level
 * set $organism_classification_level_id
 * fill hashmap $organism_classification_level_name2organism_classification_level_id
 */
{
	$classification_data = array(
			'family' => array(
					'columnname' => 'familie'
			),
			'genus' => array(
					'columnname' => 'gattung'
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
				// ...but make sure that just species an genus entries can be NULL
				if ($classification_level_name != 'species'
						&& $classification_level_name != 'genus') {
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
 */
{
	$columns = array(
			'nr',
			'familie',
			'gattung',
			'deutsch',
			'franzoesisch',
			'italienisch',
			'name',
			'florahelvetica',
			'xenophyte'
	);
	$sql = "FROM
				$importTable
			WHERE
				nr < 1000000
			ORDER BY
				nr";
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
		$organism_family = $row['familie'];
		$organism_genus = $row['gattung'];
		$organism_scientific_name = $row['name'];
		$sisf_nr = $row['nr'];
		$florahelvetica_nr = $row['florahelvetica'];
		$xenophyte_nr = $row['xenophyte'] == NULL ? 0 : 1;
		$organism_lang_name = array(
				'de' => $row['deutsch'],
				'fr' => $row['franzoesisch'],
				'it' => $row['italienisch']
		);

		$organism_classification_id = '';
		$organism_classification_name = $organism_genus;
		assert($organism_classification_name);
		$organism_classification_id = $classification_data['genus']['classifications'][$organism_classification_name];
		if (false)
			print
				("Got genus id $organism_classification_id for organism '$organism_scientific_name'\n");
		assert($organism_classification_id);
		if (!$db->haveScientificName($organism_scientific_name)) {
			$organism_id = $db->createOrganism(NULL, NULL);
			$organism_scientific_name_id = $db->createScientificName(
					$organism_id,
					$organism_scientific_name);
			assert($organism_id != 0);

			// Create the sisf number as attribute
			if ($sisf_nr != NULL) {
				if (!$db->haveAttributeValueNumber(
						$organism_attribute_sisfnr_id,
						$sisf_nr)) {
					$organism_attribute_value_id = $db->createAttributeValueNumber(
							$organism_attribute_sisfnr_id,
							$sisf_nr);
				} else {
					$organism_attribute_value_id = $db->getAttributeValueNumberId(
							$organism_attribute_sisfnr_id,
							$sisf_nr);
				}
				$db->createAttributeValueSubscription(
						$organism_id,
						$organism_attribute_value_id);
			}

			// Create the flora helvetica number as attribute
			if ($florahelvetica_nr != NULL) {
				if (!$db->haveAttributeValueNumber(
						$organism_attribute_florahelvetica_id,
						$florahelvetica_nr)) {
					$organism_attribute_value_id = $db->createAttributeValueNumber(
							$organism_attribute_florahelvetica_id,
							$florahelvetica_nr);
				} else {
					$organism_attribute_value_id = $db->getAttributeValueNumberId(
							$organism_attribute_florahelvetica_id,
							$florahelvetica_nr);
				}
				$db->createAttributeValueSubscription(
						$organism_id,
						$organism_attribute_value_id);
			}

			// Set the Xenophyte attribute
			if (!$db->haveAttributeValueBoolean(
					$organism_attribute_xenophyte_id,
					$xenophyte_nr)) {
				$organism_attribute_value_id = $db->createAttributeValueBoolean(
						$organism_attribute_xenophyte_id,
						$xenophyte_nr);
			} else {
				$organism_attribute_value_id = $db->getAttributeValueBooleanId(
						$organism_attribute_xenophyte_id,
						$xenophyte_nr);
			}
			$db->createAttributeValueSubscription(
					$organism_id,
					$organism_attribute_value_id);

			// Subscribe the organism to its classification
			if (!$db->haveOrganismClassificationSubscription(
					$organism_id,
					$organism_classification_id)) {
				$organism_classification_subscription_id = $db->createOrganismClassificationSubscription(
						$organism_id,
						$organism_classification_id);
			}
			if (false && $organism_subgenus)
				print
					("Hooked up organism_id $organism_classification_id/$organism_classification_name  to organism_classification_subscription_id $organism_classification_subscription_id\n");

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
		assert($organism_id != 0);
	}
	$db->stopTransactionIfPossible();
}
?>
