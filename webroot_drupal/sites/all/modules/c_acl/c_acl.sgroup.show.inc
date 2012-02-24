<?php
/**
 * Shows details of a group
 * 
 * @param	string	$sgroup_id	id of group to show
 * @return	Array				drupal form array
 */
function sgroup_show($sgroup_id) {
	global $user;
	// get group infos
	$db_groups = db_select('sgroup', 'g')->fields('g')
		->condition('g.id', $sgroup_id)
		->execute();
	if ($record = $db_groups->fetch()) {
		// set group name as title
		drupal_set_title(check_plain($record->name));
		// display group description
		$output['description'] = array(
				'#markup' => $record->description,
		);
		// display collapsible box with all group members
		$output['member'] = array(
				'#type' => 'fieldset',
				'#title' => t('Group Members'),
				'#weight' => 1,
				'#collapsible' => TRUE,
				'#collapsed' => FALSE,
				'#attributes' => array(
						'class' => array(
								'collapsible'
						),
				),
		);
		// get all members of group
		$acl_users = get_users_in_acl($record->acl_id);
		$users = array();
		// generate array with user info
		foreach ($acl_users as $acl_user) {
			$name = $acl_user->name;
			if ($acl_user->uid == $record->users_id) {
				$name .= " (" . t('Owner') . ")";
			}
			if ($acl_user->uid == $user->uid) {
				$name = l($name, 'user/' . $acl_user->uid);
			}
			$users[] = $name;
		}
		// display group members in collapsible box mentioned above  
		$output['member']['list'] = array(
				'list' => array(
						'#theme' => 'item_list',
						'#type' => 'ul',
						'#attributes' => array(),
						'#items' => $users,
				),
		);
	} else {
		// could not find db entry
		$output['details'] = array(
				'#markup' => t('No details avaliable'),
		);
	}

	return $output;
}

?>