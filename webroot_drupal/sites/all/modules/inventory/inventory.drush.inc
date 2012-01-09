<?php

/**
 * Implements hook_drush_command().
 */
function inventory_drush_command() {
	$items['inventory-migrate-postgis'] = array(
			'description' => t('Migrate location into PostGIS column geom.'),
			'callback' => 'inventory_drush_migrate_postgis',
			'drupal dependencies' => array(
					'area'
			),
	);
	return $items;
}

function inventory_drush_migrate_postgis() {

	// add geom column
	try {

		db_query('SELECT AddGeometryColumn(\'inventory_entry\', \'geom\', 4326, \'GEOMETRY\', 2);');

	} catch (PDOException $e) {

		// maybe column was already added?
		echo $e->getMessage();
		echo "\n";
	}

	$entries = db_query('SELECT * FROM inventory_entry WHERE location IS NOT NULL');

	foreach ($entries as $entry) {

		// first lng, then lat
		$location = trim($entry->location, '()');
		$location = explode(',', $location);

		$geom = 'ST_GeomFromText(\'POINT(' . $location[1] . ' ' . $location[0] . ')\', 4326)';

		db_update('inventory_entry')->expression('geom', $geom)->condition('id', $entry->id)->execute();
	}
}
