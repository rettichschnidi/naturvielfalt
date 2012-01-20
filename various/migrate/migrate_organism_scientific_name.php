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
 * Copy all Latin names
 */
{
	$ownselect = 'SELECT DISTINCT ON(name_sc)';
	$columns = array(
			'id',
			'name_sc'
	);
	$sql = "FROM organism WHERE name_sc IS NOT NULL AND name_sc != ''";
	$typeArray = array();
	$typeValue = array();
	$rows = $dbswissmon -> select_query($columns, $sql, $typeArray, $typeValue, false, $ownselect);

	print 'Got ' . count($rows) . " rows, starting to transfer them into our new DB.\n";

	//print "Result:\n" . var_export($rows, true) . "\n";

	$success = $dbnaturvielfalt -> multiinsert_query(array(
			'organism_id',
			'value'
	), $drupalprefix . 'organism_scientific_name', array(
			'integer',
			'text'
	), $rows);

	if (!$success) {
		print "Errors: " . var_dump($errors, true) . "\n\n";
	}
}
?>