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
function open_identification_gallery_types() {
  return array(
    'open_identification' => array(
      'image_caption' => 'open_identification_gallery_image_caption',
      'manage_title' => 'open_identification_gallery_manage_title',
      'manage_description' => 'open_identification_gallery_manage_description'
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
function open_identification_gallery_manage_title($id) {
 
  return t('Manage title open_identification_gallery_manage_title');
}

/**
 * Helper function referenced in organism_gallery_types to get the name 
 * of a selected item type and id previously defined by 
 * organism_type_gallery_autocomplete
 * 
 */
function open_identification_gallery_item_provider($type, $id) {

  return $id;
}

// Load all edit functions from inventory.


/**
 * Helper function referenced in organism_gallery_types to define the caption
 * for images added to organisms
 * 
 * @param $id
 * @return string
 */
function open_identification_gallery_image_caption($id) {

  return $id;
}

/**
 * Helper function referenced in organism_gallery_types to define the title of 
 * the manage images page for organisms
 * 
 * @param string $id
 *   The id of object
 * @return string
 */
function open_identification_gallery_manage_title($id) {
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
function open_identification_gallery_manage_description($id,$mediatype) {
  return t('Add and edit '.$mediatype.' of the specified organism.');
}

/**
 * Implements hook of gallery module to define the conversion of an organism
 * to the related inventory type
 * 
 * @return array
 */
function open_identification_gallery_type_conversions() {
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
function open_identification_gallery_type_edit_conversions() {
  return open_identification_gallery_type_conversions();
}


?>