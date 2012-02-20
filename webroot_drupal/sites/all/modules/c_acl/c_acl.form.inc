<?php
// use base acl functions in hooks
module_load_include('inc', 'c_acl', 'c_acl.base');

/**
 * Callback function of c_acl theme
 * 
 * @param	Array	$vars	theme parameters are passed by this parameter. See (*1) ad (*2)
 */
function c_acl_show_form($vars) {
	// (*1) Array with permission types and its default value 
	$defaults = $vars['defaults'];
	// (*2) acl id of element to modify acl permissions
	$acl_id = $vars['acl_id'];
	if (check_permission($acl_id, variable_get("ACL_ADMIN"))) {
		// get groups from acl
		$groups = get_groups_in_acl($acl_id);
		// get users from acl
		$users = get_users_in_acl($acl_id);
		// get form and draw it
		$form = drupal_get_form('c_acl_form', $defaults, $groups, $users,
			$acl_id);
	}
	else {
		$form['introduction'] = array(
			'#markup' => t("Permission denied"),
			'#weight' => 0,
		);
	}
	print drupal_render($form);
}

/**
 * Implements hook_form()
 * 
 * @param	obj		$form			drupal object
 * @param	objPtr	&$form_state	drupal object
 * @param	Array	$defaults		array containing permission defaults
 * @param	Array	$groups			array containing group acl permissions
 * @param	Array	$users			array containing user acl permissions
 * 
 * @return	Array					form array to be rendered by drupal
 */
function c_acl_form($form, &$form_state, $defaults, $groups, $users, $acl_id) {
	$form_state['storage']['acl_id'] = $acl_id;
	$tableHeader[] = '&nbsp;';
	foreach ($defaults as $default) {
		$tableHeader[] = $default['name'];
	}
	$rows = array();
	$row = array(
		array(
			'data' => t('Group Permissions'),
			'colspan' => count($tableHeader),
			'header' => true,
		),
	);
	$rows[] = $row;
	foreach ($groups as $group) {
		$row = array();
		$row[] = $group->name;
		foreach ($defaults as $default) {
			if ($group->level >= $default['type']) {
				$default['value'] = TRUE;
			}
			$name = "group." . $group->id . "." . $default['type'];
			$form[$name] = array(
				'#type' => 'checkbox',
				'#value' => $default['value'],
				'#checked' => $default['value'],
				'#name' => $name,
			);
			$row[] = array(
				'data' => drupal_render($form[$name])
			);
		}
		$rows[] = $row;
	}

	$row = array(
		array(
			'data' => t('User Permissions'),
			'colspan' => count($tableHeader),
			'header' => true,
		),
	);
	$rows[] = $row;
	foreach ($users as $user) {
		$row = array();
		$row[] = $user->name;
		foreach ($defaults as $default) {
			if ($user->level >= $default['type']) {
				$default['value'] = TRUE;
			}
			$name = "user." . $user->uid . "." . $default['type'];
			$form[$name] = array(
				'#type' => 'checkbox',
				'#value' => $default['value'],
				'#checked' => $default['value'],
				'#name' => $name,
			);
			$row[] = array(
				'data' => drupal_render($form[$name])
			);
		}
		$rows[] = $row;
	}
	$output['right_overview'] = array(
		'#theme' => 'table',
		'#header' => $tableHeader,
		'#rows' => $rows,
		'#attributes' => array(
			'id' => 'right_overview_other'
		),
	);

	$output['upload_submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
		'#submit' => array(
			'c_acl_form_submit'
		),
	);

	return $output;
}

/**
 * 
 * Enter description here ...
 * @param unknown_type $form
 * @param unknown_type $form_state
 */
function c_acl_form_submit($form, &$form_state) {
	drupal_set_message(t('Permissions Saved'));
	$acl_id = $form_state['storage']['acl_id'];
	$groups = array();
	$users = array();
	foreach ($form_state['input'] as $id => $val) {
		$elem = explode('_', $id);
		if ($elem[0] == "group") {
			if (($val == '1')
					&& (!isset($groups[$elem[1]])
							|| ($elem[2] > $groups[$elem[1]]))) {
				// set highest permission lvl
				$groups[$elem[1]] = $elem[2];
			}
		}
		else if ($elem[0] == "user") {
			if (($val == '1')
					&& (!isset($users[$elem[1]])
							|| ($elem[2] > $users[$elem[1]]))) {
				// set highest permission lvl
				$users[$elem[1]] = $elem[2];
			}
		}
	}
	// add or modify group in acl
	foreach ($groups as $group_id => $level) {
		add_group_to_acl($acl_id, $group_id, $level);
	}
	// add or modify user in acl
	foreach ($users as $user_id => $level) {
		add_user_to_acl($acl_id, $user_id, $level);
	}
}

?>