<?php
/**
 * Hook implementations for gallery integration
 */

/**
 * Implements hook of gallery module to activate the gallery
 * functionality for area
 * 
 * @return array
 */
function area_gallery_types() {
  return array(
    'area' => array(
      'image_caption' => 'area_gallery_image_caption',
      'manage_title' => 'area_gallery_manage_title',
      'manage_description' => 'area_gallery_manage_description'
    ),
    'area_habitat' => array(
      'image_caption' => 'area_habitat_gallery_image_caption',
      'manage_title' => 'area_habitat_gallery_manage_title',
      'manage_description' => 'area_habitat_gallery_manage_description',
      'crosslinked' => false
    )
  );
}

/**
 * Helper function referenced in area_gallery_types to define the caption
 * for images added to areas
 * 
 * @param $id
 * @return string
 */
function area_gallery_image_caption($id) {
  return l(area_gallery_caption($id), 'area/'.$id, array('html' => true));
}

/**
 * Helper function referenced in area_gallery_types to define the title of 
 * the manage images page for areas
 * 
 * @param string $id
 *   The id of object
 * @return string
 */
function area_gallery_manage_title($id) {
  return t('Images of the area !title', array('!title' => '<em>'.area_gallery_caption($id).'</em>'));
}

/**
 * Helper function creating a generic caption for an area
 * 
 * @param string $id
 *   The id of object
 * @return string
 */
function area_gallery_caption($id) {
  $result = db_select('area', 'a')
    ->condition('id', $id)
    ->fields('a')
    ->execute()
    ->fetch();
  return $result->field_name.' <small>'.$result->locality.' '.$result->canton.', '.$result->country.'</small>';
}

/**
 * Helper function referenced in area_gallery_types to define the description of 
 * the manage images page for areas
 * 
 * @param string $id
 *   The id of object
 * @return string
 */
function area_gallery_manage_description($id) {
  return t('Add and edit generic images of this area.');
}

/**
 * Helper function referenced in area_gallery_types to define the caption
 * for images added to area habitats
 * 
 * @param $id
 * @return string
 */
function area_habitat_gallery_image_caption($id) {
  $result = db_select('area_habitat', 'ah')
    ->condition('id', $id)
    ->fields('ah')
    ->execute()
    ->fetch();
  return habitat_gallery_image_caption($result->habitat_id);
}

/**
 * Helper function referenced in area_gallery_types to define the title of 
 * the manage images page for area habitats
 * 
 * @param string $id
 *   The id of object
 * @return string
 */
function area_habitat_gallery_manage_title($id) {
  $result = db_select('area_habitat', 'ah')
    ->condition('id', $id)
    ->fields('ah')
    ->execute()
    ->fetch();
  return t('Images of the habitat !title<br>taken in the area !context', array(
    '!title' => '<em>'.habitat_gallery_caption($result->habitat_id).'</em>',
    '!context' => '<em>'.area_gallery_caption($result->area_id).'</em>'
  ));
}

/**
 * Helper function referenced in area_gallery_types to define the description of 
 * the manage images page for area habitats
 * 
 * @param string $id
 *   The id of object
 * @return string
 */
function area_habitat_gallery_manage_description($id) {
  return t('Add and edit images of the specified habitat located in the mentioned area.');
}

/**
 * Implements hook of gallery module to define the conversion of a area habitat
 * to the related habitat
 * 
 * @return array
 */
function area_gallery_type_conversions() {
  return array(
    array(
      array(
        'source' => 'area_habitat',
        'source_key' => 'habitat_id',
        'target' => 'habitat',
        'target_key' => 'id',
      )
    ),
    array(
      array(
        'source' => 'habitat',
        'source_key' => 'id',
        'target' => 'area_habitat',
        'target_key' => 'habitat_id',
      )
    )
  );
}

/**
 * Implements hook of gallery module to define the different category
 * conditions available for area types
 * 
 * @return array
 */
function area_gallery_category_conditions() {
  return array(
    'area' => array(
      'name' => t('Areas'),
      'type' => 'area'
    ),
  );
}

?>