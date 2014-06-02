<?php

/**
 * @param array $form
 * @param array $form_state
 * @param int $organism_id
 * @return Drupal form
 */
function organism_edit_files_form($form, &$form_state, $organism_id) {
	global $user;

	$form['OrganismFileForm']['upload'] = array(
			'#type' => 'file',
			'#title' => t('File'),
			'#size' => 100
	);

	$form['OrganismFileForm']['file_description'] = array(
			'#type' => 'textfield',
			'#title' => t('Description of File'),
			'#size' => 100,
			'#maxlength' => 128,
			'#required' => false
	);
	$form['OrganismFileForm']['author'] = array(
			'#type' => 'textfield',
			'#title' => t('Author'),
			'#size' => 100,
			'#maxlength' => 128,
			'#required' => false
	);

	$form['OrganismFileForm']['upload_submit'] = array(
			'#type' => 'submit',
			'#value' => t("Upload"),
			'#submit' => array(
					'organism_edit_file'
			),
	);

	$form['file_overview'] = organism_files($organism_id);

	// Saving something to the form_state makes it accessible in the submit function.
	// We do this to make sure the sgid is taken from the url which has passed security checkpoints.
	$form_state['storage']['organism_id'] = $organism_id;

	return $form;
}

function organism_edit_file($form, &$form_state) {
	$organism_id = $form_state['storage']['organism_id'];

	// File upload
	$validators = array(
			'file_validate_extensions' => array(
					'doc docx pdf txt xls xlsx'
			),
			'file_validate_size' => array(
					8388608,
					0
			),
	);

	$destination = 'public://naturvielfalt/organism/' . $organism_id;

	file_prepare_directory($destination, FILE_CREATE_DIRECTORY);

	$file = file_save_upload('upload', $validators, $destination);
	if ($file) {

		$file->status |= FILE_STATUS_PERMANENT;
		$file = file_save($file);

		$form_state['values']['upload'] = $file;

		$nid = db_insert('organism_file_managed')->fields(
				array('organism_id' => $organism_id,
						'file_managed_fid' => $file->fid,
						'description' => check_plain($form_state['values']['file_description']),
						'author' => check_plain($form_state['values']['author'])
				))
			->execute();
		drupal_set_message(t('File saved'));
	} else {
		drupal_set_message(t('File could not be saved!'), 'error');
	}

	return $form_state['redirect'] = 'organism/' . $organism_id . '/edit';
}

function organism_delete_file($form, &$form_state, $organism_id, $file_id) {
	global $user;
	$query = db_select('organism_file_managed', 'ofm')->condition(
			'ofm.organism_id',
			$organism_id)
		->condition('ofm.file_managed_fid', $file_id)
		->fields('ofm', array('description'));
	$query->join('file_managed', 'fm', 'fm.fid = ofm.file_managed_fid');
	$query->fields('fm', array('filename'));
	// make sure administrators can delete every file -> if user != admin only allow to delete their own files
	if (!user_access('bypass node access'))
		$query->condition('fm.uid', $user->uid);
	$file = $query->execute()
		->fetch();
	
	$form_state['organism_id'] = $organism_id;
	$form_state['file_id'] = ($file) ? $file_id : '';	// !!! this prevents non-fileowners from deleting a file if they know the correct address

	$name = $file->description ? $file->description : $file->filename;
	return confirm_form(
		$form,
		t('Delete %name?', array('%name' => $name)),
		'organism/' . $organism_id . '/edit',
		t(
			'Are you sure you want to delete the file %name?',
			array('%name' => $name)),
		t('Delete'),
		t('Cancel'));
}

function organism_delete_file_submit($form, &$form_state) {
	$organism_id = $form_state['organism_id'];
	$file_id = $form_state['file_id'];

	$file = ($file_id != '') ? file_load($file_id) : false;		// !!! also needs this line to prevent file deletion if not owner of the file to be deleted
	if($file) {
		file_delete($file);
		db_delete('organism_file_managed')->condition('organism_id', $organism_id)
		->condition('file_managed_fid', $file_id)
		->execute();
		drupal_set_message(t('The file was successfully deleted!'));
	} else {
		drupal_set_message(t('The file could not be deleted!'), 'error');
	}
	$form_state['redirect'] = 'organism/' . $organism_id . '/edit';
}

?>
