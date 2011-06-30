<?php 

function area_edit($area_id) {
	//include code for gmap_static_image_path
 	module_load_include('inc', 'area', 'area.search');
 	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
	drupal_add_js(drupal_get_path('module', 'area') . '/js/habitats.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/autocomplete-select.js');
	$render_array['area']['details'] = drupal_get_form('area_edit_form', $area_id);
	$render_array['area']['map'] = area_map($area_id);
	$render_array['file']['upload'] = drupal_get_form('area_edit_files_form', $area_id);
	return $render_array;
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
  		'#weight' => 7, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => FALSE,
		'#attached' => array (
      		'js' => array (
      		'misc/form.js',
        	'misc/collapse.js',
      		),
    	),
    	'#attributes' => array(
      		'class' => array('collapsible'),
      		'id' => 'area-files'
    	),
	);
	
  $tableHeader[] = t('Description');
  $tableHeader[] = t('Filename');
  $tableHeader[] = t('Typ');
  $tableHeader[] = t('Size');
  $tableHeader[] = t('Date');

  $result = db_query('SELECT fm.filename, fm.filemime, fm.timestamp, fm.fid, fm.filesize, afm.description
            FROM area_file_managed afm, file_managed fm
            WHERE fm.fid = afm.file_id
            AND afm.area_id = :id;', array(':id' => $area_id)
  );

  if(!empty($result)) {
    foreach ($result as $record) {
	    $mimeIconMap = array(
	      'path' => drupal_get_path('module', 'inventory').'/images/' . area_get_icon_map($record->filemime).'.png',
	      'alt' => $record->filemime,
	      'title' => $record->filemime,
	    );
	    $img_mime = theme('image', $mimeIconMap);
	    
	    $description = $record->description;
	    $filename = l($record->filename, 'sites/default/files/swissmon/area/'.$area_id.'/'.$record->filename);

      $filemime = $img_mime;
      $filesize = ($record->filesize / 1000)." KB";
      $timestamp = date('d.m.Y',$record->timestamp);
      $row = array();
      $row[] = $description;
      $row[] = $filename;
      $row[] = $filemime;
      $row[] = $filesize;
      $row[] = $timestamp;
      $rows[] = $row;
    }

    if(isset($rows)) {
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

  $form['AreaFileForm']['upload_submit'] = array(
    '#type' => 'submit', 
    '#value' => t("Upload"),
    '#submit' => array('area_update_file'),
  );
  
  // Saving something to the form_state makes it accessible in the submit function.
  // We do this to make sure the sgid is taken from the url which has passed security checkpoints.
  $form_state['storage']['area_id'] = $area_id;
  
  return $form;
}


function area_update_file($form, &$form_state){

  $area_id = $form_state['storage']['area_id'];

  // File upload
  $validators = array(
    'file_validate_extensions' => array('doc txt pdf docx'),
    'file_validate_size' => array(1000000, 0),
  );

  $destination = 'public://swissmon/area/'.$area_id;

  file_prepare_directory($destination, FILE_CREATE_DIRECTORY);

  $file = file_save_upload('upload', $validators, $destination);
  if ($file) {

    $file->status |= FILE_STATUS_PERMANENT;
    $file = file_save($file);

    $form_state['values']['upload'] = $file;

    $nid = db_insert('area_file_managed')->fields(array(
        'area_id' => $area_id,
        'file_id' => $file->fid,
        'description' => $form_state['values']['file_description']
      ))
      ->execute();
    drupal_set_message(t('File saved'));
  }
  else {
    drupal_set_message(t('File could not be saved!'), 'error');
  }

  return $form_state['redirect'] = 'area/'.$area_id.'/edit';
}

function area_edit_form($form, &$form_state, $area_id) {
	$area = db_query('
	SELECT *
	FROM area
	WHERE id = :id',
	array(':id' => $area_id)
	);
	$area = $area->fetch();
	drupal_set_title($area->field_name);
	
	
	$form['AreaEditForm'] = array(
		'#type' => 'fieldset',
  		'#title' => t('Edit area properties'), 
  		'#weight' => 1, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => FALSE,
		'#attached' => array (
      		'js' => array (
      		'misc/form.js',
        	'misc/collapse.js',
      		),
    	),
    	'#attributes' => array(
      		'class' => array('collapsible'),
      		'id' => 'area-details'
    	),
	);
	$form['AreaEditForm']['field_name'] = array(
  	'#type' => 'textfield',
  	'#title' => t('Area name'),
	'#value' => $area->field_name,
	'#size' => 50,
	'#required' => true,
	);
	
	$form['AreaEditForm']['habitatText'] = array(
	'#title' => 'Habitats',
	'#type' => 'textfield',
	'#size' => 50,
    '#autocomplete_path' => 'area/habitat_autocomplete',
	'#suffix' => '<div id="habitatSelected"><table><tr>'.area_get_habitat_values($area_id).'</tr></table></div>',
	);
	$form['AreaEditForm']['comment'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
	'#value' => $area->comment,
    '#cols' => 50,
    '#rows' => 10,
	);
	$form['AreaEditForm']['area_id'] = array(
    '#type' => 'hidden',
	'#attributes' => array('id' => 'edit-id'),
	'#value' => $area_id,
	);
	$form['AreaEditForm']['area_type'] = array(
    '#type' => 'hidden',
	'#attributes' => array('id' => 'edit-area-type'),
	);
	$form['AreaEditForm']['AreaEditForm_submit'] = array(
	  	'#type' => 'submit',
	  	'#value' => t('Submit'),
	);

	return $form;
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
//	global $user;
	
	// update area
	$area_update = db_update('area')->fields(array(
    	'field_name' => $field_name,
		'comment' => $comment,
		'modify_time' => 'now()',
	))
	->condition('id',$area_id)
	->execute();

	// insert habitats
	$area_habitat_delete = db_delete('area_habitat')
	->condition('area_id', $area_id)
	->execute();
	
	//if (array_key_exists('habitat', $form_state['input']) OR array_key_exists('habitat', $form_state['values'])) {
		foreach (array_values($form_state['input']['habitat']) as $habitat) {
				db_insert('area_habitat')->fields(array(
					'area_id' => $area_id,
					'habitat_id' => $habitat,
				))
				->execute();
		//}
	}

	// redirect to create a new inventory assigned to the area just created
	$form_state['redirect'] = 'area/'.$area_id.'/edit';
}

function area_get_habitat_values($area_id) {
	$result = db_query('
		SELECT h.id, h.name_de, h.label
		FROM AREA
		LEFT JOIN area_habitat ah ON area.id = ah.area_id
		LEFT JOIN habitat h ON ah.habitat_id = h.id
		WHERE area.id = :id',
		array('id' => $area_id)
	);
	$habitat = '';
	$removeImg = drupal_get_path('module', 'inventory')."/images/can_delete.png";
	foreach($result as $record) {
		if(!empty($record->id)) {
			$id = "<input type='hidden' name='habitat[" . $record->id."]' value='" . $record->id."'/>";
			$label = "<td>" . $record->label . "</td>";
			$name = "<td>" .$record->name_de. "</td>";
			$remove = "<td><img alt ='remove' src='../../" . $removeImg . "' onClick='removeHabitat(this);' style='cursor:pointer'/>";
			$habitat = $habitat."<tr>" . $id . $label . $name . $remove . "</tr>";
			$form_state['input']['habitat'][$record->id] = $record->id;
		}
	}
	return $habitat;	
}

function area_map($area_id) {
	$output['area_map'] = array(
    '#type' => 'fieldset',
    '#title' => t('Map'),
    '#weight' => 2,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#attached' => array (
      'js' => array (
        'misc/form.js',
        'misc/collapse.js',
      ),
    ),
    '#attributes' => array(
      'class' => array('collapsible'),
      'id' => 'area-map'
    ),
  );

  $imagepath = gmap_static_image_path($area_id, 500, 500);
  $variables_map= array(
    'path' => $imagepath,
    'alt' => t('Map'),
    'title' => t('Map'),
  );
  $output['area_map']['image'] = array(
    '#markup' => theme('image', $variables_map)
  );
  return $output;
}

?>