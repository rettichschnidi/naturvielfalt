<?php
/**
 * Hook implementations for gallery integration
 */

/**
 * Implements hook of gallery module to activate the gallery
 * functionality for open_identification
 * 
 * @return array
 */
function open_identification_gallery_types() {
  return array(
    'open_identification' => array(
     'image_caption' => 'open_identification_gallery_image_caption',
       'manage_title' => 'open_identification_gallery_manage_title',
      'manage_description' => 'open_identification_gallery_manage_description',
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
 * Helper function referenced in organism_gallery_types to define the caption
 * for images added to organisms
 * 
 * @param $id
 * @return string
 */
function open_identification_gallery_image_caption($id) {

  return (l(t('open identification ').$id,'open_identification/'.$id));
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
  return t('Add and edit '.$mediatype.' of the specified open identification.');
}

?>