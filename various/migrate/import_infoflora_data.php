<?php
/**
 * @file import_infoflora_data.php
 * @author Flurin Rindisbacher, 2012, info@flursicht.ch
 *
 * THIS SCRIPT CAN BE RUN MULTIPLE TIMES. each time when we get a 
 * new excel.
 *
 * imports the infoflora/crsf data. infoflora gave us an excel-file
 * with the flora-data. this script adds the new organisms   
 * or updates them if they already exist in our db.
 * 
 * the scientific_names are always stored in organism.scientific_name. 
 * due to the migration from organism_scientific_name to organism_synonym
 * there exist some scientific names that are not really synonyms. 
 * if NOM_COMPLET is found in the synonym table it is deleted 
 * (because we know, that it's not a synonym but a real scientific name) 
 * 
 * new organisms are added sing @rettichschnidi's classification class.
 * existing organisms are updated using the organism_update_* functions (using the drupal api)
 * 
 * @see improt_evab_crsf_basedata_v2.php for the initial data import
 */

// do a full drupal bootstrap to get the db API
define('DRUPAL_ROOT', getcwd() . '/../../webroot_drupal');
include_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

// add the classification class
require_once(dirname(__FILE__) . '/lib/bootstrap.php');
require_once($libdir . '/Classification.php');
require_once($configdir . '/databases.php');
global $errors;

// set here your local drupal database prefix
$drupalprefix = 'naturvielfalt_';

$classifier = new Classification(
	$config['naturvielfalt_dev']['driver'],
	$config['naturvielfalt_dev']['name'],
	$config['naturvielfalt_dev']['user'],
	$config['naturvielfalt_dev']['password'],
	$config['naturvielfalt_dev']['host']);

// the array used in $plant and $db_plant indizes
define('ORGANISM_CRSF_ID', 0);
define('ORGANISM_FAMILY', 1);
define('ORGANISM_GENUS', 2);
define('ORGANISM_SCIENTIFIC_NAME', 3);
define('ORGANISM_DE', 4);
define('ORGANISM_FR', 5);
define('ORGANISM_IT', 6);
define('ORGANISM_DB_ID', 7); // organism_id for existing plants

/*
 * the following array controls, which data will be updated and how.
 * array-structure: 
 * array{
 * 		INDEX_TO_COMPARE => UPDATE_FUNCTION
 * }
 * 
 * INDEX_TO_COMPARE is the array index, which is used to compare our
 * database-data with the new infoflora data. 
 * UPDATE_FUNCTION is the function that can be called to upate the
 * this attribute. 
 * 
 * the interface of all UPDATE_FUNCTION's is the following:
 * UPDATE_FUNCTION($organism_id, $plant)
 * 		$organism_id --> the database-id of the existing organism
 * 		$plant --> the csv-structure for this plant 
 * 
 */
$update_indizes = array(
		ORGANISM_FAMILY => 'organism_update_family_or_genus',
		ORGANISM_GENUS => 'organism_update_family_or_genus',
		ORGANISM_SCIENTIFIC_NAME => 'organism_update_scientific_name',
		ORGANISM_DE => 'organism_update_lang_de',
		ORGANISM_FR => 'organism_update_lang_fr',
		ORGANISM_IT => 'organism_update_lang_it'
);

// check for a given filename
$arguments = getopt("i:o:m:");

if (!isset($arguments['i'])) {
	die("No csv-filename (-i) given!\n");
}

if (!file_exists($arguments['i'])) {
	die("File " . $arguments['i'] . " does not exist!\n");
}
$filename = $arguments['i'];

/*
 *  cache all existing plants, their family, genus and crsf_id
 *  the key for the cache is the crsf_id
 */
echo "Building the Flora-Cache: \n";

$cache = array(
		// flora: the current database flora data
		flora => array(),
		
		// caches all organisms, for which family or genus have been updated
		// there is only one update for family OR genus needed
		family_genus_updated => array(),
);
$result = db_query(
	"select o.id, o.scientific_name, oav.number_value as crsf_id, oc.name as genus, family.name as family, lang_de.name as de, lang_fr.name as fr, lang_it.name as it
	from {organism} o
	inner join {organism_attribute_value_subscription} oas on o.id = oas.organism_id
	inner join {organism_attribute_value} oav on oav.id = oas.organism_attribute_value_id
	inner join {organism_attribute} oa on oa.id = oav.organism_attribute_id
	inner join {organism_classification_subscription} ocs on ocs.organism_id = o.id
	inner join {organism_classification} oc on ocs.organism_classification_id = oc.id
	inner join {organism_classification} family on family.id = oc.parent_id
	left join {organism_lang} lang_de on lang_de.organism_id = o.id and lang_de.languages_language = 'de'
	left join {organism_lang} lang_fr on lang_fr.organism_id = o.id and lang_fr.languages_language = 'fr'
	left join {organism_lang} lang_it on lang_it.organism_id = o.id and lang_it.languages_language = 'it'
where oa.name = 'CRSF number'
order by crsf_id asc;");

echo "\t" . $result->rowCount() . " flora entries found\n";

foreach ($result as $record) {
	$cache["flora"][$record->crsf_id] = array(
			$record->crsf_id,
			trim($record->family),
			trim($record->genus),
			trim($record->scientific_name),
			trim($record->de),
			trim($record->fr),
			trim($record->it),
			trim($record->id),
	);
}

echo "DONE\n";

/*
 * open and parse the csf file. we expect the following structure:
 *
 *     Column     |          Description      
 *----------------+------------------------+-----------
 * NO_ISFS        | the identifying number. corresponds 
 *                | to our 'CRSF number' organism attribute 
 * NOM_FAMILLE    | => Classification level 8 in our tables
 * NOM_GENRE      | => Classification level 9 in our tables
 * NOM_COMPLET    | organism.scientific_name
 * de             | german translation
 * fr          	  | french translation 
 * it             | italian translation 
 * 
 * the first line in the .csv file is handled as the title-bar (therefore ignored)
 * 
 */
echo "Starting the import:\n";
$csv = file($filename);
$new_organisms = array(
		0 => array(
				'classificator' => 'CRSF',
				'classifications' => array(
						array(
								'classificationlevelname' => 'family'
						),
						array(
								'classificationlevelname' => 'genus'
						)
				)
		)
);
for ($i = 1; $i < count($csv); $i++) { // skip the first row

	$plant = str_getcsv($csv[$i], ';');
	$db_plant = NULL;

	$plant[ORGANISM_SCIENTIFIC_NAME] = trim($plant[ORGANISM_SCIENTIFIC_NAME]);
	$plant[ORGANISM_FAMILY] = trim($plant[ORGANISM_FAMILY]);
	$plant[ORGANISM_GENUS] = trim($plant[ORGANISM_GENUS]);
	$plant[ORGANISM_DE] = trim($plant[ORGANISM_DE]);
	$plant[ORGANISM_FR] = trim($plant[ORGANISM_FR]);
	$plant[ORGANISM_IT] = trim($plant[ORGANISM_IT]);

	// some checks to prevent data loss (after the update)
	assert(count($plant) == 7);
	assert(!empty($plant[ORGANISM_SCIENTIFIC_NAME]));
	assert(!empty($plant[ORGANISM_FAMILY]));
	assert(!empty($plant[ORGANISM_GENUS]));

	// is this an update or a new plant?
	if (array_key_exists($plant[ORGANISM_CRSF_ID], $cache["flora"])) {
		$db_plant = $cache["flora"][$plant[ORGANISM_CRSF_ID]];

		organism_check_and_update($db_plant, $plant);

	} else {
		// create the array for inserting a new organism
		$new_organisms[$plant[ORGANISM_CRSF_ID]] = organism_new_organism($plant);
		echo "NEW: " . $plant[ORGANISM_CRSF_ID] . ", "
		. $plant[ORGANISM_SCIENTIFIC_NAME] . ", $plant[ORGANISM_DE]\n";
	}
}

// add the new organisms
$classifier->addOrganisms($new_organisms);

echo "import finished!\n";

/*
 * ################################################################
 * 
 * ################################################################
 */

/**
 * compares our database-infos with the new infoflora data.
 * a plant is only updated, if there was a change.
 * 
 * the db-attributes are updated using its "$update_func" function.
 * the global $update_indizes array controls, which attributes
 * are checked and how they can be updated 
 * (have a look at the update_* functions)
 * 
 * @param array $db_plant the current plant-data from our database
 * @param array $plant the infoflora-data (csv)
 */
function organism_check_and_update($db_plant, $plant) {
	global $update_indizes;

	/* 
	 * search for changed attributes and update them
	 */
	$didUpdate = false;
	foreach ($update_indizes as $idx => $update_func) {
		if ($plant[$idx] != $db_plant[$idx]) {
			$update_func($db_plant[ORGANISM_DB_ID], $plant);
			$didUpdate = true;
		}
	}

	if ($didUpdate)
		echo "UPDATE: " . $plant[ORGANISM_CRSF_ID] . ", "
				. $plant[ORGANISM_SCIENTIFIC_NAME] . ", $plant[ORGANISM_DE]\n";
}

/**
 * returns the structure for a new organism. the plants
 * are stored using the Classification class
 *  
 * @param $plant the data from the infoflora csv
 * @see mport_evab_crsf_basedata_v2.php
 */
function organism_new_organism($plant) {

	$organism =  array(
			'classificator' => 'CRSF',
			'classifications' => array(
					array(
							'classificationlevelname' => 'family',
							'classificationname' => $plant[ORGANISM_FAMILY]
					),
					array(
							'classificationlevelname' => 'genus',
							'classificationname' => $plant[ORGANISM_GENUS]
					)
			),
			'artgroups' => array(
					'Flora'
			),
			'scientific_name' => $plant[ORGANISM_SCIENTIFIC_NAME],
			'classification_name_translations' => array(

			),
			'attributes' => array(
					'CRSF number' => array(
							'comment' => 'Number which got assigned to this organism by the CRSF.',
							'valuetype' => 'n',
							'values' => array(
									$plant[ORGANISM_CRSF_ID]
							)
					)
			)
	);

	if(!empty($plant[ORGANISM_DE])){
		$organism['classification_name_translations']['de'] = array($plant[ORGANISM_DE]);
	}
	
	if(!empty($plant[ORGANISM_FR])){
		$organism['classification_name_translations']['fr'] = array($plant[ORGANISM_FR]);
	}
	
	if(!empty($plant[ORGANISM_IT])){
		$organism['classification_name_translations']['it'] = array($plant[ORGANISM_IT]);
	}

	return $organism;
}

/*
 * ################################################################
 * the db-update functions  
 * 
 * these functions are callled by organism_check_and_update()
 * ################################################################
 */

/**
 * is called when the family or the genus has changed
 * NOTE: we always update genus AND family. if genus was 
 * already updated, there is no need to update the family also
 * that is why all updated id's are cached 
 * @param int $organism_id the database-id of the organism
 * @param array $plant the plant-data from the infoflora csv
 */
function organism_update_family_or_genus($organism_id, $plant) {
	global $classifier;
	
	assert($organism_id > 0);
	
	// prevent classifying the same organism multiple times
	if(in_array($organism_id, $cache['family_genus_updated']))
			return;
	
	// get or create the classificationId for this family/genus
	$organism =  array(
			'classificator' => 'CRSF',
			'classifications' => array(
					array(
							'classificationlevelname' => 'family',
							'classificationname' => $plant[ORGANISM_FAMILY]
					),
					array(
							'classificationlevelname' => 'genus',
							'classificationname' => $plant[ORGANISM_GENUS]
					)
			)
	);
	
	$classificationId = $classifier->getClassificationId($organism);
	
	// make sure we got a number
	assert(is_numeric($classificationId) && $classificationId > 0);
	
	// write the new classification
	$txn = db_transaction();
	
	$num_updated = db_update('organism_classification_subscription')
	->fields(array(
			'organism_classification_id' => $classificationId
	))
	->condition('organism_id', $organism_id, '=')
	->execute();
		
	if($num_updated > 1){
		$txn->rollback();
		die("Updated more than one classification! ($organism_id)");
	}
	
	// cache this organism_id
	$cache['family_genus_updated'][] = $organism_id;
}

/**
 * is called when the scientific-name has changed.
 * updates organism.scientific_name
 * 
 * @param int $organism_id the database-id of the organism
 * @param array $plant the plant-data from the infoflora csv
 */
function organism_update_scientific_name($organism_id, $plant) {
	assert($organism_id > 0);

	$txn = db_transaction();

	// update the scientific name
	$num_updated = db_update('organism')->fields(
			array('scientific_name' => $plant[ORGANISM_SCIENTIFIC_NAME]))
		->condition('id', $organism_id, '=')
		->execute();

	if ($num_updated != 1) {
		$txn->rollback();
		die("Updated more than one scientific name! ($organism_id)");
	}

	// check and update the synonym table
	$result = db_query(
		'select id from {organism_synonym}
			where organism_id = :organism_id and name = :scientific_name',
		array('organism_id' => $organism_id,
				'scientific_name' => $plant[ORGANISM_SCIENTIFIC_NAME]
		));

	foreach ($result as $record) {
		$rows_deleted = db_delete('organism_synonym')->condition(
				'id',
				$record->id)
			->condition('organism_id', $organism_id) // just to make sure we don't delete too much
			->execute();
		if ($rows_deleted > 1) {
			$txn->rollback();
			die("Probably deleted too many synonyms($organism_id)");
		}
	}
}

/**
 * is called, when the german translation has changed
 * 
 * @param int $organism_id the database-id of the organism
 * @param array $plant the plant-data from the infoflora csv
 */
function organism_update_lang_de($organism_id, $plant) {
	organism_update_lang($organism_id, 'de', $plant[ORGANISM_DE]);
}

/**
 * is called, when the french translation has changed
 *
 * @param int $organism_id the database-id of the organism
 * @param array $plant the plant-data from the infoflora csv
 */
function organism_update_lang_fr($organism_id, $plant) {
	organism_update_lang($organism_id, 'fr', $plant[ORGANISM_FR]);
}

/**
 * is called, when the italian translation has changed
 * 
 * @param int $organism_id the database-id of the organism
 * @param array $plant the plant-data from the infoflora csv
 */
function organism_update_lang_it($organism_id, $plant) {
	organism_update_lang($organism_id, 'it', $plant[ORGANISM_IT]);
}

/*
 * ################################################################
 * helpers
 * ################################################################
 */
/**
 * updates the $lang-translation for the given organism.
 * 
 * @param int $organism_id the db-id of this organism
 * @param string $lang the language 'de', 'fr', 'it'
 * @param string $translation the translated organism-name
 */
function organism_update_lang($organism_id, $lang, $translation) {

	assert($organism_id > 0);
	assert($lang == 'de' || $lang == 'fr' || $lang == 'it');

	$txn = db_transaction();

	// remove the existing translation (if available)
	$rows_deleted = db_delete('organism_lang')->condition(
			'organism_id',
			$organism_id)
		->condition('languages_language', $lang)
		->execute();

	if ($rows_deleted > 2) { // some organisms had two french translations...
	// what???
		$txn->rollback();
		die(
			"Deleted more than two translation! Rolling back...($organism_id, $lang)");
	}


	// don't store empty translations
	$translation = trim($translation);
	if(empty($translation))
		return;
	
	// insert a new translation
	$query = db_insert('organism_lang')->fields(
			array('languages_language',
					'organism_id',
					'name'
			));
	$query->values(
			array(languages_language => $lang,
					organism_id => $organism_id,
					name => $translation
			));
	$new_id = $query->execute();

	if (!is_numeric($new_id)) {
		$txn->rollback();
		die("Insert of translation failed! ($organism_id, $lang)");
	}
}
?>
