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
	drupal_set_title(t('Organisms - Grouped by Classifier'));
	drupal_add_js(
		drupal_get_path('module', 'organism')
				. '/js/organism.datatable.select.js');

	$header = array(
			t('Classifiers of Organisms'),
			'id'
	);

	$result = db_query(
		'SELECT id, name FROM {organism_classification} WHERE prime_father_id=id');

	foreach ($result as $record) {
		$row = array();
		$row[] = $record->name;
		$row[] = $record->id;
		$rows[] = array(
				'data' => $row,
				'id' => 'classification_' . $record->id
		);
	}

	$render_array['organisms'] = array(
			'#theme' => 'datatable',
			'#header' => $header,
			'#rows' => $rows,
			'#id_table' => 'organisms',
	);
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
			oc.id,
			oc.name,
			oc.organism_classification_level_id
		FROM
			{organism_classification} oc
		WHERE
			oc.parent_id = :ocid',
		array(':ocid' => $classificationid));

	$render_array = array();

	if ($resultClassification->rowCount() > 0) {
		$header = array(
				t('Classification of Organisms'),
				'id',
				'level'
		);

		$rows = array();
		foreach ($resultClassification as $record) {
			$row = array();
			$row[] = $record->name;
			$row[] = $record->id;
			$row[] = $record->organism_classification_level_id;
			$rows[] = array(
					'data' => $row,
					'id' => 'classification_' . $record->id
			);
		}

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

		$rows = array();
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
		'SELECT osc.id, osc.name FROM {organism_scientific_name} osc WHERE osc.id = :oscid',
		array(':oscid' => $organismid));

	foreach ($result as $record) {
		$row = array();
		$row[] = $record->name;
		$row[] = $record->id;
		$rows[] = array(
				'data' => $row,
				'id' => 'organism_' . $record->id
		);
	}

	$render_array['organisms'] = array(
			'#theme' => 'datatable',
			'#header' => $header,
			'#rows' => $rows,
			'#id_table' => 'organisms',
	);
	return $render_array;
}
?>
