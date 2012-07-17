<?php
/**
 * 
 */


// Load all edit functions from inventory.
module_load_include('inc', 'inventory', 'inventory.edit');
// Load all show functions from inventory.
module_load_include('inc', 'inventory', 'inventory.show');

/**
 * Menu callback; List of protection lists
 * 
 * @return array
 *  Drupal render array
 */
function inventory_protection_list() {
	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');
	global $user;
	$render_array = array();

	$headings = array(
			t('Protection list name'),
			t('Species'),
			t('Created on'),
			t('Modified on'),
			t('Owner'),
			'%nbsp'
	);

	// Query protection lists
	$protections = db_select('inventory_custom_protection', 'p')->condition('p.public', '1')->fields('p');
	$protections->leftJoin('users', 'u', 'u.uid = p.owner_id');
	$protections->leftJoin('inventory_custom_protection_inventory', 'pi', 'pi.inventory_custom_protection_id = p.id');
	$protections
			->leftJoin('inventory_custom_protection_inventory_entry', 'pie',
					'pie.inventory_custom_protection_inventory_id = pi.id');
	$protections->addExpression('COUNT(pie.id)', 'organisms');
	$protections->addField('u', 'name', 'owner');
	$protections
			->groupBy(
					'p.id, p.name, p.description, p.public, p.owner_id, p.create_time, p.modify_time, p.name, p.enabled, u.name');
	$protections = $protections->execute();

	array_pop($headings);
	$headings[count($headings) - 1] = t('Owner');

	$rows = array();
	foreach ($protections as $record) {
		array_push($rows,
				array('id' => 'protection_' . $record->id,
						'data' => array($record->name,
								$record->organisms,
								date("j.m.Y", strtotime($record->create_time)),
								date("j.m.Y", strtotime($record->modify_time)),
								$record->owner
						)
				));
	}

	if ($rows) {
		$render_array['lists'] = array(
				'#type' => 'fieldset',
				'#title' => t('Protection lists'),
				'#weight' => 2,
				'#collapsible' => TRUE,
				'#collapsed' => FALSE,
				'#attached' => array(
						'js' => array(
								'misc/form.js',
								'misc/collapse.js',
						),
				),
				'#attributes' => array(
						'class' => array(
								'collapsible'
						), // add 'collapsed' to array, to close by default
				),
		);
		$render_array['lists']['protection'] = array(
				'#theme' => 'datatable',
				'#header' => $headings,
				'#rows' => $rows,
				'#id_table' => 'protection_lists',
		);
	} else {
		$render_array['lists']['protection'] = array(
				'#markup' => t(
						'No protection lists have been created.'
								. l(' Create a protection list', 'protection_lists/new'))
		);
	}

	return $render_array;
}

/**
 * Menu callback; View of an inventory template
 * 
 * @param int $plist_id
 * @return array
 *  Drupal render array
 */
function inventory_protection_show($plist_id) {
	$protection = _inventory_protection_load($plist_id);
	drupal_set_title($protection['name']);

	$rows = array();
	$rows[] = array(
			array(
					'data' => t('Owner'),
					'header' => true
			),
			l($protection['owner'], 'user/' . $protection['owner'] . '/contact')
	);
	$rows[] = array(
			array(
					'data' => t('Modified on'),
					'header' => true
			),
			date("j.m.Y", strtotime($protection['modify_time']))
	);
	$rows[] = array(
			array(
					'data' => t('Created on'),
					'header' => true
			),
			date("j.m.Y", strtotime($protection['create_time']))
	);
	$rows[] = array(
			array(
					'data' => t('Public'),
					'header' => true
			),
			$template['public'] ? t('Yes') : t('No')
	);
	$rows[] = array(
			array(
					'data' => t('Description'),
					'header' => true
			),
			$protection['description']
	);

	$render_array['details'] = array(
			'#type' => 'fieldset',
			'#title' => t('Protection list details'),
			'#weight' => 1,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
			),
	);
	$render_array['details']['table'] = array(
			'#theme' => 'table',
			'#rows' => $rows,
			'#attributes' => array(
					'class' => array(
							'inventory_show'
					),
			),
	);

	$render_array = array_merge($render_array,
			inventory_inventories($head_inventory_id, _inventory_protection_data($protection['id']), false));

	return $render_array;
}

/**
 * Form builder; Form for deleting a protection list.
 *
 * @param $plist_id
 *   The ID of the protection list
 *
 * @ingroup forms
 * @see inventory_protection_delete_form_submit()
 */
function inventory_protection_delete_form($form, &$form_state, $plist_id) {
	$protection = _inventory_protection_load($plist_id);
	$form_state['protection'] = $protection;

	return confirm_form($form, t('Delete %name?', array('%name' => $protection['name'])),
			'inventorytemplate/' . $plist_id,
			t('Are you sure you want to delete the template %name?', array('%name' => $protection['name'])),
			t('Delete'), t('Cancel'));
}

/**
 * Submit handler to delete a protection list.
 */
function inventory_protection_delete_form_submit($form, &$form_state) {
	$protection = $form_state['protection'];

	db_delete('inventory_custom_protection')->condition('id', $protection['id'])->execute();
	$result = db_select('inventory_custom_protection_inventory', 'i')
			->condition('i.inventory_custom_protection_id', $protection['id'])->fields('i', array('id'))->execute()
			->fetchAll();
	$inventories = array();
	foreach ($result as $record)
		array_push($inventories, $record->id);

	if ($inventories) {
		db_delete('inventory_custom_protection_inventory')->condition('id', $inventories, 'IN')->execute();
		db_delete('inventory_custom_protection_inventory_entry')
				->condition('inventory_custom_protection_inventory_id', $inventories, 'IN')->execute();
	}
	drupal_set_message(t('Protection list %name was deleted.', array('%name' => $protection['name'])));
	$form_state['redirect'] = 'protection_lists';
}

/**
 * Menu callback; Edit forms for editing a template's metadata and entries
 * 
 * @param int $template_id
 * @return array
 *  Drupal render array
 */
function inventory_protection_edit($plist_id) {
	$render_array = array();
	_include_js_and_css();
	$protection = _inventory_protection_load($plist_id);

	drupal_set_title(
			t('Modify %name (%id)', array('%name' => $protection['name'],
					'%id' => $protection['id']
			)), PASS_THROUGH);

	$render_array['basics'] = array(
			'#type' => 'fieldset',
			'#title' => t('Basics'),
			'#weight' => 2,
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible',
							'collapsed'
					),
			),
	);

	$render_array['basics']['edit'] = drupal_get_form('inventory_protection_edit_metadata', $protection);

	$render_array['entries'] = array(
			'#type' => 'fieldset',
			'#title' => t('Species'),
			'#weight' => 6,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
			),
	);

	$render_array['entries']['edit'] = drupal_get_form('inventory_protection_edit_entries', $protection);

	return $render_array;
}

/**
 * Menu callback; Form to edit the metadata of a protection list
 * 
 * @param array $protection[optional]
 * @return array
 *  Drupal form array
 */
function inventory_protection_edit_metadata($form, &$form_state, $protection = false) {
	if (!$protection)
		drupal_set_title(t('Create new protection list'));

	$form_state['protection'] = $protection;

	$form['name'] = array(
			'#type' => 'textfield',
			'#title' => t('Name'),
			'#size' => 100,
			'#maxlength' => 128,
			'#required' => TRUE,
			'#default_value' => $protection ? $protection['name'] : ''
	);

	$form['description'] = array(
			'#type' => 'textarea',
			'#title' => t('Description'),
			'#default_value' => $protection ? $protection['description'] : ''
	);

	$form['public'] = array(
			'#type' => 'checkbox',
			'#title' => t('Public'),
			'#description' => t('Allow other user to view this protection list'),
			'#default_value' => $protection ? $protection['public'] : '',
	);
	$form['enabled'] = array(
			'#type' => 'checkbox',
			'#title' => t('Enabled'),
			'#description' => t('Enable this protection list'),
			'#default_value' => $protection ? $protection['enabled'] : '',
	);

	// Show the Override or Submit button for this style.
	$form['submit'] = array(
			'#type' => 'submit',
			'#value' => $protection ? t('Save') : t('Create')
	);
	$form['close'] = array(
			'#markup' => l(t('Cancel'), 'protection_lists')
	);

	return $form;
}

/**
 * Submit handler for inventory_custom_protection_edit_metadata
 * @return 
 */
function inventory_protection_edit_metadata_submit($form, &$form_state) {
	$protection = $form_state['protection'];

	if (!$protection) {
		global $user;
		$protection = array(
				'owner_id' => $user->uid,
				'create_time' => 'now()'
		);
	}
	$protection['modify_time'] = 'now()';
	$protection['name'] = $form_state['values']['name'];
	$protection['description'] = $form_state['values']['description'];
	$protection['public'] = $form_state['values']['public'];
	$protection['enabled'] = $form_state['values']['enabled'];
	if (isset($protection['id']) && is_numeric($protection['id']))
		drupal_write_record('inventory_custom_protection', $protection, 'id');
	else
		drupal_write_record('inventory_custom_protection', $protection);
	drupal_set_message(t('Changes successfully saved'));
	$form_state['redirect'] = 'protection_lists/' . $protection['id'] . '/edit';
}

/**
 * Form to edit the entries of a protection list
 * 
 * @param array $template
 * @return array
 *  Drupal form array
 */
function inventory_protection_edit_entries($form, &$form_state, $protection) {
	global $user;
	drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
	drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
	drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js', array(
			'weight' => 100
	));

	$form_state['plist_id'] = $protection['id'];

	$form['inventories_start'] = array(
			'#markup' => '<div id="inventories">',
			'#weight' => 0
	);

	$data = _inventory_protection_data($protection['id']);
	$weight = 1;
	foreach ($data as $inventory_id => $inventory_group) {
		$form = array_merge(
				_inventory_group_factory($protection['id'], $inventory_group['invDesc'], $inventory_id,
						$inventory_group, $weight, false), $form);
		$weight += 2;
	}
	$form['inventories_end'] = array(
			'#markup' => '</div>',
			'#weight' => $weight++
	);

	if ($weight == 2)
		$form['empty'] = array(
				'#markup' => '<p class="empty">'
						. t(
								'There are no inventory groups available yet. Add a new one by using the select box below.</p>')
		);

	$form['inventory_protection_id'] = array(
			'#type' => 'hidden',
			'#value' => $protection['id'],
			'#attributes' => array(
					'id' => 'invId'
			)
	);

	$result = db_query("
    SELECT it.id, it.name
    FROM inventory_type it
    ORDER BY it.id desc;");

	$options = array(
			'' => t('- Please choose -')
	);
	foreach ($result as $record)
		$options[$record->id] = $record->name;
	$form['inventory_types'] = array(
			'#name' => 'add',
			'#type' => 'select',
			'#title' => t('Add new inventory group:'),
			'#options' => $options,
			'#weight' => $weight++,
			'#attributes' => array(
					'id' => 'add-inventory-group'
			)
	);

	// Show the Override or Submit button for this style.
	$form['actions'] = array(
			'#type' => 'actions'
	);
	$form['actions']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Save')
	);

	return $form;
}

function inventory_protection_save_entries($plist_id) {
	$form_state = form_state_defaults();
	$form_state['input'] = $_POST;
	$result = inventory_protection_edit_entries_submit(array(), $form_state, $plist_id);

	return drupal_json_output($result);
}

/**
 * Submit handler for the inventory_protection_edit_entries form
 * 
 * @param array $form
 * @param array $form_state
 * @param int $plist_id[optional]
 */
function inventory_protection_edit_entries_submit($form, &$form_state, $plist_id = false) {
	if (!$plist_id)
		$plist_id = $form_state['protection_id'];
	$ajax = isset($_GET['ajax']);

	_inventory_protection_update_modify_time($plist_id);

	$result = _inventory_save_entries($plist_id, $form_state['input'], 'inventory_custom_protection_inventory',
			'inventory_custom_protection_id', 'inventory_custom_protection_inventory_entry',
			'inventory_custom_protection_inventory_id', true);

	if ($ajax)
		return $result;
	drupal_set_message(t('Changes successfully saved'));
	$form_state['redirect'] = 'protection_lists/' . $plist_id . '/edit';
}

/**
 * Private helper function to get the data (inventories and their columns and rows)
 * of a template
 * 
 * @param int $template_id
 * @return array
 */
function _inventory_protection_data($plist_id, $category) {
	$invArr = array();
	if ($plist_id != NULL and $category == NULL) {
		$query = db_select('inventory_custom_protection_inventory', 'pi')
				->condition('pi.inventory_custom_protection_id', $plist_id);
		$query->join('inventory_type', 'it', 'it.id = pi.inventory_type_id');
		$query->fields('pi');
		$query->addField('it', 'name', 'it_name');
		$query->addField('it', 'id', 'it_id');
	}
	if ($plist_id == NULL and $category != NULL) {
		$query = db_select('inventory_custom_protection_inventory', 'pi')->condition('iccpl.name', $category);
		$query->join('inventory_type', 'it', 'it.id = pi.inventory_type_id');
		$query->fields('pi');
		$query->addField('icpie', 'protection_level');
		$query
				->join('inventory_custom_protection_inventory_entry', 'icpie',
						'icpie.inventory_custom_protection_inventory_id = pi.id');
		$query->join('inventory_custom_protection_protection_level', 'iccpl', 'iccpl.id = icpie.protection_level');
		$query->addField('it', 'name', 'it_name');
		$query->addField('it', 'id', 'it_id');
	}
	//echo $query;
	$inventories = $query->execute();

	foreach ($inventories as $inventory) {
		$inventory_id = $inventory->id;
		if ($plist_id != NULL and $category == NULL)
			$invArr[$inventory_id] = inventory_type_entries($inventory_id,
					'inventory_custom_protection_inventory_entry', 'inventory_custom_protection_inventory_id', false,
					false, false, false, 'protection_level');
		if ($plist_id == NULL and $category != NULL) {
			$invCriteria = array(
					'protection_level' => $inventory->protection_level,
					'inv_id' => $inventory_id
			);
			$invArr[$inventory_id] = inventory_type_entries($invCriteria,
					'inventory_custom_protection_inventory_entry', 'inventory_custom_protection_inventory_id', false,
					false, false, false, 'protection_level');
		}
		$invArr[$inventory_id]['invDesc'] = array(
				'id' => $inventory->it_id,
				'name' => $inventory->it_name
		);
		// if(!array_key_exists('invDesc', $invArr[$id]))
		$invArr[$inventory_id]["invDesc"]['cols'] = _inventory_protection_column_description();
	}
	return $invArr;
}

/**
 * Menu callback; Deletes a protection list inventory entry (called over AJAX)
 * 
 * @param int $plist_id
 * @param int $entry_id
 * @param boolean $plain[optional]
 * @return 
 */
function inventory_protection_delete_entry($plist_id, $entry_id, $plain = false) {
	try {
		db_delete('inventory_custom_protection_inventory_entry')->condition('id', $entry_id)->execute();
		_inventory_protection_update_modify_time($plist_id);
		$result = array(
				'result' => '1'
		);
	} catch (Exception $e) {
		$result = array(
				'result' => '0',
				'message' => t('Deleting the inventory entry failed.')
		);
	}
	if (!$plain)
		return drupal_json_output($result);
	return $result;
}

/**
 * Menu callback; Returns the HTML for a new inventory group (called over AJAX)
 * 
 * @return 
 * @param int $template_id
 * @param int $inventory_type_id
 * @param mixed $key
 */
function inventory_protection_add_inventory_group($plist_id, $inventory_type_id, $key) {
	$desc = db_select('inventory_type', 'it')->condition('it.id', $inventory_type_id)->fields('it')->execute()
			->fetchAssoc();
	return drupal_json_output(
			array('data' => drupal_render(_inventory_group_factory($plist_id, $desc, 'new_' . $key, array(), 1, false))));
}

/**
 * Private helper function to load the metadata of a protection list
 * 
 * @param int $inventory_protection_id
 * @return array
 */
function _inventory_protection_load($inventory_protection_id) {
	$query = db_select('inventory_custom_protection', 'ip')->condition('id', $inventory_protection_id)->fields('ip');
	$query->join('users', 'u', 'u.uid = ip.owner_id');
	$query->addField('u', 'name', 'owner');
	return $query->execute()->fetchAssoc();
}

/**
 * Private helper function to update the modification date of a template
 * 
 * @param int $template_id
 * @return 
 */
function _inventory_protection_update_modify_time($plist_id) {
	db_update('inventory_custom_protection')->fields(array('modify_time' => 'now()'))->condition('id', $plist_id)
			->execute();
}

function _inventory_protection_column_description() {
	$invColDesc = array();

	$resAttributes = db_query('
  		SELECT *
		FROM inventory_custom_protection_protection_level
		ORDER BY name');

	$i = 0;
	$i++;
	$invColDesc["cols"][$i] = array(
			"name" => t('Protection level'),
			"id" => '0',
			"format" => 'dropdown',
			"visible" => TRUE,
	);
	$invColDesc["cols"][$i]["dropdown_values"] = array();
	foreach ($resAttributes as $attribute) {
		array_push($invColDesc["cols"][$i]["dropdown_values"],
				array("id" => $attribute->id,
						"value" => $attribute->name . ' ' . $attribute->description
				));
	}
	return $invColDesc['cols'];
}

function _inventory_protection_save_entry_attributes($entry_id, $data) {
	$value = str_replace('_dropdown', '', $data['col_0']);
	$attribute['protection_level'] = $value;
	if ($value)
		db_update('inventory_custom_protection_inventory_entry')->fields($attribute)->condition('id', $entry_id)
				->execute();
}

function inventory_protection_category_show($category) {
	$categories = _inventory_protection_category_load($category);
	drupal_set_title($categoriy);

	$rows = array();
	$rows[] = array(
			array(
					'data' => t('Description'),
					'header' => true
			),
			$categories['description']
	);
	$rows[] = array(
			array(
					'data' => t('Radius'),
					'header' => true
			),
			$categories['radius']
	);

	$render_array['details'] = array(
			'#type' => 'fieldset',
			'#title' => t('Protection list details'),
			'#weight' => 1,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
			),
	);
	$render_array['details']['table'] = array(
			'#theme' => 'table',
			'#rows' => $rows,
			'#attributes' => array(
					'class' => array(
							'inventory_show'
					),
			),
	);

	$render_array = array_merge($render_array,
			inventory_inventories($head_inventory_id, _inventory_protection_data(NULL, $category), false));
	return $render_array;
}

function _inventory_protection_category_load($category) {
	$query = db_select('inventory_custom_protection_inventory', 'pi')->condition('iccpl.name', $category)->fields('pi');
	$query->join('inventory_type', 'it', 'it.id = pi.inventory_type_id');
	$query
			->join('inventory_custom_protection_inventory_entry', 'icpie',
					'icpie.inventory_custom_protection_inventory_id = pi.id');
	$query->join('inventory_custom_protection_protection_level', 'iccpl', 'iccpl.id = icpie.protection_level');
	$query->addField('it', 'name', 'it_name');
	$query->addField('it', 'id', 'it_id');
	return $query->execute()->fetchAssoc();
}

?>