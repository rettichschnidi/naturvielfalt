<?php

module_load_include('inc', 'inventory', 'inventory.edit');

function inventory_template_list() {
  global $user;
  drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');
  $render_array = array();
  
  $templates = db_select('inventory_template', 't')
    ->condition('t.owner_id', $user->uid)
    ->fields('t');
  $templates->leftJoin('inventory_template_inventory', 'ti', 'ti.inventory_template_id = t.id');
  $templates->leftJoin('inventory_template_inventory_entry', 'tie', 'tie.inventory_template_inventory_id = ti.id');
  $templates->addExpression('COUNT(tie.id)', 'organisms');
  $templates->groupBy('t.id, t.name, t.description, t.public, t.owner_id, t.create_time, t.modify_time');
  $templates = $templates->execute();
  
  //Delete Image
  $img_delete = theme('image', array(
    'path' => 'misc/fatcow_icons/16x16/delete.png',
    'alt' => t('Delete'),
    'title' => t('Delete'),
  ));
  
  $headings = array(
    t('Template name'),
    t('Organisms'),
    t('Created on'),
    t('Modified on'),
    '&nbsp;'
  );
  $rows = array();
  foreach($templates as $record) {
    array_push($rows, array(
      'id' => 'template_'.$record->id,
      'data' => array(
        $record->name,
        $record->organisms,
        date("j.m.Y", strtotime($record->create_time)),
        date("j.m.Y", strtotime($record->modify_time)),
        l($img_delete, 'inventorytemplate/'.$record->id.'/delete', array('html' => true))
      )
    ));
  }
  
  if($rows)
    $render_array['templates'] = array(
      '#theme' => 'datatable',
      '#header' => $headings,
      '#rows' => $rows,
      '#id_table' => 'templates',
    );
  else
    $render_array['empty'] = array(
      '#markup' => t('You have not yet created any templates. !add', array(
        '!add' => l(t('Add one'), 'inventorytemplate/new')
      )),
    );
  
  return $render_array;
}

/**
 * Form builder; Form for deleting an inventory template.
 *
 * @param $template_id
 *   The ID of the template
 *
 * @ingroup forms
 * @see inventory_template_delete_form_submit()
 */
function inventory_template_delete_form($form, &$form_state, $template_id) {
  $template = _inventory_template_load($template_id);
  $form_state['template'] = $template;

  return confirm_form(
    $form,
    t('Delete %name?', array('%name' => $template['name'])),
    'inventorytemplate',
    t('Are you sure you want to delete the template %name?', array('%name' => $template['name'])),
    t('Delete'),  t('Cancel')
  );
}

/**
 * Submit handler to delete a template.
 */
function inventory_template_delete_form_submit($form, &$form_state) {
  $template = $form_state['template'];

  db_delete('inventory_template')->condition('id', $template['id'])->execute();
  $result = db_select('inventory_template_inventory', 'i')
    ->condition('i.inventory_template_id', $template['id'])
    ->fields('i', array('id'))
    ->execute()
    ->fetchAll();
  $inventories = array();
  foreach($result as $record)
    array_push($inventories, $record->id);
  
  if($inventories) {
    db_delete('inventory_template_inventory')->condition('id', $inventories, 'IN')->execute();
    db_delete('inventory_template_inventory_entry')->condition('inventory_template_inventory_id', $inventories, 'IN')->execute();
  }
  drupal_set_message(t('Template %name was deleted.', array('%name' => $template['name'])));
  $form_state['redirect'] = 'inventorytemplate';
}

function inventory_template_edit($template_id) {
  $render_array = array();
  _include_js_and_css();
  $template = _inventory_template_load($template_id);
  
  drupal_set_title(t('Modify %name (%id)', array(
    '%name' => $template['name'],
    '%id' => $template['id']
  )), PASS_THROUGH);
  
  $render_array['basics'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Basics'), 
    '#weight' => 2, 
    '#collapsible' => TRUE, 
    '#collapsed' => TRUE,
    '#attached' => array (
      'js' => array (
        'misc/form.js',
        'misc/collapse.js',
      ),
    ),
    '#attributes' => array(
      'class' => array('collapsible', 'collapsed'),
    ),
  );
  
  $render_array['basics']['edit'] = drupal_get_form('inventory_template_edit_metadata', $template);
  
  $render_array['entries'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Organisms'), 
    '#weight' => 6, 
    '#collapsible' => TRUE, 
    '#collapsed' => FALSE,
    '#attached' => array (
      'js' => array (
        'misc/form.js',
        'misc/collapse.js',
      ),
    ),
    '#attributes' => array(
      'class' => array('collapsible'),
    ),
  );
  
  $render_array['entries']['edit'] = drupal_get_form('inventory_template_edit_entries', $template);
  
  return $render_array;
}

function inventory_template_edit_metadata($form, &$form_state, $template=false) {
  if(!$template)
    drupal_set_title(t('Create new template'));
  
  $form_state['template'] = $template;
  
  $form['name'] = array (
    '#type' => 'textfield', 
    '#title' => t('Name'), 
    '#size' => 100, 
    '#maxlength' => 128,
    '#required' => TRUE,
    '#default_value' => $template ? $template['name'] : ''
  );
  
  $form['description'] = array (
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $template ? $template['description'] : ''
  );
  
  $form['public'] = array (
    '#type' => 'checkbox', 
    '#title' => t('Public'),
    '#description' => t('Allow other user to use this template'),
    '#default_value' => $template ? $template['public'] : '',
  );
  
  // Show the Override or Submit button for this style.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $template ? t('Save') : t('Create')
  );
  $form['close'] = array(
    '#markup' => l(t('Cancel'), 'inventorytemplate')
  );
  
  return $form;
}

function inventory_template_edit_metadata_submit($form, &$form_state) {
  $template = $form_state['template'];
  
  if(!$template) {
    global $user;
    $template = array(
      'owner_id' => $user->uid,
      'create_time' => 'now()'
    );
  }
  $template['modify_time'] = 'now()';
  $template['name'] = $form_state['values']['name'];
  $template['description'] = $form_state['values']['description'];
  $template['public'] = $form_state['values']['public'];
  if(isset($template['id']) && is_numeric($template['id']))
    drupal_write_record('inventory_template', $template, 'id');
  else
    drupal_write_record('inventory_template', $template);
  drupal_set_message(t('Changes successfully saved'));
  $form_state['redirect'] = 'inventorytemplate/'.$template['id'].'/edit';
}

function inventory_template_edit_entries($form, &$form_state, $template) {
  global $user;
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js', array('weight' => 100));
  
  $form_state['template_id'] = $template['id'];
  
  $form['inventories_start'] = array(
    '#markup' => '<div id="inventories">',
    '#weight' => 0
  );
  
  $data = _inventory_template_data($template['id']);
  $weight = 1;
  foreach($data as $inventory_id => $inventory_group) {
    $form = array_merge(_inventory_group_factory($template['id'], $inventory_group['invDesc'], $inventory_id, array(), $inventory_group, $weight, false), $form);
    $weight += 2;
  }
  $form['inventories_end'] = array(
    '#markup' => '</div>',
    '#weight' => $weight++
  );
  
  if($weight == 2)
    $form['empty'] = array(
      '#markup' => '<p class="empty">'.t('There are no inventory groups available yet. Add a new one by using the select box in the lower left.</p>')
    );
  
  $form['inventory_template_id'] = array(
    '#type' => 'hidden', 
    '#value' => $template['id'],
    '#attributes' => array(
      'id' => 'invId'
    )
  );
  
  $result = db_query("
    SELECT it.id, it.name
    FROM inventory_type it
    ORDER BY it.id desc;"
  );
  
  $options = array('' => t('- Please choose -'));
  foreach($result as $record)
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
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );
  
  return $form;
}

/**
 * Menu callback; Saving the entries of a template using AJAX
 *  (wrapper around inventory_template_save_entries)
 * 
 * @return JSON Object with the following keys:
 *  - "entries": Mapping of ids which changed after saving. Array of arrays with the following keys:
 *    - "id_old": The ID of the entry before saving
 *    - "id_new": The ID of the entry after saving
 *  - "inventories": Mapping of ids which changed after saving. Array of arrays with the following keys:
 *    - "id_old": The ID of the inventory group before saving
 *    - "id_new": The ID of the inventory group after saving
 *  - "result": Whether the request was successfull or not (1|0)
 *  - "group_count": The new group count (Determining which group ID to use for the next new inventory group)
 *  - "message": The message to be displayed to the user
 * @param object $template_id
 */
function inventory_template_save_entries($template_id) {
  $form_state = form_state_defaults();
  $form_state['input'] = $_POST;
  $result = inventory_template_edit_entries_submit(array(), $form_state, $template_id);
  
  return drupal_json_output($result);
}

/**
 * Submit handler for the inventory_template_edit_entries form
 * 
 * @param object $form
 * @param object $form_state
 * @param object $template_id[optional]
 */
function inventory_template_edit_entries_submit($form, &$form_state, $template_id=false) {
  if(!$template_id)
    $template_id = $form_state['template_id'];
  $ajax = $_GET['ajax'];
  
  _inventory_template_update_modify_time($template_id);
  
  $result = _inventory_save_entries($template_id, $form_state['input'], 'inventory_template_inventory', 'inventory_template_id', 'inventory_template_inventory_entry', 'inventory_template_inventory_id', false);
  
  if($ajax)
    return $result;
  drupal_set_message(t('Changes successfully saved'));
  $form_state['redirect'] = 'inventorytemplate/'.$template_id.'/edit';
}

function _inventory_template_data($template_id) {
  $invArr = array();
  $query = db_select('inventory_template_inventory', 'ti')
    ->condition('ti.inventory_template_id', $template_id);
  $query->join('inventory_type', 'it', 'it.id = ti.inventory_type_id');
  $query->fields('ti');
  $query->addField('it', 'name', 'it_name');
  $query->addField('it', 'id', 'it_id');
  $inventories = $query->execute();
  
  foreach($inventories as $inventory) {
    $inventory_id = $inventory->id;
    $invArr[$inventory_id] = inventory_type_entries($inventory_id, 'inventory_template_inventory_entry', 'inventory_template_inventory_id', false, false, false, false);
    $invArr[$inventory_id]['invDesc'] = array(
      'id' => $inventory->it_id,
      'name' => $inventory->it_name
    );
  }
  return $invArr;
}

/**
 * Menu callback; Deletes a template inventory entry (called over AJAX)
 * 
 * @param object $template_id
 * @param object $entry_id
 * @param object $plain[optional]
 * @return 
 */
function inventory_template_delete_entry($template_id, $entry_id, $plain=false) {
  try {
    db_delete('inventory_template_inventory_entry')->condition('id', $entry_id)->execute();
    _inventory_template_update_modify_time($template_id);
    $result = array(
      'result' => '1'
    );
  } catch(Exception $e) {
    $result = array(
      'result' => '0',
      'message' => t('Deleting the inventory entry failed.')
    );
  }
  if(!$plain)
    return drupal_json_output($result);
  return $result;
}

/**
 * Menu callback; Returns the HTML for a new inventory group (called over AJAX)
 * 
 * @return 
 * @param object $template_id
 * @param object $inventory_type_id
 * @param object $key
 */
function inventory_template_add_inventory_group($template_id, $inventory_type_id, $key) {
  $desc = db_select('inventory_type', 'it')
    ->condition('it.id', $inventory_type_id)
    ->fields('it')
    ->execute()
    ->fetchAssoc();
  return drupal_json_output(array(
    'data' => drupal_render(_inventory_group_factory($template_id, $desc, 'new_'.$key, array(), array(), 1, false))
  ));
}

function _inventory_template_load($inventory_template_id) {
  return db_select('inventory_template', 'it')
    ->condition('id', $inventory_template_id)
    ->fields('it')
    ->execute()
    ->fetchAssoc();
}

/**
 * Private helper function to update the modification date of a template
 * 
 * @param object $template_id
 * @return 
 */
function _inventory_template_update_modify_time($template_id) {
  db_update('inventory_template')
  ->fields(array('modify_time'=>'now()'))
  ->condition('id', $template_id)
  ->execute();
}

?>