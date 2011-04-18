<?php
function sgroup_overview() {
	global $user;

	// create table
	$tableHeader[] = t('group name');
	$tableHeader[] = t('my status');
	$tableHeader[] = t('actions');

	// get all groups ordered by status and name
	$groups = db_query('SELECT g.sgid, sgroup_name, admin
		FROM sgroup g LEFT JOIN sgroup_users gu ON g.sgid = gu.sgid
		WHERE gu.uid = :uid', array(':uid' => $user->uid));

	foreach ($groups as $group) {
		$row[] = $group->sgroup_name;
		if ($group->admin == 1) {
			// this is an administrator of that group
			$row[] = t('Administrator');
			$row[] = l('Edit', 'sgroup/' . $group->sgid . '/edit');
		} else {
			$row[] = t('Member');
			$row[] = '';
		}
		$rows[] = $row;
	}

	$render_array['mySGroups'] = array(
    '#theme' => 'table', 
   	'#header' => $tableHeader,
	'#rows' => $rows,
	);
	return $render_array;
}
?>