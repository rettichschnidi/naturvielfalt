<?php
/**
 * Menu callback. This will be called if someone visits drupal/showcase.
 */
function organism_overview() {
	drupal_set_title(t('Organisms - Grouped by Categories'));
	drupal_add_js(drupal_get_path('module', 'organism') . '/js/organism.datatable.select.js');

	$header = array(
			t('Organism Groups'),
			t('Organism count')
	);

	$result = db_query(
			'
		SELECT it.name, o.inventory_type_id, count (o.inventory_type_id) FROM organism o
			LEFT JOIN inventory_type it ON it.id = organism.inventory_type_id
		GROUP BY o.inventory_type_id, it.name
		ORDER BY it.name
	');

	foreach ($result as $record) {
		$row = array();
		$row[] = $record->name;
		$row[] = $record->count;
		$rows[] = array(
				'data' => $row,
				'id' => 'organismtype_' . $record->inventory_type_id
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