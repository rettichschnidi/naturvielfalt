<?php
/**
 * This routine was only used to debug acl module
 * ATTENTION: to use this correctly you have to define the users and groups listed below
 * 
 * @return 	Array	drupal form array with table with testresults
 */
function run_acl_test() {
	global $user;
	$def_user_id = array();
	$def_group_id = array();

	/* START SET TEST VARIABLES */
	// set user ids
	$def_user_id['user1'] = 2;
	$def_user_id['user2'] = 3; // must be member of group 1
	$def_user_id['user3'] = 4;
	// set group ids
	$def_group_id['group1'] = 2; // must contain user 2 as member (not owner)
	$def_group_id['group2'] = 3;
	/* END SET TEST VARIABLES */

	$output['introduction'] = array(
			'#markup' => t(
				"ACL tests are executed and the result is listed in the table below"),
			'#weight' => 0,
	);
	$success = t('success');
	$fail = t('fail');
	$tableHeader = array();
	$tableHeader[] = t('#');
	$tableHeader[] = t('Name');
	$tableHeader[] = t('Description');
	$tableHeader[] = t('Result');

	// cerate new acl with user 1
	$row = array();
	$rowCount = 1;
	$displayCount = 1;
	$user_id = $def_user_id['user1'];
	$row[] = $rowCount;
	$row[] = t('Create new ACL');
	$row[] = t('Create a new ACL by user_id = ' . $user_id);
	$acl_id = new_acl("Description #2", $user_id);
	$do_next = FALSE;
	if ($acl_id != NULL) {
		$row[] = $success;
		$do_next = TRUE;
	} else {
		$row[] = $fail;
	}
	$rows[] = $row;

	// check if new user is in acl and if lvl is ok
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('User in ACL');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' is in created ACL id = '
					. $acl_id . ' and lvl = ' . variable_get("ACL_ADMIN"));
		$lvl = check_user_in_acl($acl_id, $user_id);
		$do_next = FALSE;
		if ($lvl == variable_get("ACL_ADMIN")) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if new user owns acl
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('User owns ACL');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' owns ACL with id = ' . $acl_id);
		$do_next = FALSE;
		if (check_user_owns_acl($acl_id, $user_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show user in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All users in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_users = get_users_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_users as $my_user) {
		$t_row = array();
		$t_row[] = $my_user->uid;
		$t_row[] = $my_user->name;
		$t_row[] = $my_user->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// add user 2, lvl 2 to acl
	$user_id = $def_user_id['user2'];
	$acl_lvl = 2;
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Add user to ACL');
		$row[] = t(
			'Add user with id = ' . $user_id . ' to ACL with id = ' . $acl_id
					. ' with level = ' . $acl_lvl);
		$do_next = FALSE;
		if (add_user_to_acl($acl_id, $user_id, $acl_lvl)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if user 2 is in acl and if lvl is ok
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('User in ACL');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' is in ACL id = ' . $acl_id
					. ' with level = ' . $acl_lvl);
		$lvl = check_user_in_acl($acl_id, $user_id);
		$do_next = FALSE;
		if ($lvl == $acl_lvl) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if new user owns acl (negative test)
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('User owns ACL');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' owns ACL with id = ' . $acl_id
					. ' (negative test)');
		$do_next = FALSE;
		if (!check_user_owns_acl($acl_id, $user_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show user in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All users in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_users = get_users_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_users as $my_user) {
		$t_row = array();
		$t_row[] = $my_user->uid;
		$t_row[] = $my_user->name;
		$t_row[] = $my_user->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// change lvl of user 2 in acl
	$user_id = $def_user_id['user2'];
	$acl_lvl = 3;
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Change user level');
		$row[] = t(
			'Chenge level of user with id = ' . $user_id . ' in ACL with id = '
					. $acl_id . ' to level = ' . $acl_lvl);
		$do_next = FALSE;
		if (update_user_in_acl($acl_id, $user_id, $acl_lvl)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if user 2 is in acl and if lvl is ok
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('User in ACL');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' is in ACL id = ' . $acl_id
					. ' with level = ' . $acl_lvl);
		$lvl = check_user_in_acl($acl_id, $user_id);
		$do_next = FALSE;
		if ($lvl == $acl_lvl) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show user in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All users in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_users = get_users_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_users as $my_user) {
		$t_row = array();
		$t_row[] = $my_user->uid;
		$t_row[] = $my_user->name;
		$t_row[] = $my_user->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// change ownership of acl to user 3
	$user_id = $def_user_id['user3'];
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Chown ACL');
		$row[] = t(
			'Change ownsership of acl with id = ' . $acl_id
					. ' to user with id = ' . $user_id);
		$do_next = FALSE;
		if (chown_acl($acl_id, $user_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if new user owns acl
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('User owns ACL');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' owns ACL with id = ' . $acl_id);
		$do_next = FALSE;
		if (check_user_owns_acl($acl_id, $user_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show user in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All users in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);
	$displayCount++;

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_users = get_users_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_users as $my_user) {
		$t_row = array();
		$t_row[] = $my_user->uid;
		$t_row[] = $my_user->name;
		$t_row[] = $my_user->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// add group 1, lvl 2 to acl
	$group_id = $def_group_id['group1'];
	$acl_lvl = 3;
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Add group to ACL');
		$row[] = t(
			'Add group with id = ' . $group_id . ' to ACL with id = ' . $acl_id
					. ' with level = ' . $acl_lvl);
		$do_next = FALSE;
		if (add_group_to_acl($acl_id, $group_id, $acl_lvl)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if group 1 is in acl and if lvl is ok
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Group in ACL');
		$row[] = t(
			'Check if group_id = ' . $group_id . ' is in ACL id = ' . $acl_id
					. ' with level = ' . $acl_lvl);
		$lvl = check_group_in_acl($acl_id, $group_id);
		$do_next = FALSE;
		if ($lvl == $acl_lvl) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show group in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All groups in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_groups = get_groups_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_groups as $my_group) {
		$t_row = array();
		$t_row[] = $my_group->id;
		$t_row[] = $my_group->name;
		$t_row[] = $my_group->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// add group 2, lvl 4 to acl
	$group_id = $def_group_id['group2'];
	$acl_lvl = 4;
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Add group to ACL');
		$row[] = t(
			'Add group with id = ' . $group_id . ' to ACL with id = ' . $acl_id
					. ' with level = ' . $acl_lvl);
		$do_next = FALSE;
		if (add_group_to_acl($acl_id, $group_id, $acl_lvl)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if group 1 is in acl and if lvl is ok
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Group in ACL');
		$row[] = t(
			'Check if group_id = ' . $group_id . ' is in ACL id = ' . $acl_id
					. ' with level = ' . $acl_lvl);
		$lvl = check_group_in_acl($acl_id, $group_id);
		$do_next = FALSE;
		if ($lvl == $acl_lvl) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show group in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All groups in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_groups = get_groups_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_groups as $my_group) {
		$t_row = array();
		$t_row[] = $my_group->id;
		$t_row[] = $my_group->name;
		$t_row[] = $my_group->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// change group lvl of group 1 in acl
	$group_id = $def_group_id['group1'];
	$acl_lvl = 2;
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Change group level');
		$row[] = t(
			'Change level of group with id = ' . $group_id
					. ' in ACL with id = ' . $acl_id . ' to level = '
					. $acl_lvl);
		$do_next = FALSE;
		if (update_group_in_acl($acl_id, $group_id, $acl_lvl)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if group 1 is in acl and if lvl is ok
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Group in ACL');
		$row[] = t(
			'Check if group_id = ' . $group_id . ' is in ACL id = ' . $acl_id
					. ' with level = ' . $acl_lvl);
		$lvl = check_group_in_acl($acl_id, $group_id);
		$do_next = TRUE;
		if ($lvl == $acl_lvl) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show group in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All groups in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_groups = get_groups_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_groups as $my_group) {
		$t_row = array();
		$t_row[] = $my_group->id;
		$t_row[] = $my_group->name;
		$t_row[] = $my_group->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// check permission of user 2 (all permissions)
	// this user is in group 1
	$user_id = $def_user_id['user2'];
	$acl_lvl = 3;
	// user 2 has lvl 3 -> wins
	// group 1 has lvl 2
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Check Permission All');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' in ACL id = ' . $acl_id
					. ' has permission level = ' . $acl_lvl);
		$do_next = TRUE;
		if (check_permission($acl_id, $acl_lvl, $user_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check permission of user 2 (only group permission)
	// this user is in group 1
	$user_id = $def_user_id['user2'];
	$acl_lvl = 2;
	// user 2 has lvl 3
	// group 1 has lvl 2 -> wins (only group lvls are checked)
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Check Permission Group');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' in ACL id = ' . $acl_id
					. ' has permission level = ' . $acl_lvl
					. ' (only group-permission check)');
		$do_next = TRUE;
		if (check_permission($acl_id, $acl_lvl, $user_id, true)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check permission of user 1 (all permissions)
	// this user is in no group
	$user_id = $def_user_id['user1'];
	$acl_lvl = variable_get("ACL_ADMIN");
	// user 1 has lvl variable_get("ACL_ADMIN")
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Check Permission All');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' (in no group) in ACL id = '
					. $acl_id . ' has permission level = ' . $acl_lvl);
		$do_next = TRUE;
		if (check_permission($acl_id, $acl_lvl, $user_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check permission of user_id 2 (only group permission)
	// this user is in no group
	// this is a negative-test
	$user_id = $def_user_id['user1'];
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Check Permission Group');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' (in no group) in ACL id = '
					. $acl_id
					. ' has no permission (only group-permission check)');
		$do_next = TRUE;
		if (!check_permission($acl_id, $acl_lvl, $user_id, true)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// remove user 3 from acl
	$user_id = $def_user_id['user3'];
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Remove user from ACL');
		$row[] = t(
			'Remove user with id = ' . $user_id . ' from ACL with id = '
					. $acl_id);
		$do_next = FALSE;
		if (delete_user_from_acl($acl_id, $user_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if user 3 is not in acl
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('User in ACL');
		$row[] = t(
			'Check if user_id = ' . $user_id . ' is not in ACL id = ' . $acl_id);
		$do_next = FALSE;
		if (!check_user_in_acl($acl_id, $user_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show user in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All users in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_users = get_users_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_users as $my_user) {
		$t_row = array();
		$t_row[] = $my_user->uid;
		$t_row[] = $my_user->name;
		$t_row[] = $my_user->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// remove group 2 from acl
	$group_id = $def_group_id['group2'];
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Remove group from ACL');
		$row[] = t(
			'Remove group with id = ' . $group_id . ' from ACL with id = '
					. $acl_id);
		$do_next = FALSE;
		if (delete_group_from_acl($acl_id, $group_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if group 2 is not in acl
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Group in ACL');
		$row[] = t(
			'Check if group_id = ' . $group_id . ' is not in ACL id = '
					. $acl_id);
		$do_next = FALSE;
		if (!check_group_in_acl($acl_id, $group_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show group in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All groups in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_groups = get_groups_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_groups as $my_group) {
		$t_row = array();
		$t_row[] = $my_group->id;
		$t_row[] = $my_group->name;
		$t_row[] = $my_group->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// copy acl
	$acl_id_copy = 0;
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Copy ACL');
		$row[] = t('Copy the ACL id = ' . $acl_id);
		$acl_id_copy = copy_acl($acl_id);
		$do_next = FALSE;
		if ($acl_id_copy) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show user in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All users in ACL with id = " . $acl_id_copy
						. " after testcase " . $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_users = get_users_in_acl($acl_id_copy);
	$t_rows = array();
	foreach ($my_users as $my_user) {
		$t_row = array();
		$t_row[] = $my_user->uid;
		$t_row[] = $my_user->name;
		$t_row[] = $my_user->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	/* DISPLAY START */
	// show group in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All groups in ACL with id = " . $acl_id_copy
						. " after testcase " . $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_groups = get_groups_in_acl($acl_id_copy);
	$t_rows = array();
	foreach ($my_groups as $my_group) {
		$t_row = array();
		$t_row[] = $my_group->id;
		$t_row[] = $my_group->name;
		$t_row[] = $my_group->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// clear acl
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Clear ACL');
		$row[] = t('Clear the ACL id = ' . $acl_id);
		$do_next = FALSE;
		if (clear_acl($acl_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	/* DISPLAY START */
	// show user in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All users in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_users = get_users_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_users as $my_user) {
		$t_row = array();
		$t_row[] = $my_user->uid;
		$t_row[] = $my_user->name;
		$t_row[] = $my_user->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	/* DISPLAY START */
	// show group in acl
	$output['info_display' . $displayCount] = array(
			'#markup' => t(
				"All groups in ACL with id = " . $acl_id . " after testcase "
						. $rowCount),
			'#weight' => $displayCount * 10,
	);

	$t_tableHeader = array();
	$t_tableHeader[] = t('Id');
	$t_tableHeader[] = t('Name');
	$t_tableHeader[] = t('Level');

	$my_groups = get_groups_in_acl($acl_id);
	$t_rows = array();
	foreach ($my_groups as $my_group) {
		$t_row = array();
		$t_row[] = $my_group->id;
		$t_row[] = $my_group->name;
		$t_row[] = $my_group->level;
		$t_rows[] = $t_row;
	}

	$t_table = array(
			'#theme' => 'table',
			'#header' => $t_tableHeader,
			'#rows' => $t_rows,
			'#weight' => $displayCount * 10 + 1,
	);
	$output['table_display' . $displayCount] = $t_table;
	$displayCount++;
	/* DISPLAY END */

	// delete acl
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Delete ACL');
		$row[] = t('Delete the ACL id = ' . $acl_id);
		$do_next = FALSE;
		if (delete_acl($acl_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if acl is deleted
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Check ACL deleted');
		$row[] = t('Check if the ACL with id = ' . $acl_id . ' is deleted');
		$do_next = FALSE;
		if (!get_acl($acl_id)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// delete acl
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Delete ACL');
		$row[] = t('Delete the ACL id = ' . $acl_id_copy);
		$do_next = FALSE;
		if (delete_acl($acl_id_copy)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	// check if acl is deleted
	if ($do_next) {
		$rowCount++;
		$row = array();
		$row[] = $rowCount;
		$row[] = t('Check ACL deleted');
		$row[] = t('Check if the ACL with id = ' . $acl_id_copy . ' is deleted');
		$do_next = FALSE;
		if (!get_acl($acl_id_copy)) {
			$row[] = $success;
			$do_next = TRUE;
		} else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}

	$table = array(
			'#theme' => 'table',
			'#header' => $tableHeader,
			'#rows' => $rows,
			'#weight' => 1,
	);
	$output['table_test1'] = $table;

	$theme = array(
			'#theme' => 'c_acl_form',
			'#weight' => -1,
	);

	$output['theme'] = $theme;

	return $output;
}

/**
 * This routine was only used to debug acl module
 * 
 * @return 	Array	drupal form array with acl of specified acl
 */
function show_acl_form() {
	$formDefaults = array(
			array(
					'type' => variable_get("ACL_RED_READ"),
					'name' => t('Reduced Read'),
					'value' => FALSE,
			),
			array(
					'type' => variable_get("ACL_READ"),
					'name' => t('Read'),
					'value' => TRUE,
			),
			array(
					'type' => variable_get("ACL_RED_WRITE"),
					'name' => t('Reduces Write'),
					'value' => FALSE,
			),
			array(
					'type' => variable_get("ACL_WRITE"),
					'name' => t('Write'),
					'value' => FALSE,
			),
			array(
					'type' => variable_get("ACL_ADMIN"),
					'name' => t('Admin'),
					'value' => FALSE,
			),
	);

	$groupDefaults = array(
			(object) array(
					'id' => variable_get("ACL_GROUP_ALL_ID"),
					'name' => "All",
					'level' => variable_get("ACL_READ"),
			),
	);
	$render_array['test'] = array(
			'#theme' => 'c_acl',
			'#defaults' => $formDefaults,
			'#defaults_group' => $groupDefaults,
			'#acl_id' => 21, /*'#child_acl_ids' => array(
							         54,
							         55,
							         56,
							 ),*/
	);

	return $render_array;
}

?>