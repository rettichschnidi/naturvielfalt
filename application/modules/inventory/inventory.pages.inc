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

	//return $output;

	$options = array();
	 
	$result = db_query("SELECT it.id, it.name
		FROM inventory_type it
		ORDER BY it.id desc;");

	foreach ($result as $record) {
		$options[$record->id] = $record->name;
	}

	$form['inventory_types']['name'] = array(
    '#type' => 'select',
    '#title' => t('Inventargruppe'),
    '#default_value' => variable_get('name', 259200),
    '#options' => $options,
	);
	$form['add_inventory'] = array(
    '#type' => 'submit',
    '#value' => t('Artgruppe hinzufuegen'),
	);
	$form['save_inventory'] = array(
    '#type' => 'submit',
    '#value' => t('speichern'),
	);

	return $form;

	/*
	 error_log("start zend");

	 set_include_path('.' . PATH_SEPARATOR . '/library' . PATH_SEPARATOR . get_include_path());

	 require_once 'library/Zend/Loader/Autoloader.php';
	 $loader = Zend_Loader_Autoloader::getInstance();
	 */
}

function inventory_add_inventory($form_id, $form) {
  drupal_set_message(t('inventory add'));
}

function inventory_save_inventory($form_id, $form) {
  drupal_set_message(t('inventory save'));
}

?>