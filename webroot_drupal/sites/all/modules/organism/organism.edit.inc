<?php

function organism_edit_files_form($form, &$form_state, $organism_id) {
	global $user;
	$form['OrganismFileForm'] = array(
			'#type' => 'fieldset',
			'#title' => t('Files'),
			'#weight' => 1,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
					'id' => 'organism-files'
			),
	);

	$tableHeader[] = t('Description');
	$tableHeader[] = t('Filename');
	$tableHeader[] = t('Typ');
	$tableHeader[] = t('Size');
	$tableHeader[] = t('Date');
	$tableHeader[] = t('Author');
	$tableHeader[] = t('Owner');
	$tableHeader[] = '&nbsp;';

	$result = db_query(
		'SELECT
  			fm.filename,
			fm.filemime,
			fm.timestamp,
			fm.fid,
			fm.filesize,
			ofm.description,
			ofm.author
		FROM
			{organism_file_managed} ofm,
			{file_managed} fm
		WHERE
			fm.fid = ofm.file_managed_fid
            AND ofm.organism_id = :id
  			AND fm.uid = :uid;',
		array(':id' => $organism_id,
				':uid' => $user->uid
		));

	if (!empty($result)) {
		foreach ($result as $record) {
			$mimeIconMap = array(
					'path' => drupal_get_path('module', 'commonstuff')
							. '/images/' . commonstuff_get_icon_map($record->filemime)
							. '.png',
					'alt' => $record->filemime,
					'title' => $record->filemime,
			);
			$img_mime = theme('image', $mimeIconMap);

			$description = $record->description;
			$filename = l(
				$record->filename,
				'sites/default/files/naturvielfalt/organism/' . $organism_id
						. '/' . $record->filename);

			$img_delete = '<img src="'
					. url(
						drupal_get_path('module', 'commonstuff')
								. '/images/can_delete.png') . '" alt="'
					. t('Delete') . '" />';
			$filemime = $img_mime;
			$filesize = ($record->filesize / 1024) . " KB";
			$timestamp = date('d.m.Y', $record->timestamp);
			$author = isset($record->author) ? $record->author : '';
			$row = array();
			$row[] = $description;
			$row[] = $filename;
			$row[] = $filemime;
			$row[] = $filesize;
			$row[] = $timestamp;
			$row[] = $author;
			$row[] = $user->name;
			$row[] = l(
				$img_delete,
				'organism/' . $organism_id . '/delete_file/' . $record->fid,
				array('html' => true));
			$rows[] = $row;
		}

		if (isset($rows)) {
			$form['AreaFileForm']['file_overview'] = array(
					'#theme' => 'table',
					'#header' => $tableHeader,
					'#rows' => $rows,
			);
		}
	}

	$form['OrganismFileForm']['upload'] = array(
			'#type' => 'file',
			'#title' => t('Upload File'),
			'#size' => 100,
	);

	$form['OrganismFileForm']['file_description'] = array(
			'#type' => 'textfield',
			'#title' => t('Description of File'),
			'#size' => 100,
			'#maxlength' => 128,
			'#required' => FALSE,
	);
	$form['OrganismFileForm']['author'] = array(
			'#type' => 'textfield',
			'#title' => t('Author'),
			'#size' => 100,
			'#maxlength' => 128,
			'#required' => FALSE,
	);

	$form['upload_submit'] = array(
			'#type' => 'submit',
			'#value' => t("Upload"),
			'#submit' => array(
					'organism_edit_file'
			),
			'#weight' => 2,
	);

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
					'doc txt pdf docx mp3'
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
						'description' => $form_state['values']['file_description'],
						'author' => $form_state['values']['author']
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
	$query->condition('fm.uid', $user->uid);
	$file = $query->execute()
		->fetch();

	$form_state['organism_id'] = $organism_id;
	$form_state['file_id'] = $file_id;

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

	$file = file_load($file_id);
	if($file) {
		file_delete($file);
		db_delete('organism_file_managed')->condition('organism_id', $organism_id)
		->condition('file_managed_fid', $file_id)
		->execute();
		drupal_set_message(t('The file was successfully deleted!'));
	} else {
		drupal_set_message(t('The file could not be deleted!'));
	}
	$form_state['redirect'] = 'organism/' . $organism_id . '/edit';
}

?>
