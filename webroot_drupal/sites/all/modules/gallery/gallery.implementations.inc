<?php
/**
 * @file gallery.implementations.inc
 * @author Ramon Gamma, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'gallery', 'gallery.content');

/**
 * Save images & files on a easy way
 * @param string $module_name
 * 					provide the drupal module name
 * @param array $entry_array
 *		'item_id' => ID of the parent elemet obervation_id OR area_id,
 *		'author' => optional, will be overwritten is provided by user,
 *		'location' => optional, will be overwritten is provided by user
 * @param string $file_managed_module_col
 * 		the column in the module db table where's the item_id should be inserted
 */
function gallery_implementation_save_manual($module_name, $entry_array,
		$file_managed_module_col = false) {
	if (!isset($module_name) || empty($module_name))
		return false;
	if (!isset($entry_array) || !is_array($entry_array)
			|| count($entry_array) < 2)
		return false;
	// if no files to work with, abort
	if (!isset($_FILES['files']['tmp_name']) || count($_FILES['files']['tmp_name']) < 1)
		return false;

	$categories = array();
	$destination = array();

	// check the directory, create it if not exists
	$key = $module_name . ':' . $entry_array['item_id'];
	$destination[$key] = 'public://naturvielfalt/gallery/' . $module_name . '/'
			. $entry_array['item_id'];
	file_prepare_directory($destination[$key], FILE_CREATE_DIRECTORY);

	$img_ids = array();

	// save each files
	foreach ($_FILES['files']['error'] as $fkey => $message) {
		if ($message != 0)
			continue;
		
		// prevent xss:
		$title = filter_xss($_REQUEST['meta_title'][$fkey]);
		$description = filter_xss($_REQUEST['meta_description'][$fkey]);
		$author = filter_xss($_REQUEST['meta_author'][$fkey]);
		$location = filter_xss($_REQUEST['meta_location'][$fkey]);
		
		// build the db entry for the gallery module
		$image = array(
				'item_type' => $module_name,
				'item_id' => $entry_array['item_id'],
				'title' => $title,
				'description' => $description,
				'author' => empty($author)
						? $entry_array['author']
						: $author,
				'location' => empty($location)
						? $entry_array['location']
						: $location,
				'categories' => array(),
				'subtypes' => array()
		);

		// make the validator for each file
		$mediaInfo = gallery_get_MediaInfo_by_filename($_FILES['files']['name'][$fkey]);
		if ($mediaInfo) {
			$extensions = implode(' ', $mediaInfo->fileExtensions);
			$file_size = $mediaInfo->maxFileSize;

			// calculate file size to byte
			$file_size = $file_size * 1024;
			$validators = array(
					'file_validate_extensions' => array(
							$extensions
					),
					'file_validate_size' => array(
							parse_size($file_size),
							0
					),
			);
			$file = file_save_upload($fkey, $validators, $destination[$key], FILE_EXISTS_RENAME);
			if ($file) {
				$file->status |= FILE_STATUS_PERMANENT;
				$file = file_save($file);
				$image['file_managed_fid'] = $file->fid;
				$add_to_gallery = in_array($mediaInfo->mediaType, array(GALLERY_MEDIATYPE_IMAGE, GALLERY_MEDIATYPE_SOUND, GALLERY_MEDIATYPE_VIDEO));
				if ($add_to_gallery) {
					$img_ids[$fkey] = gallery_image_save($image);
				} else {
					$img_ids[$fkey] = $file->fid;
					if ($file_managed_module_col) {
						$file_managed_table_entry[$file_managed_module_col] = $entry_array['item_id'];
						$file_managed_table_entry['file_managed_fid'] = $file->fid;
						$file_managed_table_entry['description'] = $image['description'];
						db_insert($module_name . '_file_managed')->fields(
								$file_managed_table_entry)
							->execute();
					}
				}
			} else {
				//FIXME
				// 			error about didn't saved...
				// die('fehlfer');
				$img_ids[$fkey] = 'Error! File not saved!';
			}
		}
		//FIXME
		// 			error about didn't saved...
		// die('fehlfer');
		$img_ids[$fkey] = 'Error! File not saved!';
	}
	return $img_ids;
}

/**
 * Return a list of files in edit mode, with
 * field for comment and checkboy for delete
 * @param string $module
 * 			The drupal module name
 * @param int $id
 * 			the id of the parent entry
 * @return array
 */
function gallery_implementation_edit_existing_file_list($module = false,
		$id = false) {

	if (!$module || !$id)
		return false;

	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js');
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js');
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/divbox.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	drupal_add_js(drupal_get_path('module', 'gallery') . '/js/divbox.js');
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/gallery.divbox.js');
	$types = _gallery_types();

	$subtypes = _gallery_get_subtypes($module, false);

	$header = array(
			'',
			t('Filename'),
			t('Title')
	);
	if ($subtypes)
		$header[] = t('Type');
	$header[] = t('Author');
	$header[] = t('Location');
	$header[] = t('Caption');
	$header[] = t('Operations');
	$header[] = t('Delete');

	$mediatype = 'medias';

	$images = gallery_images($mediatype, $module, $id, 'edit');
	$rows = array();
	foreach ($images[$mediatype] as $image) {

		if ($mediatype == "images"
				|| ($mediatype == "medias"
						&& ($image['filemime'] == "image/png"
								|| $image['filemime'] == "image/jpg"
								|| $image['filemime'] == "image/jpeg"
								|| $image['filemime'] == "image/gif"))) {

			$url_href = url(
				'gallery/' . $image['item_type'] . '/' . $image['item_id']
						. '/thumb/' . $image['id'] . '/gallery_large');
			$url_image = url(
				'gallery/' . $image['item_type'] . '/' . $image['item_id']
						. '/thumb/' . $image['id'] . '/gallery_mini');
			$extras = 'rel="lightbox"';
		} elseif ($mediatype == "sounds"
				|| ($mediatype == "medias"
						&& ($image['filemime'] == "audio/mpeg"))) {

			$url_href = file_uri_target($image['uri']);
			$url_href = url('sites/default/files/' . $url_href);
			$url_image = url(
				drupal_get_path('module', 'gallery')
						. '/images/music_icon_mini.png');
			$extras = 'class="divbox_sound"';
		} elseif ($mediatype == "videos"
				|| ($mediatype == "medias"
						&& ($image['filemime'] == "video/mp4"))) {

			$url_href = file_uri_target($image['uri']);
			$url_href = url('sites/default/files/' . $url_href);
			$url_image = url(
				drupal_get_path('module', 'gallery')
						. '/images/video_icon_mini.png');
			$extras = 'class="divbox_video"';
		}

		$caption = '';
		if (array_key_exists($image['item_type'], $types)
				&& array_key_exists(
					'image_caption',
					$types[$image['item_type']]))
			$caption = call_user_func_array(
				$types[$image['item_type']]['image_caption'],
				array($image['item_id']));
		$editable = gallery_edit_access_callback(
			$image['item_type'],
			$image['item_id'],
			$image);
		$row = array();
		$row[] = '<a ' . $extras . ' href="' . $url_href . '"><img src="'
				. $url_image . '" alt="' . $image['title'] . '" /></a>';
		$row[] = $image['filename'];
		$row[] = $image['title'];
		if ($subtypes) {
			$stypes = array();
			foreach ($image['subtypes'] as $subtype)
				$stypes[] = $subtypes[$subtype];
			$row[] = implode(', ', $stypes);
		}
		$row[] = $image['author'];
		$row[] = $image['location'];
		$row[] = $caption;
		if ($editable) {
			$link = 'gallery/' . $image['item_type'] . '/' . $image['item_id'] . '/' . $mediatype . '/edit/' . $image['id'];
			$row[] = l(t('edit'), $link);
		} else {
			$row[] = '&nbsp;';
		}
		$row[] = $editable
				? '<input type="checkbox" name="delete[' . $image['id']
						. ']" value="1" />' : '&nbsp;';
		$rows[] = $row;
	}

	if (!$rows)
		$form['empty'] = array(
				'#markup' => '<p>'
						. t(
							'There are no @mediatype available yet.',
							array('@mediatype' => t($mediatype))) . '</p>',
		);
	else
		$form['overview'] = array(
				'#theme' => 'table',
				'#header' => $header,
				'#rows' => $rows,
		);

	return $form;

}

/**
 * Delete gallery images
 * @param array $delete_arr
 * @return array
 * 			with img id's with true on success
 */
function gallery_implementation_delete($delete_arr) {

	if (!isset($delete_arr) || !is_array($delete_arr))
		return false;
	$state = array();

	foreach ($delete_arr as $iid => $del) {
		if (!$del)
			continue;
		$image = gallery_image_load($iid);
		if (!gallery_edit_access_callback(
			$image['item_type'],
			$image['item_id'],
			$image)) {
			continue;
		}
		$state[$iid] = gallery_image_delete($image);
	}
	return $state;
}

/**
 * Returns an array of files for a module with id
 * @param string $module
 * @param int $item_id
 * @return array
 */
function gallery_implementation_get_files($module = false, $item_id = false) {
	if (!$module || !$item_id)
		return false;
	return db_query(
		'SELECT fm.*, mfm.description FROM {' . $module
				. '_file_managed} mfm
			JOIN {file_managed} fm ON fm.fid=mfm.file_managed_fid
			WHERE ' . $module . '_id = :item_id',
		array(':item_id' => $item_id))->fetchAllAssoc('fid');

}

/**
 * Get the details for a file
 * @param int $fid
 * @return array
 */
function gallery_implementation_get_file($fid) {
	return (object) db_query(
		'SELECT * FROM {file_managed} WHERE fid = :fid',
		array(':fid' => $fid))->fetchAssoc();

}

/**
 * Return a files table for a module and id
 * @param string $modulename
 * @param int $item_id
 * @param bool $edit
 * return array
 */
function gallery_implementation_files_table($modulename = false,
		$item_id = false, $edit = false) {
	global $user;
	$render_array = array();
	if (!$modulename || !$item_id)
		return $render_array;

	$files = gallery_implementation_get_files($modulename, $item_id);
	if (count($files) < 1)
		return $render_array;

	foreach ($files as $record) {
		$mimeIconMap = array(
				'path' => drupal_get_path('module', 'commonstuff') . '/images/'
						. commonstuff_get_icon_map($record->filemime) . '.png',
				'alt' => $record->filemime,
				'title' => $record->filemime,
		);
		$img_mime = theme('image', $mimeIconMap);

		$description = isset($record->description) ? $record->description : '';
		$filename = l(
			$record->filename,
			"sites/default/files/naturvielfalt/$modulename/" . $item_id . '/'
					. $record->filename);
		$filemime = $img_mime;
		$filesize = ($record->filesize / 1000) . " KB";
		$edit
				? $desc = '<input type="text" name="file_description['
						. $record->fid . ']" id="file_description['
						. $record->fid . ']" value="' . $description . '">'
				: $desc = $description;
		$row = array();
		$row[] = $filemime;
		$row[] = $filename;
		$row[] = $desc;
		$row[] = $filesize;
		if ($edit)
			$row[] = '<input type="checkbox" value="1" name="file_delete['
					. $record->fid . ']"';
		$rows[] = $row;
	}

	$tableHeader[] = t('Type');
	$tableHeader[] = t('Filename');
	$tableHeader[] = t('Description');
	$tableHeader[] = t('Size');
	if ($edit)
		$tableHeader[] = t('Delete');

	$render_array = array(
			'#type' => 'fieldset',
			'#title' => t('Files'),
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
			),
	);

	$render_array['data'] = array(
			'#theme' => 'table',
			'#header' => $tableHeader,
			'#rows' => $rows,
			'#tableWidth' => 900,
			'#id_table' => 'files',
	);
	return $render_array;
}

/**
 * Delete a file managed
 * IMPORTANT: acl not checked
 * @param array $file_managed_ids
 * @param string $module
 */
function gallery_implementation_files_delete($file_managed_ids = array(),
		$module = false) {
	if (!is_array($file_managed_ids) || count($file_managed_ids) < 1
			|| !$module)
		return false;
	foreach ($file_managed_ids as $key => $value) {
		db_delete($module . '_file_managed')->condition(
				'file_managed_fid',
				$key)
			->execute();
		file_delete(gallery_implementation_get_file($key));
	}

}

/**
 * Update a file db entry
 * @param array $file
 */
function gallery_implementation_file_update($file = array()) {
	db_update('file_managed')->fields($file)
		->condition('fid', $file['fid'])
		->execute();
}

/**
 * Update the comment for a file in the module file_managed table
 * @param string $module
 * @param array $file_managed_descriptions
 * 			$file_managed_descriptions[$fid] = $value
 */
function gallery_implementation_file_updateComment($module,
		$file_managed_descriptions) {
	if (!isset($module) || !isset($file_managed_descriptions) || empty($module)
			|| !is_array($file_managed_descriptions))
		return false;

	foreach ($file_managed_descriptions as $key => $value) {
		$value = check_plain($value);
		$key = intval($key);
		db_update($module . '_file_managed')->fields(
				array('description' => $value))
			->condition('file_managed_fid', $key)
			->execute();
	}
}
