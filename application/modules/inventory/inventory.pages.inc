<?php
/**
 * Menu callback. This will be called if someone visits for example drupal/inventory/3.
 */
function inventory_form() {
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-1.4.2.min.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-ui-1.8.6.custom.min.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/jquery/jquery-texotela-numeric.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/ext/ext-jquery-adapter-debug.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/lib/ext/ext-base-debug.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/JSON.js');

	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/util.js');
	drupal_add_js(drupal_get_path('module', 'inventory') .'/js/inventory.js');

	// inventories ajax form
	$form['inventories']['#markup'] = '<div id="inventories"></div><input onclick="initInventory();" type="button" id="edit-start-inventory" name="button" value="start inventory">';

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
	'#submit'=>array('add_inventory'),
	);
	$form['save_inventory'] = array(
    '#type' => 'submit',
    '#value' => t('speichern'),
	'#submit'=>array('save_inventory'),
	);

	return $form;

	/*
	 error_log("start zend");

	 set_include_path('.' . PATH_SEPARATOR . '/library' . PATH_SEPARATOR . get_include_path());

	 require_once 'library/Zend/Loader/Autoloader.php';
	 $loader = Zend_Loader_Autoloader::getInstance();
	 */
}

function inventory_show($inventory_id) {
	drupal_set_title(t('Arteninventar für Inventar '.$inventory_id));
	return drupal_get_form('inventory_form');
}

function save_inventory($form_id, $form) {
	error_log("save inventoory");
}

function add_inventory($form, &$form_state) {
	error_log("submit with page");
}

?>