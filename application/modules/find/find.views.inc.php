<?php

module_load_include('inc.php', 'find', 'find');

use Naturwerk\Find\Organisms;
use Naturwerk\Find\Sightings;
use Naturwerk\Find\Inventories;
use Naturwerk\Find\Areas;
use Naturwerk\Find\Parameters;
use Naturwerk\Find\Finder;

function find_query_param($key, $default = array()) {
    return isset($_GET[$key]) ? $_GET[$key] : $default;
}

function find_show_search() {
    return drupal_goto('find/sightings');
}

function find_search($key) {
    
    drupal_add_css(drupal_get_path('module', 'find') . '/css/find_search.css', array('group' => CSS_DEFAULT, 'every_page' => true));
    drupal_add_css(drupal_get_path('module', 'find') . '/css/find_print.css', array('group' => CSS_DEFAULT, 'every_page' => true, 'media' => 'print'));
    drupal_add_js('http://maps.google.com/maps/api/js?sensor=false&libraries=geometry', array('group' => JS_LIBRARY));
    drupal_add_library('system', 'ui.sortable');
    drupal_add_js(drupal_get_path('module', 'area') . '/css/overlay-style.js');
    drupal_add_js(drupal_get_path('module', 'area') . '/js/geometry.js');
    drupal_add_js(drupal_get_path('module', 'find') . '/js/find.js');

    $variables = array('#theme' => 'find.search');

    $uid = $GLOBALS['user']->uid;
    $search = find_query_param('search', '');
    $geo = find_query_param('geo');
    $date = find_query_param('date');
    $class = find_query_param('class');
    $user = find_query_param('user');
    $family = find_query_param('family');
    $genus = find_query_param('genus');
    $town = find_query_param('town');
    
    $columns = find_query_param('columns', array());
    $sort = find_query_param('sort');

    $client = new Elastica_Client();
    $index = $client->getIndex('naturwerk');

    $parameters = new Parameters($uid, $search, $geo, $date, $class, $user, $family, $genus, $town, $sort, $columns);

    $variables['#organisms'] = new Organisms($index, $parameters);
    $variables['#sightings'] = new Sightings($index, $parameters);
    $variables['#inventories'] = new Inventories($index, $parameters);
    $variables['#areas'] = new Areas($index, $parameters);

    $parameters = drupal_get_query_parameters();
    if (count($parameters) == 0) {
        $parameters = new stdClass(); // force empty JSON object
    }
    drupal_add_js(array('find' => array('url' => url($_GET['q']), 'parameters' => $parameters, 'geo' => $geo)), 'setting');

    $variables['#key'] = $key;
    $variables['#current'] = $variables['#' . $key];

    $output = array();
    $output['search'] = $variables;

    return $output;
}

function find_show_search_organisms() {
    return find_search('organisms');
}

function find_show_search_sightings() {
    return find_search('sightings');
}

function find_show_search_inventories() {
    return find_search('inventories');
}

function find_show_search_areas() {
    return find_search('areas');
}

function find_export_csv($key) {

    $output = find_search($key);

    header('Content-Type: text/csv');
    header('Content-Disposition: attachment; filename="naturwerk-' . $output['search']['#key'] . '.csv"');

    $out = fopen('php://output', 'w');
    $writer = new Csv_Writer($out, new Csv_Dialect_Excel());

    $current = $output['search']['#current'];
    $result = $current->search();

    $data = array();
    foreach ($current->getActiveColumns() as $column) {
        $data[] = $column->getTitle();
    }
    $writer->writeRow($data);

    foreach ($result as $row) {

        // convert to ISO-8859-1 for Excel
        $data = array();
        foreach ($current->getActiveColumns() as $column) {

            $data[] = utf8_decode($row->__get($column->getName()));
        }

        $writer->writeRow($data);
    }
}

function find_export_csv_organisms() {
    find_export_csv('organisms');
}

function find_export_csv_sightings() {
    find_export_csv('sightings');
}

function find_export_csv_inventories() {
    find_export_csv('inventories');
}

function find_export_csv_areas() {
    find_export_csv('areas');
}
