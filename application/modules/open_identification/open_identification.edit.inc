<?php

/**
 * Menu callback; View of open identification
 * 
 * @param int $id
 *  The ID of the inventory
 * @return array
 *  Drupal render array
 */
function open_identification_edit($id) {

	drupal_set_title(t('Open Identification '));
	$render_array['open_identification_details'] = array(
	    '#theme' => 'open_identification_details',  // The theme function to apply to the #items
	    '#text' => 'This is the edit page', // The list itself.
		);
	return $render_array;
}
?>