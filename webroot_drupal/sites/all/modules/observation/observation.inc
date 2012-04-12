<?php
/**
 * @file observation.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'observation', 'observation.template');

/**
 * Return the single observation form to add a observation
 */
function observation_edit($observation_id){
	return observation_form($observation_id);
}



