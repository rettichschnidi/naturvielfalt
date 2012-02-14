<?php
/**
 * @file organism.overview.inc
 * @author Reto Schneider, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

/**
 * Menu callback. This will be called for visits to /organism.
 * @return array
 */
function organism_classifier_view() {
	drupal_set_title(t('Organisms - Grouped by Classifier'));
	drupal_add_js(drupal_get_path('module', 'organism') . '/js/organism.datatable.select.js');

	$header = array(
			t('Classifiers of Organisms'),
			'id'
	);

	$result = db_query('SELECT id, name FROM {organism_classifier}');

	foreach ($result as $record) {
		$row = array();
		$row[] = $record -> name;
		$row[] = $record -> id;
		$rows[] = array(
				'data' => $row,
				'id' => 'classifier_' . $record -> id
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
 * Menu callback. This will be called for visits to /organism/classifier/%.
 * @return array
 */
function organism_classification_top_view($classifierid) {
	drupal_set_title(t('Organisms - Grouped by class'));
	drupal_add_js(drupal_get_path('module', 'organism') . '/js/organism.datatable.select.js');

	$header = array(
			t('Classification of Organisms'),
			'id'
	);

	$result = db_query('SELECT oc.id, oc.name FROM {organism_classification_level} ocl JOIN {organism_classification} oc ON oc.organism_classification_level_id = ocl.id ' . 'WHERE ocl.organism_classifier_id = :ocid', array(':ocid' => $classifierid));

	foreach ($result as $record) {
		$row = array();
		$row[] = $record -> name;
		$row[] = $record -> id;
		$rows[] = array(
				'data' => $row,
				'id' => 'classifierclassification_' . $classifierid . '_' . $record -> id
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
 * Menu callback. This will be called for visits to /organism/classifier/%.
 * @return array
 */
function organism_classification_view($classifierid, $classificationid) {
	print "1: " . var_export($classifierid, TRUE) . "\n";
	print "2: " . var_export($classificationid, TRUE) . "\n";
	exit();
	drupal_set_title(t('Organisms - Grouped by class'));
	drupal_add_js(drupal_get_path('module', 'organism') . '/js/organism.datatable.select.js');

	$header = array(
			t('Classification of Organisms'),
			'id'
	);

	$result = db_query('SELECT oc.id, oc.name FROM {organism_classification_level} ocl JOIN {organism_classification} oc ON oc.organism_classification_level_id = ocl.id ' . 'WHERE ocl.organism_classifier_id = :ocid', array(':ocid' => $classifierid));

	foreach ($result as $record) {
		$row = array();
		$row[] = $record -> name;
		$row[] = $record -> id;
		$rows[] = array(
				'data' => $row,
				'id' => 'classification_' . $record -> id
		);
	}

	$render_array['organisms'] = array(
			'#theme' => 'datatable',
			'#header' => $header,
			'#rows' => $rows,
			'#id_table' => 'organisms1',
	);
	return $render_array;
}
?>