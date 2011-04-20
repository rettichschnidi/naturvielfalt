<?php
function sgroup_edit($sgid) {
	// load record
	$sgroups = db_query('SELECT sgid, sgroup_name FROM sgroup WHERE sgid = :sgid', array(':sgid' => $sgid));
	$sgroup = $sgroups->fetch();

	// show edit form
	$output['groupform'] = drupal_get_form('sgroup_edit_form', $sgroup);

	$output['fieldset'] = array(
		'#type' => 'fieldset',
		'#title' => t('Members:'),
		'memberTable' => _sgroup_get_member_table($sgroup),
		'addmemberform' => drupal_get_form('sgroup_add_member_form', $sgroup),
	);

	return $output;
}

function _sgroup_get_member_table($sgroup) {
	// get users from db
	$users = db_query('SELECT u.uid, u.name, sgu.admin FROM sgroup_users sgu LEFT JOIN users u ON sgu.uid = u.uid
		WHERE sgu.sgid = :sgid', array(':sgid' => $sgroup->sgid));
	foreach ($users as $user) {
		$row = array();
		$row[] = $user->name;
		$row[] = $user->admin == 1 ? t('Administrator') : t('Member');
		$rows[] = $row;
	}
	$header[] = t('User');
	$header[] = t('Role');
	
	// compose table
	$output['#theme'] = 'table';
	$output['#header'] = $header;
	$output['#rows'] = $rows;
	
	return $output;
}

/**
 * Returns the edit form for a group
 */
function sgroup_edit_form($form, &$form_state, $sgroup) {
	$form['sgroup_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Group name'),
		'#value' => $sgroup->sgroup_name,
		'#required' => TRUE,
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t("Save group"),
	);
	
	// Saveing something to the form_state makes it accessible in the submit function.
	// We do this to make sure the sgid is taken from the url which has passed security checkpoints.
	$form_state['storage']['sgid'] = $sgroup->sgid;
	
	return $form;
}

/**
 * Saves an edited group
 * @param $form
 * @param $form_state
 */
function sgroup_edit_form_submit($form, &$form_state) {
	$tx = db_transaction();

	try {
		$sgid = $form_state['storage']['sgid']; // Get sgid from form_state to make sure it is the security checked id.
		$sgroup_name = $form_state['input']['sgroup_name'];

		// save group
		$affectedRows = db_update('sgroup')
		->fields(array(
			'sgroup_name' => $sgroup_name,
		))->condition('sgid', $sgid, '=')
		->execute();
		
		// check success
		if ($affectedRows == 1) {
			// show success message
			drupal_set_message(t('Group @sgroup_name was successfully updated', array('@sgroup_name'=> $sgroup_name)));
		} else {
			drupal_set_message(t('Group could not be updated!'), 'error');
		}

	} catch (Exception $e) {
		$tx->rollback();
		drupal_set_message(t('Group @sgroup_name could not be updated. (@exception)', array('@sgroup_name' => $sgroup_name, '@exception' => $e->message)), 'error');
	}

	return $form_state['redirect'] = 'sgroup';
}

/**
 * Returns the form to add a member
 */
function sgroup_add_member_form($form, &$form_state, $sgroup) {
	$form['user'] = array(
		'#title' => 'Add user',
		'#type' => 'textfield',   
    	'#autocomplete_path' => 'user/autocomplete',
	);
	$form['isAdmin'] = array(
		'#type' => 'checkbox',
		'#title' => 'with administrator rights',
	);
	$form['sgid'] = array(
		'#type' => 'hidden',
		'#value' => $sgroup->sgid,
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Add user',
	);
	return $form;
}

function sgroup_add_member_form_submit($form, &$form_state) {
	// try to find the user
	$username = $form_state['input']['user'];
	$admin = $form_state['input']['isAdmin'];
	$sgid = $form_state['values']['sgid'];
	$users = db_query('SELECT uid FROM {users} WHERE name = :username', array(':username' => $username));
	$user = $users->fetch();

	// yes, there is one
	if ($user->uid > 0) {
		// find out if there is already a sgroup_user record
		$sgroup_users = db_query('SELECT count(*) AS count FROM {sgroup_users} WHERE uid = :uid AND sgid = :sgid', array(':uid' => $user->uid, ':sgid' => $sgid));
		$count = $sgroup_users->fetch();
		if ($count->count == 0) {
			db_insert('sgroup_users')
			->fields(array(
			'sgid' => $sgid,
			'uid' => $user->uid,
			'admin' => $admin,
			))
			->execute();
		} else {
			drupal_set_message(t('This user is already a member.'), 'warning');
		}
	} else {
		drupal_set_message(t('User does not exist.'), 'warning');
	}
}
?>