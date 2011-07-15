<?php

module_load_include('inc', 'inventory', 'inventory.edit');
module_load_include('inc', 'inventory', 'inventory.show');

/**
 * @see inventory_show
 */
function inventory_single_current_show() {
  global $user;
  return inventory_single_show($user);
}

/**
 * @see inventory_edit
 */
function inventory_single_current_edit() {
  global $user;
  return inventory_single_edit($user);
}

/**
 * @see inventory_delete_entry
 */
function inventory_single_current_delete_entry($entry_id, $plain=false) {
  global $user;
  return inventory_single_delete_entry($user, $entry_id, $plain);
}

/**
 * @see inventory_save_entries
 */
function inventory_single_current_save_entries() {
  global $user;
  return inventory_single_save_entries($user);
}

/**
 * @see inventory_add_inventory_group
 */
function inventory_single_current_add_inventory_group($inventory_type_id, $key) {
  global $user;
  return inventory_single_add_inventory_group($user, $inventory_type_id, $key);
}

/**
 * @see inventory_gallery
 */
function inventory_single_current_gallery() {
  global $user;
  return inventory_single_gallery($user);
}

/**
 * @see inventory_list
 */
function inventory_single_current_list() {
  global $user;
  return inventory_single_list($user);
}

/**
 * @see inventory_show
 */
function inventory_single_show($user) {
  $head_inventory_id = _inventory_single_get_id($user);
  $render_array = inventory_show($head_inventory_id, false);
  unset($render_array['details']);
  unset($render_array['area']);
  $render_array['map']['#attributes']['id'] = 'singleobservations-map';
  drupal_set_title(t('Single observations by %user', array('%user' => $user->name)), PASS_THROUGH);
  return $render_array;
}

/**
 * @see inventory_edit
 */
function inventory_single_edit($user) {
  $head_inventory_id = _inventory_single_get_id($user);
  $render_array = inventory_edit($head_inventory_id, false);
  unset($render_array['basics']);
  drupal_set_title(t('Edit single observations by %user', array('%user' => $user->name)), PASS_THROUGH);
  return $render_array;
}

/**
 * @see inventory_delete_entry
 */
function inventory_single_delete_entry($user, $entry_id, $plain=false) {
  return inventory_delete_entry(_inventory_single_get_id($user), $entry_id, $plain);
}

/**
 * @see inventory_save_entries
 */
function inventory_single_save_entries($user) {
  return inventory_save_entries(_inventory_single_get_id($user));
}

/**
 * @see inventory_add_inventory_group
 */
function inventory_single_add_inventory_group($user, $inventory_type_id, $key) {
  return inventory_add_inventory_group(_inventory_single_get_id($user), $inventory_type_id, $key);
}

/**
 * @see inventory_gallery
 */
function inventory_single_gallery($user) {
  $render_array = inventory_gallery(_inventory_single_get_id($user));
  drupal_set_title(t('Single observations by %user', array('%user' => $user->name)), PASS_THROUGH);
  return $render_array;
}

/**
 * @see inventory_list
 */
function inventory_single_list($user) {
  $render_array = inventory_list(_inventory_single_get_id($user));
  drupal_set_title(t('Single observations by %user', array('%user' => $user->name)), PASS_THROUGH);
  return $render_array;
}

?>