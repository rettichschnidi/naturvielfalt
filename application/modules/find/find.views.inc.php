<?php

module_load_include('inc.php', 'find', 'find');

use Naturwerk\Find\Organisms;
use Naturwerk\Find\Sightings;
use Naturwerk\Find\Inventories;
use Naturwerk\Find\Parameters;
use Naturwerk\Find\Finder;

function find_query_param($key, $default = array()) {
    return isset($_GET[$key]) ? $_GET[$key] : $default;
}

function find_show_search() {
    return drupal_goto('find/organisms');
}

function find_search($key) {

    drupal_add_css(drupal_get_path('module', 'find') . '/css/search.css', array('group' => CSS_DEFAULT, 'every_page' => true));
    drupal_add_js('http://maps.google.com/maps/api/js?sensor=false', array('group' => JS_LIBRARY));
    drupal_add_js(drupal_get_path('module', 'find') . '/js/find.js');

    $variables = array('#theme' => 'find.search');

    $search = find_query_param('search', '');
    $geo = find_query_param('geo');
    $date = find_query_param('date');
    $class = find_query_param('class');
    $user = find_query_param('user');
    $family = find_query_param('family');
    $genus = find_query_param('genus');

    $sort = find_query_param('sort');

    $client = new Elastica_Client();
    $index = $client->getIndex('naturwerk');

    $parameters = new Parameters($search, $geo, $date, $class, $user, $family, $genus, $sort);

    try {
        $organisms = new Organisms($index, $parameters);
        $variables['#organisms'] = $organisms->search();

        $sightings = new Sightings($index, $parameters);
        $variables['#sightings'] = $sightings->search();

        $inventories = new Inventories($index, $parameters);
        $variables['#inventories'] = $inventories->search();

    } catch (Elastica_Exception_Response $e) {
        trigger_error($e->getMessage(), E_USER_WARNING);
        $variables['#organisms'] = new Elastica_ResultSet(new Elastica_Response(''));
        $variables['#sightings'] = new Elastica_ResultSet(new Elastica_Response(''));
        $variables['#inventories'] = new Elastica_ResultSet(new Elastica_Response(''));
    }

    // calculate bounding box for static map
    if (count($geo) == 2) {

        list($a, $b) = explode(',', $geo[0]);
        list($c, $d) = explode(',', $geo[1]);

        $variables['#box'] = array($a . ',' . $b, $a . ',' . $d, $c . ',' . $d, $c . ',' . $b, $a . ',' . $b);
    }

    $parameters = drupal_get_query_parameters();
    if (count($parameters) == 0) {
        $parameters = new stdClass(); // force empty JSON object
    }
    drupal_add_js(array('find' => array('url' => url($_GET['q']), 'parameters' => $parameters, 'geo' => $geo)), 'setting');

    $variables['#search'] = $search;
    $variables['#geo'] = $geo;
    $variables['#date'] = $date;
    $variables['#class'] = $class;
    $variables['#user'] = $user;
    $variables['#family'] = $family;
    $variables['#genus'] = $genus;

    $variables['#sort'] = $sort;

    $variables['#result'] = $variables['#' . $key];

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
