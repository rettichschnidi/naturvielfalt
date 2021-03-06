<?php

/**
 * Callback function of c_acl theme
 * 
 * @param	Array	$vars	theme parameters are passed by this parameter. See (*x)
 * @return	Array			rendered ACL form
 */
function c_acl_show_form($vars) {
	drupal_add_js(drupal_get_path('module', 'c_acl') . '/js/c_acl.js');
	drupal_add_css(drupal_get_path('module', 'c_acl') . '/css/c_acl.css');
	// (*1) Array with permission types and their default permission values (default = NULL)
	$defaults = $vars['defaults'];
	// (*2) Array with groups int their default permission values (default = NULL)
	$defaults_group = $vars['defaults_group'];
	// (*3) acl id of element to modify acl permissions (default = 0)
	$acl_id = $vars['acl_id'];
	// (*4) Array with child acl ids of acl_id (default = NULL)
	$child_acl_ids = $vars['child_acl_ids'];
	// (*5) Boolean indicating if only users shall be displayed (default = FALSE)
	$add_element = $vars['add_element'];

	// get groups from acl
	$groups = NULL;
	$groups = get_groups_in_acl($acl_id);
	// merge groups and defaults_group (if same key exists, overwrite value of defaults_group)
	// search for the same id in groups and defaults_group
	if (isset($defaults_group)) {
		foreach ($groups as $group) {
			$rm_cnt = 0;
			foreach ($defaults_group as $default_group) {
				if ($group->id == $default_group->id) {
					// already a value set in acl for this default group
					$group->default = TRUE;
					if ($default_group->level == variable_get("ACL_RESTRICT")) {
						// group is restricted and should not be in group table
						// -> restrict it here and delete group from acl
						$group->level = $default_group->level;
						delete_group_from_acl($acl_id, $group->id);
					}
					// remove matching group from defaults_group
					array_splice($defaults_group, $rm_cnt, 1);
					break;
				}
				$rm_cnt++;
			}
		}
		// prepend defaults_group to groups
		foreach ($defaults_group as $default_group) {
			$default_group->default = TRUE;
			add_group_to_acl($acl_id, $default_group->id, $default_group->level);
			array_unshift($groups, $default_group);
		}
	}
	// get users from acl
	$users = get_users_in_acl($acl_id);
	// get form and draw it
	$form = drupal_get_form(
		'c_acl_form',
		$defaults,
		$groups,
		$users,
		$acl_id,
		$child_acl_ids,
		$add_element);
	return drupal_render($form);
}

/**
 * Implements hook_form()
 * 
 * @param	obj		$form			drupal object
 * @param	objPtr	&$form_state	drupal object
 * @param	Array	$defaults		array containing permission defaults
 * @param	Array	$groups			array containing group acl permissions
 * @param	Array	$users			array containing user acl permissions
 * @param	string	$acl_id			id of acl_element the permissions will be set
 * @param	Array	$child_acl_ids	array with child acl ids of acl_id
 * @param	boolean	$add_element	indication if elements can bee added to acl
 * 
 * @return	Array					form array to be rendered by drupal
 */
function c_acl_form($form, &$form_state, $defaults, $groups, $users, $acl_id,
		$child_acl_ids, $add_element) {
	$form_state['storage']['acl_id'] = $acl_id;
	$form_state['storage']['child_acl_ids'] = $child_acl_ids;
	// create table header
	$tableHeader[] = '&nbsp;';
	foreach ($defaults as $default) {
		$tableHeader[] = $default['name'];
	}
	if ($add_element) {
		$tableHeader[] = t('Remove');
	}
	$rows = array();

	// check if group table shall be displayed
	if ($groups !== NULL) {
		// group title in table
		$row = array(
				array(
						'data' => t('Group Permissions'),
						'colspan' => count($tableHeader),
						'header' => true,
				),
		);
		$rows[] = $row;
		// group permissions
		foreach ($groups as $group) {
			if (($group->level >= variable_get("ACL_PERMISSION_NOT_SET")) /*	&& (isset($group->default)
																		              || ($group->level
																		                      > variable_get("ACL_PERMISSION_NOT_SET")))*/) {
				$row = array();
				$name = $group->name;
				if(isset($group->default)) {
					$name = t($group->name);
				}
				
				$row[] = array(
						'data' => l($name, 'sgroup/' . $group->id),
						'style' => 'min-width:80px',
				);

				foreach ($defaults as $default) {
					if ($group->level == $default['type']) {
						// permission level of selected group is equal
						// to actual permission level of iteration element to draw
						$default['value'] = TRUE;
					} else {
						$default['value'] = FALSE;
					}
					// render form into table
					$name = "group-" . $group->id;
					$form[$name] = array(
							'#type' => 'radio',
							'#value' => $default['value'] ? $default['type']
									: NULL,
							'#return_value' => $default['type'],
							'#name' => $name,
					);
					$row[] = array(
							'data' => $form[$name],
					);
				}
				if ($add_element && !isset($group->default)) {
					$row[] = array(
							'data' => '<div id="delete-group-' . $group->id
									. '"></div>',
							'class' => 'delete'
					);
				}
				$rows[] = $row;
			}
		}

		if ($add_element) {
			// new group row
			$row = array();
			// add group autocomplete field in table
			$form['add_group'] = array(
					'#type' => 'textfield',
					'#size' => '20',
					'#attributes' => array(
							'id' => 'add-sgroup',
					),
					'#autocomplete_path' => 'c_acl/' . $acl_id
							. '/autocomplete_sgroup',
			);
			$row[] = array(
					'data' => $form['add_group'],
			);
			// add default radio buttons in table
			foreach ($defaults as $default) {
				$name = "group-new";
				$form[$name] = array(
						'#type' => 'radio',
						'#value' => $default['value'] ? $default['type'] : NULL,
						'#return_value' => $default['type'],
						'#name' => $name,
				);
				$row[] = array(
						'data' => $form[$name],
				);
			}
			$row[] = array(
					'data' => '-',
			);
			$rows[] = $row;
		}

	}

	// user title in table
	$row = array(
			array(
					'data' => t('User Permissions'),
					'colspan' => count($tableHeader),
					'header' => true,
			),
	);
	$rows[] = $row;
	// user permissions
	foreach ($users as $user) {
		if ($user->level >= variable_get("ACL_PERMISSION_NOT_SET")) {
			$row = array();
			$owner = check_user_owns_acl($acl_id, $user->uid);
			$name = $user->name;
			$row[] = l($name, 'user/' . $user->uid . '/contact');

			foreach ($defaults as $default) {
				if ($user->level == $default['type']) {
					// permission level of selected group is superior or equal
					// to actual permission level of iteration element to draw
					$default['value'] = TRUE;
				} else {
					$default['value'] = FALSE;
				}
				// render form into table
				$name = "user-" . $user->uid;
				$form[$name] = array(
						'#type' => 'radio',
						'#value' => $default['value'] ? $default['type'] : NULL,
						'#return_value' => $default['type'],
						'#name' => $name,
				);
				$row[] = array(
						'data' => $form[$name],
				);
			}
			if ($add_element) {
				if (!$owner) {
					// only allow to delete non-owner
					$row[] = array(
							'data' => '<div id="delete-user-' . $user->uid
									. '"></div>',
							'class' => 'delete',
					);
				} else {
					$row[] = array(
							'data' => '<div class="description">' . t("Owner")
									. "<div>",
					);
				}
			}
			$rows[] = $row;
		}
	}

	if ($add_element) {
		// new user row
		$row = array();
		// add user autocomplete field in table
		$form['add_user'] = array(
				'#type' => 'textfield',
				'#size' => '20',
				'#attributes' => array(
						'id' => 'add-users',
				),
				'#value' => "",
				'#name' => "users",
				// unfortuantely we cannot use "user/autocomplete"
				// because we dont want to add users already in the list
				'#autocomplete_path' => 'c_acl/' . $acl_id
						. '/autocomplete_users',
		);
		$row[] = array(
				'data' => $form['add_user'],
		);
		// add default radio buttons in table
		foreach ($defaults as $default) {
			$name = "user-new";
			$form[$name] = array(
					'#type' => 'radio',
					'#value' => $default['value'] ? $default['type'] : NULL,
					'#return_value' => $default['type'],
					'#name' => $name,
			);
			$row[] = array(
					'data' => $form[$name],
			);
		}
		$row[] = array(
				'data' => '-',
		);
		$rows[] = $row;
	}

	// create propagate checkbox if child_acl_ids are defined
	if (isset($child_acl_ids)) {
		$output['propagate'] = array(
				'#type' => 'checkbox',
				'#title' => t('Propagate permission changes to child elements'),
				'#value' => 1,
				'#checked' => TRUE,
		);
	}
	// create table
	$output['permission_overview'] = array(
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
					'c_acl_form_submit'),
			'#attributes' => array(
					'id' => 'acl_submit')
	);

	return $output;
}

/**
 * Saves ACL permissions to db
 * 
 * @param	Array	$form			drupal generated form array
 * @param	Array	&$form_state	drupal generated form array
 */
function c_acl_form_submit($form, &$form_state) {
	$error = 0;
	$acl_count = 0;
	$acl_id = $form_state['storage']['acl_id'];

	// generate user and group arrays from input values
	$propagate = FALSE;
	$groups = array();
	$users = array();
	foreach ($form_state['input'] as $id => $val) {
		$elem = explode('-', $id);
		if (isset($elem[1]) && ($elem[1] != 'new')) {
			// ignore the emty row
			if ($elem[0] == "group") {
				$groups[$elem[1]] = $val;
			} else if ($elem[0] == "user") {
				$users[$elem[1]] = $val;
			} else if ($id == 'propagate') {
				// do a propagation of permissions to child elements
				$propagate = TRUE;
			}
		}
	}

	// get old users and groups from acl
	$users_old = get_users_in_acl($acl_id);
	$groups_old = get_groups_in_acl($acl_id);
	// remove all users and groups from acl
	clear_acl($acl_id);

	// save permissions of main acl
	// add or modify group in acl
	foreach ($groups as $group_id => $level) {
		if (!add_group_to_acl($acl_id, $group_id, $level)) {
			drupal_set_message(
				t(
					'Error occured during group permission change (group_id = "@group_id" / main acl, acl_id = "@acl_id")',
					array('@group_id' => $group_id,
							'@acl_id' => $acl_id,
					)),
				'error');
			$error++;
		}
		$acl_count++;
	}
	// save old not activated groups
	foreach ($groups_old as $group_old) {
		if ($group_old->level < variable_get("ACL_PERMISSION_NOT_SET")) {
			//add groups not displayed in acl form but must still be preserverd
			if (!add_group_to_acl($acl_id, $group_old->id, $group_old->level)) {
				drupal_set_message(
					t(
						'Error occured during group permission change (group_id = "@group_id" / main acl, acl_id = "@acl_id")',
						array('@group_id' => $group_id,
								'@acl_id' => $acl_id,
						)),
					'error');
				$error++;
			}
		}
		$acl_count++;
	}
	// add or modify user in acl
	foreach ($users as $user_id => $level) {
		if (!add_user_to_acl($acl_id, $user_id, $level)) {
			drupal_set_message(
				t(
					'Error occured during user permission change (user_id = "@user_id" / main acl, acl_id = "@acl_id")',
					array('@user_id' => $user_id,
							'@acl_id' => $acl_id,
					)),
				'error');
			$error++;
		}
		$acl_count++;
	}
	// save old not activated users
	foreach ($users_old as $user_old) {
		if ($user_old->level < variable_get("ACL_PERMISSION_NOT_SET")) {
			// add users not displayed in acl form but must still be preserverd
			if (!add_user_to_acl($acl_id, $user_old->uid, $user_old->level)) {
				drupal_set_message(
					t(
					'Error occured during user permission change (user_id = "@user_id" / main acl, acl_id = "@acl_id")',
					array('@user_id' => $user_id,
							'@acl_id' => $acl_id,
					)),
				'error');
				$error++;
			}
		}
		$acl_count++;
	}

	// propagate permissions to child elements
	$child_set = 0;
	$child_fail = 0;
	if (isset($form_state['storage']['child_acl_ids']) && $propagate) {
		// save permissions for all child acls
		$child_acl_ids = $form_state['storage']['child_acl_ids'];
		foreach ($child_acl_ids as $child_acl_id) {
			if (check_admin_acl($child_acl_id)) {
				// permission to modify acl ok
				copy_acl($acl_id, $child_acl_id);
				$child_set++;
			} else {
				$child_fail++;
			}
		}
	}

	if ($error == 0) {
		drupal_set_message(t('Permission changes saved in main element'));
		if ($child_set > 0) {
			// display count of set child elements
			$txt = t('Permission changes saved in') . ' ' . $child_set . ' '
					. t('child element');
			$txt .= ($child_set > 1) ? 's' : '';
			drupal_set_message(t($txt));
		}
		if ($child_fail > 0) {
			// display count of failed child elements
			if ($child_fail > 1) {
				$txt = t(
					'Permission denied for @cnt child elements',
					array('@cnt' => $child_fail));
			} else {
				$txt = t('Permission denied for one child element');
			}
			drupal_set_message($txt, 'warning');
		}
	} else {
		// show only overview of failed actions. Error messages have already been displayed
		drupal_set_message(
			t(
				'Only @ok_cnt of @cnt entries have been saved',
				array('@ok_cnt' => ($acl_count - $error),
						'@cnt' => $acl_count
				)),
			'warning');
	}
}

?>