<?php
/**
 * @file import_swisslichens_basedatav2.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import lichens to the new naturvielfalt DB.
 */

require_once(dirname(__FILE__) . '/lib/bootstrap.php');
require_once($libdir . '/Classification.php');
require_once($configdir . '/databases.php');
global $config;

$db = new Db(
	$config['naturvielfalt_dev']['driver'],
	$config['naturvielfalt_dev']['name'],
	$config['naturvielfalt_dev']['user'],
	$config['naturvielfalt_dev']['password'],
	$config['naturvielfalt_dev']['host']);

$classifier = new Classification(
	$config['naturvielfalt_dev']['driver'],
	$config['naturvielfalt_dev']['name'],
	$config['naturvielfalt_dev']['user'],
	$config['naturvielfalt_dev']['password'],
	$config['naturvielfalt_dev']['host']);

$importTable = 'import_swisslichens_basedata';
/**
 * Copy all SwissLichens into naturvielfalt DB
 *
 *    Column        | Type | Modifiers
 *  ----------------+------+-----------
 *  genus           | text | not null
 *  species         | text | not null
 *  scientific_name | text | not null
 */

$classification_data = array(
		'genus' => array(
				'columnname' => 'genus'
		),
		'species' => array(
				'columnname' => 'species'
		)
);

// store all level names in $classification_level_columns
$classification_level_columns = array();
foreach ($classification_data as $classification_level_name => $data) {
	$classification_level_columns[] = $data['columnname'];
}

// extract all unique organism classifications
$columnstring = implode(',', $classification_level_columns);
$sql = "SELECT
			$columnstring,
			scientific_name
		FROM
			$importTable
		GROUP BY
			genus,
			species,
			scientific_name
		ORDER BY
			scientific_name";

if (false) {
	print "Query: $sql\n";
}
$rows = $db->query($sql, array(), array());

if (count($rows) <= 1) {
	die(print_r($rows));
}
$classificator = 'Swisslichens';
$organisms = array();
$organisms[] = array(
		'classificator' => $classificator,
		'classifications' => array(
				array(
						'classificationlevelname' => 'genus'
				),
				array(
						'classificationlevelname' => 'species'
				)
		)
);

foreach ($rows as $row) {
	$classifications = array();
	foreach ($classification_data as $class => $table) {
		$value = $row[$table['columnname']];
		assert($value != NULL);
		$classifications[] = array(
				'classificationlevelname' => $class,
				'classificationname' => $value,
		);
	}
	$organism = array(
			'classificator' => $classificator,
			'classifications' => $classifications,
			'scientific_names' => array(
					$row['scientific_name']
			),
	);

	$organisms[] = $organism;
}

$classifier->addOrganisms($organisms);

?>
