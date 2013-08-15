<?php
module_load_include('inc', 'inventory');

/**
 * Returns all inventories of an area in json format for datatable
 * 
 * @param array $area
 * 	Array with all data about the area
 */
function inventory_get_area_inventories_json($area) {
	
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'i.id AS id',
			'i.acl_id AS acl_id',
			'i.name',
			'i.description',
			'i.observer',
			'i.area_id',
			'o.count',
	);

	$sql['dbSearchColumns'] = array(
			'i.name',
			'i.observer'
	);

	$sql['dbTable'] = '{inventory} i';
	$sql['dbJoins'] = 'JOIN {area} a ON a.id = i.area_id
			LEFT JOIN (SELECT o.inventory_id, COUNT(o.id) AS count FROM {observation} o GROUP BY o.inventory_id) o ON o.inventory_id = i.id';
	
	$sql['dbWhere'] = 'i.area_id = '.$area['id'];
	$sql['id'] = 'id';
	$sql['acl_level'] = 'ACL_READ';
	$output = datatable_dbRequest($sql);

	global $base_url;
	$iconBaseUrl = '/' . path_to_theme() . '/images/icons/enabled/';
		
	foreach ($output['rows'] as &$row) {
		$inventory = inventory_get($row['cell']['id']);
		
		// select box
// 		$row['cell']['selectColumn'] = '<input class="gridSelect" type="checkbox" value="' . $row['id'] . '" title="' . t('Select') . '"" />';
		
		$row['cell']['removeColumn'] = check_admin_acl($inventory['acl_id'])
		? 
			'<img onclick="area.deleteInventory('.$row['id'].'); event.stopPropagation();" src="' . $iconBaseUrl . 'Erase.png" alt="' . t('Delete Inventory') . '" title="' . t('delete inventory') . '" />'
			: '';
		
		
		$row['cell']['addColumn'] = inventory_check_access($inventory, 'ACL_RED_WRITE') && user_access(CREATE_OBSERVATION)
		? l(
				'<img src="' . $iconBaseUrl . 'Add.png" alt="' . t('Add Observation') . '" title="' . t('Add Observation') . '" />',
				$base_url . '/observation/new/' . $row['id'],
				array(
						'html' => true
				))
				: '';
		
		$row['cell']['showColumn'] = l(
				'<img src="' . $iconBaseUrl . 'Zoom.png" alt="' . t('Show') . '" title="' . t('Show') . '" />',
				$base_url . '/inventory/' . $row['id'] . '/show',
				array(
						'html' => true
				));
		
		$row['cell']['editColumn'] = (inventory_write_access_by_id($row['cell']['id']))
			? l(
				'<img src="' . $iconBaseUrl . 'Modify.png" alt="' . t('Edit') . '" title="' . t('Edit') . '" />',
				$base_url . '/inventory/' . $row['id'] . '/edit',
				array(
					'html' => true
				))
			: '';
		
		if(!isset($row['cell']['count'])) {
			$row['cell']['count'] = 0;
		}
		
		// Link observer soll Link sein
		$row['cell']['observer'] = l(
				$row['cell']['observer'],
				$base_url . '/user/' . $area['userid'] ,
				array(
						'html' => true
				));
		
	}

	return drupal_json_output($output);
}

/**
 * Returns all observations of an inventory in json format for datatable.
 * Includes the area geometry (to display on the map)
 *
 * @param array $area
 * 	Array with all data about the area
 * @param array $inventory
 * 	Array with all data about the inventory
 */
function inventory_get_observations_json($area, $inventory = NULL) {
	$inventory_ids = array();
	if(isset($area) && empty($inventory)) {
		foreach(inventory_get_area_inventories($area['id']) as $inv) {
			$inv = (array)$inv;
			$inventory_ids[] = $inv['id'];
		}
	}
	else if(!empty($inventory)) $inventory_ids[] = $inventory['id']; 
	
	return observation_get_observations_of_inventory_json('ACL_READ', $inventory_ids);
}

/**
 * Deletes an inventory with all observations (with json output)
 *
 * @param array $inventory
 * 	Array with all data about the inventory
 */
function inventory_delete($inventory) {
	global $user;
	//	if (!isset($user)) {
	if ($user->uid == 0) {
		$output['success'] = false;
		$output['type'] = 'error';
		$output['message'] = t('Please log in first');
		return drupal_json_output($output);
	}

	if (!isset($inventory)) {
		$output['success'] = false;
		$output['type'] = 'error';
		$output['message'] = t('Please provide an inventory to delete');
		return drupal_json_output($output);
	}

	$success = inventory_delete_by_id($inventory['id']);

	$output['success'] = $success;
	$output['count'] = $success ? 1 : 0;
	$output['type'] = 'status';
	$output['message'] = t('Inventory deleted');

	return drupal_json_output($output);
}
