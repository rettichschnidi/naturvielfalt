<?php
module_load_include('inc', 'observation');
module_load_include('inc', 'observation', 'observation.ajax');
module_load_include('inc', 'organism');
module_load_include('inc', 'gallery', 'gallery.implementations');
module_load_include('inc', 'observation', 'observation.validate');

/**
 * Show the form to add or edit an observation
 * @param array $observation
 */
function observation_form($form = null, &$form_state = null, $observation = false) {
	drupal_add_library('system', 'ui.dialog');
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_js($observation_path . '/js/observation.js');
	drupal_add_js($observation_path . '/js/tooltip.js');
	global $user;
	$edit = false;

	drupal_set_title(t('Capture a new observation'));
	$url = base_path() . 'observation/save';
	if ($observation) {
		$url = base_path() . 'observation/' . $observation['observation']['id']. '/save';
		$species = $observation['organism']['name_lat'];
		if (isset($observation['organism']['name_lang']))
			$species .= ' / ' . $observation['organism']['name_lang'];
		drupal_set_title(t('Edit observation of <i>@species</i>', array('@species' => check_plain($species))), PASS_THROUGH);
		$edit = true;
	}
	// if not edit, set standard values
	if (!isset($observation['observation']['id']))
		$observation['observation']['id'] = '';
	if (!isset($observation['observation']['found_as_lang']))
		$observation['observation']['found_as_lang'] = 'false';
	if (!isset($observation['observation']['found_as_lat']))
		$observation['observation']['found_as_lat'] = 'false';
	if (!isset($observation['observation']['organism_artgroup_id'])) {
		// Assuming artgroup 'all' has the id 1.
		$observation['observation']['organism_artgroup_id'] = '1';
	}
	if (!isset($observation['observation']['organism_id']))
		$observation['observation']['organism_id'] = '';
	if (!isset($observation['organism']['name_lat']))
		$observation['organism']['name_lat'] = '';
	if (!isset($observation['organism']['name_lang']))
		$observation['organism']['name_lang'] = '';
	isset($observation['observation']['date'])
			? $timestamp = $observation['observation']['date']
			: $timestamp = time();
	if (!isset($observation['observation']['count']))
		$observation['observation']['count'] = '1';
	if (!isset($observation['observation']['observer']))
		$observation['observation']['observer'] = $user->name;
	if (!isset($observation['observation']['organism_artgroup_detmethod_id']))
		$observation['observation']['organism_artgroup_detmethod_id'] = false;

	$form_id = "observation_form";

	module_load_include('inc', 'organism', 'organism.artgroup');

	$form = array();
	$form['group'] = array(
			'#id' => 'organism_group',
			'#prefix' => '<div id="organism_replace">',
			'#suffix' => '</div>',
			'#tree' => true,
	);	

	$json = !isset($form_state['values']['artgroup_id']) ? 'organism/search/json/'.'1' : 'organism/search/json/'.$form_state['values']['artgroup_id'];
	$form['group']['organism'] = array(
			'#id' => 'organism_name',
			'#title' => t('Search for species'),
			'#type' => 'textfield',
			'#autocomplete_path' => $json,
			'#description' => t('Maximum 30 search results will be showed.'),
			'#attributes' => array(
					'placeholder' => t('Start typing to see the species...'),
			),
			'#tree' => true,
			'#required' => true,
			'#default_value' => $observation['organism']['name_lang'],
	);
	

	
	$form['group']['date'] = array(
			'#id' => 'date',
			'#title' => t('Date of the observation'),
			'#type' => 'textfield',
			'#attributes' => array('class' => array('datepicker')),
			'#required' => true,
			'#tree' => true,
			'#default_value' => date('d.m.Y', $timestamp),
	);
	
	$form['#after_build'] = array('observation_form_uidatepicker');
	
	$form['group']['count'] = array(
			'#id' => 'count',
			'#title' => t('Count of found individuals'),
			'#type' => 'textfield',
			'#required' => true,
			'#default_value' => $observation['observation']['count'],
			'#attributes' => array(
					'placeholder' => t('Some text'),
			),
			'#element_validate' => array('element_validate_number'),
			'#tree' => true,
	);
	$form['group']['observer'] = array(
			'#id' => 'observer',
			'#title' => t('Observer'),
			'#type' => 'textfield',
			'#disabled' => true,
			'#default_value' => $observation['observation']['observer'],
			'#attributes' => array(
					'placeholder' => t('Some text'),
			),
	);
	
	if(!$edit)
		$artgroup_id = !empty($form_state['values']['artgroup_id']) ? $form_state['values']['artgroup_id'] : 1;
	else $artgroup_id = $observation['observation']['organism_artgroup_id'] ;
	

	
	$form['group']['details'] = array(
			'#type' => 'fieldset',
			'#title' => t('Details'),
			'#tree' => true,
			'#collapsible' => true,
			'#collapsed' => false,
	);

	if($artgroup_id > 1) {
		$options = organism_artgroup_get_determation_methods($observation['observation']['id'], $artgroup_id);
		if(!empty($options)) { // don't show determination method if no options
			$form['group']['details']['determination'] = array(
					'#title' => t('Determination method'),
					'#type' => 'select',
					'#options' => 	organism_artgroup_get_determation_methods($observation['observation']['id'], $artgroup_id),
					'#description' => 'Determination Method',
					'#default_value' => $observation['observation']['organism_artgroup_detmethod_id'],
					'#tree' => true,
			);
		}
		
		$observation_attributes = observation_get_attributes($observation['observation']['id'], $artgroup_id);
		foreach ($observation_attributes as $key => $attribute) {
			$form['group']['details']['attributes'][$attribute['id']] = array(
					'#id' => $attribute['id'],
					'#title' => $attribute['name'],
					'#type' => $attribute['type_drupal'],
					'#weight' =>$attribute['id']+12,
					'#tree' => true,
					'#default_value' => $attribute['value'],
			);
		}
	}

  
	// show artgroup select if new observation, otherwise show the selected artgroup (not changeable)
	if($edit) {
		
		$artgroup_name = organism_artgroup_get_artgroup($observation['observation']['organism_artgroup_id'],true);
		
		$form['artgroup_text'] = array(
				'#id' => 'artgroup_text',
				'#title' => t('Artgroup'),
				'#type' => 'textfield',
				'#default_value' => $artgroup_name,
				'#weight' => -1,
				'#disabled' => true,
		);
		
		$form['artgroup_id'] = array(
				'#id' => 'area_artgroup',
				'#type' => 'hidden',
				'#default_value' => $observation['observation']['organism_artgroup_id'],
		);
	}
	else {
		
		$form['artgroup_id'] = array(
				'#id' => 'artgroup_id',
				'#title' => t('Filter on species group'),
				'#type' => 'select',
				'#options' => organism_artgroup_get_all_as_array(),
				'#default_value' => $observation['observation']['organism_artgroup_id'],
				'#ajax' => array(
						'event' => 'change',
						'callback' => 'observation_organism_callback',
						'wrapper' => 'organism_replace'
				),
				'#weight' => -1,
		);
	}
	
	$form['button'] = array(
			'#type' => 'submit',
			'#value' => t('Save'),
			'#submit' => array(
					'observation_save'
			)
	);
	
	$form['div'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
	);
	
	/**
	 * Area map include
	 */
	$form['observation_map'] = array(
			'#type' => 'textfield',
			'#attributes' => array(
					'id' => 'coordinate',
					'name' => 'coordinate',
					'style' => array(
							'display: none;',
							'height: 0px;'
					)
			),
	);
	
	$form['div']['hiddenfields'] = area_address_hidden_form();

	if(empty($observation['observation']['id'])) {
		$form['div']['map'] = array(
				'#theme' => 'area',
				'#mapid' => 'observationmap',
				'#coordinate_storage_id' => 'coordinate',
				'#ch1903' => true,
				'#search' => true,
				'#height' => '250px',
				'#action' => 'getcoordinate',
		);
	} else {
		$form['div']['map'] = array(
				'#theme' => 'area',
				'#mapid' => 'observationmap',
				'#coordinate_storage_id' => 'coordinate',
				'#ch1903' => true,
				'#search' => true,
				'#infowindow_content_fetch_url' => base_path() . 'observation/{ID}/overview/ajaxform',
				'#geometries_fetch_url' => base_path() . 'observation/'
				. $observation['observation']['id'] . '/geometry/json',
				'#geometry_edit_id' => $observation['observation']['id'],
				'#height' => '350px',
				'#action' => 'edit',
		);
		
	}
	// warn when leaving with changes not yet saved
	$form['safetynet'] = array(
			'#theme' => 'commonstuff.safetynet'
	);

	/**
	 * include a upload form for images
	 */
	// Product picture
	if ($observation['observation']['id'] != '') {
		$form['picture'] = array(
				'#type' => 'fieldset',
				'#title' => t('Manage existing media for this observation'),
				'#collapsible' => true,
				'#collapsed' => false,
		);
	$form['picture']['media_table'] = gallery_implementation_edit_existing_file_list(
			'observation',
			$observation['observation']['id']);
	$form['files'] = gallery_implementation_files_table(
			'observation',
			$observation['observation']['id'],
			true);
	}
	$file_extensions = implode(' ', gallery_get_valid_file_extensions());
	
	$form['media'] = array(
		'#type'=> 'fieldset',
		'#title'=> t('Add media for this observation'),
		'#collapsible' => true,
		'#collapsed' => false,
		'#tree' => true,
	);
	
	$numbers = array();
	for($i=1; $i <= 10; $i++) {
		$numbers[$i] = $i;
	}
	$default = !empty($form_state['values']['media']['file_number']) ? $form_state['values']['media']['file_number'] : 1;
	$form['media']['file_number'] = array(
			'#title' => t('Number of files to upload?'),
			'#type' => 'select',
			'#options' => $numbers,
			'#default_value' => $default,
			'#ajax' => array(
					'event' => 'change',
					'callback' => 'observation_add_uploadslot',
					'wrapper' => 'upload-div',
					'method' => 'replace',
					'effect' => 'fade',
			),
			'#tree' => true,
	);
	
	$form['media']['upload'] = array(
			'#prefix' => '<div id="upload-div"> <p>' . t('Allowed files endings are: ') . $file_extensions . '</p>',
			'#suffix' => '</div>',
			'#type' => 'fieldset',
			'#tree' => true,
	);
	for ($i=0; $i<$default; $i++) {	
		$form['media']['upload'][$i] = array(
			'#name' => 'files['.$i.']',
			'#id' => 'file_input'.$i,
			'#type' => 'file',
			'#prefix' => '<div id="picture_upload__'.$i.'">
						 <input type="text" style="display: none; height: 0px;" id="meta_title" name="meta_title[]" value="">
 						 <input type="text" style="display: none; height: 0px;" id="meta_description" name="meta_description[]" value="">
 					     <input type="text" style="display: none; height: 0px;" id="meta_location" name="meta_location[]" value="">
					     <input type="text" style="display: none; height: 0px;" id="meta_author" name="meta_author[]" value="">
						',
			'#suffix' => ($i+1 < $default) ? '<a onclick="observation.galleryMetaDataDialog($(this).parent());return false;" > '	. t('Click here to add details to this file') . '</a><hr></div>' 
										 : '<a onclick="observation.galleryMetaDataDialog($(this).parent());return false;" > '	. t('Click here to add details to this file') . '</a></div>',
											
			'#title' => t('Choose a file'),
			'#size' => 60,
			'#tree' => true,
		);
	}
	
	$form['media']['button'] = array(
			'#type' => 'submit',
			'#value' => t('Save'),
			'#submit' => array(
					'observation_save'
			),
			'#tree' => true
	);
	
	/* Tooltip texts */
	$tooltip['text']['artengruppe'] = t('Select a specimen group from the dropdown menu to constrain the search.');
	$tooltip['text']['art'] = t('Search for the specimen by inputing at least 2 chars.');
	$tooltip['text']['datum'] = t('Date of observation (dd/mm/yyyy)');
	$tooltip['text']['anzahl'] = t('Number of observed specimens.');
	$tooltip['text']['observer'] = t('Name of observer (You)');
	$tooltip['text']['methode'] = t('Select a method for determination from the dropdown menu.');
	$tooltip['text']['bemerkung'] = t('Provide any additional information.');
	$tooltip['text']['position'] = t('Precision of place of observation.');
	
	/* Hidden fields */
	$form['observer'] = array('#type' => 'hidden', '#id' => 'observer', '#name' => 'observer', '#value' => $observation['observation']['observer']);
	
	
	$form['observation_id'] = array('#type' => 'hidden', '#id' => 'observation_id', '#name' => 'observation_id', '#value' => $observation['observation']['id']);
	
	if(isset($observation['observation']['area_geometry_id']))
		$form['area_geometry_id'] = array('#type' => 'hidden', '#id' => 'area_geometry_id', '#name' => 'area_geometry_id', '#value' => $observation['observation']['area_geometry_id']);
	
	$form['recent_observation'] = array(
			'#theme' => 'datatable',
			'#header' => observation_get_standard_datatable_headers(9, 'desc'),
			'#title' => t(
				'Recently recorded and saved observations (click on a row, to edit an entry)'),
			'#id_table' => 'recent_observations',
			'#options' => array(
					'jsonUrl' => base_path()
							. "observation/get/writeable",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
									window.location.href = Drupal.settings.basePath+'observation/'+id+'/edit';})}",
					'rpShowAll' => true,
					'onSuccessHandler' => 'observation.onTableSuccess',
			),
			'#weight' => 100,
	);

	if(observation_admin_access($observation)) {
		$form['acl'] =  array(
				'#type' => 'fieldset',
				'#title' => t('Permissions'),
				'#weight' => 99,
				'#prefix' => '</form>',
				'#collapsible' => true,
				'#collapsed' => false,
				'#tree' => true,
		);
		$form['acl']['content'] = observation_acl_template($observation);
	}
	
	return $form;
}

function observation_add_uploadslot($form, &$form_state) {
	return $form['media']['upload'];
}
/**
 * ACL permission template
 * @param array $observation
 */
function observation_acl_template($observation = NULL) {
	$defaults = array(
			array(
					'type' => variable_get("ACL_READ"),
					'name' => t('Read'),
					'value' => FALSE,
			),
			array(
					'type' => variable_get("ACL_WRITE"),
					'name' => t('Write'),
					'value' => FALSE,
			),
			array(
					'type' => variable_get("ACL_ADMIN"),
					'name' => t('Administration'),
					'value' => FALSE,
			)
	);

	$acl = array(
			'#theme' => 'c_acl',
			'#defaults' => $defaults,
			'#acl_id' => $observation['observation']['acl_id'],
			'#add_element' => true,
			'#tree' => true,
	);
	return $acl;
}

/**
 * Show an observation
 * @param array $observationdata
 */
function observation_show_observation($observationdata = NULL) {
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_css($observation_path . '/css/observation.css');
	drupal_add_js('misc/form.js');
	drupal_add_js('misc/collapse.js');
	
	$species = $observationdata['organism']['name_lat'];
	if (isset($observationdata['organism']['name_lang']))
		$species .= ' / ' . $observationdata['organism']['name_lang'];
	drupal_set_title(t('Show observation of <i>@species</i>', array('@species' => check_plain($species))), PASS_THROUGH);

	/* Create a fieldset for the tabular data */
	$output['observation'] = array(
			'#type' => 'fieldset',
			'#title' => t('Observation'),
			'#weight' => 1,
			'#attributes' => array(
					'id' => 'observation-show-details' // required for CSS
			)
	);

	$output['observation']['table'] = observation_get_table($observationdata);

	/* Create a fieldset for the google maps */
	$output['observation_map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 3,
			'#attributes' => array(
					'id' => 'observation-show-details-map' // required for CSS
			)
	);
	
	$output['observation_map']['map'] = array(
			'#theme' => 'area',
			'#mapid' => 'observationmap',
			'#scalecontrol' => true,
			'#defaultzoom' => '15',
			'#reticle' => false,
			'#infowindow_content_fetch_url' => base_path() . 'observation/{ID}/overview/ajaxform',
			'#geometries_fetch_url' => base_path() . 'observation/'
			. $observationdata['observation']['id'] . '/geometry/json',
			'#geometry_edit_id' => $observationdata['observation']['id'],
			'#height' => '350px',
			'#action' => 'show',
	);

	$output['observation_images'] = gallery_list_renderer(
		'images',
		'observation',
		$observationdata['observation']['id']);
	$output['observation_images']['#weight'] = 11;

	$output['observation_videos'] = gallery_list_renderer(
		'videos',
		'observation',
		$observationdata['observation']['id']);
	$output['observation_videos']['#weight'] = 13;

	$output['observation_mp3'] = gallery_list_renderer(
		'sounds',
		'observation',
		$observationdata['observation']['id']);
	$output['observation_mp3']['#weight'] = 12;

	$output['observation_files'] = gallery_implementation_files_table(
		'observation',
		$observationdata['observation']['id']);
	$output['observation_files']['#weight'] = 14;

	/* Create a fieldset for the comment text field */
	$output['observation_geometry'] = array(
			'#type' => 'fieldset',
			'#title' => t('Position data'),
			'#weight' => 9,
			'#collapsible' => true,
			'#collapsed' => false,
			'#attributes' => array(
					'id' => 'observation-geometry',
					'class' => array(
							'clear',
							'collapsible',
							'collapsed'
					),
			)
	);

	$output['observation_geometry']['table'] = observation_get_geometry_table(
		$observationdata);

	/* Create a fieldset for the comment text field */
	$output['observation_attributes'] = array(
			'#type' => 'fieldset',
			'#title' => t('Details'),
			'#weight' => 10,
			'#collapsible' => true,
			'#collapsed' => false,
			'#attributes' => array(
					'id' => 'observation-attributes',
					'class' => array(
							'clear',
							'collapsible',
							'collapsed'
					)
			)
	);

	$output['observation_attributes']['table'] = observation_get_details_table(
		$observationdata);

	return $output;
}

function observation_organism_callback($form, &$form_state) {
	
	return $form['group'];
}

function observation_form_uidatepicker($form, $form_state) {
	drupal_add_library('system', 'ui.datepicker');
	 drupal_add_js('jQuery(document).ready(function(){jQuery( ".datepicker" ).datepicker({
      dateFormat: "dd.mm.yy",
      autoSize: true
    });});', 'inline'); 
	return $form;
}
