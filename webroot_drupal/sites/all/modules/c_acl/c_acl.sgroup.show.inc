<?php
// use base acl functions in hooks
module_load_include('inc', 'c_acl', 'c_acl.sgroup.invite');
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
		$acl_users = get_users_in_acl($record->acl_id, variable_get("ACL_PERMISSION_NOT_SET"));
		$users = array();
		// generate array with user info
		$user_is_member = FALSE;
		$user_is_owner = FALSE;
		foreach ($acl_users as $acl_user) {
			$name = $acl_user->name;
			if ($acl_user->uid == $record->users_id) {
				$name .= " (" . t('Owner') . ")";
			}
			$name = l($name, 'user/' . $acl_user->uid);
			if ($acl_user->uid == $user->uid) {
				$user_is_member = TRUE;
				if ($user->uid == $record->users_id) {
					$user_is_owner = TRUE;
				}
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
		// check if active user is member of group but not owner
		if ($user_is_member && !$user_is_owner) {
			// display box with buttons
			$output['box1'] = array(
					'#type' => 'fieldset',
					'#weight' => 20,
					'#collapsible' => FALSE,
			);
			// provide possibility to leave group
			$output['quit_group'] = array(
					'#markup' => t(
						'If you wish to quit membership of this group, click the button "Leave Group" below.'),
					'#weight' => 10,
			);
			//$form_state['storage']['acl_id'] = $record->acl_id;
			$output['box1']['form'] = drupal_get_form(
				'sgroup_quit_form',
				$record->acl_id);
			$output['form']['#weight'] = 11;
		}
		// check if active user neither member nor owner of group
		if (!$user_is_member && !$user_is_owner) {
			// display box with buttons
			$output['box2'] = array(
					'#type' => 'fieldset',
					'#weight' => 20,
					'#collapsible' => FALSE,
			);
			// provide possibility to leave group
			$output['join_group'] = array(
					'#markup' => t(
						'If you wish to join this group, click the button')
							. ' "' . t("Request Membership") . '" '
							. t('below') . '.',
					'#weight' => 10,
			);
			$item = array(
					'request' => TRUE,
			);
			$output['box2']['form'] = drupal_get_form(
				// this form can be found in the file c_acl.sgroup.invite.inc
				'sgroup_invite_form',
				$record,
				$item);
			$output['form']['#weight'] = 11;
		}
	} else {
		// could not find db entry
		$output['details'] = array(
				'#markup' => t('No details avaliable'),
		);
	}

	return $output;
}

/**
 * Creates the quit group form
 * 
 * @param	Array	$form			drupal generated form array
 * @param	Array	&$form_state	drupal generated form array
 * @param	Sting	$acl_id			id of acl user wants to leave
 * @return form array
 */
function sgroup_quit_form($form, &$form_state, $acl_id) {
	$form_state['storage']['acl_id'] = $acl_id;
	$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t("Leave Group"),
			'#submit' => array(
					'sgroup_quit_form_submit'
			),
	);
	return $form;
}

/**
 * Saves a new group
 * 
 * @param	Array	$form			drupal generated form array
 * @param	Array	&$form_state	drupal generated form array
 * @return	Array					drupal generated form array
 */
function sgroup_quit_form_submit($form, &$form_state) {
	global $user;
	delete_user_from_acl($form_state['storage']['acl_id'], $user->uid);

	return $form_state['redirect'] = 'user/' . $user->uid . '/sgroup';
}

?>