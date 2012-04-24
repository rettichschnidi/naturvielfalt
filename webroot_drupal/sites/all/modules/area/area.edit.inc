<?php
/**
 * @file area.edit.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');

/**
 * Create a form which lets the user edit the geometry of an area.
 * 
 * @param array $form
 *  Drupal form
 * @param $form_state
 *  An associative array containing the current state of the form.
 * @param $areadata
 * 	Array with area data
 */
function area_edit_geometry_form($form, &$form_state, $areadata = NULL) {
	$output['helptext'] = array(
			'#type' => 'markup',
			'#markup' => t(
				'<p>Please note: Changes will be saved instantly.</p>')
	);
	$output['map'] = array(
			'#theme' => 'area',
			'#showall' => false,
			'#scalecontrol' => true,
			'#action' => 'edit',
			'#area_id' => (int) $areadata['id']
	);
	return $output;
}

/**
 * Return a form which allows all textual data of an area to be edited.
 *
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 * @param array $areadata
 */
function area_edit($areadata = NULL) {
	assert(!empty($areadata));
	drupal_set_title($areadata['name']);
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css');
	$output['area'] = drupal_get_form('area_edit_form', $areadata);
	$output['file']['upload'] = drupal_get_form(
		'area_edit_files_form',
		$areadata);
	return $output;
}

/**
 * Return a form which allows all textual data of an area to be edited.
 * 
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 * @param array $areadata
 */
function area_edit_form($form, &$form_state, $areadata = NULL) {
	/* call area_get_static_image_url() to get an url to the google maps visualization */
	$staticimageurl = area_get_static_image_url(
		array('coordinates' => json_encode($areadata['coordinatejson']),
				'type' => $areadata['geometry_type']
		),
		450,
		320);
	$editgeometryurl = l(
		"<img src='$staticimageurl' alt='Static Map'/>",
		'area/' . $areadata['id'] . '/editgeometry',
		array('html' => true));

	/* create a fieldset for the tabular data */
	$form['area_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area details'),
			'#weight' => 1,
			'#attributes' => array(
					'id' => 'area-edit-details' // required for CSS
			),
	);

	/* add a table with the most imporant information, leave out empty fields */
	$form['area_edit']['table'] = area_get_infotable_of_specific_area($areadata);

	/* Create a fieldset for the comment text field */
	$form['area_description_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Description'),
			'#weight' => 10,
			'#collapsible' => true,
			'#collapsed' => false,
			'#attributes' => array(
					'id' => 'area-edit-comment'
			)
	);

	/* add the name of the area as an editable textfield */
	$form['area_description_edit']['areaname'] = array(
			'#type' => 'textfield',
			'#title' => t('Area name'),
			'#size' => 25,
			'#required' => true,
			'#default_value' => $areadata['name']
	);

	/* add the comments for the area as an editable textarea */
	$form['area_description_edit']['comment'] = array(
			'#type' => 'textarea',
			'#title' => t('Description'),
			'#default_value' => $areadata['comment']
	);

	/* create a fieldset for the strategies text fields */
	$form['area_concept_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area concept'),
			'#weight' => 11,
			'#collapsible' => true,
			'#collapsed' => true
	);

	/* add the protectiont target textarea */
	$form['area_concept_edit']['protection_target'] = array(
			'#type' => 'textarea',
			'#title' => t('Protection target'),
			'#default_value' => $areadata['protection_target']
	);

	/* add the tending strategies target textarea */
	$form['area_concept_edit']['tending_strategies'] = array(
			'#type' => 'textarea',
			'#title' => t('Tending strategies'),
			'#default_value' => $areadata['tending_strategies']
	);

	/* add the tending strategies target textarea */
	$form['area_concept_edit']['safety_precautions'] = array(
			'#type' => 'textarea',
			'#title' => t('Safety precautions'),
			'#default_value' => $areadata['safety_precautions']
	);

	/* create a fieldset for the strategies text fields */
	$form['area_habitats_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Linked habitats'),
			'#weight' => 12,
			'#collapsible' => true,
			'#collapsed' => true,
	);

	/* add the habitats table */
	$form['area_habitats_edit']['table'] = area_edit_habitat($areadata);

	/* create a fieldset for the static google maps */
	$form['area_static_map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 3,
			'#attributes' => array(
					'id' => 'area-edit-map' // required for CSS
			)
	);

	/* add the picture */
	$form['area_static_map']['image'] = array(
			'#type' => 'markup',
			'#markup' => $editgeometryurl
	);

	if (function_exists('gallery_list_renderer')) {
		$output['area_videos'] = gallery_list_renderer(
			'videos',
			'area',
			$areadata['id']);
		$output['area_images'] = gallery_list_renderer(
			'images',
			'area',
			$areadata['id']);
	}

	$form['actions'] = array(
			'#type' => 'actions'
	);
	
	/* add a submit button to save the changes */
	$form['actions']['submit'] = array(
			'#type' => 'submit',
			'#weight' => 101,
			'#value' => t('Save changes')
	);

	return $form;
}

/**
 * Validate the changes to the area
 *
 * @param array $form
 * @param array $form_state
 */
function area_edit_form_validate($form, &$form_state) {
	if (empty($form_state['values']['areaname'])) {
		form_set_error('areaname', t('You have to enter an area name.'));
	}
}

/**
 * Return a drupal render array with the habitats linked to the given area.
 * 
 * @param $areadata
 * 	Array with all data about an area
 */
function area_edit_habitat($areadata = NULL) {
	$results = db_query(
		'SELECT
			h.id AS id,
			h.label AS label,
			h.name AS name,
			CASE
				WHEN ah.id IS NOT NULL
					THEN 1
					ELSE 0
				END
			AS selected
		FROM
			{habitat} h
			LEFT JOIN (
				SELECT
					id,
					habitat_id,
					area_id
				FROM
					{area_habitat}
				WHERE
					area_id = :areaid
			) AS ah ON h.id = ah.habitat_id;',
		array('areaid' => $areadata['id']));

	$habitats = array();
	foreach ($results->fetchAll() as $habitatAsObject) {
		$row = array();
		$id = $habitatAsObject->id;
		$checkText = $habitatAsObject->selected ? "checked='true'" : '';
		$checkbox = "<input type='checkbox' name='selectedhabitats[]' value='"
				. $habitatAsObject->id . "' $checkText />";
		$row[] = $checkbox;
		$row[] = $habitatAsObject->label;
		$row[] = $habitatAsObject->name;
		$habitats[] = $row;
	}

	$form = array(
			'#theme' => 'datatable',
			'#header' => array(
					array(
							'name' => t('Select'),
							'width' => 40
					),
					array(
							'name' => t('Label'),
							'width' => 200
					),
					array(
							'name' => t('Name'),
							'width' => 600
					)
			),
			'#tableWidth' => 900,
			'#rows' => $habitats,
			'#id_table' => 'habitats'
	);
	return $form;
}

/**
 * Save changes to the area texts.
 *
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 */
function area_edit_form_submit($form, &$form_state) {
	/* Escape values before submitting them to the database */
	$area_id = (int) $form_state['build_info']['args'][0]['id'];
	$name = filter_xss($form_state['values']['areaname']);
	$comment = filter_xss($form_state['values']['comment']);
	$protection_target = filter_xss($form_state['values']['protection_target']);
	$tending_strategies = filter_xss(
		$form_state['values']['tending_strategies']);
	$safety_precautions = filter_xss(
		$form_state['values']['safety_precautions']);
	$selectedhabitatIds = isset($_POST['selectedhabitats'])
			? $_POST['selectedhabitats'] : array();
	// flush all existing area_habitat records for this area...
	db_delete('area_habitat')->condition('area_id', $area_id)
		->execute();
	// ...and populate them with the current new ones
	$values = array();
	$insert = db_insert('area_habitat')->fields(
			array('area_id',
					'habitat_id'
			));
	foreach ($selectedhabitatIds as $habitatId) {
		$insert->values(
				array('area_id' => $area_id,
						'habitat_id' => $habitatId
				));
	}
	$insert->execute();

	db_update('area')->fields(
			array('name' => $name,
					'comment' => $comment,
					'protection_target' => $protection_target,
					'tending_strategies' => $tending_strategies,
					'safety_precautions' => $safety_precautions,
					'modify_time' => 'NOW()',
			))
		->condition('id', $area_id)
		->execute();
	drupal_set_message(t("Area Saved"));
}

function area_update_file($form, &$form_state) {
	$area_id = $form_state['storage']['area_id'];
	// File upload
	$validators = array(
			'file_validate_extensions' => array(
					'doc txt pdf docx'
			),
			'file_validate_size' => array(
					1000000,
					0
			),
	);

	$destination = 'public://naturvielfalt/area/' . $area_id;

	file_prepare_directory($destination, FILE_CREATE_DIRECTORY);

	$file = file_save_upload('upload', $validators, $destination);
	if ($file) {

		$file->status |= FILE_STATUS_PERMANENT;
		$file = file_save($file);

		$form_state['values']['upload'] = $file;

		$nid = db_insert('area_file_managed')->fields(
				array('area_id' => $area_id,
						'file_managed_fid' => $file->fid,
						'description' => $form_state['values']['file_description']
				))
			->execute();
		drupal_set_message(t('File saved'));
	} else {
		drupal_set_message(t('File could not be saved!'), 'error');
	}
	return $form_state['redirect'] = 'area/' . $area_id . '/edit';
}

/**
 * Form to upload and comment new files
 *
 * @return form to upload files
 */
function area_edit_files_form($form, &$form_state, $areadata) {
	$area_id = $areadata['id'];
	$form['AreaFileForm'] = array(
			'#type' => 'fieldset',
			'#title' => t('Files'),
			'#weight' => 77,
			'#collapsible' => true,
			'#collapsed' => true,
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
					'id' => 'area-files'
			),
	);

	$tableHeader[] = t('Description');
	$tableHeader[] = t('Filename');
	$tableHeader[] = t('Typ');
	$tableHeader[] = t('Size');
	$tableHeader[] = t('Date');
	$tableHeader[] = '&nbsp;';

	$result = db_query(
		'SELECT
				fm.filename,
				fm.filemime,
				fm.timestamp,
				fm.fid,
				fm.filesize,
				afm.description
			FROM
				{area_file_managed} AS afm
				JOIN {file_managed} AS fm ON afm.file_managed_fid = fm.fid
			WHERE
				afm.area_id = :id;',
		array(':id' => $area_id));

	if (!empty($result)) {
		foreach ($result as $record) {
			$mimeIconMap = array(
					'path' => drupal_get_path('module', 'inventory')
							. '/images/' . area_get_icon_map($record->filemime)
							. '.png',
					'alt' => $record->filemime,
					'title' => $record->filemime,
			);
			$img_mime = theme('image', $mimeIconMap);

			$description = $record->description;
			$filename = l(
				$record->filename,
				'sites/default/files/naturvielfalt/area/' . $area_id . '/'
						. $record->filename);

			$img_delete = '<img src="'
					. url(
						drupal_get_path('module', 'inventory')
								. '/images/can_delete.png') . '" alt="'
					. t('Delete') . '" />';
			$filemime = $img_mime;
			$filesize = ($record->filesize / 1000) . " KB";
			$timestamp = date('d.m.Y', $record->timestamp);
			$row = array();
			$row[] = $description;
			$row[] = $filename;
			$row[] = $filemime;
			$row[] = $filesize;
			$row[] = $timestamp;
			$row[] = l(
				$img_delete,
				'area/' . $area_id . '/delete_file/' . $record->fid,
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

	$form['AreaFileForm']['upload'] = array(
			'#type' => 'file',
			'#title' => t('Upload File'),
			'#size' => 48,
	);

	$form['AreaFileForm']['file_description'] = array(
			'#type' => 'textfield',
			'#title' => t('Description of file'),
			'#size' => 95,
			'#maxlength' => 120,
			'#required' => FALSE,
	);
	$form['AreaFileForm']['upload_submit'] = array(
			'#type' => 'submit',
			'#value' => t('Upload'),
			'#submit' => array(
					'area_update_file'
			),
			'#weight' => 88,
	);
	// Saving something to the form_state makes it accessible in the submit function.
	// We do this to make sure the sgid is taken from the url which has passed security checkpoints.
	$form_state['storage']['area_id'] = $area_id;
	return $form;
}

/**
 * Ask user before deleting an area file.
 * 
 * @param $form
 * @param $form_state
 * @param $area_id
 * @param $file_id
 */
function area_delete_file($form, &$form_state, $areadata, $file_id) {
	$area_id = $areadata['id'];
	$query = db_select('area_file_managed', 'afm')->condition(
			'afm.area_id',
			$area_id)
		->condition('afm.file_managed_fid', $file_id)
		->fields('afm', array('description'));
	$query->join('file_managed', 'fm', 'fm.fid = afm.file_managed_fid');
	$query->fields('fm', array('filename'));
	$file = $query->execute()
		->fetch();

	$form_state['area_id'] = $area_id;
	$form_state['file_id'] = $file_id;

	$name = $file->description ? $file->description : $file->filename;
	return confirm_form(
		$form,
		t('Delete %name?', array('%name' => $name)),
		'area/' . $area_id . '/edit',
		t(
			'Are you sure you want to delete the file %name?',
			array('%name' => $name)),
		t('Delete'),
		t('Cancel'));
}

/**
 * Delete an area file.
 * @param $form
 * @param $form_state
 */
function area_delete_file_submit($form, &$form_state) {
	$area_id = $form_state['area_id'];
	$file_id = $form_state['file_id'];

	$file = file_load($file_id);
	if ($file) {
		file_delete($file);
		db_delete('area_file_managed')->condition('area_id', $area_id)
			->condition('file_managed_fid', $file_id)
			->execute();
		drupal_set_message(t('The file was successfully deleted'));
	} else {
		drupal_set_message(t('The file could not be deleted.'), 'error');
	}
	$form_state['redirect'] = 'area/' . $area_id . '/edit';
}

?>
