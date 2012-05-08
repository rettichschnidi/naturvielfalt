<?php
/**
 * @file observation.gallery.inc
 * @abstract Hook implementations for gallery integration
 * @author ?, 2011
 * @author Ramon Gamma, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

/**
 * Implements hook of gallery module to activate the gallery
 * functionality for observation
 *
 * @return array
 */
function observation_gallery_types() {
	return array(
			'observation' => array(
					'image_caption' => 'observation_gallery_image_caption',
					'manage_title' => 'observation_gallery_manage_title',
					'manage_description' => 'observation_gallery_manage_description'
			)
	);
}

/**
 * Helper function referenced in observation_gallery_types to define the caption
 * for images added to intentory entries
 *
 * @return string
 */
function observation_gallery_image_caption($id) {
	return l(
		observation_gallery_caption($id),
		'observation/' . $id,
		array(
				'html' => true
		));
}

/**
 * Helper function referenced in observation_gallery_types to define the title of
 * the manage images page for observations
 *
 * @param string $id
 *   The id of object
 * @return string
 */
function observation_gallery_manage_title($id, $mediatype) {
	return t(
		'' . $mediatype . ' of the observation !title',
		array('!title' => '<em>' . observation_gallery_caption($id) . '</em>'));
}

/**
 * Helper function creating a generic caption for a observation
 *
 * @param string $id
 *   The id of object
 * @return string
 */
function observation_gallery_caption($id) {
	$observationdata = observation_read_access(observation_load($id));
	return t("Observation @observation_organism from @observation_date.", array(
				'@observation_organism' => $observationdata['organism']['name_lang']
											.'('.$observationdata['organism']['name_lat'].')',
				'@observation_date' => date('d.m.Y', $observationdata['observation']['date'])));
}

/**
 * Helper function referenced in observation_gallery_types to define the description of
 * the manage images page for observations
 *
 * @param string $id
 *   The id of object
 * @return string
 */
function observation_gallery_manage_description($id, $mediatype) {
	return t('Add and edit generic ' . $mediatype . ' of this observation.');
}

/**
 * Implements hook of gallery module to define the different category
 * conditions available for observation types
 *
 * @return array
 */
function observation_gallery_category_conditions() {
	return array(
			'observation' => array(
					'name' => t('Observations'),
					'type' => 'observation'
			),
	);
}

?>