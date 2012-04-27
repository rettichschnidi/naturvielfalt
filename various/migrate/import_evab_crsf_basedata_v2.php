<?php
/**
 * @file import_evab_cscf_basedatav2.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import evab's classification of the fauna to the new naturvielfalt DB
 */

require_once(dirname(__FILE__) . '/lib/bootstrap.php');
require_once($libdir . '/Classification.php');
require_once($configdir . '/databases.php');
global $errors;

$classifier = new Classification(
	$config['naturvielfalt_dev']['driver'],
	$config['naturvielfalt_dev']['name'],
	$config['naturvielfalt_dev']['user'],
	$config['naturvielfalt_dev']['password'],
	$config['naturvielfalt_dev']['host']);

$dbevab = new Db(
	$config['evab']['driver'],
	$config['evab']['name'],
	$config['evab']['user'],
	$config['evab']['password'],
	$config['evab']['host']);

/**
 * Copy all crsf organisms including metadata into naturvielfalt DB
 *
 * relevant view:
 *  - 'public.import_evab_crsf_view'
 *
 *     Column     |          Type          | Modifiers 
 *----------------+------------------------+-----------
 * nr             | integer                | 
 * status         | character varying(1)   | 
 * familie        | character varying(255) | 
 * gattung        | character varying(100) | 
 * art            | character varying(100) | 
 * autor          | character varying(100) | 
 * rang           | character varying(50)  | 
 * nameunterart   | character varying(100) | 
 * gueltigenamen  | character varying(255) | 
 * offizielleart  | integer                | 
 * deutsch        | character varying(255) | 
 * franzoesisch   | character varying(255) | 
 * florahelvetica | character varying(50)  | 
 * xenophyte      | integer                | 
 */

/**
 * Create classifier if not already existing,
 * set $organism_classifier_id
 */
$importTable = 'import_evab_crsf_view';

$classification_data = array(
		'family' => array(
				'columnname' => 'familie'
		),
		// order is a reserved word so evaborder got used
		'genus' => array(
				'columnname' => 'gattung'
		),
		'species' => array(
				'columnname' => 'art'
		),
		'subspecies' => array(
				'columnname' => 'nameunterart'
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
			nr,
			$columnstring,
			deutsch,
			franzoesisch,
			italienisch,
			name,
			florahelvetica,
			xenophyte,
			autor,
			rang
		FROM
			$importTable
		WHERE
			nr < 1000000 AND status IN ('A', 'Z', 'E')
		ORDER BY
			$columnstring";

$rows = $dbevab->query($sql, array(), array());
assert(!empty($rows));

$classificator = 'CRSF';
$organisms = array();
$organisms[] = array(
		'classificator' => $classificator,
		'classifications' => array(
				array(
						'classificationlevelname' => 'family'
				),
				array(
						'classificationlevelname' => 'genus'
				),
				array(
						'classificationlevelname' => 'species'
				),
				array(
						'classificationlevelname' => 'subspecies'
				)
		)
);

foreach ($rows as $row) {
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
					$row['name']
			),
			'classification_name_translations' => array(),
			'attributes' => array(
					'CRSF number' => array(
							'valuetype' => 'n',
							'values' => array(
									$row['nr']
							)
					),
					'Author' => array(
							'valuetype' => 't',
							'values' => array(
									$row['autor']
							)
					),
					'Xenophyte' => array(
							'valuetype' => 'b',
							'values' => array(
									$row['xenophyte'] ? 1 : 0
							)
					),
			)
	);
	if ($row['deutsch'] != NULL) {
		$organism['classification_name_translations']['de'] = array(
				$row['deutsch']
		);
	}
	if ($row['franzoesisch'] != NULL) {
		$organism['classification_name_translations']['fr'] = array(
				$row['franzoesisch']
		);
	}
	if ($row['italienisch'] != NULL) {
		$organism['classification_name_translations']['it'] = array(
				$row['italienisch']
		);
	}
	if ($row['florahelvetica'] != NULL) {
		$organism['attributes']['Flora Helvetica'] = array(
				'valuetype' => 't',
				'values' => array(
						$row['florahelvetica']
				)
		);
	}
	$organisms[] = $organism;
}

$classifier->addOrganisms($organisms);
?>
