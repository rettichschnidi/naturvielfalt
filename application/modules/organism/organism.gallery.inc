<?php
/**
 * Hook implementations for gallery integration
 */

/**
 * Implements hook of gallery module to activate the gallery
 * functionality for organism
 * 
 * @return array
 */
function organism_gallery_types() {
  return array(
    'organism' => array(
      'image_caption' => 'organism_gallery_image_caption',
      'manage_title' => 'organism_gallery_manage_title',
      'manage_description' => 'organism_gallery_manage_description'
    ),
    'inventory_type' => array(
      'manage_title' => 'organism_type_gallery_manage_title',
      'item_provider' => 'organism_type_gallery_item_provider',
      'item_provider_autocomplete' => 'organism_type_gallery_autocomplete'
    )
  );
}

/**
 * Helper function referenced in organism_gallery_types to define the title of 
 * the manage images page for inventory types
 * 
 * @param string $id
 *   The id of the object
 * @return string
 */
function organism_type_gallery_manage_title($id) {
  $result = db_select('inventory_type', 't')
    ->condition('id', $id)
    ->fields('t', array('name'))
    ->execute()
    ->fetch();
  return t('Images of the organism group !group', array('!group' => '<em>'.$result->name.'</em>'));
}

/**
 * Helper function referenced in organism_gallery_types to get the name 
 * of a selected item type and id previously defined by 
 * organism_type_gallery_autocomplete
 * 
 */
function organism_type_gallery_item_provider($type, $id) {
  if($type != 'organism' || !$id)
    return '';
  $result = db_query('SELECT '.
                      'o.organism_type, '.
                      'fauna.name_de AS fauna_name, '.
                      'flora.name_de AS flora_name '.
                     'FROM '.
                      'organism AS o '.
                     'LEFT OUTER JOIN fauna_organism AS fauna ON fauna.id = o.organism_id '.
                     'LEFT OUTER JOIN flora_organism AS flora ON flora.id = o.organism_id '.
                     'WHERE '.
                      'o.id = :id', array(':id' => $id));
  $result = $result->fetch();
  return $result->organism_type == '1' ? $result->fauna_name : $result->flora_name;
}

// Load all edit functions from inventory.
module_load_include('inc', 'inventory', 'inventory.edit');

/**
 * Menu callback; Autocomplete for searching the related organism in the gallery 
 *  upload form when editing an inventory type
 * 
 * @return 
 * @param int $id
 */
function organism_type_gallery_autocomplete($id) {
  $organisms = inventory_organism_autocomplete($id, $_REQUEST['term'], true);
  $results = array();
  $term = trim($_REQUEST['term']);
  $terms = explode(' ', $term);
  foreach($organisms as $organism) {
    $label = $organism['label'];
    $label_latin = $organism['label_latin'];
    $old_label = $organism['old_label'];
    $old_label_latin = $organism['old_label_latin'];
    if($old_label || $old_label_latin)
      $old_label = preg_replace('/'.$term.'/i', '<span class="ui-state-highlight">$0</span>', $old_label);
    else
      $label = preg_replace('/'.$term.'/i', '<span class="ui-state-highlight">$0</span>', $label);
    foreach($terms as $t) {
      if($old_label || $old_label_latin)
        $old_label_latin = preg_replace('/'.$t.'/i', '<span class="ui-state-highlight">$0</span>', $old_label_latin);
      else
        $label_latin = preg_replace('/'.$t.'/i', '<span class="ui-state-highlight">$0</span>', $label_latin);
    }
    $old = '';
    if($old_label || $old_label_latin)
      $old = '<small>'.$old_label.'<em>'.$old_label_latin.'</em></small>';
    array_push($results, array(
      'item_type' => 'organism',
      'item_id' => $organism['id'],
      'name' => $organism['label'],
      'label' => $label.'<em>'.$label_latin.'</em>'.$old
    ));
  }
  return drupal_json_output($results);
}

/**
 * Helper function referenced in organism_gallery_types to define the caption
 * for images added to organisms
 * 
 * @param $id
 * @return string
 */
function organism_gallery_image_caption($id) {
  $result = db_query('SELECT '.
                      'o.organism_type, '.
                      'fauna.name_de AS fauna_name, '.
                      'fauna.genus AS fauna_genus, '.
                      'fauna.species AS fauna_species, '.
                      'flora.name_de AS flora_name, '.
                      'flora."Gattung" AS flora_gattung, '.
                      'flora."Art" AS flora_art '.
                     'FROM '.
                      'organism AS o '.
                     'LEFT OUTER JOIN fauna_organism AS fauna ON fauna.id = o.organism_id '.
                     'LEFT OUTER JOIN flora_organism AS flora ON flora.id = o.organism_id '.
                     'WHERE '.
                      'o.id = :id', array(':id' => $id));
  $result = $result->fetch();
  if($result->organism_type == '1')
    $caption = $result->fauna_name.' <small>'.$result->fauna_genus.' '.$result->fauna_species.'</small>';
  else
    $caption = $result->flora_name.' <small>'.$result->flora_gattung.' '.$result->flora_art.'</small>';
  return l($caption, 'organism/'.$id, array('html' => true));
}

/**
 * Helper function referenced in organism_gallery_types to define the title of 
 * the manage images page for organisms
 * 
 * @param string $id
 *   The id of object
 * @return string
 */
function organism_gallery_manage_title($id) {
  return t('Images of the organism !title', array('!title' => '<em>'.organism_gallery_image_caption($id).'</em>'));
}

/**
 * Helper function referenced in organism_gallery_types to define the description of 
 * the manage images page for organisms
 * 
 * @param string $id
 *   The id of object
 * @return string
 */
function organism_gallery_manage_description($id) {
  return t('Add and edit images of the specified organism.');
}

/**
 * Implements hook of gallery module to define the conversion of an organism
 * to the related inventory type
 * 
 * @return array
 */
function organism_gallery_type_conversions() {
  return array(
    array(
      array(
        'source' => 'organism',
        'source_key' => 'inventory_type_id',
        'target' => 'inventory_type',
        'target_key' => 'id'
      )
    )
  );
}

/**
 * Implements hook of gallery module to define the conversion of an organism
 * to the related inventory type while editing the images of an inventory type
 * 
 * @return array
 */
function organism_gallery_type_edit_conversions() {
  return organism_gallery_type_conversions();
}

/**
 * Implements hook of gallery module to define the different category
 * conditions available for organisms
 * 
 * @return array
 */
function organism_gallery_category_conditions() {
  return array(
    'organism' => array(
      'name' => t('Organisms'),
      'type' => 'organism'
    ),
    'organism_flora' => array(
      'name' => t('Flora'),
      'type' => 'organism',
      'table' => 'organism',
      'column' => 'organism_type',
      'value' => '2'
    ),
    'organism_fauna' => array(
      'name' => t('Fauna'),
      'type' => 'organism',
      'table' => 'organism',
      'column' => 'organism_type',
      'value' => '1'
    ),
    'organism_flora_custom' => array(
      'name' => t('Flora individual'),
      'type' => 'organism',
      'table' => array('flora_organism', array(
        'organism_id' => 'flora_organism.id',
        'organism_type' => '2'
      )),
      'columns' => array(/*
        'Familie' => t('Familie'),
        'Gattung' => t('Gattung'),
        'Art' => t('Art'),*/
        'is_neophyte' => t('Neophyte')
      )
    ),
    'organism_fauna_custom' => array(
      'name' => t('Fauna individual'),
      'type' => 'organism',
      'table' => array('fauna_organism', array(
        'organism_id' => 't.id',
        'organism_type' => '1'
      )),
      'columns' => array(
        'fauna_class_id' => array(t('Inventory Group'), 'organism_gallery_catgory_invgroup_columns'),/*
        'family' => t('Family'),
        'genus' => t('Genus'),
        'sub_genus' => t('Sub genus'),
        'species' => t('Species'),
        'sub_species' => t('Sub species')*/
      ),
    ),
  );
}

/**
 * Option factory for the organism_fauna_custom condition
 * 
 * @return array
 */
function organism_gallery_catgory_invgroup_columns() {
  $result = db_select('fauna_class', 'fc', array('fetch' => PDO::FETCH_ASSOC))->fields('fc')->execute()->fetchAll();
  $columns = array();
  foreach($result as $record)
    $columns[$record['id']] = $record['name_de'];
  return $columns;
}

?>