<?php

function area_edit($area_id) {
	//include code for gmap_static_image_path
	module_load_include('inc', 'area', 'area.search');
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
	drupal_add_js(drupal_get_path('module', 'area') . '/js/habitats.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/parcels.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/autocomplete-select.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/area-edit.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/geometry-edit.js');
	//$render_array['area']['details'] = drupal_get_form('area_edit_form', $area_id);
	//$render_array['area']['parcels'] = drupal_get_form('area_parcels_form', $area_id);
	//$render_array['area']['map'] = area_map($area_id);
	$output['area'] = drupal_get_form('area_edit_form', $area_id);
	$output['file']['upload'] = drupal_get_form('area_edit_files_form', $area_id);

	return $output;
}

/**
 * Form to upload and comment new files
 * 
 * @return form to upload files
 */
function area_edit_files_form($form, &$form_state, $area_id) {
	$form['AreaFileForm'] = array(
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
			'SELECT fm.filename, fm.filemime, fm.timestamp, fm.fid, fm.filesize, afm.description
            FROM area_file_managed afm, file_managed fm
            WHERE fm.fid = afm.file_id
            AND afm.area_id = :id;', array(':id' => $area_id));

	if (!empty($result)) {
		foreach ($result as $record) {
			$mimeIconMap = array(
					'path' => drupal_get_path('module', 'inventory') . '/images/'
							. area_get_icon_map($record->filemime) . '.png',
					'alt' => $record->filemime,
					'title' => $record->filemime,
			);
			$img_mime = theme('image', $mimeIconMap);

			$description = $record->description;
			$filename = l($record->filename, 'sites/default/files/swissmon/area/' . $area_id . '/' . $record->filename);

			$img_delete = '<img src="' . url(drupal_get_path('module', 'inventory') . '/images/can_delete.png')
					. '" alt="' . t('Delete') . '" />';
			$filemime = $img_mime;
			$filesize = ($record->filesize / 1000) . " KB";
			$timestamp = date('d.m.Y', $record->timestamp);
			$row = array();
			$row[] = $description;
			$row[] = $filename;
			$row[] = $filemime;
			$row[] = $filesize;
			$row[] = $timestamp;
			$row[] = l($img_delete, 'area/' . $area_id . '/delete_file/' . $record->fid, array('html' => true));
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
			'#title' => t('Description of File'),
			'#size' => 100,
			'#maxlength' => 128,
			'#required' => FALSE,
	);

	$form['upload_submit'] = array(
			'#type' => 'submit',
			'#value' => t('Upload'),
			'#submit' => array(
					'area_update_file'
			),
			'#weight' => 2,
	);

	// Saving something to the form_state makes it accessible in the submit function.
	// We do this to make sure the sgid is taken from the url which has passed security checkpoints.
	$form_state['storage']['area_id'] = $area_id;

	return $form;
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

	$destination = 'public://swissmon/area/' . $area_id;

	file_prepare_directory($destination, FILE_CREATE_DIRECTORY);

	$file = file_save_upload('upload', $validators, $destination);
	if ($file) {

		$file->status |= FILE_STATUS_PERMANENT;
		$file = file_save($file);

		$form_state['values']['upload'] = $file;

		$nid = db_insert('area_file_managed')
				->fields(
						array('area_id' => $area_id,
								'file_id' => $file->fid,
								'description' => $form_state['values']['file_description']
						))->execute();
		drupal_set_message(t('File saved'));
	} else {
		drupal_set_message(t('File could not be saved!'), 'error');
	}

	return $form_state['redirect'] = 'area/' . $area_id . '/edit';
}

function area_edit_form($form, &$form_state, $area_id) {
	$title = db_select('area', 'a')->condition('id', $area_id)->fields('a')->execute()->fetch();
	drupal_set_title($title->field_name);
	$output['area']['basics'] = area_edit_basics_form($form, $form_state, $area_id);
	$output['area']['map'] = area_edit_map_form($form, $form_state, $area_id);
	$output['area']['comment'] = area_edit_comment_form($form, $form_state, $area_id);
	$output['area']['habitat'] = area_edit_habitat_form($form, $form_state, $area_id);
	$output['area']['concept'] = area_edit_concept_form($form, $form_state, $area_id);

	$output['actions'] = array(
			'#type' => 'actions'
	);
	$output['actions']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Save')
	);

	return $output;
}

/**
 * Saves the area changes after user clicks on the submit button
 * 
 * Updates existing entry in the DB with the area that was just edited
 * 
 * @param array $form
 * @param *array $form_state
 */

function area_edit_form_submit($form, &$form_state) {
	// get values from form
	$area_id = $form_state['values']['area_id'];
	$field_name = $form_state['input']['field_name'];
	$comment = $form_state['input']['comment'];
	$protection_target = $form_state['input']['protection_target'];
	$safety_precautions = $form_state['input']['safety_precautions'];
	$tending_strategies = $form_state['input']['tending_strategies'];
	$area_surface = $form_state['input']['area_surface'];

	//	global $user;

	$areaValuesArray = array(
			'field_name' => $field_name,
			'comment' => $comment,
			'modify_time' => 'now()',
			'protection_target' => $protection_target,
			'safety_precautions' => $safety_precautions,
			'tending_strategies' => $tending_strategies,
			'surface_area' => $area_surface
	);

	//check if the area is set in the formular
	//TODO maybe change if-statement to something more secure
	if ($form_state['input']['area_coords'] != "") {
		// field coordinates
		$coords = json_decode($form_state['input']['area_coords'], true);
		$points = array();
		foreach ($coords as $coord) {
			$lat = $coord[0];
			$lng = $coord[1];
			$points[] = $lng . ' ' . $lat;
		}
		$areaType = $form_state['input']['area_type'];
		switch ($areaType) {
		case 'polyline':
			$linestring = implode(', ', $points);
			$geom = 'ST_GeomFromText(\'LINESTRING(' . $linestring . ')\', 4326)'; // SRID 4326 = WGS 84
			break;

		case 'polygon':
		// make sure line is closed
			if (end($points) != reset($points)) {
				$points[] = reset($points);
			}

			$linestring = implode(', ', $points);
			$geom = 'ST_GeomFromText(\'POLYGON((' . $linestring . '))\', 4326)'; // SRID 4326 = WGS 84
			break;

		case 'marker':
			$geom = 'ST_GeomFromText(\'POINT(' . reset($points) . ')\', 4326)'; // SRID 4326 = WGS 84
			break;
		}
		db_update('area')->expression('geom', $geom)->condition('id', $area_id)->execute();
	}

	// update area
	$area_update = db_update('area')->fields($areaValuesArray)->condition('id', $area_id)->execute();

	// insert habitats
	$existing = db_select('area_habitat', 'ah')->condition('ah.area_id', $area_id)
			->fields('ah', array('habitat_id',
					'id'
			))->execute()->fetchAllKeyed(0);
	$new = array_values($form_state['input']['habitat']);
	array_walk($existing, 'intval');
	array_walk($new, 'intval');

	foreach ($new as $habitat) {
		if (array_key_exists($habitat, $existing)) // habitat already stored
			continue;
		db_insert('area_habitat')->fields(array('area_id' => $area_id,
						'habitat_id' => $habitat,
				))->execute();
	}
	foreach ($existing as $habitat => $record) {
		if (in_array($habitat, $new)) // habitat still selected
			continue;
		db_delete('area_habitat')->condition('id', $record)->execute();
		$images = db_select('gallery_image', 'gi', array('fetch' => PDO::FETCH_ASSOC))
				->condition('gi.item_type', 'area_habitat')->condition('gi.item_id', $record)
				->fields('gi', array('id'));
		$images->join('file_managed', 'f', 'f.fid = gi.fid');
		$images->fields('f');
		$images = $images->execute();
		foreach ($images as $image)
			gallery_image_delete($image);
	}

	//insert parcels
	$parcels = $form_state['input']['parcel'];
	area_insert_parcels($area_id, $parcels);

	// redirect to create a new inventory assigned to the area just created
	$form_state['redirect'] = 'area/' . $area_id . '/edit';
	drupal_set_message(t('Area successfully updated'), 'status');
}

function area_delete_file($form, &$form_state, $area_id, $file_id) {
	$query = db_select('area_file_managed', 'afm')->condition('afm.area_id', $area_id)
			->condition('afm.file_id', $file_id)->fields('afm', array('description'));
	$query->join('file_managed', 'fm', 'fm.fid = afm.file_id');
	$query->fields('fm', array('filename'));
	$file = $query->execute()->fetch();

	$form_state['area_id'] = $area_id;
	$form_state['file_id'] = $file_id;

	$name = $file->description ? $file->description : $file->filename;
	return confirm_form($form, t('Delete %name?', array('%name' => $name)), 'area/' . $area_id . '/edit',
			t('Are you sure you want to delete the file %name?', array('%name' => $name)), t('Delete'), t('Cancel'));
}

function area_delete_file_submit($form, &$form_state) {
	$area_id = $form_state['area_id'];
	$file_id = $form_state['file_id'];

	$file = file_load($file_id);
	file_delete($file);
	db_delete('area_file_managed')->condition('area_id', $area_id)->condition('file_id', $file_id)->execute();
	drupal_set_message(t('The file was successfully deleted'));
	$form_state['redirect'] = 'area/' . $area_id . '/edit';
}
?>