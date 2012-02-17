<?php
/**
 * @file organism.overview.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

/**
 * Menu callback. This will be called for visits to /organism.
 * @return array
 */
function organism_show_all_classifiers() {
	drupal_set_title(t('Please select the classifier'));
	drupal_add_js(
		drupal_get_path('module', 'organism')
				. '/js/organism.datatable.select.js');

	$header = array(
			t('Classifiers'),
			'id',
			'number of classification levels'
	);

	$result = db_query(
		'SELECT
			oc.id AS oc_id,
			oc.name AS oc_name,
			COUNT(ocl.id) - 1 AS levelcount
		FROM
			{organism_classification} oc
			JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.prime_father_id
		WHERE
			oc.prime_father_id = oc.id
		GROUP BY
			oc_id,
			oc_name');

	$render_array = array();
	if ($result->rowCount() > 0) {
		foreach ($result as $record) {
			$row = array();
			$row[] = $record->oc_name;
			$row[] = $record->oc_id;
			$row[] = $record->levelcount;
			$rows[] = array(
					'data' => $row,
					'id' => 'classification_' . $record->oc_id
			);
		}
		$render_array['organisms'] = array(
				'#theme' => 'datatable',
				'#header' => $header,
				'#rows' => $rows,
				'#id_table' => 'organisms',
		);
	}
	return $render_array;
}

/**
 * Menu callback. This will be called for visits to /organism/classification/%.
 * @return array
 */
function organism_classification_view($classificationid) {
	drupal_set_title(t('Organisms - Grouped by class'));
	drupal_add_js(
		drupal_get_path('module', 'organism')
				. '/js/organism.datatable.select.js');

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
		drupal_set_title(t('Organisms - Grouped by' . ' ' . $levelname));

		$header = array(
				t('Classification of Organisms'),
				'id',
				'level'
		);
		$render_array['organisms'] = array(
				'#theme' => 'datatable',
				'#header' => $header,
				'#rows' => $rows,
				'#id_table' => 'organisms',
		);
	}

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
		$render_array['organisms'] = array(
				'#theme' => 'datatable',
				'#header' => $header,
				'#rows' => $rows,
				'#id_table' => 'organisms',
		);
	}

	return $render_array;
}

/**
 * Menu callback. This will be called for visits to /organism/%.
 * @return array
 */
function organism_show_organism($organismid) {
	drupal_set_title(t('Show specific organisms'));
	drupal_add_js(
		drupal_get_path('module', 'organism')
				. '/js/organism.datatable.select.js');

	$header = array(
			t("Synonyms"),
			'id'
	);

	$result = db_query(
		'SELECT osc.id, osc.name FROM {organism_scientific_name} osc WHERE osc.organism_id = :oscid LIMIT 1',
		array(':oscid' => $organismid));

	if ($record = $result->fetchObject()) {
		drupal_set_title(t('Show organism') . ' ' . $record->name);
	}
	$result = db_query(
		'SELECT osc.id, osc.name FROM {organism_scientific_name} osc WHERE osc.organism_id = :oscid',
		array(':oscid' => $organismid));

	if ($result->rowCount() > 0) {
		foreach ($result as $record) {
			$row = array();
			$row[] = $record->name;
			$row[] = $organismid;
			$rows[] = array(
					'data' => $row,
					'id' => 'organism_' . $organismid
			);
		}
		$render_array = array();
		$render_array['organisms'] = array(
				'#theme' => 'datatable',
				'#header' => $header,
				'#rows' => $rows,
				'#id_table' => 'organisms',
		);
	}
	return $render_array;
}
?>
