<?php
/**
 * Show invite form of a group
 * 
 * @param	string	$sgroup_id	id of group to show
 * @return	Array				drupal form array
 */
function sgroup_member($sgroup_id) {
	drupal_add_js(drupal_get_path('module', 'c_acl') . '/js/c_acl.js');
	drupal_add_css(drupal_get_path('module', 'c_acl') . '/css/c_acl.css');
	// get group infos
	$db_groups = db_select('sgroup', 'g')->fields('g')
		->condition('g.id', $sgroup_id)
		->execute();
	if ($record = $db_groups->fetch()) {
		// set group name as title
		drupal_set_title(t("Group Members"));
		// display group description
		$output['description'] = array(
				'#markup' => t(
					'Invite users to your group by adding them to the table below with help of the autocompletion form')
						. '. '
						. t(
							'To remove a user from a group use the delete checkbox in the table and click on')
						. ' "' . t('Save') . '".',
		);
		$acl_id = $record->acl_id;
		// get users from acl
		$users = get_users_in_acl($acl_id);
		// get form and draw it
		$output['table_form'] = drupal_get_form(
			'sgroup_member_form',
			$users,
			$acl_id,
			$record);
	}
	return $output;
}

/**
 * Implements hook_form()
 * 
 * @param	obj		$form			drupal object
 * @param	objPtr	&$form_state	drupal object
 * @param	Array	$users			array containing user acl permissions
 * @param	Array	$acl_id			id of acl to add or remove users
 * @param	Object	$group			object containing information about the group
 * 
 * @return	Array					form array to be rendered by drupal
 */
function sgroup_member_form($form, &$form_state, $users, $acl_id, $group) {
	$form_state['storage']['acl_id'] = $acl_id;
	$form_state['storage']['group_id'] = $group->id;
	$form_state['storage']['group_name'] = $group->name;
	// create table header
	$tableHeader = array();
	$tableHeader[] = t('Username');
	$tableHeader[] = t('Membership');
	$tableHeader[] = t('Action');

	$rows = array();

	// user permissions
	foreach ($users as $user) {
		$row = array();
		$owner = check_user_owns_acl($acl_id, $user->uid);
		$name = $user->name;
		if ($owner) {
			$name .= " (" . t("Owner") . ")";
		}
		$row[] = l($name, 'user/' . $user->uid);
		if ($user->level > variable_get("ACL_PERMISSION_NOT_SET")) {
			$row[] = t('active');
			$options = array(
					0 => t('no action'),
					1 => t('remove'),
			);
		} else if ($user->level == variable_get("ACL_PENDING")) {
			$row[] = t('invited');
			$options = array(
					0 => t('no action'),
					3 => t('remove'),
			);
		} else if ($user->level == variable_get("ACL_ASKING")) {
			$row[] = t('requested');
			$options = array(
					0 => t('no action'),
					1 => t('accept'),
					2 => t('decline'),
					3 => t('remove'),
			);
		} else if ($user->level == variable_get("ACL_DECLINED")) {
			$row[] = t('declined');
			$options = array(
					0 => t('no action'),
					1 => t('accept'),
					3 => t('remove'),
			);
		}
		if (!$owner) {
			// no action on owner
			$form[$name . $user->uid] = array(
					'#type' => 'select',
					'#options' => $options,
					'#default_value' => 0,
					'#name' => 'user-action-' . $user->uid
			);
			$row[] = array(
					'data' => $form[$name . $user->uid],
			);
		}
		$rows[] = $row;
	}

	// new user row
	$row = array();
	// add user autocomplete field in table
	$form['invite_user'] = array(
			'#type' => 'textfield',
			'#size' => '20',
			'#attributes' => array(
					'id' => 'invite-users',
			),
			'#value' => "",
			'#name' => "user-add",
			// unfortuantely we cannot use "user/autocomplete"
			// because we dont want to add users already in the list
			'#autocomplete_path' => 'c_acl/' . $acl_id . '/autocomplete_users',
	);
	$row[] = array(
			'data' => $form['invite_user'],
	);
	$row[] = '-';
	$row[] = '-';
	$rows[] = $row;

	// create table
	$output['user_overview'] = array(
			'#theme' => 'table',
			'#header' => $tableHeader,
			'#rows' => $rows,
			'#attributes' => array(
					'id' => 'permission_overview'
			),
	);

	// create submit button
	$output['upload_submit'] = array(
			'#type' => 'submit',
			'#value' => t('Save'),
			'#submit' => array(
					'sgroup_member_form_submit'
			),
	);

	return $output;
}

/**
 * Saves ACL permissions to db
 * 
 * @param	Array	$form			drupal generated form array
 * @param	Array	&$form_state	drupal generated form array
 */
function sgroup_member_form_submit($form, &$form_state) {
	$error = 0;
	$acl_count = 0;
	$acl_id = $form_state['storage']['acl_id'];

	// generate user and group arrays from input values
	$add_users = array();
	$rm_users = array();
	foreach ($form_state['input'] as $id => $val) {
		$elem = explode('-', $id);
		if (isset($elem[2]) && ($elem[0] == 'user')) {
			$user_id = $elem[2];
			if ($elem[1] == 'add') {
				$add_users[$user_id] = variable_get("ACL_PENDING");
			} else if ($elem[1] == 'action') {
				if ($val == '1') {
					$add_users[$user_id] = variable_get("ACL_READ");
				} else if ($val == '2') {
					$add_users[$user_id] = variable_get("ACL_DECLINED");
				} else if ($val == '3') {
					$rm_users[] = $user_id;
				}
			}
		}
	}
	$add_user = 0;
	// add or modify user in acl
	foreach ($add_users as $add_user_id => $level) {
		if (!add_user_to_acl($acl_id, $add_user_id, $level)) {
			drupal_set_message(
				t(
					'Error occured during user invitation change (user_id = "'
							. $user_id . '" / main acl, acl_id = "' . $acl_id
							. '")'),
				'error');
			$error++;
		} else {
			$my_user = user_load($user_id);
			$emailTo = $my_user->mail;
			$params = array(
					'user_id' => $my_user->uid,
					'user_name' => $my_user->name,
					'group_id' => $form_state['storage']['group_id'],
					'group_name' => $form_state['storage']['group_name'],
			);
			drupal_mail(
				'sgroup_invite',
				'invitation',
				$emailTo,
				user_preferred_language($my_user),
				$params);
		}
		$add_user++;
	}

	// remove user from acl
	$rm_user = 0;
	foreach ($rm_users as $rm_user_id) {
		if (!delete_user_from_acl($acl_id, $rm_user_id)) {
			drupal_set_message(
				t(
					'Error occured during remove user from acl (user_id = "'
							. $user_id . '" / main acl, acl_id = "' . $acl_id
							. '")'),
				'error');
			$error++;
		} else {
			$my_user = user_load($user_id);
			$emailTo = $my_user->mail;
			$params = array(
					'user_id' => $my_user->uid,
					'user_name' => $my_user->name,
					'group_id' => $form_state['storage']['group_id'],
					'group_name' => $form_state['storage']['group_name'],
			);
			drupal_mail(
				'sgroup_member',
				'remove',
				$emailTo,
				user_preferred_language($my_user),
				$params);
		}
		$rm_user++;
	}

	if (($add_user > 0) && ($error == 0)) {
		drupal_set_message($add_user . t(' user invited to Group'));
	}
	if (($rm_user > 0) && ($error == 0)) {
		drupal_set_message($rm_user . t(' user removed from Group'));
	}
}

/**
 * email handler to send emai to user if user has been added or removed from group
 * 
 * @param	string	$key		A key to identify the e-mail sent.
 * @param 	array	$message	Drupal array to handle email
 * @param	array	$params		Array with user defined parameters used in email content
 */
function sgroup_member_mail($key, &$message, $params) {
	switch ($key) {
	case 'invitation':
	// send email to user indication that he was invited to a group
		$message['subject'] = t(
			'Invitation to Group' . ' ' . $params['group_name']);
		$message['body'] = array(
				t('Dear') . ' ' . $params['user_name'],
				t('You have been invited to join the group ')
						. l(
							$params['group_name'],
							'sgroup/' . $params['group_id']),
				t('To join the group please visit your') . ' '
						. l(
							t('user profile'),
							'user/' . $params['user_id'] . '/sgroup') . ' '
						. t('and accept the invitation') . '.',
		);
		break;
	case 'remove':
	// send email to user indication that he was removed from group
		$message['subject'] = t(
			'You were removed from Group' . ' ' . $params['group_name']);
		$message['body'] = array(
				t('Dear') . ' ' . $params['user_name'],
				t('You have been removed from the group ')
						. l(
							$params['group_name'],
							'sgroup/' . $params['group_id']),
				t('You can see a list of groups you are member of') . ' '
						. l(t('here'), 'user/' . $params['user_id'] . '/sgroup')
						. '.',
		);
		break;
	}
}

/**
 * Show Accep / Decline query on invitation
 * 
 * @param	string	$sgroup_id	id of group to show query
 * @return	Array				drupal rendered form Array
 */
function sgroup_invite($sgroup_id) {
	global $user;
	// set group name as title
	drupal_set_title(t("Handle Membership"));

	$db_group = db_select('sgroup', 'g')->fields('g')
		->condition('g.id', $sgroup_id)
		->execute();
	if ($record = $db_group->fetch()) {

		// display group description
		$output['description'] = array(
				'#markup' => t(
					'You can accept or decline membership to the group') . ' '
						. $record->name . ' ' . t('by clicking') . ' "'
						. t('Accept') . '" ' . t('or') . ' "' . t('Decline')
						. '" ' . t('below') . '. ' . t('By clicking') . ' "'
						. t('Remove') . '", '
						. t(
							'you will decline membership and the group will be removed from your')
						. ' "' . t('Pending Membership') . '" ' . t('list')
						. '. ' . t('Click') . ' '
						. l(t('here'), 'user/' . $user->uid . '/sgroup') . ' '
						. t('to abort.'),
		);

		// display collapsible box with all group members
		$output['member'] = array(
				'#type' => 'fieldset',
				//'#title' => t('Accept Membership'),
				'#weight' => 1,
				'#collapsible' => FALSE,
		);
		$item = array(
				'accept' => TRUE,
				'decline' => TRUE,
				'remove' => TRUE,
		);
		$output['member']['form'] = drupal_get_form(
			'sgroup_invite_form',
			$record,
			$item);

	}
	return $output;
}

/**
 * Form to display invite query buttons
 * 
 * @param	obj		$form			drupal object
 * @param	objPtr	&$form_state	drupal object
 * @param	Object	$group			db object with all field of group to act query on
 * @param	Array	$item			selector array to indicate which button to display
 * 
 * @return	Array					form array to be rendered by drupal
 */
function sgroup_invite_form($form, &$form_state, $group, $item = NULL) {
	if ($item == NULL) {
		$item = array(
				'accept' => TRUE,
				'decline' => TRUE,
				'remove' => TRUE,
				'request' => TRUE,
		);
	}
	$form_state['storage']['group_id'] = $group->id;
	$form_state['storage']['group_name'] = $group->name;
	$form_state['storage']['group_owner'] = $group->users_id;
	$form_state['storage']['acl_id'] = $group->acl_id;
	if (isset($item['request']) && $item['request']) {
		$form['request'] = array(
				'#type' => 'submit',
				'#value' => t('Request Membership'),
				'#submit' => array(
						'sgroup_invite_form_request_submit'
				)
		);
	}
	if (isset($item['accept']) && $item['accept']) {
		$form['accept'] = array(
				'#type' => 'submit',
				'#value' => t('Accept'),
				'#submit' => array(
						'sgroup_invite_form_accept_submit'
				)
		);
	}
	if (isset($item['decline']) && $item['decline']) {
		$form['decline'] = array(
				'#type' => 'submit',
				'#value' => t('Decline'),
				'#submit' => array(
						'sgroup_invite_form_decline_submit'
				)
		);
	}
	if (isset($item['remove']) && $item['remove']) {
		$form['remove'] = array(
				'#type' => 'submit',
				'#value' => t('Remove'),
				'#submit' => array(
						'sgroup_invite_form_remove_submit'
				)
		);
	}
	return $form;
}

/**
 * Save permissions on accept group invitation
 * 
 * @param	obj		$form			drupal object
 * @param	objPtr	&$form_state	drupal object
 * 
 * @return	Array					drupal form array to redirect page to sgroup member list
 */
function sgroup_invite_form_accept_submit($form, &$form_state) {
	global $user;
	$sgroup_name = $form_state['storage']['group_name'];
	$acl_id = $form_state['storage']['acl_id'];
	$error = TRUE;
	if (add_user_to_acl($acl_id, $user->uid, variable_get("ACL_READ"))) {
		drupal_set_message(
			t('You were added as member to the group') . ' ' . $sgroup_name);
		$error = FALSE;
	}
	if ($error) {
		drupal_set_message(
			'You cannot be added to the group, it does not exist.',
			'error');
	}
	return $form_state['redirect'] = 'user/' . $user->uid . '/sgroup';
}

/**
 * Save permissions on decline group invitation
 * 
 * @param	obj		$form			drupal object
 * @param	objPtr	&$form_state	drupal object
 * 
 * @return	Array					drupal form array to redirect page to sgroup member list
 */
function sgroup_invite_form_decline_submit($form, &$form_state) {
	global $user;
	$sgroup_name = $form_state['storage']['group_name'];
	$acl_id = $form_state['storage']['acl_id'];
	$error = TRUE;
	if (add_user_to_acl($acl_id, $user->uid, variable_get("ACL_DECLINED"))) {
		drupal_set_message(
			t('You declined membership to the group') . ' ' . $sgroup_name);
		$error = FALSE;
	}
	if ($error) {
		drupal_set_message(
			'This group is inconsistent, please contact the administartor.',
			'error');
	}
	return $form_state['redirect'] = 'user/' . $user->uid . '/sgroup';
}

/**
 * Save permissions on remove group invitation
 * 
 * @param	obj		$form			drupal object
 * @param	objPtr	&$form_state	drupal object
 * 
 * @return	Array					drupal form array to redirect page to sgroup member list
 */
function sgroup_invite_form_remove_submit($form, &$form_state) {
	global $user;
	$sgroup_name = $form_state['storage']['group_name'];
	$acl_id = $form_state['storage']['acl_id'];
	$error = TRUE;
	if (delete_user_from_acl($acl_id, $user->uid)) {
		drupal_set_message(
			t('You declined membership to the group') . ' ' . $sgroup_name
					. ' ' . t('and were removed.'));
		$error = FALSE;
	}
	if ($error) {
		drupal_set_message(
			'This group is inconsistent, please contact the administartor.',
			'error');
	}
	return $form_state['redirect'] = 'user/' . $user->uid . '/sgroup';
}

/**
 * Save permissions and send mail on request group membership
 * 
 * @param	obj		$form			drupal object
 * @param	objPtr	&$form_state	drupal object
 * 
 * @return	Array					drupal form array to redirect page to sgroup member list
 */
function sgroup_invite_form_request_submit($form, &$form_state) {
	global $user;
	$sgroup_name = $form_state['storage']['group_name'];
	$acl_id = $form_state['storage']['acl_id'];
	$error = TRUE;
	if (add_user_to_acl($acl_id, $user->uid, variable_get("ACL_ASKING"))) {
		drupal_set_message(
			t('You required membership to the group') . ' ' . $sgroup_name);
		$my_user = user_load($form_state['storage']['group_owner']);
		$emailTo = $my_user->mail;
		$params = array(
				'user_id' => $user->uid,
				'user_name' => $user->name,
				'group_id' => $form_state['storage']['group_id'],
				'group_name' => $form_state['storage']['group_name'],
				'group_owner' => $form_state['storage']['group_owner'],
				'group_owner_name' => $my_user->name,
		);
		drupal_mail(
			'sgroup_member_request',
			'request',
			$emailTo,
			user_preferred_language($user),
			$params);
		$error = FALSE;
	}
	if ($error) {
		drupal_set_message(
			'You cannot be added to the group, it does not exist.',
			'error');
	}
	return $form_state['redirect'] = 'user/' . $user->uid . '/sgroup';
}

/**
 * Show Request / Remove Query on request
 * 
 * @param	string	$sgroup_id	id of group to show query
 * @return	Array				drupal rendered form Array
 */
function sgroup_request($sgroup_id) {
	global $user;
	// set group name as title
	drupal_set_title(t("Handle Membership"));

	$db_group = db_select('sgroup', 'g')->fields('g')
		->condition('g.id', $sgroup_id)
		->execute();
	if ($record = $db_group->fetch()) {

		// display group description
		$output['description'] = array(
				'#markup' => t('You can request membership to the group') . ' '
						. $record->name . ' ' . t('by clicking') . ' "'
						. t('Request Membership') . '" '
						. t(
							'or remove the group from your "Pending Membership" list by clicking')
						. ' "' . t('Remove') . '" ' . t('below') . '. '
						. t('Click') . ' '
						. l(t('here'), 'user/' . $user->uid . '/sgroup') . ' '
						. t('to abort.'),
		);

		// display box buttons
		$output['member'] = array(
				'#type' => 'fieldset',
				//'#title' => t('Accept Membership'),
				'#weight' => 1,
				'#collapsible' => FALSE,
		);
		$item = array(
				'request' => TRUE,
				'remove' => TRUE,
		);
		$output['member']['form'] = drupal_get_form(
			'sgroup_invite_form',
			$record,
			$item);

	}
	return $output;
}

/**
 * email handler to send email to group owner if a user has requestet group membership
 * 
 * @param	string	$key		A key to identify the e-mail sent.
 * @param 	array	$message	Drupal array to handle email
 * @param	array	$params		Array with user defined parameters used in email content
 */
function sgroup_member_request_mail($key, &$message, $params) {
	switch ($key) {
	case 'request':
	// send email to group owner to indicate that a user requestet membership
		$message['subject'] = $params['user_name'] . ' '
				. t('is requesting membership to Group') . ' '
				. $params['group_name'];
		$message['body'] = array(
				t('Dear') . ' ' . $params['group_owner_name'],
				t('The user') . ' ' . $params['user_name'] . ' '
						. t('requests membership to your group') . ' '
						. l(
							$params['group_name'],
							'sgroup/' . $params['group_id']),
				t('To grant or decline hid membership to the group go to the')
						. ' '
						. l(
							t('grpoup member management'),
							'sgroup/' . $params['group_id'] . '/member') . ' '
						. t('and change the user status') . '.',
		);
		break;
	}
}
?>