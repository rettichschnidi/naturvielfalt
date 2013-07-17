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
					'manage_description' => 'area_gallery_manage_description',
					'location_provider' => 'area_gallery_location_provider',
					'item_provider' => 'area_gallery_item_provider'
			),
			'area_habitat' => array(
					'image_caption' => 'area_habitat_gallery_image_caption',
					'manage_title' => 'area_habitat_gallery_manage_title',
					'manage_description' => 'area_habitat_gallery_manage_description',
					'location_provider' => 'area_habitat_gallery_location_provider',
					'crosslinked' => false,
			)
	);
}

/**
 * Helper function function referenced in area_gallery_types providing the 
 * selectable "Related items" for images added to an area
 * 
 * @param int $id
 *   The ID of the area
 * @return array
 */
function area_gallery_item_provider($id) {
	$items = array();
	$items['area:' . $id] = t('Not related to a habitat');
	$query = db_select('area_habitat', 'ah')->condition('area_id', $id);
	$query->join('habitat', 'h', 'h.id = ah.habitat_id');
	$query->fields('h');
	$query->addField('ah', 'id', 'ah_id');
	$habitats = $query->execute()
		->fetchAll();
	$items[t('Habitats')] = array();
	foreach ($habitats as $habitat) {
		$items[t('Habitats')]['area_habitat:' . $habitat->ah_id] = $habitat->label
				. ' ' . t($habitat->name);
	}
	return $items;
}

/**
 * Helper function referenced in area_gallery_types to define the caption
 * for images added to areas
 * 
 * @param int $id
 *   The ID of the area
 * @return string
 */
function area_gallery_image_caption($id) {
	return l(
		area_gallery_caption($id),
		'area/' . $id . '/show',
		array(
				'html' => true
		));
}

/**
 * Helper function referenced in area_gallery_types to define the title of 
 * the manage images page for areas
 * 
 * @param int $id
 *   The ID of the area
 * @return string
 */
function area_gallery_manage_title($id) {
	return t(
		'Images of the area !title',
		array('!title' => '<em>' . area_gallery_caption($id) . '</em>'));
}

/**
 * Helper function creating a generic caption for an area
 * 
 * @param int $id
 *   The ID of the area
 * @return string
 */
function area_gallery_caption($id) {
	$result = db_query(
		'SELECT
			a.name As name,
			a_s.township AS township,
			a_s.canton AS canton,
			a_s.country AS country
		FROM
			{area} AS a
			JOIN {area_geometry} AS a_s ON a.area_geometry_id = a_s.id
		WHERE
			a.id = :id',
		array(':id' => $id))->fetch();
	return $result->name . ' <small>' . $result->township . ' '
			. $result->canton . ', ' . $result->country . '</small>';
}

/**
 * Helper function referenced in area_gallery_types to define the description of 
 * the manage images page for areas
 * 
 * @param int $id
 *   The ID of the area
 * @return string
 */
function area_gallery_manage_description($id, $mediatype) {
	return t(
		'Add and edit @mediatype of this area and optionally link them to an available habitat.',
		array('@mediatype' => $mediatype));
}

/**
 * Helper function referenced in area_gallery_types providing the default location
 * for images added to an area.
 * 
 * @param int $id
 *   The ID of the area
 * @return string
 */
function area_gallery_location_provider($id) {
	$result = db_query(
		'SELECT
			a_s.township AS township,
			a_s.canton AS canton,
			a_s.country AS country
		FROM
			{area} AS a
			JOIN {area_geometry} AS a_s ON a.area_geometry_id = a_s.id
		WHERE
			a.id = :id',
		array(':id' => $id))->fetch();
	$location = '';
	if (isset($result->township)) $location .= $result->township;
	if (isset($result->canton)) $location .= $result->canton;
	if (isset($result->country)) $location .= ','.$result->country;
	return $location;
}

/**
 * Helper function referenced in area_gallery_types to define the caption
 * for images added to area habitats
 * 
 * @param int $id
 *   The ID of the area_habitat
 * @return string
 */
function area_habitat_gallery_image_caption($id) {
	$result = db_select('area_habitat', 'ah')->condition('id', $id)
		->fields('ah')
		->execute()
		->fetch();
	return t(
		'!habitat in !area',
		array('!habitat' => habitat_gallery_image_caption($result->habitat_id),
				'!area' => '<br>'
						. area_gallery_image_caption($result->area_id)
		));
}

/**
 * Helper function referenced in area_gallery_types to define the title of 
 * the manage images page for area habitats
 * 
 * @param int $id
 *   The ID of the area_habitat
 * @return string
 */
function area_habitat_gallery_manage_title($id) {
	$result = db_select('area_habitat', 'ah')->condition('id', $id)
		->fields('ah')
		->execute()
		->fetch();
	return t(
		'Images of the habitat !title<br>taken in the area !context',
		array('!title' => '<em>' . habitat_gallery_caption($result->habitat_id)
						. '</em>',
				'!context' => '<em>' . area_gallery_caption($result->area_id)
						. '</em>'
		));
}

/**
 * Helper function referenced in area_gallery_types to define the description of 
 * the manage images page for area habitats
 * 
 * @param int $id
 *   The ID of the area_habitat
 * @return string
 */
function area_habitat_gallery_manage_description($id, $mediatype) {
	return t(
		'Add and edit @mediatype of the specified habitat located in the mentioned area.', array('@mediatype' => $mediatype));
}

/**
 * Helper function referenced in area_gallery_types providing the default location
 * for images added to an area habitat
 * 
 * @param int $id
 *   The ID of the area_habitat
 * @return string
 */
function area_habitat_gallery_location_provider($id) {
	$result = db_select('area_habitat', 'ah')->condition('id', $id)
		->fields('ah')
		->execute()
		->fetch();
	return area_gallery_location_provider($result->id);
}

/**
 * Implements hook of gallery module to define the conversion of an area's habitat
 * to the related area type while editing the images of an area
 * 
 * @return array
 */
function area_gallery_type_edit_conversions() {
	return array(
			array(
					array(
							'source' => 'area_habitat',
							'source_key' => 'area_id',
							'target' => 'area',
							'target_key' => 'id',
					)
			)
	);
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
			),
			array(
					array(
							'source' => 'area_habitat',
							'source_key' => 'area_id',
							'target' => 'area',
							'target_key' => 'id',
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
