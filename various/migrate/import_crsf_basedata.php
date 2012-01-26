<?php
/**
 * @file migrate_organism.php
 * @author Reto Schneider, 2011, github@reto-schneider.ch
 *
 * Migrate the organisms to the new naturvielfalt DB
 */

require_once (dirname(__FILE__) . '/lib/bootstrap.php');
require_once ($libdir . '/Db.php');
global $drupalprefix;
global $errors;

$dbnaturvielfalt = new Db($config['naturvielfalt_dev']['driver'], $config['naturvielfalt_dev']['name'], $config['naturvielfalt_dev']['user'], $config['naturvielfalt_dev']['password'], $config['naturvielfalt_dev']['host']);

/**
 * Copy all crsf organisms into our php hashtable
 */
/*
 $columns = array(
 'sisf_nr',
 'status',
 'artname',
 'familienname',
 'eingeschlossen_in_sisf_nr',
 'synonym_von_sisf_nr',
 'synonym_von_artname',
 'quellen',
 'fh_nr',
 'xenophyt',
 'angrenzendes_gebiet'
 );
 */

/**
 * Simply check if $errors is set and if, print out its values and exit
 */
function checkForErrors() {
	global $errors;
	if (count($errors) > 0) {
		foreach ($errors as $key => $value) {
			print("$value\n");
		}
		debug_backtrace();
		exit(-1);
	}
}

// Create classifier CRSF if not already existing, set $organism_classifier_id
{
	$organism_classifier_id = 0;
	$table = $drupalprefix . 'organism_classifier';
	$updateColumnArray = array(
			'name',
			'scientific_classification'
	);
	$updateTypesArray = array(
			'text',
			'integer'
	);
	$updateValuesArray = array(
			'CRSF',
			'1'
	);
	$whereColumnArray = array('name');
	$whereTypesArray = array('text');
	$whereValuesArray = array('CRSF');
	$organism_classifier_id = $dbnaturvielfalt -> upsert_query($table, $updateColumnArray, $updateTypesArray, $updateValuesArray, $whereColumnArray, $whereTypesArray, $whereValuesArray);
	checkForErrors();
	if ($organism_classifier_id == 0) {
		die("Error while setting CRSF classifier.");
	}
}

// create organism classification level according to the CRSF (there is just one: family)
{
	$familiId = 0;
	$table = $drupalprefix . 'organism_classification_level';
	$updateColumnArray = array(
			'parent_id',
			'organism_classifier_id',
			'left_value',
			'right_value',
			'name'
	);
	$updateTypesArray = array(
			'integer',
			'integer',
			'integer',
			'integer',
			'text'
	);
	$updateValuesArray = array(
			0,
			$organism_classifier_id,
			'1',
			'2',
			'family'
	);
	$whereColumnArray = array(
			'name',
			'organism_classifier_id'
	);
	$whereTypesArray = array(
			'text',
			'integer'
	);
	$whereValuesArray = array(
			'family',
			$organism_classifier_id
	);
	$familyId = $dbnaturvielfalt -> upsert_query($table, $updateColumnArray, $updateTypesArray, $updateValuesArray, $whereColumnArray, $whereTypesArray, $whereValuesArray);
	checkForErrors();
	if ($familyId == 0) {
		die("Error while setting CRSF family.");
	}
}

// add all families to the classification_level family, set $organism_classification_id
{
	$columns = array('familienname');
	$sql = "FROM import_crsf_basedata WHERE familienname <> '#N/A' GROUP BY familienname ORDER BY familienname";
	$typeArray = array();
	$typeValue = array();
	$rows = $dbnaturvielfalt -> select_query($columns, $sql, $typeArray, $typeValue);
	checkForErrors();
	$dbnaturvielfalt -> startTransactionIfPossible();
	foreach ($rows as $row) {
		$organism_classification_id = 0;
		$familyname = $row['familienname'];
		$table = $drupalprefix . 'organism_classification';
		$updateColumnArray = array(
				'organism_classification_level_id',
				'name'
		);
		$updateTypesArray = array(
				'integer',
				'text'
		);
		$updateValuesArray = array(
				$familyId,
				$familyname
		);

		$whereColumnArray = array(
				'organism_classification_level_id',
				'name'
		);
		$whereTypesArray = array(
				'integer',
				'text'
		);
		$whereValuesArray = array(
				$familyId,
				$familyname
		);

		$organism_classification_id = $dbnaturvielfalt -> upsert_query($table, $updateColumnArray, $updateTypesArray, $updateValuesArray, $whereColumnArray, $whereTypesArray, $whereValuesArray);
		checkForErrors();
		if ($organism_classification_id == 0) {
			die("Error while setting CRSF classification_level.");
		}
		//print "familyname = $familyname, organism_classification_id = $organism_classification_id\n";
	}
	$dbnaturvielfalt -> stopTransactionIfPossible();
}

// add all artnames to the organism_scientific_name table, add organism entry accordingly
{
	$columns = array('artname');
	$sql = "FROM import_crsf_basedata WHERE status = 'A' OR status = 'C' GROUP BY artname ORDER BY artname";
	$typesArray = array();
	$valuesArray = array();
	$rows = $dbnaturvielfalt -> select_query($columns, $sql, $typeArray, $typeValue);
	print "Got " . count($rows) . " species\n";
	checkForErrors();
	$dbnaturvielfalt -> startTransactionIfPossible();
	foreach ($rows as $row) {
		$organism_scientific_name_id = 0;
		$speciesname = $row['artname'];
		$speciesnameCount = $dbnaturvielfalt -> select_query(array('count(*)'), "FROM " . $drupalprefix . 'organism_scientific_name WHERE name = ?', array('text'), array($speciesname), false);
		assert(count($speciesnameCount) == 1);
		if ($speciesnameCount[0][0] == 0) {
			$organism_scientific_name_id = 0;
			// add organism entry
			$nextval = $dbnaturvielfalt -> get_nextval('drupal_organism_classification_level_id_seq', "", $typesArray, $valuesArray);
			$table = $drupalprefix . 'organism';
			$updateColumnArray = array(
					'id',
					'parent_id',
					'left_value',
					'right_value'
			);
			$updateTypesArray = array(
					'integer',
					'integer',
					'integer',
					'integer'
			);
			$updateValuesArray = array(
					$nextval,
					$nextval,
					1,
					2
			);

			$whereColumnArray = array(
					'id',
					'parent_id',
					'left_value',
					'right_value'
			);
			$whereTypesArray = array(
					'integer',
					'integer',
					'integer',
					'integer'
			);
			$whereValuesArray = array(
					$nextval,
					$nextval,
					1,
					2
			);
			$organism_id = $dbnaturvielfalt -> upsert_query($table, $updateColumnArray, $updateTypesArray, $updateValuesArray, $whereColumnArray, $whereTypesArray, $whereValuesArray);

			$table = $drupalprefix . 'organism_scientific_name';
			$updateColumnArray = array(
					'organism_id',
					'name'
			);
			$updateTypesArray = array(
					'integer',
					'text'
			);
			$updateValuesArray = array(
					$organism_id,
					$speciesname
			);

			$whereColumnArray = array('name');
			$whereTypesArray = array('text');
			$whereValuesArray = array($speciesname);

			$organism_scientific_name_id = $dbnaturvielfalt -> upsert_query($table, $updateColumnArray, $updateTypesArray, $updateValuesArray, $whereColumnArray, $whereTypesArray, $whereValuesArray);
			checkForErrors();
			if ($organism_scientific_name_id == 0) {
				die("Error while setting CRSF organism_scientific_name_id = $organism_scientific_name_id\n");
			}
			// print "speciesname = $speciesname, organism_scientific_name_id = $organism_scientific_name_id\n";
		}
	}
	$dbnaturvielfalt -> stopTransactionIfPossible();
}
?>
