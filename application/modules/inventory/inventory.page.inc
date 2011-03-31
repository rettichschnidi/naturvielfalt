<?php
function inventory_page() {
	drupal_set_title(t('Inventory'));

	$render_array['inventories'] = array(
    '#theme' => 'inventory_page', 
   		'#items' => array(
			'id' => 1,
			'name' => 'yes'
	)
	);
	return $render_array;
}
?>