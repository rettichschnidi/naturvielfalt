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

	$output = datatable_dbRequest($sql);
	return drupal_json_output($output);
}


/**
* Menu callback. This will be called for visits to /organism/classification/%/json.
* @return array
* @todo Find a better title
*/
function organism_show_classification_sjon($classificationid) {
	drupal_set_title(t('Classifications'));
	// Extract the name of the current classification
	{
		$resultClassification = db_query(
			'SELECT
				oc.id AS id,
				oc.name AS name
			FROM
				{organism_classification} oc
			WHERE
				oc.id = :ocid',
		array(':ocid' => $classificationid));

		$render_array = array();

		if ($resultClassification->rowCount() == 1) {
			$resultClassificationObject = $resultClassification->fetchObject();
			drupal_set_title(
			t('Classification') . ' ' . $resultClassificationObject->name);
		}
	}

	// Show all the subclassifications of the current classification
	{
		$resultClassification = db_query(
			'SELECT
			oc.id AS id,
			oc.name AS name,
			oc.organism_classification_level_id AS classification_level_id,
			ocl.name AS levelname
		FROM
			{organism_classification} oc
			JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.id
		WHERE
			oc.parent_id != oc.id AND oc.parent_id = :ocid',
		array(':ocid' => $classificationid));

		$render_array = array();

		// only render subclassification table if there are any subclassifications
		if ($resultClassification->rowCount() > 0) {
			$levelname = '';
			foreach ($resultClassification as $record) {
				$row = array();
				$row[] = $record->name;
				$row[] = $record->id;
				$row[] = $record->classification_level_id;
				$rows[] = array(
						'data' => $row,
						'id' => 'classification_' . $record->id
				);
				$levelname = $record->levelname;
			}

			$header = array(
			t($levelname),
					'id',
					'level'
			);

			$render_array['organismsClassificationViewClassifications'] = array(
					'#theme' => 'datatable',
					'#header' => $header,
					'#rows' => $rows,
					'#id_table' => 'organisms',
			);
		}
	}

	// If this classification has organisms attached, then show those organisms
	{
		$resultOrganism = db_query(
			'SELECT
			osc.id AS name_id,
			osc.name AS name,
			o.id AS organism_id
		FROM
			{organism_classification} oc
			JOIN {organism_classification_subscription} ocs ON ocs.organism_classification_id = oc.id
			JOIN {organism} o ON ocs.organism_id = o.id
			JOIN {organism_scientific_name} osc ON osc.organism_id = o.id
		WHERE
			oc.id = :ocid',
		array(':ocid' => $classificationid));

		if ($resultOrganism->rowCount() > 0) {
			$header = array(
			t('Organism'),
					'organism id'
			);

			foreach ($resultOrganism as $record) {
				$row = array();
				$row[] = $record->name;
				$row[] = $record->organism_id;
				$rows[] = array(
						'data' => $row,
						'id' => 'organism_' . $record->organism_id
				);
			}
			$render_array['organismsClassificationViewOrganisms'] = array(
					'#theme' => 'datatable',
					'#header' => $header,
					'#rows' => $rows,
					'#id_table' => 'organisms',
			);
		}
	}
	return $render_array;
}
?>
