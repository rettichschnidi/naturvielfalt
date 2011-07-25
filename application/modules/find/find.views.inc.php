<?php

module_load_include('inc.php', 'find', 'find');

function find_show_search() {

    $variables = array();

    try {

        $search = $_GET['search'] ?: '';
        $class = $_GET['class'] ?: array();
        $family = $_GET['family'] ?: array();
        $genus = $_GET['genus'] ?: array();

        $client = new Elastica_Client();
        $index = $client->getIndex('naturwerk');

        $finder = new Naturwerk\Find\Finder($index);
        $result = $finder->search('organism', $search, $class, $family, $genus);

        $variables['result'] = $result;
        $variables['class'] = $class;
        $variables['family'] = $family;
        $variables['genus'] = $genus;

    } catch (Exception $e) {
        var_dump($e->getMessage());
        var_dump($e->getTraceAsString());
        exit;
    }

    return theme('find.search', $variables);
}
