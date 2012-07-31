<?php
/**
 * @file import_fungus_basedatav2.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import fungus to the new naturvielfalt DB.
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

$importTable = 'import_fungus_basedata';

/**
 * Copy all fungus data including metadata into naturvielfalt DB.
 *
 * $columns = array(
 * 	'fungus_artnr',
 * 	'pilzname',
 * 	'author',
 * 	'klasse',
 * 	'erstnachweis',
 * 	'anzahl_funde'
 * );
 *
 */

$classification_data = array(
		'class' => array(
				'columnname' => 'klasse'
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
			fungus_artnr,
			$columnstring,
			pilzname,
			author,
			erstnachweis,
			anzahl_funde
		FROM
			$importTable
		ORDER BY
			$columnstring, fungus_artnr";

if (false) {
	print "Query: $sql\n";
}

$rows = $db->query($sql, array(), array());

if (count($rows) <= 1) {
	die(print_r($rows));
}
$classificator = 'Fungus';
$organisms = array();
$organisms[] = array(
		'classificator' => $classificator,
		'classifications' => array(
				array(
						'classificationlevelname' => 'class'
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
			'artgroups' => array('Fungus'),
			'scientific_names' => array(
					$row['pilzname']
			),
			'attributes' => array(
					'Fungus number' => array(
							'comment' => 'Number which got assigned to this organism by the WSL.',
							'valuetype' => 'n',
							'values' => array(
									$row['fungus_artnr']
							)
					),
					'First find' => array(
							'comment' => 'The year of the first documented sight of this organism.',
							'valuetype' => 't',
							'values' => array(
									$row['erstnachweis']
							)
					),
			)
	);

	if ($row['author'] != '(null)') {
		$organism['attributes']['Author'] = array(
				'comment' => 'Name of the scientist who classified this organism.',
				'valuetype' => 't',
				'values' => array(
						$row['author']
				)
		);
	}

	$organisms[] = $organism;
}

$classifier->addOrganisms($organisms);
?>
