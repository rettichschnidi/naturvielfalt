<?php

/**
 * Shows the users overview, includes a description of what he can do and his group
 * 
 * @see user/%user/sgroup
 * @return 
 */
function sgroup_overview() {
	$output['introduction'] = array(
			'#markup' => t(
					'Groups are designed to share inventories with other users. You can !link
		and invite others to join you. Currently you are member of following groups:',
					array('!link' => l('create your own group', 'sgroup/new'))),
	);
	$output['groupTable'] = _sgroup_create_group_table();
	return $output;
}

/**
 * Shows the users group in a table
 * 
 * @return Table of groups the user is member of
 */
function _sgroup_create_group_table() {
	global $user;

	// create table header
	$tableHeader[] = t('group name');
	$tableHeader[] = t('my role');
	$tableHeader[] = t('Operations');

	// get all groups ordered by status and name
	$groups = db_query(
			'SELECT 
				g.sgid, 
				sgroup_name,
				admin
			FROM
				sgroup g 
				LEFT JOIN sgroup_users gu ON g.sgid = gu.sgid
			WHERE
				gu.uid = :uid', array(':uid' => $user->uid));

	foreach ($groups as $group) {
		$row = array();
		$row[] = $group->sgroup_name;
		if ($group->admin == 1) {
			// this is an administrator of that group
			$row[] = t('Administrator');
			$row[] = l(t('Edit'), 'user/' . $user->uid . '/sgroup/' . $group->sgid . '/edit');
		} else {
			// this is a normal member
			$row[] = t('Member');
			$row[] = '';
		}
		$rows[] = $row;
	}

	$table = array(
			'#theme' => 'table',
			'#header' => $tableHeader,
			'#rows' => $rows
	);
	return $table;
}
?>