<?php
/**
 * @file import_evab_cscf_basedata_v2.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import evab's classification of the fauna to the new naturvielfalt DB
 */

require_once(dirname(__FILE__) . '/lib/bootstrap.php');
require_once($libdir . '/Classification.php');
require_once($configdir . '/databases.php');
global $config;

$dbevab = new Db(
	$config['evab']['driver'],
	$config['evab']['name'],
	$config['evab']['user'],
	$config['evab']['password'],
	$config['evab']['host']);

$classifier = new Classification(
	$config['naturvielfalt_dev']['driver'],
	$config['naturvielfalt_dev']['name'],
	$config['naturvielfalt_dev']['user'],
	$config['naturvielfalt_dev']['password'],
	$config['naturvielfalt_dev']['host']);

/**
 * Copy all cscf organisms including metadata into naturvielfalt DB.
 *
 * relevant view:
 *  - 'public.import_evab_cscf_view'
 *
 *    Column   |          Type          | Modifiers
 * ------------+------------------------+-----------
 *  nuesp      | integer                |
 *  class      | character varying(255) |
 *  evaborder  | character varying(255) |
 *  family     | character varying(255) |
 *  genus      | character varying(255) |
 *  subgenus   | character varying(255) |
 *  species    | character varying(255) |
 *  subspecies | character varying(255) |
 *  name_de    | character varying(255) |
 *  name_fr    | character varying(255) |
 *  name_it    | character varying(255) |
 *  name_rm    | character varying(255) |
 *  name_en    | character varying(255) |
 *  name_latin | character varying(255) |
 */

$importTable = 'import_evab_cscf_view';

$classification_data = array(
		'class' => array(
				'columnname' => 'class'
		),
		// order is a reserved word so evaborder got used
		'order' => array(
				'columnname' => 'evaborder'
		),
		'family' => array(
				'columnname' => 'family'
		),
		'genus' => array(
				'columnname' => 'genus'
		),
		'subgenus' => array(
				'columnname' => 'subgenus'
		),
// 		'species' => array(
// 				'columnname' => 'species'
// 		),
// 		'subspecies' => array(
// 				'columnname' => 'subspecies'
// 		)
);

// store all level names in $classification_level_columns
$classification_level_columns = array();
foreach ($classification_data as $classification_level_name => $data) {
	$classification_level_columns[] = $data['columnname'];
}

// extract all unique organism classifications
$columnstring = implode(',', $classification_level_columns);
$sql = "SELECT
			nuesp,
			$columnstring,
			name_de,
			name_fr,
			name_en,
			name_it,
			name_rm,
			name_en,
			name_latin
		FROM
			$importTable
		ORDER BY
			$columnstring, nuesp";
if (false) {
	print "Query: $sql\n";
}

$typeArray = array();
$typeValue = array();
$rows = $dbevab->query($sql, $typeArray, $typeValue);

$classificator = 'CSCF';
$organisms = array();
$organisms[] = array(
		'classificator' => $classificator,
		'classifications' => array(
				array(
						'classificationlevelname' => 'class'
				),
				array(
						'classificationlevelname' => 'order'
				),
				array(
						'classificationlevelname' => 'family'
				),
				array(
						'classificationlevelname' => 'genus'
				),
				array(
						'classificationlevelname' => 'subgenus'
				),
// 				array(
// 						'classificationlevelname' => 'species'
// 				),
// 				array(
// 						'classificationlevelname' => 'subspecies'
// 				)
		)
);

foreach ($rows as $row) {
	// leave out the 2nd entry of same organism (failure in CSCF database)
	if ($row['nuesp'] == 23927) {
		continue;
	}
	$classifications = array();
	foreach ($classification_data as $class => $table) {
		$value = $row[$table['columnname']];
		assert(
			$value != NULL || $class == 'subgenus' || $class == 'species'
					|| $class == 'subspecies');
		if ($value == NULL) {
			continue;
		}
		$classifications[] = array(
				'classificationlevelname' => $class,
				'classificationname' => $value,
		);
	}
	$organism = array(
			'classificator' => $classificator,
			'classifications' => $classifications,
			'scientific_names' => array(
					$row['name_latin']
			),
			'classification_name_translations' => array(),
			'attributes' => array(
					'NUESP' => array(
							'comment' => 'Number which got assigned to this organism by the CSCF.',
							'valuetype' => 'n',
							'values' => array(
									$row['nuesp']
							)
					),
			)
	);
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
	if ($row['name_fr'] != NULL) {
		$organism['classification_name_translations']['fr'] = array(
				$row['name_fr']
		);
	}
	if ($row['name_it'] != NULL) {
		$organism['classification_name_translations']['it'] = array(
				$row['name_it']
		);
	}
	if ($row['name_rm'] != NULL) {
		$organism['classification_name_translations']['rm'] = array(
				$row['name_rm']
		);
	}

	$organisms[] = $organism;
}

$classifier->addOrganisms($organisms);
?>
