<?php
/**
 * @file import_oldhabitat_basedata.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import data from the old habitat module to the new naturvielfalt DB
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

$allData = $db->query(
		'SELECT label, name_de, name_lt, name_fr, name_it FROM habitat ORDER BY label');

var_dump($errors);
foreach ($allData as $data) {
	$label = $data['label'];
	$name = $data['name_de'];
	$name_de = $data['name_de'];
	$name_fr = $data['name_fr'];
	$name_it = $data['name_it'];
	$name_scientific = $data['name_lt'];
	print "Label: $label, Name: $name, Latin: $name_scientific\n";
	$db->query(
			'INSERT INTO ' . $drupalprefix
					. 'habitat (label, name, name_scientific, name_de, name_fr, name_it) VALUES(?, ?, ?, ?, ?, ?)',
			array('text',
					'text',
					'text',
					'text',
					'text',
					'text'
			),
			array($label,
					$name,
					$name_scientific,
					$name_de,
					$name_fr,
					$name_it
			),
			false);
}
;

?>
