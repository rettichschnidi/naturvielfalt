<?php

/**
 * Implements hook_drush_command().
 */
function organism_drush_command() {
    $items['organism-import-redlist'] = array(
        'description' => t('Import red list.'),
        'callback' => 'organism_drush_import_redlist',
	    'arguments' => array(
	    	'file' => 'CSV file to import.',
	    ),
    );
    return $items;
}

function organism_drush_import_redlist($file) {

    if (($handle = fopen($file, 'r')) !== false) {

        if (($data = fgetcsv($handle, 1000, ',')) !== false) {

            // read header
            $header = $data;

            while (($raw = fgetcsv($handle, 1000, ',')) !== false) {

                // create associative array
                $data = array_combine($header, $raw);

                // redlist not needed in WHERE clause
                $redlist = $data['redlist'];
                unset($data['redlist']);

                // build WHERE from data
                $where = array();
                $args = array(':redlist' => $redlist);
                foreach ($data as $column => $value) {
                    $where[] = '"' . $column . '" = :' . $column;
                    $args[':' . $column] = $value;
                }

                // update redlist column
                $statement = db_query('UPDATE fauna_organism SET redlist = :redlist WHERE ' . implode(' AND ', $where), $args);

                // log if not imported
                if ($statement->rowCount() == 0) {
                    echo 'IMPORT FAILED FOR: ' . implode(', ', $raw) . "\n";
                }
            }
        }

        fclose($handle);
    }
}
