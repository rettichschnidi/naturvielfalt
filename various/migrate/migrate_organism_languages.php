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

$dbswissmon = new Db($config['swissmon']['driver'], $config['swissmon']['name'], $config['swissmon']['user'], $config['swissmon']['password'], $config['swissmon']['host']);
$dbnaturvielfalt = new Db($config['naturvielfalt_dev']['driver'], $config['naturvielfalt_dev']['name'], $config['naturvielfalt_dev']['user'], $config['naturvielfalt_dev']['password'], $config['naturvielfalt_dev']['host']);

/**
 * Copy all available translations from this language
 */
function copyLanguage($languageSourceColumn, $languageShort) {
	global $dbswissmon;
	global $dbnaturvielfalt;
	global $drupalprefix;
	global $errors;
	
	$columns = array(
			'id',
			$languageSourceColumn
	);
	$ownselect = 'SELECT \'' . $languageShort . '\', ';
	$sql = "FROM organism WHERE $languageSourceColumn IS NOT NULL AND $languageSourceColumn != ''";
	$typeArray = array();
	$typeValue = array();
	$rows = $dbswissmon -> select_query($columns, $sql, $typeArray, $typeValue, false, $ownselect);

	print 'Got ' . count($rows) . " rows for language '" . $languageSourceColumn . "', starting to transfer them into our new DB.\n";

	// print "Result:\n" . var_export($rows, true) . "\n";

	$success = $dbnaturvielfalt -> multiinsert_query(array(
			'languages_language',
			'organism_id',
			'name'
	), $drupalprefix . 'organism_lang', array(
			'text',
			'integer',
			'text'
	), $rows);

	if (!$success) {
		print "Errors: " . var_dump($errors, true) . "\n\n";
	}
}

copyLanguage('name_fr', 'fr');
copyLanguage('name_de', 'de');
copyLanguage('name_en', 'en');
copyLanguage('name_it', 'it');
copyLanguage('name_rm', 'rm');
?>