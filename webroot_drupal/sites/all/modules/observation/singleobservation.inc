<?php
/**
 * @file singleobservation.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'observation', 'observation.template');

/**
 * Return the single observation form to add a observation
 */
function singleobservation_new(){
// 	return drupal_get_form('observation_new_form');
	return observation_form();
}

/**
 * Return a list with all singleobservations which the user has read access
 */
function singleobservation_show_observation($observationdata = NULL){
	return observation_show_observation($observationdata);
}
