<?php
/**
 * @file organism.gallery.inc
 * @abstract Hook implementations for gallery integration
 * @author Reto Schneider, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

/**
 * Implements hook of gallery module to activate the gallery
 * functionality for organismreference
 *
 * @return array
 */
function organism_gallery_types() {
	return array(
			'organismreference' => array(
					'image_caption' => 'organism_reference_gallery_image_caption',
					'manage_title' => 'organism_reference_gallery_manage_title',
					'manage_description' => 'organism_reference_gallery_manage_description'
			)
	);
}

/**
 * Helper function referenced in organism_reference_gallery_types to define the caption
 * for images added to inventory entries
 *
 * @return string
 */
function organism_reference_gallery_image_caption($id) {
	
	return l(
		organism_reference_gallery_caption($id),
		'organism/' . $id,
		array(
				'html' => true
		));
}

/**
 * Helper function referenced in organism_reference_gallery_types to define the title of
 * the manage images page for organisms
 *
 * @param string $id
 *   The id of object
 * @return string
 */
function organism_reference_gallery_manage_title($id, $mediatype) {
	
	return t(
		'@mediatype of the organism !title',
		array('!title' => '<em>' . organism_reference_gallery_caption($id)
						. '</em>',
				'@mediatype' => $mediatype
		));
}

/**
 * Helper function creating a generic caption for a organism
 *
 * @param string $id
 *   The id of object
 * @return string
 */
function organism_reference_gallery_caption($id) {
	global $language;
	$result = organism_load($id);
	
	$returnValue = "";
	if(isset($result["scientific_name"])) {
		$returnValue .= $result["scientific_name"];
	} else {
		$returnValue .= "";
	}
	if(isset($result['classification_name_translations'][$language->language])) {
		$returnValue .= ' <small>['.reset($result['classification_name_translations'][$language->language]).']</small>';
	} else {
		$returnValue .= "";
	}
	
	return $returnValue;
}

/**
 * Helper function referenced in organism_reference_gallery_types to define the description of
 * the manage reference images page for organisms.
 *
 * @param string $id
 *   The id of object
 * @return string
 */
function organism_reference_gallery_manage_description($id, $mediatype) {
	return t(
		'Add and edit generic @mediatype of this organism.',
		array('@mediatype' => $mediatype));
}

/**
 * Implements hook of gallery module to define the different category
 * conditions available for organism types
 *
 * @return array
 */
function organism_gallery_category_conditions() {
	return array(
			'organismreference' => array(
					'name' => t('Organisms References'),
					'type' => 'organismreference'
			),
	);
}

?>
