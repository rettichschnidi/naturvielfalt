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
 * Copy all organisms
 */
$columns = array(
		'id',
		'id AS parent_id'
);
$sql = "FROM organism";
$typeArray = array();
$typeValue = array();
$rows = $dbswissmon -> select_query($columns, $sql, $typeArray, $typeValue, false);

print 'Got ' . count($rows) . " rows, starting to transfer them into our new DB.\n";

$success = $dbnaturvielfalt -> multiinsert_query(array(
		'id',
		'parent_id'
), $drupalprefix . 'organism', array(
		'integer',
		'integer'
), $rows);

if (!$success) {
	print "Errors: " . var_dump($errors, true) . "\n\n";
}
?>