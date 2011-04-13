<?php
function show_area_new_map() {
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css');
	drupal_set_title(t('Create a new area'));

	// add some text
	$output['info'] = array(
		'#markup' => t('Please create a new area by clicking on the map bellow. Then continue by pressing the following button: '),
	);
	
	// add button continue
	$output['continue'] = array(
		'#type' => 'button',
		'#value' => t('continue'),
		'#id' => 'continue',
	);
	
	// call our own theme for displaying the map
	$output['map'] = array(
		'#theme' => 'area.new.map',
	);
	return $output;
}
?>