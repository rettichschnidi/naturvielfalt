<?php 

function area_edit($area_id) {
	//include code for gmap_static_image_path
 	module_load_include('inc', 'area', 'area.search');
 	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
	drupal_add_js(drupal_get_path('module', 'area') . '/js/habitats.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/parcels.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/autocomplete-select.js');
	$render_array['area']['details'] = drupal_get_form('area_edit_form', $area_id);
	//$render_array['area']['parcels'] = drupal_get_form('area_parcels_form', $area_id);
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
	$form['AreaEditForm']['habitat_dropdown'] = array(
  	'#type' => 'select',
  	'#title' => t('Habitats (hold CTRL to select multiple values)'),
	'#options' => area_habitat_dropdown_values($area_id),
	'#multiple' => TRUE,
	'#default_value' => area_habitat_values($area_id),
	'#size' => 10
	);
/*	$form['AreaEditForm']['habitatText'] = array(
	'#title' => 'Habitats',
	'#type' => 'textfield',
	'#size' => 50,
    '#autocomplete_path' => 'area/habitat_autocomplete',
	'#suffix' => '<div id="habitatSelected"><table><tr>'.area_habitat_values($area_id).'</tr></table></div>',
	);
*/	$form['AreaEditForm']['parcels'] = area_parcels_form($area_id);	
	$form['AreaEditForm']['protection_target'] = array(
    '#type' => 'textarea',
    '#title' => t('Protection target'),
	'#value' => $area->protection_target,
    '#cols' => 50,
    '#rows' => 2,
	);
	$form['AreaEditForm']['safety_precautions'] = array(
    '#type' => 'textarea',
    '#title' => t('Safety precautions'),
	'#value' => $area->safety_precautions,
    '#cols' => 50,
    '#rows' => 2,
	);
	$form['AreaEditForm']['tending_strategies'] = array(
    '#type' => 'textarea',
    '#title' => t('Tending strategies'),
	'#value' => $area->tending_strategies,
    '#cols' => 50,
    '#rows' => 2,
	);
	$form['AreaEditForm']['comment'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
	'#value' => $area->comment,
    '#cols' => 50,
    '#rows' => 3,
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
	$protection_target = $form_state['input']['protection_target'];
	$safety_precautions = $form_state['input']['safety_precautions'];
	$tending_strategies = $form_state['input']['tending_strategies'];
//	global $user;
	
	// update area
	$area_update = db_update('area')->fields(array(
    	'field_name' => $field_name,
		'comment' => $comment,
		'modify_time' => 'now()',
		'protection_target' => $protection_target,
		'safety_precautions' => $safety_precautions,
		'tending_strategies' => $tending_strategies,
	))
	->condition('id',$area_id)
	->execute();

	// insert habitats
	$area_habitat_delete = db_delete('area_habitat')
	->condition('area_id', $area_id)
	->execute();
	
	foreach (array_values($form_state['input']['habitat_dropdown']) as $habitat) {
			db_insert('area_habitat')->fields(array(
				'area_id' => $area_id,
				'habitat_id' => $habitat,
			))
			->execute();
	}
	
	//insert parcels
	$area_parcel_delete = db_delete('area_parcel')
	->condition('area_id', $area_id)
	->execute();
	
	foreach(array_values($form_state['input']['parcel']) as $parcel) {
		$parcel_nr = $parcel['parcel_nr'];
		$parcel_owner = $parcel['owner'];
		$delete = $parcel['delete'];
		if($delete != '1') {
			db_insert('area_parcel')->fields(array(
				'area_id' => $area_id,
				'parcel' => $parcel_nr,
				'parcel_owner_name' => $parcel_owner,
			))
			->execute();
		}
	}

	// redirect to create a new inventory assigned to the area just created
	$form_state['redirect'] = 'area/'.$area_id.'/edit';
}

function area_habitat_values($area_id) {
	$result = db_query('
		SELECT h.id, h.name_de, h.label
		FROM AREA
		LEFT JOIN area_habitat ah ON area.id = ah.area_id
		LEFT JOIN habitat h ON ah.habitat_id = h.id
		WHERE area.id = :id',
		array('id' => $area_id)
	);
	$habitat = '';
/*	$removeImg = drupal_get_path('module', 'inventory')."/images/can_delete.png";
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
*/
	foreach ($result as $record) {
		$habitat[] = $record->id;
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

function area_parcels_form($area_id) {
	$parcels = db_select('area_parcel', 'ap')
		->condition('area_id', $area_id)
		->fields('ap')
		->execute();
		
	$count = $parcels->rowCount();
	$headings = array(
		t('Parcel'),
		t('Parcel owner'),
		t('Delete')
		);
	$rows = array();
	$i = 0;
	if($count != '0') {
		foreach($parcels as $parcel) {
		array_push($rows, array(
			'<input type="text" size="4" name="parcel['.$i.'][parcel_nr]" value="'.$parcel->parcel.'" />',
			'<input type="text" size="33" name="parcel['.$i.'][owner]" value="'.$parcel->parcel_owner_name.'" />'.
			'<input type="hidden" name="parcel['.$i.'][id]" value="'.$parcel->id.'" />',
			'<input type="checkbox" name="parcel['.$i++.'][delete]" value="1" />'
			));
		}
	}
	else {
		array_push($rows, array(
			'<input type="text" size="4" name="parcel['.$i.'][parcel_nr]" value="'.'" />',
			'<input type="text" size="33" name="parcel['.$i.'][owner]" value="'.'" />'.
			'<input type="hidden" name="parcel['.$i.'][id]" value="'.'" />',
			'<input type="checkbox" name="parcel['.$i++.'][delete]" value="1" />'
			));
	}

	$form['parcels'] = array(
	'#theme' => 'table',
	'#header' => $headings,
	'#rows' => $rows,
	'#attributes' => array(
		'id' => 'parcels',
	),
	);

return $form;	
}
?>