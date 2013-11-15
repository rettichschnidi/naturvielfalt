<?php

/**
 * Return a array with all data for a inventory
 * @param int $inventory_id
 * @return array
 */
function inventory_get($inventory_id) {
	$inventory_id = db_escape_field($inventory_id);
	if ($inventory_id > 0) {
		$query = db_select('inventory', 'i')
						->fields('i')
						->condition('id', $inventory_id)
						->execute();
		return $query->fetchAssoc();
	}
	return false;
}

/**
 * Return a array with all inventories of an area
 * @param int $area_id
 * @return array
 */
function inventory_get_area_inventories($area_id) {
	$area_id = db_escape_field($area_id);
	if($area_id > 0) {
		$query = db_select('inventory', 'i');
		$query->join('area', 'a', 'a.id = i.area_id');
		$query = $query->fields('i')
		->condition('i.area_id', $area_id)
		->execute();

		return $query->fetchAll();
	} 
	return false;
}

/**
 * Return a array with all observations of an inventory
 * @param int $inventory_id
 * @return array
 */
function inventory_get_observations($inventory_id) {
$inventory_id = db_escape_field($inventory_id);
	if ($inventory_id > 0) {
		$query = db_select('observation', 'o')
						->fields('o')
						->condition('inventory_id', $inventory_id)
						->execute();
		return $query->fetchAll();
	}
	return false;
}

/**
 * Save an existing/new inventory
 * Post vars are used
 * @param array $form
 * 				not implemented, maybe in the future
 * @param array $custom_data
 * 				array with the data (either from form POST or the webservice)
 * @param boolean $from_api
 * 				set true for handling request from the webservice api
 */
function inventory_save($form = false, $custom_data = false, $from_api = false) {
	
	global $user;
	
	if($from_api) $values = $custom_data;
	else $values = $_POST;
	
	//prevent xss
	$inventory['id'] = !empty($values['id']) ?  filter_xss($values['id']) : 0;
	$inventory['name'] = filter_xss($values['name']);
	if(isset($values['description'])) $inventory['description'] = filter_xss($values['description']);
	$inventory['observer'] = filter_xss($values['observer']);
	$inventory['area_id'] = filter_xss($values['area_id']);
	
	//set user_uid
	$inventory['users_uid'] = isset($values['set_user_uid']) && $values['set_user_uid'] ? $values['set_user_uid']
	: $user->uid;
	
	//create and modify time
	$inventory['modify_time'] =date('Y-m-d H:i:s', time());
	
	$date_parts = explode('.', filter_xss($values['date']));
	$timesplit = explode(':', filter_xss($values['time']));
	$timestamp = mktime($timesplit[0], $timesplit[1], null, $date_parts[1], $date_parts[0], $date_parts[2]);
	$inventory['create_time'] = date('Y-m-d H:i:s', $timestamp);
	/**
	 * Save to the DB
	 */
	/**
	 * save the observation
	 */
	$transaction = db_transaction();

	try {
		if ($inventory['id'] > 0) {
			// do update
			db_update('inventory')->fields($inventory)
			->condition('id', $inventory['id'])
			->execute();
			$output['update'] = true;
		} else {
			//save the inventory
			$inventory['id'] = db_query(
					"SELECT nextval('{inventory}_id_seq') as nextval")->fetchField();
			$inventory['acl_id'] = new_acl('inventory', $inventory['users_uid']);
			db_insert('inventory')->fields($inventory)
			->execute();
			
			// Read access for ALL group
			add_group_to_acl($inventory['acl_id'], variable_get('ACL_GROUP_ALL_ID'), variable_get('ACL_READ'));
			
		}
	
		// Set the return statement
		$output['success'] = true;
		$output['guid'] = $inventory['id'];
	} catch (Exception $e) {
		$transaction->rollback();
		watchdog_exception('inventory', $e);
		$output['success'] = false;
		$output['message'] = array(
				t('Could not save the inventory because of an unknown error.')
		);
	}
	
	if ($from_api) {
		return $output;
	}
	
	if($output['success']) {
		drupal_set_message(t('successfull saved'));
		drupal_goto('inventory/'.$inventory['id'].'/edit');
	} else {
		form_set_error('inventory_form', $output['message']);
		$inventory['redirect'] = FALSE;
	}
	
	return drupal_json_output($output);

}

/**
 * Delete an inventory by id
 * An Inventory can only be deleted, once it does not contain any observation. Those need to be deleted first.
 * 
 * @param int $inventory_id
 * 	The inventory to delete
 */
function inventory_delete_by_id($inventory_id) {
	$inventory = inventory_get($inventory_id);
	$observations = inventory_get_observations($inventory_id);
	if(count($observations)) {
		drupal_set_message(
		t(
		'Can not delete inventory @inventoryname while it still contains observations. Please remove its observations first.',
		array('@inventoryname' => $inventory['name'])),
		'error');
		return;
	}
	$transaction = db_transaction();
	try {
		
		db_delete('inventory')->condition('id', $inventory_id)
		->execute();
		
		//delete acl entries
		delete_acl($inventory['acl_id']);

		return true;
	} catch (Exception $e) {
		$transaction->rollback();
		drupal_set_message(
		t(
		'Could not delete @inventoryname.',
		array('@inventoryname' => $inventory['name'])),
		'error');
		return false;
	}
}

/**
 * Returns all writeable inventories (adding observations) together with its area
 * Only returns inventories of readable areas
 */
function inventory_get_writeable_invetories_with_areas($json = false) {
	global $user;
	$minLevelArea = variable_get('ACL_READ');
	$minLevelInventory = variable_get('ACL_RED_WRITE'); 
	$sql =   'SELECT DISTINCT i.id as inventory_id, a.id as area_id, i.name as inventoryname, a.name as areaname, a.acl_id as area_acl_id, i.acl_id as inventory_acl_id
			FROM {inventory} i
			LEFT JOIN {area} a ON i.area_id = a.id';
	if (!check_admin_permission($user->uid)) {
		$sql .= ' LEFT JOIN {acl_sgroup} ag_a ON ag_a.acl_id = a.acl_id 
			LEFT JOIN {sgroup} g_a ON g_a.id = ag_a.sgroup_id
			LEFT JOIN {acl_users} au_a ON (au_a.acl_id = a.acl_id OR au_a.acl_id = g_a.acl_id) AND au_a.users_id = ' . $user->uid.'
			LEFT JOIN {acl_sgroup} ag_i ON ag_i.acl_id = i.acl_id 
			LEFT JOIN {sgroup} g_i ON g_i.id = ag_i.sgroup_id
			LEFT JOIN {acl_users} au_i ON (au_i.acl_id = i.acl_id OR au_i.acl_id = g_i.acl_id) AND au_i.users_id = ' . $user->uid.'
			WHERE ( (ag_a.sgroup_id = ' . variable_get('ACL_GROUP_ALL_ID').'
				AND ag_a.level >= ' . $minLevelArea . ') OR (au_a.acl_id = g_a.acl_id AND ag_a.level >= ' . $minLevelArea . ')
				OR (au_a.acl_id = a.acl_id AND au_a.level >= ' . $minLevelArea . ') ) AND
			( (ag_i.sgroup_id = ' . variable_get('ACL_GROUP_ALL_ID').'
				AND ag_i.level >= ' . $minLevelInventory . ') OR (au_i.acl_id = g_i.acl_id AND ag_i.level >= ' . $minLevelInventory . ')
				OR (au_i.acl_id = i.acl_id AND au_i.level >= ' . $minLevelInventory . ') )';
	}

	$sql .= ' ORDER BY a.name, i.name';
	$entries = db_query($sql)->fetchAll();
	
	$areas = array();
	$areasOptions = array();
	for($i = 0; $i < count((array)$entries); $i++) {
		$entry = (array)$entries[$i];
		if(!key_exists($entry['area_id'], $areas)) {
			//if area is not yet in areas array, move it there together with the inventory entry
			$areas[$entry['area_id']] = array('name' => $entry['areaname'], 'inventories' => array($entry['inventory_id'] => $entry['inventoryname']));
			$areasOptions[$entry['area_id']] = $entry['areaname'];
		} else {
			//add the inventory of the entry to the area
			$areas[$entry['area_id']]['inventories'][$entry['inventory_id']] = $entry['inventoryname'];
		}
	}
	$output = array('count' => count($areas), 'areas' => $areas, 'areas_as_options' => $areasOptions);
	if($json) return drupal_json_output($output);
	return $output;
	
	
}




