<?php
/**
 * Returns the new group page
 * @return Output array
 */
function sgroup_new() {
	$output['introduction'] = array(
			'#markup' => t('Please enter a name for your new group and click on "Create group".'),
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
			'#title' => t('Group name'),
			'#required' => TRUE,
	);
	$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t("Create group"),
			'#submit' => array(
					'sgroup_new_save'
			),
	);
	return $form;
}

/**
 * Saves a new group
 * @param $form
 * @param $form_state
 */
function sgroup_new_save($form, &$form_state) {
	global $user;

	$tx = db_transaction();

	try {
		$name = $form_state['values']['name'];

		// save group
		$sgid = db_insert('sgroup')->fields(array('sgroup_name' => $name,))->execute();

		// save user as admin of this group
		db_insert('sgroup_users')->fields(array('sgid' => $sgid,
						'uid' => $user->uid,
						'admin' => 1,
				))->execute();

		// show success message
		drupal_set_message(t('Group @name was created', array('@name' => $name)));

	} catch (Exception $e) {
		$tx->rollback();
		drupal_set_message(
				t('Group @name could not be saved. (@exception)',
						array('@name' => $name,
								'@exception' => $e->message
						)), 'error');
	}

	return $form_state['redirect'] = 'user/' . $user->uid . '/sgroup';
}
?>