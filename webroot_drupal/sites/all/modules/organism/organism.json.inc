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
 *              "levelcount":"1",
 *              "cnt":"666"
 *          }
 *      },
 *      {
 *          "id":null,
 *          "cell":{
 *              "oc_id":"7382",
 *              "oc_name":"CSCF",
 *              "levelcount":"5",
 *              "cnt":"1337"
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
			'COUNT(ocl.id) - 1 AS levelcount',
			'organismcount.cnt'
	);

	$sql['dbSearchColumns'] = array(
			'oc.id',
			'oc.name'
	);

	/* Original query:
		SELECT
			oc.id AS oc_id,
			oc.name AS oc_name,
			COUNT(ocl.id) - 1 AS levelcount,
			organismcount.cnt
		FROM
			drupal_organism_classification oc 
			JOIN drupal_organism_classification_level ocl ON oc.organism_classification_level_id = ocl.prime_father_id
			JOIN (
				SELECT
					oc.prime_father_id AS prime_father_id,
					COUNT(ocs.id) AS cnt
				FROM
					drupal_organism_classification_subscription ocs
					JOIN drupal_organism_classification oc ON oc.id = ocs.organism_classification_id 
				GROUP BY
					prime_father_id
			)
			AS organismcount ON organismcount.prime_father_id = oc.prime_father_id
		WHERE
			oc.prime_father_id = oc.id
		GROUP BY
			oc_id,
			oc_name,
			organismcount.cnt;
	 */
	/* DB table to use */
	$sql['dbTable'] = "{organism_classification} oc";
	/* Joins to use */
	$sql['dbJoins'] = "JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.prime_father_id
						JOIN (
							SELECT
								oc.prime_father_id AS prime_father_id,
								COUNT(ocs.id) AS cnt
							FROM
								drupal_organism_classification_subscription ocs
								JOIN drupal_organism_classification oc ON oc.id = ocs.organism_classification_id 
							GROUP BY
								prime_father_id
						) AS organismcount ON organismcount.prime_father_id = oc.prime_father_id";
	/* Group */
	$sql['dbGroupBy'] = "oc_id, oc_name, organismcount.cnt";
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
	$sql['dbGroupBy'] = 'oc_id, oc_name, classification_level_id, levelname';
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
			'osn.id AS osn_id',
			'osn.name AS osn_name',
			'o.id AS o_id'
	);

	$sql['dbSearchColumns'] = array(
			'osn.id',
			'osn.name',
			'o.id'
	);

	/* DB table to use */
	$sql['dbTable'] = '{organism_classification} oc';
	/* Joins to use */
	$sql['dbJoins'] = 'JOIN {organism_classification_subscription} ocs ON ocs.organism_classification_id = oc.id
						JOIN {organism} o ON o.id = ocs.organism_id 
						JOIN {organism_scientific_name} osn ON osn.organism_id = o.id';
	/* Group */
	$sql['dbWhere'] = 'oc.prime_father_id = (
								SELECT
									innerOc.prime_father_id
								FROM
									{organism_classification} innerOc
								WHERE
									innerOc.id = :ocid
								)
						AND oc.left_value >= (
								SELECT
									innerOc.left_value
								FROM
									{organism_classification} innerOc
								WHERE
									innerOc.id = :ocid
								)
						AND	oc.right_value <= (
								SELECT
									innerOc.right_value
								FROM
									{organism_classification} innerOc
								WHERE
									innerOc.id = :ocid)';
	$sql['id'] = 'o_id';
	$sql['dbGroupBy'] = ' osn_name, osn_id, o_id';
	
	$arguments = array(
			':ocid' => $classificationid
	);

	$output = datatable_dbRequest($sql, $arguments);
	return drupal_json_output($output);
}

/**
 * Menu callback. This will be called for visits to /organism/%/organismjson.
 * @return array
 */
function organism_show_organism_json($organismid) {
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'osn.id AS osn_id',
			'osn.name AS osn_name',
			'osn.organism_id AS o_id',
	);

	$sql['dbSearchColumns'] = array(
			'osn.id',
			'osn.name',
			'osn.organism_id'
	);

	/* DB table to use */
	$sql['dbTable'] = '{organism_scientific_name} osn';
	/* Group */
	$sql['dbWhere'] = 'osn.organism_id = :oid';
	$sql['id'] = 'osn_id';

	$arguments = array(
			':oid' => $organismid
	);

	$output = datatable_dbRequest($sql, $arguments);
	return drupal_json_output($output);
}
?>
