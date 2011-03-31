<?php
/**
 * Menu callback. This will be called if someone visits drupal/q=swissmon.
 */
function inventory_page($inventory_id) {
	drupal_set_title(t('Arteninventar für Inventar '.$inventory_id));

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-1.4.2.min.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-ui-1.8.6.custom.min.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-texotela-numeric.js');
	
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/ext/ext-jquery-adapter-debug.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/ext/ext-base-debug.js');
	
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/JSON.js');
		
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/util.js');	
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/inventory.js');
	
	$tmp = drupal_add_js($data = 'initInventory()', $type = 'inline');

  	$output = '<div id="inventories"></div>';
	
  	return $output;

	/*
	error_log("start zend");

	set_include_path('.' . PATH_SEPARATOR . '/library' . PATH_SEPARATOR . get_include_path());

	require_once 'library/Zend/Loader/Autoloader.php';
	$loader = Zend_Loader_Autoloader::getInstance();
	*/
}
?>