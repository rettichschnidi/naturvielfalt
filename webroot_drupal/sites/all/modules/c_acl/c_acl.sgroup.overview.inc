<?php

/**
 * Shows a list of groups the user owns or is member of
 * 
 * @param	string	$user_id	id of user to show own and member groups
 * @return	array				drupal form array with group list
 */
function sgroup_overview($user_id) {
	global $user;
	// set group list title
	drupal_set_title(check_plain($user->name).t("'s Groups"));
	// display short instruction text
	$output['introduction'] = array(
			'#markup' => t(
				'Groups are designed to share inventories with other users. You can !link
		and invite others to join you. Currently you are member of following groups:',
				array('!link' => l(
							'create your own group',
							'user/' . $user->uid . '/sgroup/new')
				)),
	);
	// get table of groups user owns
	$output['ownGroupTable'] = _sgroup_own_group_table($user_id);
	// get table of groups user is member of
	$output['memberGroupTable'] = _sgroup_member_group_table($user_id);
	return $output;
}

/**
 * Returns drupal array of groups the user owns as a table
 * 
 * @param	string	$user_id	id of user to show own and member groups
 * @return 	Array				drupal form array with table of groups the user is member of
 */
function _sgroup_own_group_table($user_id) {
	// create table header
	$tableHeader[] = t('Own Groups');
	$tableHeader[] = array(
			'data' => t('Operations'),
			'colspan' => 2,
	);
	$rows = array();

	// get all groups user is owner
	$db_groups = db_select('sgroup', 'g')->fields(
			'g',
			array('id',
					'name',
					'description',
			))
		->condition('g.users_id', $user_id)
		->execute();

	// create table
	$row_cnt = 0;
	while ($record = $db_groups->fetch()) {
		$row = array();
		$description = $record->description;
		$teaser_len = 80;
		if (strlen($description) > $teaser_len) {
			// if description is too long, display only a teaser
			$teaser_len = stripos($description, ' ', $teaser_len);
			$description = substr($description, 0, $teaser_len + 1);
			$description .= '(...)';
		}
		$row[] = array(
				'data' => l(
					$record->name,
					'sgroup/' . $record->id)
						. '<div class="description">' . $description . '</div>',
		);
		$row[] = array(
				'data' => l(
					t('Edit Group'),
					'sgroup/' . $record->id . '/edit')
		);
		$row[] = array(
				'data' => l(
					t('Permissions'),
					'sgroup/' . $record->id . '/acl')
		);
		$rows[] = $row;
		$row_cnt++;
	}

	if ($row_cnt == 0) {
		// table is empty
		$rows[] = array(
				array(
						'data' => t('No group created yet'),
						'colspan' => count($tableHeader),
				)
		);
	}

	$table = array(
			'#theme' => 'table',
			'#header' => $tableHeader,
			'#rows' => $rows
	);

	return $table;
}

/**
 * Shows the group the user is memer of in a table
 * 
 * @return Table of groups the user is member of
 */
function _sgroup_member_group_table($user_id) {
	// create table header
	$tableHeader[] = t('Member Groups');
	$tableHeader[] = t('Owner');
	$tableHeader[] = array(
			'data' => t('Operations'),
			'colspan' => 2,
	);
	$rows = array();
		
	// get all groups, user is member
	$query = db_select('sgroup', 'g')->fields(
			'g',
			array('id',
					'name',
					'description',
					'users_id',
					'acl_id',
			))
		->condition('g.users_id', $user_id, "!=");
	$query->join('acl_users', 'au', 'au.acl_id = g.acl_id');
	$query->condition('au.users_id', $user_id);
	$query->join('users', 'u', 'u.uid = g.users_id');
	$db_groups = $query->fields('u', array('name',))->execute();

	// create table
	$row_cnt = 0;
	while ($record = $db_groups->fetch()) {
		$row = array();
		$description = $record->description;
		$teaser_len = 80;
		if (strlen($description) > $teaser_len) {
			// if description is too long, display only a teaser
			$teaser_len = stripos($description, ' ', $teaser_len);
			$description = substr($description, 0, $teaser_len + 1);
			$description .= '(...)';
		}
		$row[] = array(
				'data' => l(
					$record->name,
					'sgroup/' . $record->id)
						. '<div class="description">' . $description . '</div>',
		);
		$row[] = array(
				'data' => $record->u_name,
		);
		if (check_permission($record->acl_id, variable_get("ACL_WRITE"))) {
			// write permission on group ok
			$row[] = array(
					'data' => l(
						t('Edit Group'),
						'sgroup/' . $record->id . '/edit')
			);
		}
		if (check_permission($record->acl_id, variable_get("ACL_ADMIN"))) {
			// admin permission on group ok
			$row[] = array(
					'data' => l(
						t('Permissions'),
						'sgroup/' . $record->id . '/acl')
			);
		}
		$rows[] = $row;
		$row_cnt++;
	}

	if ($row_cnt == 0) {
		// table is empty
		$rows[] = array(
				array(
						'data' => t('You are member of no group yet'),
						'colspan' => count($tableHeader),
				)
		);
	}

	$table = array(
			'#theme' => 'table',
			'#header' => $tableHeader,
			'#rows' => $rows
	);

	return $table;
}
?>