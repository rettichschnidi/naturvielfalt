<?php

module_load_include('inc.php', 'find', 'find');

function find_query_param($key, $default = array()) {
    return isset($_GET[$key]) ? $_GET[$key] : $default;
}

function find_search($key) {

    drupal_add_css(drupal_get_path('module', 'find') . '/css/search.css', array('group' => CSS_DEFAULT, 'every_page' => true));

    $variables = array();

    $search = find_query_param('search', '');
    $class = find_query_param('class');
    $family = find_query_param('family');
    $genus = find_query_param('genus');

    $client = new Elastica_Client();
    $index = $client->getIndex('naturwerk');

    $finder = new Naturwerk\Find\Finder($index);
    $variables['organisms'] = $finder->organisms($search, $class, $family, $genus);
    $variables['sightings'] = $finder->sightings($search, $class, $family, $genus);
    $variables['inventories'] = $finder->inventories($search, $class, $family, $genus);

    $variables['class'] = $class;
    $variables['family'] = $family;
    $variables['genus'] = $genus;

    $variables['result'] = $variables[$key];

    return theme('find.search', $variables);
}

function find_show_search() {
    return drupal_goto('find/organisms');
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
