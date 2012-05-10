<?php

module_load_include('inc.php', 'find', 'find');

use Naturwerk\Find\Organisms;
use Naturwerk\Find\Sightings;
use Naturwerk\Find\Inventories;
use Naturwerk\Find\Areas;
use Naturwerk\Find\Images;
use Naturwerk\Find\Parameters;
use Naturwerk\Find\Finder;

/**
 * Extract $_GET values for passed $fields.
 *
 * @param array $fields
 */
function find_query_params($fields) {

    $params = array();
    foreach ($fields as $key => $default) {
        $params[$key] = isset($_GET[$key]) ? $_GET[$key] : $default;
    }

    return $params;
}

/**
 * Redirect to sightings.
 */
function find_show_search() {
    return drupal_goto('find/sightings');
}

/**
 * Main search controller.
 * 
 * @param string $key
 */
function find_search($key) {

    drupal_add_css(drupal_get_path('module', 'find') . '/css/find_search.css', array('group' => CSS_DEFAULT, 'every_page' => true));
    drupal_add_css(drupal_get_path('module', 'find') . '/css/find_print.css', array('group' => CSS_DEFAULT, 'every_page' => true, 'media' => 'print'));
    drupal_add_js('http://maps.google.com/maps/api/js?sensor=false&libraries=geometry,places', array('group' => JS_LIBRARY));
    drupal_add_library('system', 'ui.sortable');
    drupal_add_js(drupal_get_path('module', 'area') . '/css/overlay-style.js');
    drupal_add_js(drupal_get_path('module', 'area') . '/js/geometry.js');
    drupal_add_js(drupal_get_path('module', 'find') . '/js/find.js');

    drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js', array('weight' => 100));
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js');

    $variables = array('#theme' => 'find.search');

    // fetch $_GET parameters
    $params = find_query_params(array(
    	'search' => '',
        'geo' => array(),
    	'date' => array(),
    	'class' => array(),
    	'user' => array(),
    	'family' => array(),
    	'genus' => array(),
    	'town' => array(),
    	'canton' => array(),
    	'redlist' => array(),
    	'habitat' => array(),
    	'image_type' => array(),
    	'columns' => array(),
    	'sort' => array(),
    ));
    $params['uid'] = $GLOBALS['user']->uid;

    try {
        $client = new Elastica_Client();
        $status = $client->getStatus(); // ping to make sure elasticsearch is running
        $index = $client->getIndex('naturwerk');

        $parameters = new Parameters($params);
        $variables['#organisms'] = new Organisms($index, $parameters);
        $variables['#sightings'] = new Sightings($index, $parameters);
        $variables['#inventories'] = new Inventories($index, $parameters);
        $variables['#areas'] = new Areas($index, $parameters);
        $variables['#images'] = new Images($index, $parameters);

        // make parameters available to JavaScript
        $parameters = drupal_get_query_parameters();
        if (count($parameters) == 0) {
            $parameters = new stdClass(); // force empty JSON object
        }
        drupal_add_js(array('find' => array('url' => url($_GET['q']), 'parameters' => $parameters, 'geo' => $params['geo'])), 'setting');

        $variables['#key'] = $key;
        $variables['#current'] = $variables['#' . $key];

        $output = array();
        $output['search'] = $variables;

        return $output;

    } catch (Elastica_Exception_Client $e) {

        watchdog('find', $e->getMessage(), array(), WATCHDOG_ERROR);

        drupal_set_message(t('Search error. Elasticsearch running?'), 'error');

        return array();
    }
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

function find_show_search_images() {
    return find_search('images');
}

/**
 * Export controller, generates CSV.
 * 
 * @param string $key
 */
function find_export_csv($key) {

    $output = find_search($key);

    header('Content-Type: text/csv');
    header('Content-Disposition: attachment; filename="naturwerk-' . $output['search']['#key'] . '.csv"');

    $out = fopen('php://output', 'w');
    $writer = new Csv_Writer($out, new Csv_Dialect_Excel());

    $current = $output['search']['#current'];
    $result = $current->search();

    // build header row
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
