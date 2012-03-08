<?php
module_load_include('inc', 'c_acl', 'c_acl.base');
/**
 * 
 * Enter description here ...
 */
function c_acl_autocomplete_sgroup($acl_id, $string) {
	global $user;

	$sql_res = db_select('sgroup', 'sg')->fields(
			'sg',
			array('id',
					'name',
					'acl_id',
			))
		->condition('sg.name', '%'.$string.'%', "ILIKE")
		->orderBy('sg.name')
		->execute();
	
	// get groups already in acl table
	$groups = get_groups_in_acl($acl_id);
	
	// iterate through all groups
	$items = array();
	while ($record = $sql_res->fetch()) {
		$in_table = FALSE;
		foreach ($groups as $group) {
			if ($record->id == $group->id) {
				// group already in acl table
				$in_table = TRUE;
			}
		}
		if (!$in_table) {
			// group not in acl table
			$read_permission = check_permission($record->acl_id, variable_get("ACL_READ"));
			if ($read_permission) {
				// user has read access to group
				$value = $record->name;
				$key = '{"id" : "' . $record->id . '", "name" : "' . $record->name . '"}';
				$items[$key] = check_plain($value);
			}
		}
	}

	return drupal_json_output($items);
}

/**
 * 
 * Enter description here ...
 */
function c_acl_autocomplete_users($acl_id, $string) {
	$sql_res = db_select('users', 'u')->fields(
			'u',
			array('uid',
					'name',
			))
		->condition('u.name', '%'.$string.'%', "ILIKE")
		->orderBy('u.name')
		->execute();
	
	// get users already in acl table
	$users = get_users_in_acl($acl_id);
	
	// iterate through all users
	$items = array();
	while ($record = $sql_res->fetch()) {
		$in_table = FALSE;
		foreach ($users as $user) {
			if ($record->uid == $user->uid) {
				// group already in acl table
				$in_table = TRUE;
			}
		}
		if (!$in_table) {
			// user not in acl table
			$value = $record->name;
			// user has read access to group
			$key = '{"id" : "' . $record->uid . '", "name" : "' . $record->name . '"}';
			$items[$key] = check_plain($value);
		}
	}

	return drupal_json_output($items);
}

?>