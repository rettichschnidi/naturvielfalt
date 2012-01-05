<?php

/**
 * Implements hook_drush_command().
 */
function area_drush_command() {
    $items['area-migrate-postgis'] = array(
        'description' => 'Migrate area_point into PostGIS column geom.',
        'callback' => 'area_drush_migrate_postgis',
        'drupal dependencies' => array('area'),
    );
    return $items;
}

function area_drush_migrate_postgis() {

    // add geom column and remove old centroid
    try {

        db_query('SELECT AddGeometryColumn(\'area\', \'geom\', 4326, \'GEOMETRY\', 2);');
        db_query('ALTER TABLE area DROP COLUMN centroid_lat;');
        db_query('ALTER TABLE area DROP COLUMN centroid_lng;');

    } catch (PDOException $e) {

        // maybe column was already added?
        echo $e->getMessage();
        echo "\n";
    }

    $points = db_query('SELECT * FROM area_point ORDER BY seq ASC');

    $areas = array();
    foreach ($points as $point) {

        if (!isset($areas[$point->area_id])) {
            $areas[$point->area_id] = array();
        }
        $areas[$point->area_id][] = $point->lng . ' ' . $point->lat;
    }

    foreach ($areas as $id => $points) {

        if (count($points) > 1) {
            // polygon

            // make sure line is closed
            if (end($points) != reset($points)) {
                $points[] = reset($points);
            }

            $line = implode(', ', $points);
            $geom = 'ST_GeomFromText(\'POLYGON((' . $line . '))\', 4326)';

        } else {
            // point
            $geom = 'ST_GeomFromText(\'POINT(' . reset($points) . ')\', 4326)';
        }

        db_update('area')->expression('geom', $geom)->condition('id', $id)->execute();
    }
}
