<?php
/**
 * @file import_moss_basedata.php
 * @author André Kälin, 2013
 *
 * Import moss base data.
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

$importTable = 'import_nism_basedata';

/**
 * Copy all moss data including metadata into naturvielfalt DB.
 *
 * $columns = array(
 * 	'bryo_taxa_id',
 * 	'klasse',
 * 	'familie',
 * 	'gattung',
 * 	'art',
 * 	'autor',
 *  'unterart',
 *  'autor_unterart',
 *  'name_de',
 *  'name_en',
 *  'rl2004
 * );
 *
 */

$classification_data = array(
		'class' => array(
				'columnname' => 'klasse'
		),
		'family' => array(
				'columnname' => 'familie'
		),
		'genus' => array(
				'columnname' => 'gattung'
		),
// 		'species' => array(
// 				'columnname' => 'art'
// 		),
// 		'subspecies' => array(
// 				'columnname' => 'unterart'
// 		),
);

// store all level names in $classification_level_columns
$classification_level_columns = array();
foreach ($classification_data as $classification_level_name => $data) {
	$classification_level_columns[] = $data['columnname'];
}

// extract all unique organism classifications
$columnstring = implode(',', $classification_level_columns);
$sql = "SELECT
			bryo_taxa_id,
			$columnstring,
			art,
			autor,
			unterart,
			autor_unterart,
			name_de,
			name_en,
			rl2004
		FROM
			$importTable
		ORDER BY
			$columnstring, art, unterart, bryo_taxa_id";

if (false) {
	print "Query: $sql\n";
}

$rows = $db->query($sql, array(), array());

if (count($rows) <= 1) {
	die(print_r($rows));
}
$classificator = 'NISM';
$organisms = array();
$organisms[] = array(
		'classificator' => $classificator,
		'classifications' => array(
				array(
						'classificationlevelname' => 'class'
				),
				array(
						'classificationlevelname' => 'family'
				),
				array(
						'classificationlevelname' => 'genus'
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
	//Scientific name: gattung art autor/autor unterart
	$scientificName = $row['gattung'].' '.$row['art'];
	if (!empty($row['unterart'])) $scientificName .= ' '.$row['unterart'];
	if (!empty($row['autor_unterart'])) $scientificName .= ' '.$row['autor_unterart'];
	else $scientificName .= ' '.$row['autor'];
	
	$organism = array(
			'classificator' => $classificator,
			'classifications' => $classifications,
			'artgroups' => array('Moose'),
			'scientific_name' => $scientificName,
			'synonyms' => array(),
			'attributes' => array(
					'NISM number' => array(
							'comment' => 'Number which got assigned to the moss organism by the NISM.',
							'valuetype' => 'n',
							'values' => array(
									$row['bryo_taxa_id']
							)
					),
					'Red List 2004 (CH)' => array(
							'comment' => 'The status of the organism according to the red list 2004 (CH).',
							'valuetype' => 't',
							'values' => array(
									$row['rl2004']
							)
					),
			)
	);
	
	$author = isset($row['autor']) ? $row['autor'] : $row['autor_unterart'];
	if (isset($author)) {
		$organism['attributes']['Author'] = array(
				'comment' => 'Name of the scientist who classified this organism.',
				'valuetype' => 't',
				'values' => array(
						$author
				)
		);
	}
	
	if ($row['name_de'] != NULL) {
		$organism['classification_name_translations']['de'] = array(
				$row['name_de']
		);
	}
	if ($row['name_en'] != NULL) {
		$organism['classification_name_translations']['en'] = array(
				$row['name_en']
		);
	}
	$organisms[] = $organism;
}

$classifier->addOrganisms($organisms);
?>
