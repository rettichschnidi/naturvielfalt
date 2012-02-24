<?php
/**
 * Returns the new group page
 * 
 * @return Output array
 */
function sgroup_new() {
	// set title
	drupal_set_title(check_plain(t("New Group")));
	// display short instruction text
	$output['introduction'] = array(
			'#markup' => t(
				'Please enter a name for your new group and click on "Create group".'),
	);
	$output['form'] = drupal_get_form('sgroup_new_form');

	return $output;
}

/**
 * Creates the new group form
 * @return form array
 */
function sgroup_new_form() {
	$form['name'] = array(
			'#type' => 'textfield',
			'#title' => t('Group Name'),
			'#required' => TRUE,
	);
	$form['description'] = array(
			'#type' => 'textarea',
			'#title' => t('Group Description'),
	);
	$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t("Create group"),
			'#submit' => array(
					'sgroup_new_save'
			)
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
function sgroup_new_save($form, &$form_state) {
	global $user;

	$tx = db_transaction();

	try {
		$name = $form_state['values']['name'];
		$description = $form_state['values']['description'];

		$acl_id = new_acl("group");
		// save group
		$sgid = db_insert('sgroup')->fields(
				array('name' => $name,
						'description' => $description,
						'users_id' => $user->uid,
						'acl_id' => $acl_id,
				))
			->execute();

		// show success message
		drupal_set_message(
			t('Group @name was created', array('@name' => $name)));
	} catch (Exception $e) {
		$tx->rollback();
		drupal_set_message(
			t(
				'Group @name could not be saved. (@exception)',
				array('@name' => $name,
						'@exception' => $e->message
				)),
			'error');
	}

	return $form_state['redirect'] = 'user/' . $user->uid . '/sgroup';
}
?>