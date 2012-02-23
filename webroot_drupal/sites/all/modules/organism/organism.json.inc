<?php
/**
 * @file organism.overview.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

/**
 * Menu callback. This will be called for visits to /organism/json.
 * Per default it returns all classifier as a json.
 * Post-Requests from datatable are supportet as well (search, partial response, offset, etc).
 * 
 * Example JSON answer:
 * {
 *  "page":null,
 *  "total":4,
 *  "rows":[
 *      {
 *          "id":null,
 *          "cell":{
 *              "oc_id":"1",
 *              "oc_name":"SwissLichens",
 *              "levelcount":"1"
 *          }
 *      },
 *      {
 *          "id":null,
 *          "cell":{
 *              "oc_id":"7382",
 *              "oc_name":"CSCF",
 *              "levelcount":"5"
 *          }
 *      }
 *  ]
 * }
 * @param none, except the fileds via post
 * @return json string
 */
function organism_show_classifiers_json() {
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'oc.id AS oc_id',
			'oc.name AS oc_name',
			'COUNT(ocl.id) - 1 AS levelcount'
	);

	$sql['dbSearchColumns'] = array(
			'oc.id',
			'oc.name'
	);

	/* DB table to use */
	$sql['dbTable'] = "{organism_classification} oc";
	/* Joins to use */
	$sql['dbJoins'] = "JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.prime_father_id";
	/* Group */
	$sql['dbGroupBy'] = "GROUP BY oc_id, oc_name";
	$sql['dbWhere'] = 'oc.prime_father_id = oc.id';
	$sql['id'] = 'oc_id';

	$output = datatable_dbRequest($sql);
	return drupal_json_output($output);
}

/**
 * Menu callback. This will be called for visits to /organism/classification/%/classificationjson.
 * @return array
 * @todo Find a better title
 */
function organism_show_classification_classificationjson($classificationid) {
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'oc.id AS oc_id',
			'oc.name AS oc_name',
			'oc.organism_classification_level_id AS classification_level_id',
			'ocl.name AS levelname'
	);

	$sql['dbSearchColumns'] = array(
			'oc.id',
			'oc.name',
			'ocl.name'
	);

	/* DB table to use */
	$sql['dbTable'] = '{organism_classification} oc';
	/* Joins to use */
	$sql['dbJoins'] = 'JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.id';
	/* Group */
	$sql['dbGroupBy'] = 'GROUP BY oc_id, oc_name, classification_level_id, levelname';
	$sql['dbWhere'] = 'oc.parent_id != oc.id AND oc.parent_id = :ocid';
	$sql['id'] = 'oc_id';

	$arguments = array(
			':ocid' => $classificationid
	);

	$output = datatable_dbRequest($sql, $arguments);
	return drupal_json_output($output);
}

/**
 * Menu callback. This will be called for visits to /organism/classification/%/organismjson.
 * @return array
 * @todo Find a better title
 */
function organism_show_classification_organismjson($classificationid) {
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'osc.id AS osc_id',
			'osc.name AS osc_name',
			'o.id AS organism_id'
	);

	$sql['dbSearchColumns'] = array(
			'osc.id',
			'osc.name',
			'o.id'
	);

	/* DB table to use */
	$sql['dbTable'] = '{organism_classification} oc';
	/* Joins to use */
	$sql['dbJoins'] = 'JOIN {organism_classification_subscription} ocs ON ocs.organism_classification_id = oc.id
				JOIN {organism} o ON ocs.organism_id = o.id
				JOIN {organism_scientific_name} osc ON osc.organism_id = o.id';
	/* Group */
	$sql['dbWhere'] = 'oc.id = :ocid';
	$sql['id'] = 'osc_id';

	$arguments = array(
			':ocid' => $classificationid
	);

	$output = datatable_dbRequest($sql, $arguments);
	return drupal_json_output($output);
}
// 	// If this classification has organisms attached, then show those organisms
// 	{
// 		$resultOrganism = db_query(
// 			'SELECT
// 			osc.id AS name_id,
// 			osc.name AS name,
// 			o.id AS organism_id
// 		FROM
// 			{organism_classification} oc
// 			JOIN {organism_classification_subscription} ocs ON ocs.organism_classification_id = oc.id
// 			JOIN {organism} o ON ocs.organism_id = o.id
// 			JOIN {organism_scientific_name} osc ON osc.organism_id = o.id
// 		WHERE
// 			oc.id = :ocid',
// 		array(':ocid' => $classificationid));

// 		if ($resultOrganism->rowCount() > 0) {
// 			$header = array(
// 			t('Organism'),
// 					'organism id'
// 			);

// 			foreach ($resultOrganism as $record) {
// 				$row = array();
// 				$row[] = $record->name;
// 				$row[] = $record->organism_id;
// 				$rows[] = array(
// 						'data' => $row,
// 						'id' => 'organism_' . $record->organism_id
// 				);
// 			}
// 			$render_array['organismsClassificationViewOrganisms'] = array(
// 					'#theme' => 'datatable',
// 					'#header' => $header,
// 					'#rows' => $rows,
// 					'#id_table' => 'organisms',
// 			);
// 		}
// 	}
// 	return $render_array;
?>
