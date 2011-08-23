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
	if($mode == "gallery"){
		$render_array['open_identification_details'] = array(
	    '#theme' => 'open_identification_details',  // The theme function to apply to the #items
	    '#text' => 'This is the gallery page', // The list itself.
		);
	}
	else{
		$render_array['open_identification_gallery'] = array(
	    '#theme' => 'open_identification_gallery',  // The theme function to apply to the #items
	    '#text' => 'This is the list page', // The list itself.
		);
	}
	return $render_array;
}

?>