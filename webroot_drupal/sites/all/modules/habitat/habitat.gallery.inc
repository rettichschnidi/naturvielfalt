<?php
/**
 * @file habitat.galery.inc
 * @abstract Hook implementations for gallery integration
 * @author ?, 2011
 * @author Ramon Gamma, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

/**
 * Implements hook of gallery module to activate the gallery
 * functionality for habitat
 *
 * @return array
 */
function habitat_gallery_types() {
	return array(
			'habitat' => array(
					'image_caption' => 'habitat_gallery_image_caption',
					'manage_title' => 'habitat_gallery_manage_title',
					'manage_description' => 'habitat_gallery_manage_description'
			)
	);
}

/**
 * Helper function referenced in habitat_gallery_types to define the caption
 * for images added to intentory entries
 *
 * @return string
 */
function habitat_gallery_image_caption($id) {
	return l(
		habitat_gallery_caption($id),
		'habitat/' . $id,
		array(
				'html' => true
		));
}

/**
 * Helper function referenced in habitat_gallery_types to define the title of
 * the manage images page for habitats
 *
 * @param string $id
 *   The id of object
 * @return string
 */
function habitat_gallery_manage_title($id, $mediatype) {
	return t(
		'' . $mediatype . ' of the habitat !title',
		array('!title' => '<em>' . habitat_gallery_caption($id) . '</em>'));
}

/**
 * Helper function creating a generic caption for a habitat
 *
 * @param string $id
 *   The id of object
 * @return string
 */
function habitat_gallery_caption($id) {
	$result = db_select('habitat', 'h')->condition('id', $id)
		->fields('h')
		->execute()
		->fetch();
	// 	return $result->name_de . ' <small>' . $result->name_lt . '</small>';
		return $result->name;
}

/**
 * Helper function referenced in habitat_gallery_types to define the description of
 * the manage images page for habitats
 *
 * @param string $id
 *   The id of object
 * @return string
 */
function habitat_gallery_manage_description($id, $mediatype) {
	return t('Add and edit generic ' . $mediatype . ' of this habitat.');
}

/**
 * Implements hook of gallery module to define the different category
 * conditions available for habitat types
 *
 * @return array
 */
function habitat_gallery_category_conditions() {
	return array(
			'habitat' => array(
					'name' => t('Habitats'),
					'type' => 'habitat'
			),
	);
}

?>
