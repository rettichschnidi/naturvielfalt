<?php

/**
 * Menu callback; View of open identification
 * 
 * @param int $id
 *  The ID of the inventory
 * @return array
 *  Drupal render array
 */
function open_identification_show($mode) {

	drupal_set_title(t('Open Identification '));
	$list = open_identification_get_all_entries($userid=false);
	$render_array['open_identification_list'] = array(
	    '#theme' => 'open_identification_list',  // The theme function to apply to the #items
	    '#list' => $list, // The list itself.
	);
	return $render_array;
}

?>