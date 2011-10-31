<?php

/**
 * Menu callback; Edit form of an inventory
 * 
 * @param $head_inventory_id
 *   The id of inventory to be modified
 * @return all inventory forms to modify the inventory
 */
function inventory_edit($head_inventory_id, $redirect=true) {
  _include_js_and_css();
  area_add_gmap_resources(false);
  
  drupal_set_title(t('Modify %name (%id)', array(
    '%name' => inventory_title($head_inventory_id),
    '%id' => $head_inventory_id
  )), PASS_THROUGH);
  
  if($redirect) {
    $hidden = db_select('head_inventory', 'hi')
      ->condition('hi.id', $head_inventory_id)
      ->fields('hi', array('hidden', 'owner_id'))
      ->execute()
      ->fetch();
    if($hidden->hidden) {
      global $user;
      if($user->uid == $hidden->owner_id)
        return drupal_goto('singleobservations/edit');
      else
        return drupal_goto('user/'.$hidden->owner_id.'/singleobservations/edit');
    }
  }
  
  //------------------
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

  if(isset($_SESSION['inventory']['new']) && $_SESSION['inventory']['new']) {
    $render_array['suggestion'] = array(
      '#prefix' => '<p>',
      '#markup' => t('You can now edit your newly created inventory:<ul><li>add organisms by using the "Add new inventory group" select box in the lower left</li><li>edit the permissions for this inventory</li><li>attach files to this inventory</li></ul>'),
      '#suffix' => '</p>'
    );
    unset($_SESSION['inventory']['new']);
  }
  
  $render_array['basics']['edit'] = drupal_get_form('inventory_edit_basics', $head_inventory_id, false);

  //------------------
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
  
  $render_array['entries']['edit'] = drupal_get_form('inventory_edit_entries', $head_inventory_id);
  
  //------------------
  $render_array['rights'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Permissions'), 
    '#weight' => 8, 
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
  
  $render_array['rights']['edit'] = drupal_get_form('inventory_edit_rights', $head_inventory_id);

  //------------------
  $render_array['file'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Files'), 
    '#weight' => 10, 
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

  $render_array['file']['upload'] = drupal_get_form('inventory_edit_files', $head_inventory_id);
  
  return $render_array;
}

function inventory_delete($form, &$form_state, $head_inventory_id) {
  $form_state['head_inventory_id'] = $head_inventory_id;
  
  $name = inventory_title($head_inventory_id);
  return confirm_form(
    $form,
    t('Delete %name?', array('%name' => $name)),
    'inventory/' . $head_inventory_id,
    t('Are you sure you want to delete the inventory %name? All contained entries and files will also be deleted.', array('%name' => $name)),
    t('Delete'),  t('Cancel')
  );
}

function inventory_delete_submit($form, &$form_state) {
  $head_inventory_id = $form_state['head_inventory_id'];
  
  $files = db_select('head_inventory_file_managed', 'hifm')
    ->condition('head_inventory_id', $head_inventory_id)
    ->fields('hifm', array('file_id'))
    ->execute()
    ->fetchCol(0);
  foreach($files as $fid)
    file_delete(file_load($fid));
  $images = db_select('gallery_image', 'gi', array('fetch' => PDO::FETCH_ASSOC))
    ->fields('gi', array('id'));
  $images->join('file_managed', 'f', 'f.fid = gi.fid');
  $images->fields('f');
  $images->leftJoin('inventory_entry', 'e', 'e.id = gi.item_id AND gi.item_type = \'inventory_entry\'');
  $images->leftJoin('inventory', 'i', 'i.id = e.inventory_id');
  $images->condition(db_or()
    ->condition(db_and()
      ->condition('gi.item_type', 'head_inventory')
      ->condition('gi.item_id', $head_inventory_id)
    )
    ->condition('i.head_inventory_id', $head_inventory_id)
  );
  $images = $images->execute();
  foreach($images as $image)
    gallery_image_delete($image);
  db_delete('head_inventory_file_managed')
    ->condition('head_inventory_id', $head_inventory_id)
    ->execute();
  db_delete('head_inventory_export_type')
    ->condition('head_inventory_id', $head_inventory_id)
    ->execute();
  db_delete('inventory_type_attribute')
    ->condition('head_inventory_id', $head_inventory_id)
    ->execute();
  db_delete('head_inventory')
    ->condition('id', $head_inventory_id)
    ->execute();
  
  drupal_set_message(t('The inventory was successfully deleted'));
  $form_state['redirect'] = 'inventory';
}


/**
 * Form to edit the entries of an inventory
 * 
 * @return form of inventory entries
 */
function inventory_edit_entries($form, &$form_state, $head_inventory_id) {
  global $user;
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js', array('weight' => 100));
  
  $form_state['head_inventory_id'] = $head_inventory_id;
  
  $form['templates'] = array(
    '#markup' => l(t('Add organisms from template'), 'inventory/'.$head_inventory_id.'/templates', array('attributes' => array('id' => 'template'))),
    '#weight' => -1
  );
  
  $form['inventories_start'] = array(
    '#markup' => '<div id="inventories">',
    '#weight' => 0
  );
  
  $data = inventory_data($head_inventory_id, false, true, false, true);
  $weight = 1;
  foreach($data as $inventory_id => $inventory_group) {
    $form = array_merge(_inventory_group_factory($head_inventory_id, $inventory_group['invDesc'], $inventory_id, $inventory_group, $weight), $form);
    $weight += 2;
  }
  $form['inventories_end'] = array(
    '#markup' => '</div>',
    '#weight' => $weight++
  );
  
  if($weight == 2)
    $form['empty'] = array(
      '#markup' => '<p class="empty">'.t('There are no inventory groups available yet. Add a new one by using the select box in the lower left.</p>'),
      '#weight' => $weight++
    );
  
  $form['head_inventory_id'] = array(
    '#type' => 'hidden', 
    '#value' => $head_inventory_id,
    '#attributes' => array(
      'id' => 'invId'
    )
  );
  
  $result = db_select('inventory_type', 'it')
    ->fields('it', array('id', 'name'))
    ->orderBy('it.id', 'DESC')
    ->execute();
  
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
 * Menu callback; Saving the entries of an inventory using AJAX
 *  (wrapper around inventory_edit_entries_submit)
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
 * @param object $head_inventory_id
 */
function inventory_save_entries($head_inventory_id) {
  $form_state = form_state_defaults();
  $form_state['input'] = $_POST;
  $result = inventory_edit_entries_submit(array(), $form_state, $head_inventory_id);
  
  return drupal_json_output($result);
}

/**
 * Submit handler for the inventory_edit_entries form
 * 
 * @param object $form
 * @param object $form_state
 * @param object $head_inventory_id[optional]
 */
function inventory_edit_entries_submit($form, &$form_state, $head_inventory_id=false) {
  if(!$head_inventory_id)
    $head_inventory_id = $form_state['head_inventory_id'];
  $ajax = $_REQUEST['ajax'];
  
  _inventory_update_modify_time($head_inventory_id);
  
  $result = _inventory_save_entries($head_inventory_id, $form_state['input']);
  
  if($ajax)
    return $result;
  drupal_set_message(t('Changes successfully saved'));
  $form_state['redirect'] = 'inventory/'.$head_inventory_id.'/edit';
}

/**
 * Private helper function to store entries of an inventory
 *  (used by inventory_edit_entries_submit and inventory_template_edit_entries_submit
 * 
 * @return 
 * @param object $id
 * @param object $input
 * @param object $inventory_table[optional]
 * @param object $inventory_relation[optional]
 * @param object $entry_table[optional]
 * @param object $entry_relation[optional]
 * @param object $additional[optional]
 */
function _inventory_save_entries($id, $input, $inventory_table='inventory', $inventory_relation='head_inventory_id', $entry_table='inventory_entry', $entry_relation='inventory_id', $additional=true) {
  $new_entries = array();
  $new_inventories = array();
  
  $group_count = 1;
  if(is_array($input['entries'])) {
    $iid = db_select($inventory_table, 'i')->orderBy('i.id', 'DESC')->range(0, 1)->fields('i', array('id'))->execute()->fetchColumn(0)+1;
    $eid = db_select($entry_table, 'e')->orderBy('e.id', 'DESC')->range(0, 1)->fields('e', array('id'))->execute()->fetchColumn(0)+1;
    foreach($input['entries'] as $inventory_id => $entries) {
      $inventory_type_id = $input['inventories'][$inventory_id];
      if($additional){
      	$head_inventory_id = $id;
        $desc = inventory_column_description($inventory_id, $inventory_type_id, true, $head_inventory_id); 
      }     		
      $new_inventory_id = $inventory_id;
      if(substr($inventory_id, 0, 4) == 'new_') {
        $data = array(
          'inventory_type_id' => $inventory_type_id
        );
        $data[$inventory_relation] = $id;
        $group_count = max($group_count, intval(substr($inventory_id, 4))+1);
        $new_inventory_id = db_insert($inventory_table)->fields($data)->execute();
      }
      $count = 0;
      foreach($entries as $position => $entry) {
        if($entry['entry_id'] && $entry['delete'])
          db_delete($entry_table)->condition('id', $entry['entry_id'])->execute();
        if(!$entry['organism_id'] || $entry['delete'])
          continue;
        $count++;
        $data = array(
          'id' => $entry['id'],
          'organism_id' => $entry['organism_id'],
          'position' => $position,
          'zip' => isset($entry['zip']) ? $entry['zip'] : '',
          'locality' => isset($entry['locality']) ? $entry['locality'] : '',
          'township' => isset($entry['township']) ? $entry['township'] : '',
          'canton' => isset($entry['canton']) ? $entry['canton'] : '',
          'country' => isset($entry['country']) ? $entry['country'] : '',
        );
        $data[$entry_relation] = $new_inventory_id;
        if(substr($entry['id'], 0, 4) == 'new_') {
          array_push($new_entries, array(
            'id_old' => $entry['id'],
            'id_new' => $eid
          ));
          $data['id'] = $eid++;
          db_insert($entry_table)->fields($data)->execute();
        } else {
          db_update($entry_table)->fields($data)->condition('id', $data['id'])->execute();
        }

        // update geom
        if (isset($entry['lat']) && isset($entry['lng'])) {
          $geom = 'ST_GeomFromText(\'POINT(' . $entry['lng'] . ' ' . $entry['lat'] . ')\', 4326)'; // SRID 4326 = WGS 84
          db_update($entry_table)->expression('geom', $geom)->condition('id', $data['id'])->execute();
        }

        if($additional)
          if(key_exists('inventory_protection_id', $input)) {
            _inventory_protection_save_entry_attributes($data['id'], $entry);;	
          } else {
          	_inventory_save_entry_attributes($data['id'], $desc['cols'], $entry);
          }         		
      }
      if($inventory_id != $new_inventory_id) {
        if($count > 0)
          array_push($new_inventories, array(
            'id_old' => $inventory_id,
            'id_new' => $new_inventory_id
          ));
        else
          db_delete($inventory_table)->condition('id', $iid--)->execute();
      }
    }
  }
  // delete empty inventories
  $empty = db_select($inventory_table, 'i')
    ->condition('i.'.$inventory_relation, $id)
    ->where('NOT EXISTS (SELECT e.id FROM '.$entry_table.' e WHERE e.'.$entry_relation.' = i.id)')
    ->fields('i', array('id'))
    ->execute()
    ->fetchAll();
  $delete = array();
  foreach($empty as $i => $record) {
    array_push($new_inventories, array(
      'id_old' => $record->id,
      'id_new' => 'new_'.$group_count++
    ));
    array_push($delete, $record->id);
  }
  if($delete)
    db_delete($inventory_table)->condition('id', $delete, 'IN')->execute();
  
  return array(
    'entries' => $new_entries,
    'inventories' => $new_inventories,
    'result' => '1',
    'group_count' => $group_count,
    'message' => t('Entries successfully saved (!time)', array('!time' => date('d.m.Y G:i', time())))
  );
}

/**
 * Menu callback; AJAX autocomplete for selecting organisms
 * 
 * @return array Array of organisms matching the term having the following keys:
 *  - "id": The ID of the organism
 *  - "label": The name of the organism
 *  - "label_latin": The latin name of the organism
 *  - "old_id"[Optional]: The ID of the initially found organism
 *    (only available if the found organism has a reference to an official one)
 *  - "old_label"[Optional]: The name of the initially found organism
 *    (only available if the found organism has a reference to an official one)
 *  - "old_label_latin"[Optional]: The latin name of the initially found organism
 *    (only available if the found organism has a reference to an official one)
 */
function inventory_organism_autocomplete($type_id=false, $term=false, $plain=false) {
  if(!$type_id)
    $type_id = $_REQUEST['inv_id'];
  if(!$term)
    $term = $_REQUEST['term'];
  $term = strtolower($term);
  $terms = explode(' ', trim($term));
  $terms = array_filter($terms);
  
  $limit = strlen(trim($term)) > 3 ? 100000 : 20;
  
  $fauna = ($type_id == 16 ? false : true);
  
  $organisms = array();
  
  $where = array();
  $params = array(
    ':inventoryTypeId' => $type_id,
    ':limit' => $limit
  );
  $i = 1;
  $query = 
    'SELECT 
      f.id,
      o.id o_id, 
      {values}
     FROM inventory_type_organism AS t
     INNER JOIN organism AS o ON o.id = t.organism_id
     INNER JOIN {table} AS f ON f.id = o.organism_id AND o.organism_type = '.($fauna ? '1' : '2').'
     WHERE t.inventory_type_id = :inventoryTypeId
      AND ({where})
     ORDER BY f.id
     LIMIT :limit;';
  if($fauna) {
    $table = 'fauna_organism';
    $values = array('f.name_de', 'f.genus', 'f.species', 'f.sub_species', 'f.family');
    $columns = array('genus', 'species');
  } else {
    $table = 'flora_organism';
    $values = array('f.name_de', 'f.name', 'f."Gattung" floragattung', 'f."Art" floraart', 'f."Rang" florarang', 'f."NameUnterart" floraunterart', 'f.official_flora_orfganism_id ref_id');
    $columns = array('"Gattung"', '"Art"');
  }
  array_push($where, 'f.name_de ILIKE :term'.$i);
  $params[':term'.$i++] = '%'.trim($term).'%';
  $combined = array();
  foreach($columns as $k => $column) {
    if(!array_key_exists($k, $terms))
      break;
    array_push($combined, '(f.'.$column.' LIKE :term'.$i.' OR f.'.$column.' LIKE :term'.($i+1).')');
    $params[':term'.$i++] = $terms[$k].'%';
    $params[':term'.$i++] = ucfirst($terms[$k]).'%';
  }
  array_push($where, '('.implode(' AND ', $combined).')');
  $result = db_query(
    str_replace(array(
      '{where}',
      '{table}',
      '{values}'), array(
      implode(' OR ', $where),
      $table,
      implode(', ', $values)), $query)
    , $params
  );
  foreach($result as $record){
    $organism = array(
      'id' => $record->o_id,
      'label' => strval($record->name_de),
      'label_latin_short' => strval($fauna ? trim($record->genus." ".$record->species." ".$record->sub_species) : trim($record->floragattung." ".$record->floraart." ".$record->florarang." ".$record->floraunterart)),
      'label_latin' => strval($fauna ? trim($record->family." ".$record->genus." ".$record->species." ".$record->sub_species) : $record->name)
    );
    if(!$fauna && $record->id != $record->ref_id) {
      // if it is not the official name it is an alias
      // this will let the user search for aliases but saves only the 'parent', the
      // one where the official_flora_organism_id matches the organism_id
      $parent = $record;
      while($parent->id != $parent->ref_id) {
        $parent = db_query(
          'SELECT 
            f.id,
            o.id o_id, 
            f.name_de, 
            f.name,
            f.official_flora_orfganism_id ref_id 
           FROM flora_organism f 
           INNER JOIN organism AS o ON o.organism_id = f.id
           WHERE o.organism_type = 2 AND f.id = :ref_id'
          , array(':ref_id' => $record->ref_id)
        )->fetch();
      }
      $organism['old_id'] = $organism['id'];
      $organism['old_label'] = $organism['label'];
      $organism['old_label_latin'] = $organism['label_latin'];
      $organism['id'] = $parent->o_id;
      $organism['label'] = strval($parent->name_de);
      $organism['label_latin'] = strval($parent->name);
    }
    
    $organisms[] = $organism;
  }
  if(!$plain)
    $organisms = drupal_json_output($organisms);
    
    debug($organisms,'JSON_OUTPUT',true);
  return $organisms;
}

/**
 * Menu callback; Edit form for the additional attributes of an inventory entry
 *  (wrapper around inventory_edit_entry_form)
 * 
 * @param object $head_inventory_id
 * @param object $inventory_type_id
 * @param object $entry_id
 * @return form to edit the additional attributes of an inventory entry
 */
function inventory_edit_entry($head_inventory_id, $inventory_type_id, $entry_id) {
  $ajax = $_REQUEST['ajax'];
  
  if(substr($entry_id, 0, 4) == 'new_')
    $entry_id = false;
  
  $form = drupal_get_form('inventory_edit_entry_form', $head_inventory_id, $inventory_type_id, $entry_id);
  
  if($ajax)
    return drupal_json_output(array(
      'form' => drupal_render($form),
      'head_inventory_id' => $head_inventory_id,
      'inventory_type_id' => $inventory_type_id,
      'entry_id' => $entry_id
    ));
  return $form;
}

/**
 * Edit form for the additional attributes of an inventory entry
 * 
 * @param object $form
 * @param object $form_state
 * @param object $head_inventory_id
 * @param object $inventory_type_id
 * @param object $entry_id
 * @return form to edit the additional attributes of an inventory entry
 */
function inventory_edit_entry_form($form, &$form_state, $head_inventory_id, $inventory_type_id, $entry_id) {
  _include_js_and_css();
  $ajax = $_REQUEST['ajax'];
  $form_state['head_inventory_id'] = $head_inventory_id;
  $form_state['inventory_type_id'] = $inventory_type_id;
  $form_state['entry_id'] = $entry_id;
  $form_state['ajax'] = $ajax;
  
  if($entry_id)
    $data = array_pop(inventory_data($head_inventory_id, false, true, $entry_id, true));
  else
    $data = array(
      array(),
      'invDesc' => inventory_column_description(false, $inventory_type_id, true, $head_inventory_id)
    );
  $entry = $data[0];
  drupal_set_title(t('Additional fields for %name', array('%name' => $data['invDesc']['name'])), PASS_THROUGH);
  $columns = $data['invDesc']['cols'];
  $form_state['columns'] = $columns;
  $count = 0;
  $form[null] = array(
    '#type' => 'fieldset',
    '#title' => t('Standard Attributes')
  );
  $form[$head_inventory_id] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom Attributes')
  );
  $customAttrCount = 0;
  foreach($columns as $column) {
    if($column['visible'])
      continue; // We do not want to edit fields which are visible in the edit entries form
    $name = 'col_'.$column['id'];
    $value = array_key_exists($name, $_REQUEST) ? htmlentities($_REQUEST[$name]) : $entry[$name];
    $form[$column['head_inventory_id']][$name] = array(
      '#type' => 'textfield',
      '#title' => $column['name'],
      '#required' => false,
      '#default_value' => $value,
      '#attributes' => array(
        'class' => array($column['format'])
      )
    );
    if($column['format'] == 'dropdown') {
      $options = array('');
      foreach($column['dropdown_values'] as $v)
        $options[$v['id'].'_dropdown'] .= $v['value'];
      $form[$column['head_inventory_id']][$name]['#type'] = 'select';
      $form[$column['head_inventory_id']][$name]['#options'] = $options;
    }
    if ($column['head_inventory_id'] != null) {
    	$customAttrCount++;
    }
    $count++;
  }
  
  $form['actions'] = array('#type' => 'actions');
  if($count) {
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save')
    );    
  }
  else {
    $form['message'] = array(
      '#markup' => '<div class="messages status">'.t('There are no additional fields available for this inventory group.').'</div>'
    );
  }
  if ($customAttrCount == 0) {
  	$form[$head_inventory_id]['#description'] = t ('There are no custom fields available for this inventory group.');
  }
  
  
  $form['actions']['close'] = array(
    '#markup' => l(t('Cancel'), 'inventory/'.$head_inventory_id.'/edit')
  );
  
  return $form;
}

/**
 * Submit handler for the inventory_edit_entry_form saving the additional attributes of an inventory entry
 * 
 * @return 
 * @param object $form
 * @param object $form_state
 */
function inventory_edit_entry_form_submit($form, &$form_state) {
  $ajax = $form_state['ajax'];
  
  _inventory_update_modify_time($head_inventory_id);
  
  _inventory_save_entry_attributes($form_state['entry_id'], $form_state['columns'], $form_state['input']);
  
  if($ajax)
    return;
  drupal_set_message(t('Changes successfully saved'));
  $form_state['redirect'] = 'inventory/'.$form_state['head_inventory_id'].'/edit';
}

/**
 * Private helper function to store additional attributes on an inventory entry
 * 
 * @param object $entry_id
 * @param object $columns
 * @param object $data
 * @return 
 */
function _inventory_save_entry_attributes($entry_id, $columns, $data) {
  $id = db_select('inventory_type_attribute_inventory_entry', 'i')->orderBy('i.id', 'DESC')->range(0, 1)->fields('i', array('id'))->execute()->fetchColumn(0)+1;
  $seen = array();
  foreach($columns as $column) {
    array_push($seen, $column['id']);
    if(!isset($data['col_'.$column['id']]))
      continue;
    if(!$data['col_'.$column['id']]) {
      db_delete('inventory_type_attribute_inventory_entry')
        ->condition('inventory_entry_id', $entry_id)
        ->condition('inventory_type_attribute_id', $column['id'])
        ->execute();
      continue;
    }
    if($column['format'] == 'dropdown')
      $value = array(
        'inventory_type_attribute_dropdown_value_id' => str_replace('_dropdown', '', $data['col_'.$column['id']])
      );
    else
      $value = array(
        'value' => $data['col_'.$column['id']]
      );
      
    $existing = db_select('inventory_type_attribute_inventory_entry', 'i')
      ->condition('i.inventory_entry_id', $entry_id)
      ->condition('i.inventory_type_attribute_id', $column['id'])
      ->fields('i', array('id'))
      ->execute()
      ->fetch();
    if($existing->id)
      db_update('inventory_type_attribute_inventory_entry')->fields($value)->condition('id', $existing->id)->execute();
    else {
      $value = array_merge($value, array(
        'id' => $id++,
        'inventory_entry_id' => $entry_id,
        'inventory_type_attribute_id' => $column['id']
      ));
      db_insert('inventory_type_attribute_inventory_entry')->fields($value)->execute();
    }
  }
  // clean up
  $clean = db_delete('inventory_type_attribute_inventory_entry')
    ->condition('inventory_entry_id', $entry_id);
  if($seen)
    $clean->where('NOT inventory_type_attribute_id IN ('.implode(', ', $seen).')');
  $clean->execute();
}

/**
 * Menu callback; Edit form for the location of an inventory entry
 *  (wrapper around inventory_edit_entry_location_form)
 * 
 * @param object $head_inventory_id
 * @param object $entry_id
 * @return form to edit the location of an inventory entry
 */
function inventory_edit_entry_location($head_inventory_id, $entry_id) {
  $ajax = $_REQUEST['ajax'];
  
  if(substr($entry_id, 0, 4) == 'new_')
    $entry_id = false;
  
  $form = drupal_get_form('inventory_edit_entry_location_form', $head_inventory_id, $entry_id);
  
  if($ajax)
    return drupal_json_output(array(
      'form' => drupal_render($form),
      'head_inventory_id' => $head_inventory_id,
      'entry_id' => $entry_id,
      'title' => t('Set observation location')
    ));
  return $form;
}

/**
 * Edit form for the location of an inventory entry
 * 
 * @param object $form
 * @param object $form_state
 * @param object $entry_id
 * @return form to edit the location of an inventory entry
 */
function inventory_edit_entry_location_form($form, &$form_state, $head_inventory_id, $entry_id) {
  $ajax = $_REQUEST['ajax'];
  $form_state['head_inventory_id'] = $head_inventory_id;
  $form_state['entry_id'] = $entry_id;
  $form_state['ajax'] = $ajax;
  
  drupal_set_title(t('Set observation location'));
  
  $form = inventory_entry_map($form, $form_state, $head_inventory_id, $entry_id);
  
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );
  $form['actions']['close'] = array(
    '#markup' => l(t('Cancel'), 'inventory/'.$head_inventory_id.'/edit')
  );
  
  $form['actions']['info'] = array(
    '#markup' => '<em class="inventory-location-info">'.t('Drag and drop the marker to the location you observed the organism.').'</p>'
  );
  
  return $form;
}

/**
 * Submit handler for the inventory_edit_entry_location_form saving the location of an inventory entry
 * 
 * @return 
 * @param object $form
 * @param object $form_state
 */
function inventory_edit_entry_location_form_submit($form, &$form_state) {
  $ajax = $form_state['ajax'];
  $entry_id = $form_state['entry_id'];
  
  _inventory_update_modify_time($head_inventory_id);
  
  $data = array(
    'location' => $form_state['values']['lat'].', '.$form_state['values']['lng']
  );
  
  db_update('inventory_entry')->fields($data)->condition('id', $entry_id)->execute();
  
  if($ajax)
    return;
  drupal_set_message(t('Changes successfully saved'));
  $form_state['redirect'] = 'inventory/'.$form_state['head_inventory_id'].'/edit';
}

/**
 * Menu callback; Deletes an inventory entry (called over AJAX)
 * 
 * @param object $head_inventory_id
 * @param object $entry_id
 * @param object $plain[optional]
 * @return 
 */
function inventory_delete_entry($head_inventory_id, $entry_id, $plain=false) {
  try {
    db_delete('inventory_entry')->condition('id', $entry_id)->execute();
    db_delete('inventory_type_attribute_inventory_entry')->condition('inventory_entry_id', $entry_id)->execute();
    $images = db_select('gallery_image', 'gi', array('fetch' => PDO::FETCH_ASSOC))
      ->fields('gi', array('id'));
    $images->join('file_managed', 'f', 'f.fid = gi.fid');
    $images->fields('f')
      ->condition('gi.item_type', 'inventory_entry')
      ->condition('gi.item_id', $entry_id);
    $images = $images->execute();
    foreach($images as $image)
      gallery_image_delete($image);
    _inventory_update_modify_time($head_inventory_id);
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
 * @param object $head_inventory_id
 * @param object $inventory_type_id
 * @param object $key
 */
function inventory_add_inventory_group($head_inventory_id, $inventory_type_id, $key) {
  $desc = inventory_column_description($inventory['id'], $inventory_type_id, true, $head_inventory_id);
  return drupal_json_output(array(
    'data' => drupal_render(_inventory_group_factory($head_inventory_id, $desc, 'new_'.$key))
  ));
}

/**
 * Menu callback; Edit form for custom fields of an inventory
 *  (wrapper around inventory_custom_fields_form)
 * 
 * @param object $head_inventory_id
 * @param object $inventory_type_id
 * @return Form to edit the custom fields of an inventory
 */
function inventory_custom_fields($head_inventory_id, $inventory_type_id) {
  return _inventory_form_ajax_wrapper('inventory_custom_fields_form', $head_inventory_id, $inventory_type_id);
}

/**
 * Edit form for custom fields of an inventory
 * 
 * @param object $form
 * @param object $form_state
 * @param object $head_inventory_id
 * @param object $inventory_type_id
 * @return Form to edit the custom fields of an inventory
 */
function inventory_custom_fields_form($form, &$form_state, $head_inventory_id, $inventory_type_id) {
  $ajax = $_REQUEST['ajax'];
  if($ajax && $form_state['storage']['result']) {
    $form['#result'] = $form_state['storage']['result'];
    return $form;
  }
  
  _include_js_and_css();
  $form_state['head_inventory_id'] = $head_inventory_id;
  $form_state['inventory_type_id'] = $inventory_type_id;
  $form_state['ajax'] = $ajax;
  
  $inventory_type = db_select('inventory_type', 't')
    ->condition('t.id', $inventory_type_id)
    ->fields('t', array('name'))
    ->execute()->fetch();
  drupal_set_title(t('Custom fields of %inventory', array('%inventory' => $inventory_type->name)), PASS_THROUGH);
  
  $result = db_select('attribute_format', 'f')
    ->condition('f.format', 'dropdown', '!=')
    ->fields('f')
    ->execute();
  $formats = array();
  foreach($result as $record)
    $formats[$record->id] = $record;
  
  $fields = db_select('inventory_type_attribute', 'ita')
    ->condition('ita.head_inventory_id', $head_inventory_id)
    ->condition('ita.inventory_type_id', $inventory_type_id)
    ->fields('ita')
    ->orderBy('ita.id')
    ->execute();
  $headings = array(
    t('Name'),
    t('Format'),
    t('Delete')
  );
  $rows = array();
  $i = 0;
  foreach($fields as $field) {
    array_push($rows, array(
      '<input type="text" name="fields['.$i.'][name]" value="'.$field->name.'" />'.
      '<input type="hidden" name="fields['.$i.'][id]" value="'.$field->id.'" />',
      $formats[$field->attribute_format_id]->name,
      '<input type="checkbox" name="fields['.$i++.'][delete]" value="1" />'
    ));
  }
  
  $options = '';
  foreach($formats as $value => $format)
    $options .= '<option value="'.$value.'">'.$format->name.'</option>';
  array_push($rows, array(
    '<input type="text" name="fields['.$i.'][name]" value="" />',
    '<select name="fields['.$i.'][attribute_format_id]" size="1">'.
      $options.
    '</select>',
    '<input type="checkbox" name="fields['.$i.'][delete]" value="1" disabled="disabled" />'
  ));
  
  $form['fields'] = array(
    '#theme' => 'table', 
    '#header' => $headings,
    '#rows' => $rows,
    '#attributes' => array(
      'id' => 'customfields'
    ),
  );
  
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );
  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), 'inventory/'.$head_inventory_id.'/edit')
  );
  
  return $form;
}

/**
 * Submit handler for inventory_custom_fields_form saving the custom fields
 * 
 * @param object $form
 * @param object $form_state
 * @return 
 */
function inventory_custom_fields_form_submit($form, &$form_state) {
  $ajax = $form_state['ajax'];
  $head_inventory_id = $form_state['head_inventory_id'];
  $inventory_type_id = $form_state['inventory_type_id'];
  
  _inventory_update_modify_time($head_inventory_id);
  
  $fields = $form_state['input']['fields'];
  $base = array(
    'head_inventory_id' => $head_inventory_id,
    'inventory_type_id' => $inventory_type_id
  );
  $id = db_select('inventory_type_attribute', 'ita')->orderBy('ita.id', 'DESC')->range(0, 1)->fields('ita', array('id'))->execute()->fetch();
  $id = $id->id+1;
  foreach($fields as $field) {
    if($field['delete']) {
      if($field['id'])
        db_delete('inventory_type_attribute')->condition('id', $field['id'])->execute();
      continue;
    }
    if(!trim($field['name']))
      continue;
    $field = array_merge($field, $base);
    if(isset($field['id']) && is_numeric($field['id']))
      db_update('inventory_type_attribute')->fields($field)->condition('id', $field['id'])->execute();
    else {
      $field['id'] = $id++;
      db_insert('inventory_type_attribute')->fields($field)->execute();
    }
  }
  if($ajax) {
    $form_state['storage']['result'] = array(
      'result' => '1',
      'message' => t('Fields successfully saved')
    );
    $form_state['rebuild'] = true;
    return;
  }
  drupal_set_message(t('Fields successfully saved'));
  $form_state['redirect'] = 'inventory/'.$head_inventory_id.'/custom_fields/'.$inventory_type_id;
}

/**
 * Menu callback; Form for adding organisms from a template
 *  (wrapper around inventory_templates_form)
 * 
 * @return 
 * @param object $head_inventory_id
 */
function inventory_templates($head_inventory_id) {
  return _inventory_form_ajax_wrapper('inventory_templates_form', $head_inventory_id);
}

/**
 * Form for adding organisms from a template
 * 
 * @return 
 * @param object $form
 * @param object $form_state
 * @param object $head_inventory_id
 */
function inventory_templates_form($form, &$form_state, $head_inventory_id) {
  $ajax = $_REQUEST['ajax'];
  if($ajax && $form_state['storage']['result']) {
    $form['#result'] = $form_state['storage']['result'];
    return $form;
  }
  drupal_set_title(t('Add organisms from template'));
  
  _include_js_and_css();
  $form_state['head_inventory_id'] = $head_inventory_id;
  $form_state['ajax'] = $ajax;
  
  $result = db_select('inventory_type', 'it')
    ->fields('it', array('id', 'name'))
    ->orderBy('it.id', 'DESC')
    ->execute();
  
  $options = array();
  foreach($result as $record)
    $options[$record->id] = $record->name;
  $form['types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Organism types to include'),
    '#options' => $options,
    '#required' => true,
    '#default_value' => array_keys($options)
  );
  
  $form['source'] = array(
    '#type' => 'radios',
    '#title' => t('Source'),
    '#required' => true,
    '#options' => array(
      'template' => t('Template'),
      'inventory' => t('Own and shared inventory'),
      'public_inventory' => t('Public inventory'),
      'habitat' => t('Habitat')
    ),
    '#default_value' => 'template'
  );
  
  $form['term'] = array(
    '#type' => 'textfield',
    '#attributes' => array(
      'class' => array('term')
    )
  );
  
  $form['id'] = array(
    '#type' => 'hidden',
    '#required' => true,
    '#attributes' => array(
      'class' => array('id')
    )
  );
  
  $form['new_groups'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create new inventory groups')
  );
  
  $form['head_inventory_id'] = array(
    '#type' => 'hidden',
    '#value' => $head_inventory_id
  );
  
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );
  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), 'inventory/'.$head_inventory_id.'/edit')
  );
  
  return $form;
}

/**
 * Submit handler of inventory_templates_form
 * 
 * @return 
 * @param object $form
 * @param object $form_state
 */
function inventory_templates_form_submit($form, &$form_state) {
  global $user;
  $ajax = $form_state['ajax'];
  $head_inventory_id = $form_state['head_inventory_id'];
  
  $inventories = is_array($_REQUEST['inventories']) ? $_REQUEST['inventories'] : array();
  $organisms = is_array($_REQUEST['organisms']) ? $_REQUEST['organisms'] : array();
  $new_groups = $form_state['values']['new_groups'];
  $id = $form_state['values']['id'];
  $source = $form_state['values']['source'];
  $types = $form_state['values']['types'];
  
  if($ajax) {
    $group_count = count($inventories);
    $new_entries = array();
    $new_inventories = array();
  } else {
    $iid = db_select('inventory', 'i')->orderBy('i.id', 'DESC')->range(0, 1)->fields('i', array('id'))->execute()->fetchColumn(0)+1;
    $eid = db_select('inventory_entry', 'e')->orderBy('e.id', 'DESC')->range(0, 1)->fields('e', array('id'))->execute()->fetchColumn(0)+1;
  }
  $seen = $new_groups ? array() : $organisms;
  $data = call_user_func_array('_inventory_template_data_'.$source, array($id, $types));
  $i = 1;
  foreach($data as $group) {
    $columns = $group['invDesc']['cols'];
    $identifier = false;
    $date = false;
    if($columns) {
      foreach($columns as $column) {
        if($column['name'] == 'Beobachter')
          $identifier = $column['id'];
        if($column['name'] == 'Funddatum')
          $date = $column['id'];
      }
    }
    $entries = array();
    foreach($group as $k => $entry) {
      if(is_string($k) && $k == 'invDesc')
        continue;
      if(in_array($entry['orgId'], $seen))
        continue;
      if($identifier)
        $entry['col_'.$identifier] = $user->name;
      if($date)
        $entry['col_'.$date] = date('j.m.Y', time());
      array_push($seen, $entry['orgId']);
      array_push($entries, $entry);
    }
    if(!$entries)
      continue;
    if($new_groups || !in_array($group['invDesc']['id'], $inventories)) {
      if($ajax)
        array_push($new_inventories, drupal_render(_inventory_group_factory($head_inventory_id, $group['invDesc'], 'new_'.$group_count++, $entries)));
      else {
        $inventory_id = $iid++;
        $record = array(
          'id' => $inventory_id,
          'head_inventory_id' => $head_inventory_id,
          'inventory_type_id' => $group['invDesc']['id']
        );
        db_insert('inventory')->fields($record)->execute();
      }
    } else {
      $inventory_id = array_search($group['invDesc']['id'], $inventories);
      if($ajax) {
        if(!array_key_exists($inventory_id, $new_entries))
          $new_entries[$inventory_id] = array();
        foreach($entries as $entry) {
          $row = _inventory_render_row(_inventory_row_factory($head_inventory_id, $entry, $i++, $inventory_id, $group['invDesc']['id'], $columns));
          array_push($new_entries[$inventory_id], $row);
        }
      }
    }
    if(!$ajax) {
      foreach($entries as $entry) {
        $record = array(
          'id' => $eid++,
          'organism_id' => $entry['orgId'],
          'inventory_id' => $inventory_id
        );
        db_insert('inventory_entry')->fields($record)->execute();
      }
    }
  }
  
  $success = count($new_entries) || count($new_inventories);
  switch($source) {
    case 'habitat':
      $source_name = t('habitat'); break;
    case 'template':
      $source_name = t('template'); break;
    default:
      $source_name = t('inventory');
  }
  $message = $success ? t('Organisms successfully imported into the inventory') : t('The selected !source did not contain any organisms', array('!source' => $source_name));
  if($ajax) {
    $form_state['storage']['result'] = array(
      'result' => $success ? '1' : '0',
      'message' => $message,
      'group_count' => $group_count,
      'new_inventories' => $new_inventories,
      'new_entries' => $new_entries,
      'inventories' => $inventories
    );
    $form_state['rebuild'] = true;
    return;
  }
  drupal_set_message($message);
  $form_state['redirect'] = 'inventory/'.$head_inventory_id.'/edit';
}

/**
 * Renders a row of a table
 *  (copied from includes/theme.inc[theme_table])
 * 
 * @return 
 * @param object $row
 */
function _inventory_render_row($row) {
  // Check if we're dealing with a simple or complex row
  if (isset($row['data'])) {
    foreach ($row as $key => $value) {
      if ($key == 'data') {
        $cells = $value;
      }
      else {
        $attributes[$key] = $value;
      }
    }
  }
  else {
    $cells = $row;
  }
  if (count($cells)) {
    // Add odd/even class
    if (empty($row['no_striping'])) {
      $class = $flip[$class];
      $attributes['class'][] = $class;
    }

    // Build row
    $output = ' <tr' . drupal_attributes($attributes) . '>';
    $i = 0;
    foreach ($cells as $cell) {
      $cell = tablesort_cell($cell, $header, $ts, $i++);
      $output .= _theme_table_cell($cell);
    }
    $output .= " </tr>\n";
  }
  return $output;
}

/**
 * Private helper function to get the organism data for templates
 *  (used by inventory_templates_form_submit)
 * 
 * @return 
 * @param object $template_id
 * @param object $types
 */
function _inventory_template_data_template($template_id, $types) {
  $query = db_select('inventory_template_inventory', 'i')
    ->condition('i.inventory_template_id', $template_id)
    ->condition('i.inventory_type_id', $types, 'IN');
  $query->fields('i', array('id', 'inventory_type_id'));
  $inventories = $query->execute();
  
  $data = array();
  foreach($inventories as $inventory) {
    $data[$inventory->id] = inventory_type_entries($inventory->id, 'inventory_template_inventory_entry', 'inventory_template_inventory_id', false, true, false, false, false, false);
    $data[$inventory->id]['invDesc'] = inventory_column_description(false, $inventory->inventory_type_id);
  }
  return $data;
}

/**
 * Private helper function to get the organism data for inventories
 *  (used by inventory_templates_form_submit)
 * 
 * @return 
 * @param object $head_inventory_id
 * @param object $types
 */
function _inventory_template_data_inventory($head_inventory_id, $types) {
  $query = db_select('inventory', 'i')
    ->condition('i.head_inventory_id', $head_inventory_id)
    ->condition('i.inventory_type_id', $types, 'IN');
  $query->fields('i', array('id', 'inventory_type_id'));
  $inventories = $query->execute();
 
  $protected = inventory_redlisted_entries($head_inventory_id);
  $data = array();
  foreach($inventories as $inventory) {
    $data[$inventory->id] = inventory_type_entries($inventory->id, 'inventory_entry', 'inventory_id', false, true, false, $protected, false, false);
    $data[$inventory->id]['invDesc'] = inventory_column_description(false, $inventory->inventory_type_id);
  }
  return $data;
}

/**
 * Private helper function to get the organism data for public inventories
 *  (used by inventory_templates_form_submit)
 * 
 * @return 
 * @param object $head_inventory_id
 * @param object $types
 */
function _inventory_template_data_public_inventory($head_inventory_id, $types) {
  $query = db_select('inventory', 'i')
    ->condition('i.head_inventory_id', $head_inventory_id)
    ->condition('i.inventory_type_id', $types, 'IN');
  $query->fields('i', array('id', 'inventory_type_id'));
  $inventories = $query->execute();
  
  $protected = inventory_redlisted_entries($head_inventory_id);
  $data = array();
  foreach($inventories as $inventory) {
    $data[$inventory->id] = inventory_type_entries($inventory->id, 'inventory_entry', 'inventory_id', false, true, false, $protected, false, false);
    $data[$inventory->id]['invDesc'] = inventory_column_description(false, $inventory->inventory_type_id);
  }
  return $data;
}

/**
 * Private helper function to get the organism data for habitats
 *  (used by inventory_templates_form_submit)
 * 
 * @return 
 * @param object $habitat_id
 * @param object $types
 */
function _inventory_template_data_habitat($habitat_id, $types) {
  $sql = 'SELECT 
      oh.organism_id, 
      fauna.name_de faunanamede, 
      fauna.genus, 
      fauna.species, 
      fauna.sub_species, 
      fauna.family, 
      o.organism_type, 
      flora.name_de floranamede, 
      flora.name floraname, 
      flora."Familie" florafamilie, 
      flora."Gattung" floragattung, 
      flora."Art" floraart, 
      flora."Rang" florarang, 
      flora."NameUnterart" floraunterart,
      i.inventory_type_id
    FROM organism_habitat oh
    INNER JOIN inventory_type_organism i ON i.organism_id = oh.organism_id
    INNER JOIN organism o ON oh.organism_id = o.id
    LEFT JOIN fauna_organism fauna ON o.organism_id = fauna.id
    LEFT JOIN flora_organism flora ON o.organism_id = flora.id
    WHERE oh.habitat_id = :id AND i.inventory_type_id IN ('.implode(', ', $types).')
    ORDER BY oh.organism_id;';
  $organisms = db_query($sql, array(':id' => $habitat_id));
  
  $data = array();
  foreach($organisms as $organism) {
    if(!array_key_exists($organism->inventory_type_id, $data))
      $data[$organism->inventory_type_id] = array(
        'invDesc' => inventory_column_description(false, $organism->inventory_type_id)
      );
    array_push($data[$organism->inventory_type_id], inventory_entry_factory($organism, true));
  }
  return $data;
}

/**
 * Menu callback; AJAX autocomplete for selecting templates
 * 
 * @return 
 * @param object $head_inventory_id
 */
function inventory_template_autocomplete($head_inventory_id) {
  global $user;
  $term = '%'.trim($_GET['term']).'%';
  
  $query = db_select('inventory_template', 't', array('fetch' => PDO::FETCH_ASSOC))
    ->condition(db_or()
      ->condition('t.owner_id', $user->uid)
      ->condition('t.public', '1'))
    ->condition('t.name', $term, 'ILIKE')
    ->fields('t', array('id', 'name'))
    ->range(0, 40);
  $query->join('users', 'u', 'u.uid = t.owner_id');
  $query->addField('u', 'name', 'owner');
  
  $results = $query->execute()->fetchAll();
  
  return drupal_json_output($results);
}

/**
 * Menu callback; AJAX autocomplete for selecting inventories
 * 
 * @return 
 * @param object $head_inventory_id
 */
function inventory_inventory_autocomplete($head_inventory_id) {
  global $user;
  $term = '%'.trim($_GET['term']).'%';
  $query = db_query('
  SELECT i.id AS id, i.name AS name, u.name AS owner
	FROM 
	head_inventory i
	INNER JOIN users u ON u.uid = i.owner_id
	LEFT OUTER JOIN sgroup_inventory sgi ON sgi.hiid = i.id
	LEFT OUTER JOIN sgroup_users sgu ON sgu.sgid = sgi.sgid AND sgu.uid = 1
	WHERE  (i.name ILIKE :term) AND (i.hidden IS NOT TRUE) AND( (sgi.write = 1) OR (i.owner_id = :uid) )
	GROUP BY i.id, i.name, u.name
	LIMIT 40 OFFSET 0', array(
	    ':term' => $term,
	    ':uid' => $user->uid,
	));
  $results = $query->fetchAll();
  
  return drupal_json_output($results);
}

/**
 * Menu callback; AJAX autocomplete for selecting public inventories
 * 
 * @return 
 * @param object $head_inventory_id
 */
function inventory_public_inventory_autocomplete($head_inventory_id) {
  global $user;
  $term = '%'.trim($_GET['term']).'%'; 
  $query = db_query('
  SELECT i.id AS id, i.name AS name, u.name AS owner
  FROM 
  head_inventory i
  INNER JOIN users u ON u.uid = i.owner_id
  WHERE  (i.name ILIKE :term) AND (i.shared = TRUE) AND (i.hidden IS NOT TRUE) 
  LIMIT 40 OFFSET 0', array(
    ':term' => $term,
  ));
  $results = $query->fetchAll();
  
  return drupal_json_output($results);
}

/**
 * Menu callback; AJAX autocomplete for selecting habitats
 * 
 * @return 
 * @param object $head_inventory_id
 */
function inventory_habitat_autocomplete($head_inventory_id) {
  global $user;
  $term = '%'.trim($_GET['term']).'%';
  
  $query = db_select('habitat', 'h', array('fetch' => PDO::FETCH_ASSOC))
    ->condition(db_or()
      ->condition('h.label', $term, 'ILIKE')
      ->condition('h.name_de', $term, 'ILIKE')
      ->condition('h.name_lt', $term, 'ILIKE'))
    ->fields('h', array('id'))
    ->range(0, 40);
  $query->addField('h', 'name_de', 'name');
  $query->join('area_habitat', 'ah', 'ah.habitat_id = h.id');
  $query->join('head_inventory', 'i', 'i.area_id = ah.area_id');
  $query->condition('i.id', $head_inventory_id);

  return drupal_json_output($query->execute()->fetchAll());
}

/**
 * Private helper function to render a form where the form may
 * define the result by setting the '#result' attribute in the $form
 * array
 * 
 * @return 
 * @param object $form_id
 */
function _inventory_form_ajax_wrapper($form_id) {
  $ajax = $_REQUEST['ajax'];
  
  $form = call_user_func_array('drupal_get_form', func_get_args());
  
  if($ajax) {
    if(array_key_exists('#result', $form))
      return drupal_json_output($form['#result']);
    return drupal_json_output(array(
      'form' => drupal_render($form)
    ));
  }
  return $form;
}

/**
 * Menu callback; Gets the image of an organism (if available)
 * 
 * @param object $organism_id
 * @param object $plain[optional]
 * @return 
 */
function inventory_get_entry_image($organism_id, $plain=false) {
  $result = array(
    'data' => '',
    'class' => 'image'
  );
  if($organism_id && function_exists('gallery_images')) {
    $images = gallery_images('organism', $organism_id, true, false, 1);
    if($images['count']) {
      $result['data'] = 
        '<a href="'.url('gallery/organism/'.$organism_id.'/thumb/'.$images['images'][0]['id'].'/gallery_large').'">'.
          '<img src="'.url('gallery/organism/'.$organism_id.'/thumb/'.$images['images'][0]['id'].'/inventory_mini').'" />'.
        '</a>';
    }
  }
  if(!$plain)
    return drupal_json_output($result);
  return $result;
}

/**
 * Private helper function to update the modification date of an inventory
 * 
 * @param object $head_inventory_id
 * @return 
 */
function _inventory_update_modify_time($head_inventory_id) {
  db_update('head_inventory')
  ->fields(array('modify_time'=>'now()'))
  ->condition('id', $head_inventory_id)
  ->execute();
}

/**
 * Private helper function to create an inventory group
 * 
 * @param int $id
 *  The ID of the inventory
 * @param array $desc
 *  Description of the columns
 * @param int $inventory_id
 *  The ID of the inventory group
 * @param array $entries[optional]
 *  The rows to display
 * @param int $weight[optional]
 *  The weight for the resulting output
 * @param boolean $additional[optional]
 *  Whether to add additional fields or not
 * @return array
 *  Drupal render array
 */
function _inventory_group_factory($id, $desc, $inventory_id, $entries=array(), $weight=1, $additional=true) {
  global $user;
  $output = array();
  $headings = array();
  $invGroupName = $desc['name'];
  if(function_exists('gallery_images'))
    $headings[] = array(
      'data' => '<span></span>',
      'class' => array('image')
    );
  $headings[] = array(
    'data' => t('Name').
      '<input class="inventory" type="hidden" name="inventories['.$inventory_id.']" value="'.$desc['id'].'" />'
  );
  $headings[] = array(
    'data' => t('Latin name'),
    'class' => array('latin_name')
  );
  $last_date = false;
  $last_identifier = false;
  $columns = $desc['cols'];
  if($columns) {
    foreach($columns as $column) {
      if($column['visible'])
        $headings[] = array(
          'data' => $column['name'],
          'class' => array(($column['name'] == 'Beobachter' ? 'identifier' : ''))
        );
      if($column['name'] == 'Beobachter')
        $identifier = $column['id'];
      if($column['name'] == 'Funddatum')
        $date = $column['id'];
    }
  }
  array_push($headings[count($headings)-1]['class'], 'last');
  if($additional) {
    $headings[] = '&nbsp;';
    $headings[] = '&nbsp;';
    $headings[] = '&nbsp;';
  }
  $headings[] = t('Delete');
  $rows = array();
  $orgs = array();
  $k = 0;
  foreach($entries as $key => $entry) {
    if(is_string($key) && $key == 'invDesc')
      continue;
    if(!in_array($entry['orgId'], $orgs))
      array_push($orgs, $entry['orgId']);
    $last_date = $entry['col_'.$date];
    $last_identifier = $entry['col_'.$identifier];
    $row = _inventory_row_factory($id, $entry, $k++, $inventory_id, $desc['id'], $columns, $additional);
    array_push($rows, $row);
  }
  if(!$entries || $rows) {
    $output['title'.$inventory_id] = array(
      '#markup' => 
        '<h3 class="invTitle" id="invTitle'.$inventory_id.'">'.$invGroupName.' <small>('.count($rows).'/'.count($orgs).')</small> '.
          ($additional ? 
          '<a class="custom" href="'.url('inventory/'.$id.'/custom_fields/'.$desc['id']).'" title="'.t('Custom fields').'">'.
            '<img src="'.url(drupal_get_path('module', 'inventory').'/images/custom_fields.gif').'" alt="'.t('Custom fields').'" />'.
          '</a>' : '').
        '</h3>',
      '#weight' => $weight++
    );
    if($columns)
      $entry = array(
        'col_'.$identifier => $last_identifier ? $last_identifier : $user->name,
        'col_'.$date => $last_date ? $last_date : date('d.m.Y', time())
      );
    else
      $entry = array();
    array_push($rows, _inventory_row_factory($id, $entry, $k++, $inventory_id, $desc['id'], $columns, $additional));
    $output['table'.$inventory_id] = array(
      '#theme' => 'table', 
      '#header' => $headings,
      '#rows' => $rows,
      '#weight' => $weight,
      '#sticky' => false,
      '#attributes' => array(
        'id' => 'invTable'.$inventory_id,
        'class' => array('invTable')
      ),
    );
  }
  return $output;
}

/**
 * Private helper function to create a row of an inventory group table
 * 
 * @param int $id
 *  The ID of the inventory
 * @param array $data
 *  The data of the inventory entry
 * @param int $i
 *  Incrementing integer to identify the row
 * @param mixed $key
 *  The key for this entry (usually the ID of the inventory group)
 * @param int $inventory_type_id
 *  The ID of the inventory type
 * @param array $columns
 *  The description of the columns
 * @param boolean $additional[optional]
 *  Whether to add additional fields or not
 * @return array
 *  A row array to be used in a table
 */
function _inventory_row_factory($id, $data, $i, $key, $inventory_type_id, $columns, $additional=true) {
  $row = array();
  if(function_exists('gallery_images'))
    $row[] = inventory_get_entry_image(@$data['orgId'], true);
  $row[] = array(
    'data' => 
      '<input type="hidden" class="organism_id" name="entries['.$key.']['.$i.'][organism_id]" value="'.$data['orgId'].'" />'.
      '<input type="hidden" class="entry_id" name="entries['.$key.']['.$i.'][id]" value="'.$data['id'].'" />'.
      '<input type="text" class="organism" name="entries['.$key.']['.$i.'][organism]" value="'.($data['label'] ? $data['label'] : $data['label_latin_short']).'" />'
  );
  $row[] = array(
    'data' => '<em>'.$data['label_latin'].'</em>',
    'class' => array()
  );
  $hidden = '';
  if($columns) {
    $n = 0;
    foreach($columns as $column) {
      $n++;
      $name = 'col_'.$column['id'];
      $value = @$data[$name];
      if($column['visible']) {
        if($column['format'] == 'dropdown') {
          $cell = '<select name="entries['.$key.']['.$i.']['.$name.']" size="1">';
          $cell .= '<option value="0"></option>';
          foreach($column['dropdown_values'] as $v)
            $cell .= '<option value="'.$v['id'].'_dropdown"'.($v['id'].'_dropdown' == $value ? ' selected="selected"' : '').'>'.$v['value'].'</option>';
          $cell .= '</select>';
        } else
          $cell = '<input type="text" name="entries['.$key.']['.$i.']['.$name.']" value="'.$value.'" class="'.$column['format'].($column['name'] == 'Beobachter' ? ' identifier' : '').'" />';
        $row[] = array(
          'data' => $cell,
          'class' => array()
        );
      } else
        $hidden .= '<input type="hidden" name="entries['.$key.']['.$i.']['.$name.']" value="'.$value.'" />';
    }
  }
  array_push($row[count($row)-1]['class'], 'last');
  if($additional) {
    $row[] = array(
    	'data' =>'<a class="additional" href="'.url('inventory/'.$id.'/edit_entry/'.$inventory_type_id.'/'.$data['id']).'" title="'.t('Additional fields').'">
                 <img src="'.url(drupal_get_path('module', 'inventory').'/images/additional_fields.gif').'" alt="'.t('Additional fields').'" />
                 </a>',
         'class' => 'icon');
    $row[] = array(
      'data' => '<a class="location" href="'.url('inventory/'.$id.'/edit_entry_location/'.$data['id']).'" title="'.t('Location').'">
                 <img src="'.url(drupal_get_path('module', 'inventory').'/images/location'.($data['location'] ? '' : '_unset').'.gif').'" alt="'.t('Location').'" />
                 </a>',
       'class' => 'icon');
    $row[] = array(
    	'data' =>'<a class="images" href="'.url('gallery/inventory_entry/'.$data['id'].'/medias/edit').'" title="'.t('Medias').'">
                 <img src="'.url(drupal_get_path('module', 'inventory').'/images/images.gif').'" alt="'.t('Medias').'" />
                 </a>',
        'class' => 'icon');
      
  }
  $row[] = array(
    'data' => '<input type="checkbox" class="delete" name="entries['.$key.']['.$i.'][delete]" value="1" />'.$hidden,
  	'class' => 'icon'
  );
  return $row;
}

/**
 * Form to upload and comment new files
 * 
 * @return form to upload files
 */
function inventory_edit_files($form, &$form_state, $head_inventory_id) {
  
  $tableHeader[] = t('Description');
  $tableHeader[] = t('Filename');
  $tableHeader[] = t('Typ');
  $tableHeader[] = t('Size');
  $tableHeader[] = t('Date');
  $tableHeader[] = '&nbsp;';

  $result = db_query('SELECT fm.filename, fm.filemime, fm.timestamp, fm.fid, fm.filesize, hifm.description
            FROM head_inventory_file_managed hifm, file_managed fm
            WHERE fm.fid = hifm.file_id
            AND hifm.head_inventory_id = :id;', array(':id' => $head_inventory_id)
  );
  $img_delete = '<img src="'.url(drupal_get_path('module', 'inventory').'/images/can_delete.png').'" alt="'.t('Delete').'" />';
  
  if(!empty($result)) {
    foreach ($result as $record) {
      $description = $record->description;
      $filename = l($record->filename, 'sites/default/files/swissmon/inventory/'.$head_inventory_id.'/'.$record->filename);
      $filemime = '<img class="file-icon" alt="" title="' . $record->filemime . '" src="../../modules/inventory/images/' . inventory_icon_map($record->filemime) . '.png" />';
      $filesize = ($record->filesize / 1000)." KB";
      $timestamp = date('d.m.Y',$record->timestamp);
      $row = array();
      $row[] = $description;
      $row[] = $filename;
      $row[] = $filemime;
      $row[] = $filesize;
      $row[] = $timestamp;
      $row[] = l($img_delete, 'inventory/'.$head_inventory_id.'/delete_file/'.$record->fid, array('html' => true));
      $rows[] = $row;
    }

    if(isset($rows)) {
      $form['file_overview'] = array(
        '#theme' => 'table', 
        '#header' => $tableHeader,
        '#rows' => $rows,
      );
    }
  }

  $form['upload'] = array(
    '#type' => 'file', 
    '#title' => t('Upload File'), 
    '#size' => 48,
  );

  $form['file_description'] = array(
    '#type' => 'textfield', 
    '#title' => t('Description of File'), 
    '#size' => 100, 
    '#maxlength' => 128, 
    '#required' => FALSE,
  );

  $form['upload_submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Upload'),
    '#submit' => array('inventory_update_file'),
  );
  
  // Saving something to the form_state makes it accessible in the submit function.
  // We do this to make sure the sgid is taken from the url which has passed security checkpoints.
  $form_state['storage']['hi_id'] = $head_inventory_id;
  
  return $form;
}

/**
 * Form to edit inventory permissions
 * 
 * @return form to edit inventory rights
 */
function inventory_edit_rights($form, &$form_state, $head_inventory_id) {
  global $user;
  
  // Shared flag
  $result = db_query(
    'SELECT shared FROM head_inventory hi
    WHERE hi.id = :id;', array('id' => $head_inventory_id)
  );
  
  if(!empty($result)) {
    foreach ($result as $record) {
      $shared = $record->shared;
      $form['share'] = array (
        '#type' => 'checkbox', 
        '#title' => t('Allow read access to all users'),
        '#value' => $shared,
        '#checked' => $shared,
      );
    }
  }
  
  // Export selection
  $options = inventory_export_types();
  if($options) {
    $export = db_select('head_inventory_export_type', 'hiet')
      ->condition('hiet.head_inventory_id', $head_inventory_id)
      ->fields('hiet', array('export_type_id'))
      ->execute()
      ->fetchCol(0);
    $form['export'] = array (
      '#type' => 'checkboxes', 
      '#title' => t('Allow export to the following platforms'),
      '#options' => $options,
      '#default_value' => $export,
    );
  }
  
  $tableHeader[] = '&nbsp;';
  $tableHeader[] = t('Full Access');
  $tableHeader[] = t('Show Protected Organisms');
  $tableHeader[] = t('Read Access');
  $tableHeader[] = t('Remove');
 
  $result = db_query('
    SELECT DISTINCT sg.sgid, sg.sgroup_name, sgi.read, sgi.write, sgi.show_red, 
      (EXISTS (SELECT sgu.uid FROM sgroup_users sgu WHERE sgu.sgid = sg.sgid AND sgu.uid = :uid)) member 
    FROM sgroup sg
      LEFT JOIN sgroup_inventory sgi ON sgi.sgid = sg.sgid AND sgi.hiid = :hiid 
      LEFT JOIN sgroup_users sgu ON sgu.sgid = sg.sgid
    WHERE
      EXISTS (SELECT sgu.uid FROM sgroup_users sgu WHERE sgu.sgid = sg.sgid AND sgu.uid = :uid) OR
      sgi.read IS NOT NULL',
    array(':uid' => $user->uid, ':hiid' => $head_inventory_id)
  );
  
  $rows = array();
  $additional = array();
  $additional[] = array(
    array(
      'data' => t('Other Groups'),
      'colspan' => 5,
      'header' => true
    )
  );
  
  $cnt = 0;
  foreach ($result as $record) {
    // Saveing something to the form_state makes it accessible in the submit function.
    // We do this to make sure the sgid is taken from the url which has passed security checkpoints.
    $form_state['storage']['sg_id'.$cnt] = $record->sgid;
    
    $name = $record->sgroup_name;
    $read = $record->read;
    $write = $record->write;
    $show_red = $record->show_red;
    $form_state['storage']['set_db'.$cnt] = false;
    if($record->read === null) 
      $form_state['storage']['set_db'.$cnt] = true;  // no db entry yet -> set in submit fct
    $form['read'.$cnt] = array(
      '#type' => 'checkbox',
      '#value' => $read,
      '#checked' => $read,
      '#name' => 'read'.$cnt,
    );
    $form['write'.$cnt] = array(
      '#type' => 'checkbox',
      '#value' => $write,
      '#checked' => $write,
      '#name' => 'write'.$cnt,
    );
    $form['show_red'.$cnt] = array(
      '#type' => 'checkbox',
      '#value' => $show_red,
      '#checked' => $show_red,
      '#name' => 'show_red'.$cnt,
    );
    $row = array();
    $row[] = $name;
    $row[] = array('data' => drupal_render($form['write'.$cnt]));
    $row[] = array('data' => drupal_render($form['read'.$cnt]));
    $row[] = array('data' => drupal_render($form['show_red'.$cnt]));
    if($record->member) {
      $row[] = '&nbsp;';
      $rows[] = $row;
    } else {
      $row[] = array('data' => '<div id="'.$head_inventory_id.'/'.$form_state['storage']['sg_id'.$cnt].'"></div>', 'class' => 'delete');
      $additional[] = $row;
    }
    $cnt++;
  }
  
  if(!empty($rows))
    array_unshift($rows, array(array(
      'data' => t('Groups I am Member of'),
      'colspan' => 5,
      'header' => true
    )));
  array_splice($rows, count($rows), 0, $additional);
  
  $form['right_overview_other'] = array(
    '#theme' => 'table', 
    '#header' => $tableHeader,
    '#rows' => $rows,
    '#attributes' => array('id' => 'right_overview_other'),
  );
  
  $form['sgrouptext'] = array(
    '#title' => t('Search through existing Groups'),
    '#type' => 'textfield',
    '#autocomplete_path' => 'inventory/'.$head_inventory_id.'/sgroup_autocomplete',
  );
  
  $form['upload_submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Save'),
    '#submit' => array('inventory_update_rights'),
  );
  
  // Saving something to the form_state makes it accessible in the submit function.
  // We do this to make sure the sgid is taken from the url which has passed security checkpoints.
  $form_state['storage']['hi_id'] = $head_inventory_id;
  $form_state['storage']['sg_cnt'] = $cnt;
  
  return $form;
}

/**
 * Update Basic information of Inventory. Corresponding form in inventory.inc
 * 
 * @param array $form
 * @param array $form_state
 * @return Instruction to redirect on initial page
 */
function inventory_update_basics($form, &$form_state) {

  $head_inventory_id = $form_state['storage']['hi_id'];
  $name = $form_state['input']['name'];
  $description = $form_state['input']['description'];

  $affectedRows = db_update('head_inventory')
  ->condition('id', $head_inventory_id)
  ->fields(
    array(
      'name' => $name,
      'description' => $description['value']
    )
  )
  ->execute();
  
  // check success
  if ($affectedRows == 1) {
    // show success message
    drupal_set_message(t('Inventory @name was successfully updated', array('@name'=> $name)));
  } else {
    drupal_set_message(t('Inventory could not be updated!'), 'error');
  }

  return $form_state['redirect'] = 'inventory/'.$head_inventory_id.'/edit';
}

/**
 * Submit function to upload files
 * 
 * @param unknown_type $form
 * @param unknown_type $form_state
 * @return Instruction to redirect on initial page
 */
function inventory_update_file($form, &$form_state){

  $head_inventory_id = $form_state['storage']['hi_id'];

  // File upload
  $validators = array(
    'file_validate_extensions' => array('doc txt pdf docx'),
    'file_validate_size' => array(1000000, 0),
  );

  $destination = 'public://swissmon/inventory/'.$head_inventory_id;

  file_prepare_directory($destination, FILE_CREATE_DIRECTORY);

  $file = file_save_upload('upload', $validators, $destination);
  if ($file) {

    $file->status |= FILE_STATUS_PERMANENT;
    $file = file_save($file);

    $form_state['values']['upload'] = $file;

    $nid = db_insert('head_inventory_file_managed')->fields(array(
        'head_inventory_id' => $head_inventory_id,
        'file_id' => $file->fid,
        'description' => $form_state['values']['file_description']
      ))
      ->execute();
    drupal_set_message(t('File saved'));
  }
  else {
    drupal_set_message(t('File could not be saved!'), 'error');
  }

  return $form_state['redirect'] = 'inventory/'.$head_inventory_id.'/edit';
}

function inventory_delete_file($form, &$form_state, $head_inventory_id, $file_id) {
  $query = db_select('head_inventory_file_managed', 'hifm')
    ->condition('hifm.head_inventory_id', $head_inventory_id)
    ->condition('hifm.file_id', $file_id)
    ->fields('hifm', array('description'));
  $query->join('file_managed', 'fm', 'fm.fid = hifm.file_id');
  $query->fields('fm', array('filename'));
  $file = $query->execute()->fetch();
  
  $form_state['head_inventory_id'] = $head_inventory_id;
  $form_state['file_id'] = $file_id;
  
  $name = $file->description ? $file->description : $file->filename;
  return confirm_form(
    $form,
    t('Delete %name?', array('%name' => $name)),
    'inventory/' . $head_inventory_id . '/edit',
    t('Are you sure you want to delete the file %name?', array('%name' => $name)),
    t('Delete'),  t('Cancel')
  );
}

function inventory_delete_file_submit($form, &$form_state) {
  $head_inventory_id = $form_state['head_inventory_id'];
  $file_id = $form_state['file_id'];
  
  $file = file_load($file_id);
  file_delete($file);
  db_delete('head_inventory_file_managed')
    ->condition('head_inventory_id', $head_inventory_id)
    ->condition('file_id', $file_id)
    ->execute();
  drupal_set_message(t('The file was successfully deleted'));
  $form_state['redirect'] = 'inventory/' . $head_inventory_id . '/edit';
}

/**
 * Submit function to modify inventory permissions
 * 
 * @param unknown_type $form
 * @param unknown_type $form_state
 * @return Instruction to redirect on initial page
 */
function inventory_update_rights($form, &$form_state) {
  $head_inventory_id = $form_state['storage']['hi_id'];
  $check = false;
  
  $seen = array();
  // update own group permissions
  for($i=0; $i<$form_state['storage']['sg_cnt']; $i++) {
    $sgid = $form_state['storage']['sg_id'.$i];
    if(in_array($sgid, $seen))
      continue;
    array_push($seen, $sgid);
    $read = '0';
    $write = '0';
    $show_red = '0';
    if(isset($form_state['input']['write'.$i]) && $form_state['input']['write'.$i]) {
      $write = '1';
      $show_red = '1';
      $read = '1';
    } else if(isset($form_state['input']['show_red'.$i]) && $form_state['input']['show_red'.$i]) {
      $show_red = '1';
      $read = '1';
    } else if(isset($form_state['input']['read'.$i]) && $form_state['input']['read'.$i])
      $read = '1';
    if($form_state['storage']['set_db'.$i]) {
      // entry missing in db
      $nid = db_insert('sgroup_inventory')->fields (
          array (
            'sgid' => $sgid,
            'hiid' => $head_inventory_id,
            'read' => $read,
            'write' => $write,
            'show_red' => $show_red,
          )
        )
        ->execute();
      $check = true;
    }
    else {
      // entry already in db
      $affectedRows = db_update('sgroup_inventory')
        ->fields (
          array (
            'read' => $read,
            'write' => $write,
            'show_red' => $show_red,
          )
        )
        ->condition(db_and()->condition('sgid', $sgid, '=')->condition('hiid', $head_inventory_id, '='))
        ->execute();
      
      if($affectedRows == 1) $check = true;
      else $check = false;
    }

  }

  // insert foreign group permissions
  $i = 0;
  while(isset($form_state['input']['sgroup_id_'.$i])) {
    $sgid = $form_state['input']['sgroup_id_'.$i];
    if(in_array($sgid, $seen)) {
      $i++;
      continue;
    }
    array_push($seen, $sgid);
    $read = '0';
    $write = '0';
    $show_red = '0';
    if(isset($form_state['input']['write_new_'.$i]) && $form_state['input']['write_new_'.$i]) {
      $write = '1';
      $show_red = '1';
      $read = '1';
    } else if(isset($form_state['input']['show_red_new_'.$i]) && $form_state['input']['show_red_new_'.$i]) {
      $show_red = '1';
      $read = '1';
    } else if(isset($form_state['input']['read_new_'.$i]) && $form_state['input']['read_new_'.$i])
      $read = '1';
    if(isset($form_state['input']['read_new_'.$i]) && $form_state['input']['read_new_'.$i]) $read = '1';
    if(isset($form_state['input']['show_red_new_'.$i]) && $form_state['input']['show_red_new_'.$i]) {
      $show_red = '1';
      $read = '1';
    }
    if(isset($form_state['input']['write'.$i]) && $form_state['input']['write'.$i]) {
      $write = '1';
      $show_red = '1';
      $read = '1';
    }
    $nid = db_insert('sgroup_inventory')->fields (
        array (
          'sgid' => $sgid,
          'hiid' => $head_inventory_id,
          'read' => $read,
          'write' => $write,
          'show_red' => $show_red,
        )
      )
      ->execute();
    $check = true;
    $i++;
  }
  
  // update shared flag
  $shared = $form_state['input']['share'] ? '1' : '0';
  $affectedRows = db_update('head_inventory')
    ->condition('id', $head_inventory_id)
    ->fields(
      array('shared' => $shared)
    )
    ->execute();
  
  // update export type
  inventory_save_export_type($head_inventory_id, $form_state['input']['export']);
  
  if($affectedRows == 1) $check = true;
  else $check = false;
  
  // check success
  if ($check) {
    // show success message
    drupal_set_message(t('Inventory permissions were successfully updated'));
  } else {
    drupal_set_message(t('Inventory permissions could not be updated!'), 'error');
  }
  
  return $form_state['redirect'] = 'inventory/'.$head_inventory_id.'/edit';
}

function inventory_sgroup_autocomplete($string) {
  global $user;
  
  $result = db_query('
    SELECT sg.sgid sg_id, sgroup_name FROM sgroup sg
    LEFT JOIN sgroup_users sgu ON sgu.sgid = sg.sgid
    WHERE sg.sgid NOT IN (SELECT sguEx.sgid FROM sgroup_users sguEx WHERE sguEx.uid = :uid)
    AND sgroup_name ILIKE :term
    ORDER BY sgroup_name
    ', array(':uid' => $user->uid, ':term' => '%'.$string.'%')
  );

  $items = array();
  foreach ($result as $record) {
    $value = $record->sgroup_name;
    $key = $record->sg_id."|".$record->sgroup_name;
    $items[$key]=check_plain($value);
  }

  return drupal_json_output($items);
}

/**
 * ajax request to delete a member from a group.
 * security is done by drupal (hook_menu, access callback)
 * 
 * @param unknown_type $headInventoryId
 * @param unknown_type $sgid
 */
function inventory_delete_sgroup($headInventoryId, $sgid) {
  
  db_delete('sgroup_inventory')
  ->condition('sgid', $sgid)
  ->condition('hiid', $headInventoryId)
  ->execute();

  return drupal_json_output('success');
}

?>