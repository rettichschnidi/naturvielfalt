<?php
/**
 * Returns the edit group page
 * 
 * @param	string	$sgroup_id	id of group to edit
 * @return	Array				Output array
 */
function sgroup_edit($sgroup_id) {
	// display short instruction text
	$save_txt = t('Save');
	$output['introduction'] = array(
			'#markup' => t(
				'Edit the group name and description. When finished click "@save".', array('@save' => $save_txt)),
	);

	$db_group = db_select('sgroup', 'g')->fields('g')
		->condition('g.id', $sgroup_id)
		->execute();
	$sgroup = $db_group->fetch();
	// set group edit title
	drupal_set_title(t('Edit ').check_plain($sgroup->name));
	$output['form'] = drupal_get_form('sgroup_edit_form', $sgroup);

	return $output;
}

/**
 * Returns the edit form for a group
 * 
 * @param	Array	$form			drupal generated form array
 * @param	Array	&$form_state	drupal generated form array
 * @param	Object	$sgroup			db object of sgroup
 * @return	Array					drupal generated form array
 */
function sgroup_edit_form($form, &$form_state, $sgroup) {
	$form['name'] = array(
			'#type' => 'textfield',
			'#title' => t('Group name'),
			'#value' => $sgroup->name,
			'#required' => TRUE,
	);
	$form['description'] = array(
			'#type' => 'textarea',
			'#title' => t('Group Description'),
			'#value' => $sgroup->description,
	);
	$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t("Save"),
	);

	// Saving something to the form_state makes it accessible in the submit function.
	// We do this to make sure the sgid is taken from the url which has passed security checkpoints.
	$form_state['storage']['sgid'] = $sgroup->id;

	return $form;
}

/**
 * Saves name and description of group
 * 
 * @param	Array	$form			drupal generated form array
 * @param	Array	&$form_state	drupal generated form array
 * @return	Array					drupal generated form array
 */
function sgroup_edit_form_submit($form, &$form_state) {
	$tx = db_transaction();

	try {
		// save group
		$affectedRows = db_update('sgroup')->fields(
				array('name' => $form_state['input']['name'],
						'description' => $form_state['input']['description'],
				))
			->condition('id', $form_state['storage']['sgid'])
			->execute();

		// check success
		if ($affectedRows == 1) {
			// show success message
			drupal_set_message(
				t(
					'Group @sgroup_name was successfully updated',
					array('@sgroup_name' => $form_state['input']['name'])));
		} else {
			drupal_set_message(t('Group could not be updated!'), 'error');
		}

	} catch (Exception $e) {
		$tx->rollback();
		drupal_set_message(
			t(
				'Group @sgroup_name could not be updated. (@exception)',
				array('@sgroup_name' => $form_state['input']['name'],
						'@exception' => $e->getMessage()
				)),
			'error');
	}

	global $user;
	return $form_state['redirect'] = 'user/' . $user->uid . '/sgroup';
}
?>