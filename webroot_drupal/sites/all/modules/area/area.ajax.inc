<?php

/**
 * @file area.ajax.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area.create');

/**
 * Renders a form which allows the user to submit an area name.
 * For usage with ajax.
 */
function area_get_new_areaname_form_callback() {
	print drupal_render(drupal_get_form('area_new_areaname_form'));
}

function area_overview_ajax_callback() {
	print drupal_render(drupal_get_form('area_overview_form'));
}
?>
