<?php
/**
 * Hook implementations for gallery integration
 */

/**
 * Implements hook of gallery module to activate the gallery
 * functionality for inventory_entries
 * 
 * @return array
 */
function inventory_gallery_types() {
	return array(
			'inventory_entry' => array(
					'edit_gallery' => 'inventory_gallery_access_callback',
					'image_caption' => 'inventory_gallery_image_caption',
					'manage_title' => 'inventory_gallery_manage_title',
					'manage_description' => 'inventory_gallery_manage_description',
					'subtypes' => array(
							'presentation' => t('Presentational objects')
					),
					'condition' => 'inventory_gallery_condition',
					'location_provider' => 'inventory_gallery_location_provider'
			),
			'head_inventory' => array(
					'edit_gallery' => 'gallery_owner_access_callback',
					'image_caption' => 'inventory_head_gallery_image_caption',
					'manage_title' => 'inventory_head_gallery_manage_title',
					'manage_description' => 'inventory_head_gallery_manage_description',
					'subtypes' => array(
							'presentation' => t('Presentational objects')
					),
					'location_provider' => 'inventory_head_gallery_location_provider',
					'item_provider' => 'inventory_head_gallery_item_provider'
			)
	);
}

/**
 * Helper function referenced in inventory_gallery_types to filter out 
 * images of red listed organisms.
 * 
 * @return string SQL condition 
 *  which excludes images bound to inventory entries of red listed organisms
 *  based on the permissions of the current user
 */
function inventory_gallery_condition() {
	$protected = inventory_redlisted_entries();
	if (!$protected)
		return false;
	return '(gi.item_type != \'inventory_entry\' OR NOT gi.item_id IN (' . implode(', ', $protected) . '))';
}

/**
 * Helper function referenced in inventory_gallery_types to define the edit access
 * for images added to intentory entries
 * 
 * @param string $type
 *   The type of object to be accessed
 * @param string $id
 *   The id of object to be accessed
 * @param array $image
 *   The image to be accessed.
 * @return boolean
 */
function inventory_gallery_access_callback($type, $id, $image = false) {
	$query = db_select('inventory_entry', 'ie')->condition('ie.id', $id);
	$query->join('inventory', 'i', 'i.id = ie.inventory_id');
	$query->fields('i', array('head_inventory_id'));
	$result = $query->execute()->fetch();
	return inventory_edit_access($result->head_inventory_id);
}

/**
 * Helper function referenced in inventory_gallery_types to define the caption
 * for images added to intentory entries
 * 
 * @param int $id
 *   The ID of the inventory entry
 * @return string
 */
function inventory_gallery_image_caption($id) {
	$query = db_select('inventory_entry', 'ie')->condition('ie.id', $id)->fields('ie', array('organism_id'));
	$query->join('inventory', 'i', 'i.id = ie.inventory_id');
	$query->join('head_inventory', 'hi', 'hi.id = i.head_inventory_id');
	$query->fields('hi', array('area_id'));
	$result = $query->execute()->fetch();
	return t('!organism observed in !area',
			array('!organism' => organism_gallery_image_caption($result->organism_id),
					'!area' => '<br>'
							. l(area_gallery_caption($result->area_id), 'area/' . $result->area_id, array(
									'html' => true
							))
			));
}

/**
 * Helper function referenced in inventory_gallery_types to define the title of 
 * the manage images page for intentory entries
 * 
 * @param int $id
 *   The ID of the inventory entry
 * @return string
 */
function inventory_gallery_manage_title($id, $mediatype) {
	$query = db_select('inventory_entry', 'ie')->condition('ie.id', $id);
	$query->join('inventory', 'i', 'i.id = ie.inventory_id');
	$query->join('head_inventory', 'h', 'h.id = i.head_inventory_id');
	$query->fields('ie', array('organism_id'));
	$query->fields('h', array('id',
					'area_id'
			));
	$result = $query->execute()->fetch();
	return t('' . $mediatype . ' of the organism !title<br>observed in the inventory !context',
			array('!title' => '<em>' . organism_gallery_image_caption($result->organism_id) . '</em>',
					'!context' => '<em>' . inventory_head_gallery_caption($result->id) . '</em>'
			));
}

/**
 * Helper function referenced in inventory_gallery_types to define the description of 
 * the manage images page for intentory entries
 * 
 * @param int $id
 *   The ID of the inventory entry
 * @return string
 */
function inventory_gallery_manage_description($id, $mediatype) {
	return t('Add and edit ' . $mediatype . ' of the specified organism taken in the area of this inventory.');
}

/**
 * Helper function referenced in inventory_gallery_types providing the default location
 * for images added to an inventory entry
 * 
 * @param int $id
 *   The ID of the inventory entry
 * @return string
 */
function inventory_gallery_location_provider($id) {
	$query = db_select('inventory_entry', 'ie')->condition('ie.id', $id);
	$query->join('inventory', 'i', 'i.id = ie.inventory_id');
	$query->join('head_inventory', 'h', 'h.id = i.head_inventory_id');
	$query->fields('h', array('area_id'));
	$result = $query->execute()->fetch();
	return area_gallery_location_provider($result->area_id);
}

/**
 * Helper function function referenced in inventory_gallery_types providing the 
 * selectable "Related items" for images added to a head inventory
 * 
 * @param int $id
 *   The ID of the inventory entry
 * @return array
 */
function inventory_head_gallery_item_provider($id) {
	$items = array();
	$items['head_inventory:' . $id] = t('Not related to an organism');
	$data = inventory_data($id, 'flora.name_de, fauna.name_de ASC', false, false, false, false);
	$seen = array();
	foreach ($data as $group) {
		$name = $group['invDesc']['name'];
		if (!array_key_exists($name, $items))
			$items[$name] = array();
		foreach ($group as $k => $entry) {
			if (is_string($k) || in_array($entry['orgId'], $seen))
				continue;
			array_push($seen, $entry['orgId']);
			$items[$name]['inventory_entry:' . $entry['id']] = $entry['label'] . ' (' . $entry['label_latin'] . ')';
		}
	}
	$items = array_filter($items);
	return $items;
}

/**
 * Helper function referenced in inventory_gallery_types to define the caption
 * for images added to head intentories
 * 
 * @param int $id
 *   The ID of the head inventory
 * @return string
 */
function inventory_head_gallery_image_caption($id) {
	$result = db_select('head_inventory', 'h')->condition('id', $id)->fields('h', array('id',
					'name',
					'area_id'
			))->execute()->fetch();
	return l($result->name, 'inventory/' . $result->id) . ' ['
			. l(area_gallery_caption($result->area_id), 'area/' . $result->area_id, array(
					'html' => true
			)) . ']';
}

/**
 * Helper function referenced in inventory_gallery_types to define the title of 
 * the manage images page for head intentories
 * 
 * @param int $id
 *   The ID of the head inventory
 * @return string
 */
function inventory_head_gallery_manage_title($id, $mediatype) {
	return t('' . $mediatype . ' of the inventory !title',
			array('!title' => '<em>' . inventory_head_gallery_caption($id) . '</em>'));
}

/**
 * Helper function creating a generic caption for a head inventory
 * 
 * @param int $id
 *   The ID of the head inventory
 * @return string
 */
function inventory_head_gallery_caption($id) {
	$result = db_select('head_inventory', 'h')->condition('id', $id)->fields('h', array('id',
					'name',
					'area_id'
			))->execute()->fetch();
	return $result->name . ' [' . area_gallery_caption($result->area_id) . ']';
}

/**
 * Helper function referenced in inventory_gallery_types to define the description of 
 * the manage images page for head intentories
 * 
 * @param int $id
 *   The ID of the head inventory
 * @return string
 */
function inventory_head_gallery_manage_description($id, $mediatype) {
	return t('Add and edit ' . $mediatype
			. ' of this inventory\'s area and optionally link them to an observed organism.');
}

/**
 * Helper function referenced in inventory_gallery_types providing the default location
 * for images added to an head inventory
 * 
 * @param int $id
 *   The ID of the head inventory
 * @return string
 */
function inventory_head_gallery_location_provider($id) {
	$result = db_select('head_inventory', 'h')->condition('h.id', $id)->fields('h', array('area_id'))->execute()
			->fetch();
	return area_gallery_location_provider($result->area_id);
}

/**
 * Implements hook of gallery module to define the conversion of an inventory entry
 * to the related head inventory type while editing the images of an head inventory
 * 
 * @return array
 */
function inventory_gallery_type_edit_conversions() {
	return array(
			array(
					array(
							'source' => 'inventory_entry',
							'source_key' => 'inventory_id',
							'target' => 'inventory',
							'target_key' => 'id',
					),
					array(
							'source' => 'inventory',
							'source_key' => 'head_inventory_id',
							'target' => 'head_inventory',
							'target_key' => 'id',
					)
			)
	);
}

/**
 * Implements hook of gallery module to define the conversion of an inventory entry
 * to the related organism and head inventory to the related area
 * 
 * @return array
 */
function inventory_gallery_type_conversions() {
	return array(
			array(
					array(
							'source' => 'inventory_entry',
							'source_key' => 'organism_id',
							'target' => 'organism',
							'target_key' => 'id',
							'subtype' => 'presentation'
					)
			),
			array(
					array(
							'source' => 'inventory_entry',
							'source_key' => 'organism_id',
							'target' => 'organism',
							'target_key' => 'id',
							'subtype' => 'presentation'
					),
					array(
							'source' => 'organism',
							'source_key' => 'inventory_type_id',
							'target' => 'inventory_type',
							'target_key' => 'id'
					)
			),
			array(
					array(
							'source' => 'head_inventory',
							'source_key' => 'area_id',
							'target' => 'area',
							'target_key' => 'id',
							'subtype' => 'presentation'
					)
			),
			array(
					array(
							'source' => 'inventory_entry',
							'source_key' => 'inventory_id',
							'target' => 'inventory',
							'target_key' => 'id',
					),
					array(
							'source' => 'inventory',
							'source_key' => 'head_inventory_id',
							'target' => 'head_inventory',
							'target_key' => 'id',
					)
			)
	);
}

/**
 * Implements hook of gallery module to define the different category
 * conditions available for inventory types
 * 
 * @return array
 */
function inventory_gallery_category_conditions() {
	return array(
			'inventory_entry' => array(
					'name' => t('Inventory entries'),
					'type' => 'inventory_entry'
			),
			'head_inventory' => array(
					'name' => t('Inventories'),
					'type' => 'head_inventory'
			)
	);
}

/**
 * Implements hook_image_default_styles().
 */
function inventory_image_default_styles() {
	$styles = array();

	$styles['inventory_mini'] = array(
			'effects' => array(
					array(
							'name' => 'image_scale_and_crop',
							'data' => array(
									'width' => 40,
									'height' => 40
							),
							'weight' => 0,
					),
			)
	);
	return $styles;
}

?>