<?php

/**
 * Menu callback f�r User-Inventare
 * @return Array
 */
function inventory_list_show_users_inventory() {
	global $user;
	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');
	$render_array = array();

	generate_user_list(&$render_array);

	return $render_array;
}

/**
 * Generiert eine Liste mit den vom Benutzer erstellten Inventories
 * @param unknown_type $render_array
 * @return Array
 */
function generate_user_list(&$render_array) {
	$result = db_query(
			"SELECT a.field_name, a.id area_id, a.township, hi.id, hi.name hi_name,hi.owner_id, hi.shared, hi.create_time, hi.modify_time,
	      (SELECT COUNT(e.id)
	      FROM inventory_entry e
	      LEFT JOIN inventory i ON i.id = e.inventory_id
	      WHERE i.head_inventory_id = hi.id) entries
	    FROM head_inventory hi
	      LEFT JOIN area a ON hi.area_id = a.id
	    WHERE (hi.owner_id=:id AND hi.hidden IS NOT TRUE)
	    ORDER BY hi.name, a.field_name asc;", array(':id' => $user->uid));
	$render_array = array_merge(
			_inventory_list_factory($result, 'inventories_own', t('My own inventories'),
					t('You have not created any inventories so far'),
					array(
							t('Inventory name'),
							t('Entries'),
							t('Area name'),
							t('Township'),
							t('Created on'),
							t('Modified on'),
							t('Public'),
							t('Action')
					), '_inventory_own_row_factory'), $render_array);
	return $render_array;
}

/**
 * Generiert eine Liste mit den gemeinsamen Inventaren
 * @param unknown_type $render_array
 */
function generate_shared_list(&$render_array) {
	$result = db_query(
			"
	    SELECT hi.name hi_name, hi.id, a.field_name, a.id area_id, a.township, ua.field_address_first_name || ' ' || ua.field_address_last_name u_name, sg.sgroup_name, hi.shared, write, hi.create_time, hi.modify_time,
	      (SELECT COUNT(e.id) FROM inventory_entry e LEFT JOIN inventory i ON i.id = e.inventory_id WHERE i.head_inventory_id = hi.id) entries
	    FROM head_inventory hi
	      LEFT JOIN sgroup_inventory sgi ON sgi.hiid = hi.id
	      LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
	      LEFT JOIN users u ON hi.owner_id = u.uid
	      LEFT JOIN field_data_field_address ua ON ua.entity_id = u.uid
	      LEFT JOIN sgroup sg ON sg.sgid = sgi.sgid
	      LEFT JOIN area a ON hi.area_id = a.id
	    WHERE (u.uid != :id AND sgu.uid = :id AND sgi.read = 1 AND hi.hidden IS NOT TRUE)
	    ORDER BY hi.name, hi.id, sg.sgroup_name;", array(':id' => $user->uid));
	$render_array = array_merge(
			_inventory_list_factory($result, 'inventories_shared', t('Shared inventories'),
					t('No editable shared inventories'),
					array(
							t('Inventory name'),
							t('Entries'),
							t('Area name'),
							t('Township'),
							t('Owner'),
							t('Groups'),
							t('Created on'),
							t('Modified on'),
							t('Public'),
							t('Action')
					), '_inventory_shared_row_factory'), $render_array);
}

/**
 * Generiert eine Liste mit den �ffentlichen Inventaren
 * @param unknown_type $render_array
 */
function generate_public_list(&$render_array) {
	$result = db_query(
			"
	    SELECT hi.id, hi.name hi_name, ua.field_address_first_name || ' ' || ua.field_address_last_name u_name, a.field_name, a.id area_id, a.township, hi.create_time, hi.modify_time,
	      (SELECT COUNT(e.id) FROM inventory_entry e LEFT JOIN inventory i ON i.id = e.inventory_id WHERE i.head_inventory_id = hi.id) entries
	    FROM head_inventory hi
	      LEFT JOIN users u ON hi.owner_id = u.uid
	      LEFT JOIN field_data_field_address ua ON ua.entity_id = u.uid
	      LEFT JOIN area a ON hi.area_id = a.id
	    WHERE shared = true AND hi.id NOT IN (
	      SELECT sgiEx.hiid FROM sgroup_inventory sgiEx
	      LEFT JOIN sgroup_users sguEx ON sgiEx.sgid = sguEx.sgid
	      WHERE sguEx.uid = :id AND (sgiEx.read != 0 OR sgiEX.write != 0)
	    ) AND hi.hidden IS NOT TRUE
	    ORDER BY hi.name, hi.id;", array(':id' => $user->uid));
	$render_array = array_merge(
			_inventory_list_factory($result, 'inventories_public', t('Public inventories'),
					t('No public inventories have been created so far')), $render_array);

}

/**
 *
 * Enter description here ...
 * @return array
 */
function inventory_list() {
	global $user;
	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');
	$render_array = array();

	generate_user_list($render_array);
	generate_shared_list($render_array);
	generate_public_list($render_array);

	return $render_array;
}

/**
 *
 * Enter description here ...
 * @return multitype: array
 */
function inventory_editable() {
	global $user;
	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');
	$render_array = array();

	// Own Inventories
	$result = db_query(
			"
    SELECT a.field_name, a.id area_id, a.township, hi.id, hi.name hi_name,hi.owner_id, hi.shared, hi.create_time, hi.modify_time,
      (SELECT COUNT(e.id) FROM inventory_entry e LEFT JOIN inventory i ON i.id = e.inventory_id WHERE i.head_inventory_id = hi.id) entries
    FROM head_inventory hi
      LEFT JOIN area a ON hi.area_id = a.id
    WHERE (hi.owner_id=:id AND hi.hidden IS NOT TRUE)
    ORDER BY hi.name, a.field_name asc;", array(':id' => $user->uid));
	$render_array = array_merge(
			_inventory_list_factory($result, 'inventories_own', t('My own inventories'),
					t('You have not created any inventories so far'),
					array(
							t('Inventory name'),
							t('Entries'),
							t('Area name'),
							t('Township'),
							t('Public'),
					), '_inventory_editable_own_row_factory'), $render_array);

	// Shared inventories
	$result = db_query(
			"
    SELECT hi.name hi_name, hi.id, a.field_name, a.township, u.name u_name, sg.sgroup_name, hi.shared,
      (SELECT COUNT(e.id) FROM inventory_entry e LEFT JOIN inventory i ON i.id = e.inventory_id WHERE i.head_inventory_id = hi.id) entries
    FROM head_inventory hi
      LEFT JOIN sgroup_inventory sgi ON sgi.hiid = hi.id
      LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
      LEFT JOIN users u ON hi.owner_id = u.uid
      LEFT JOIN sgroup sg ON sg.sgid = sgi.sgid
      LEFT JOIN area a ON hi.area_id = a.id
    WHERE (u.uid != :id AND sgu.uid = :id AND sgi.write = 1 AND hi.hidden IS NOT TRUE)
    ORDER BY hi.name, hi.id, sg.sgroup_name;", array(':id' => $user->uid));
	$render_array = array_merge(
			_inventory_list_factory($result, 'inventories_shared', t('Shared inventories'), t('No shared inventories'),
					array(
							t('Inventory name'),
							t('Entries'),
							t('Area name'),
							t('Township'),
							t('Owner'),
							t('Groups'),
							t('Public')
					), '_inventory_editable_shared_row_factory'), $render_array);

	return $render_array;
}

/**
 *
 * Enter description here ...
 * @param unknown_type $row
 * @param unknown_type $record
 * @param unknown_type $rows
 * @return
 */
function _inventory_own_row_factory($row, $record, &$rows) {
	$img_edit = theme('image',
			array('path' => 'sites/all/sites/all/modules/inventory/images/fatcow_icons/16x16/application_edit.png',
					'alt' => t('Edit'),
					'title' => t('Edit'),
			));
	$row = array(
			$row[0],
			$row[1],
			$row[2],
			$row[3],
			$row[4],
			$row[5],
			$record->shared == true ? t('Yes') : t('No'),
			$row[7] . ' ' . l($img_edit, 'inventory/' . $record->id . '/edit', array('html' => TRUE,))
	);
	return $row;
}

/**
 *
 * Enter description here ...
 * @param unknown_type $row
 * @param unknown_type $record
 * @param unknown_type $rows
 * @return boolean|multitype:string unknown Ambigous <The, string, A, Returns, multitype:Ambigous <> , multitype:Optional >
 */
function _inventory_shared_row_factory($row, $record, &$rows) {
	static $seen;
	$sgroups = $record->sgroup_name;
	if (!array_key_exists($record->id, $seen))
		$seen[$record->id] = count($rows);
	else {
		$rows[$seen[$record->id]][4] .= ', ' . $sgroups;
		return false;
	}
	$write = '';
	if ($record->write == true) {
		//Edit Image
		$img_edit = theme('image',
				array('path' => 'sites/all/sites/all/modules/inventory/images/fatcow_icons/16x16/application_edit.png',
						'alt' => t('Edit'),
						'title' => t('Edit'),
				));
		$write = ' ' . l($img_edit, 'inventory/' . $record->id . '/edit', array('html' => TRUE,));
	}
	$row = array(
			$row[0],
			$row[1],
			$row[2],
			$row[3],
			$row[6],
			$sgroups,
			$row[4],
			$row[5],
			$record->shared == true ? t('Yes') : t('No'),
			$row[7] . $write
	);
	return $row;
}

/**
 *
 * Enter description here ...
 * @param unknown_type $row
 * @param unknown_type $record
 * @param unknown_type $rows
 */
function _inventory_editable_own_row_factory($row, $record, &$rows) {
	$row = array(
			l($row[0], 'inventory/' . $record->id . '/edit'),
			$row[1],
			$row[2],
			$row[3],
			$record->shared == true ? t('Yes') : t('No')
	);
	return $row;
}

/**
 *
 * Enter description here ...
 * @param unknown_type $row
 * @param unknown_type $record
 * @param unknown_type $rows
 * @return
 */
function _inventory_editable_shared_row_factory($row, $record, &$rows) {
	static $seen;
	$sgroups = $record->sgroup_name;
	if (!array_key_exists($record->id, $seen))
		$seen[$record->id] = count($rows);
	else {
		$rows[$seen[$record->id]][5] .= ', ' . $sgroups;
		return false;
	}
	$row = array(
			$row[0],
			$row[1],
			$row[2],
			$row[3],
			$row[6],
			$sgroups,
			$record->shared == true ? t('Yes') : t('No')
	);
	return $row;
}

/**
 *
 * Enter description here ...
 * @param unknown_type $result
 * @param unknown_type $id
 * @param unknown_type $title
 * @param unknown_type $msg_empty
 * @param unknown_type $headings
 * @param unknown_type $row_factory
 */
function _inventory_list_factory($result, $id, $title, $msg_empty = false, $headings = array(), $row_factory = false) {
	$render_array = array();
	//Localize Image
	$img_localize = theme('image',
			array('path' => 'sites/all/modules/inventory/images/fatcow_icons/16x16/world.png',
					'alt' => t('Localize'),
					'title' => t('Localize'),
			));
	if (!$headings)
		$headings = array(
				t('Inventory name'),
				t('Entries'),
				t('Area name'),
				t('Township'),
				t('Created on'),
				t('Modified on'),
				t('Owner'),
				t('Action')
		);
	$rows = array();
	foreach ($result as $record) {
		$row = array(
				($record->hi_name ? $record->hi_name : t('no name')) . ' (' . $record->id . ')',
				$record->entries,
				$record->field_name,
				$record->township,
				date("j.m.Y", strtotime($record->create_time)),
				date("j.m.Y", strtotime($record->modify_time)),
				@$record->u_name,
				l($img_localize, 'area/' . $record->area_id,
						array('html' => true,
								'attributes' => array('id' => 'area_' . $record->area_id)
						))
		);
		if ($row_factory && function_exists($row_factory))
			$row = call_user_func_array($row_factory, array($row,
					$record,
					&$rows
			));
		if ($row)
			$rows[] = array(
					'data' => $row,
					'id' => 'inventory_' . $record->id
			);
	}

	$render_array[$id] = array(
			'#type' => 'fieldset',
			'#title' => $title,
			'#weight' => 5,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					), // add 'collapsed' to array, to close by default
			),
	);

	if ($rows)
		$render_array[$id]['inventories'] = array(
				'#theme' => 'datatable',
				'#header' => $headings,
				'#rows' => $rows,
				'#id_table' => $id,
		);
	elseif ($msg_empty)
		$render_array[$id]['inventories'] = array(
				'#markup' => $msg_empty,
		);
	return $render_array;
}
?>