<?php

/**
 * Implements hook_drush_command().
 */
function find_drush_command() {
    $items['find-reindex'] = array(
        'description' => 'Reindex Naturwerk objects.',
        'callback' => 'find_drush_reindex',
        'drupal dependencies' => array('find'),
    );
    return $items;
}

function find_drush_reindex() {
    module_load_include('inc.php', 'find', 'find');

    try {
        $client = new Elastica_Client();
        $index = $client->getIndex('naturwerk');

        $indexer = new Naturwerk\Find\Indexer($index);
        $indexer->all();

    } catch (Exception $e) {
        var_dump($e->getMessage());
        var_dump($e->getTraceAsString());
    }
}
