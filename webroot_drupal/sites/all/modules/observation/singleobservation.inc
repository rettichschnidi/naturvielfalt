<?php
/**
 * @file singleobservation.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'observation', 'observation.template');

/**
 *
 */
function singleobservation_wizard(){
	//return drupal_get_form('observation_new_form');
	return observation_new_form(base_path().'observation/save');
}
