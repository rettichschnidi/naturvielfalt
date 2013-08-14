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
function observation_form($form = null, &$form_state = null, $observation = false, $inventory_id = NULL) {
	drupal_add_library('system', 'ui.dialog');
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_js($observation_path . '/js/observation.js');
	drupal_add_css($observation_path . '/css/observation.css');
	
	global $user;
	$edit = false;

	drupal_set_title(t('Capture a new observation'));
	$url = base_path() . 'observation/save';
	if ($observation) {
		$url = base_path() . 'observation/' . $observation['observation']['id']. '/save';
		if($observation['observation']['organism_id'] == UNKNOWN_SPECIES) {
			$species = t('Unknown species');
		} else {
			$species = $observation['organism']['name_lat'];
			if (isset($observation['organism']['name_lang']))
				$species .= ' / ' . $observation['organism']['name_lang'];
		}
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
	
	$organism_empty = false;
	//determine if a ajax callback was executed by the field artgroup or organism
	if(isset($form_state['triggering_element'])) {
		switch($form_state['triggering_element']['#id']) {
			case 'organism_name' :
				//set the artgroup of the organism if not yet selected
				if($form_state['values']['group']['artgroup_id'] == 1)  {
					if(observation_validate_organism($form_state['values']['group']['organism'])) {
						$artgroup = organism_artgroup_byname($form_state['values']['group']['organism']);
						$artgroup_id = $artgroup['organism_artgroup_id'];
					}
					else $artgroup_id = $form_state['values']['group']['artgroup_id'];
				}
				else {
					$artgroup_id = !empty($form_state['values']['group']['artgroup_id']) ? $form_state['values']['group']['artgroup_id'] : 1;
				}
				break;
			case 'artgroup_id' :
				//set the artgroup and empty organism field
				$artgroup_id = $form_state['input']['group']['artgroup_id'];
				$organism_empty = true;
				break;
			default :
				$artgroup_id = !empty($form_state['values']['group']['artgroup_id']) ? $form_state['values']['group']['artgroup_id'] : 1;
				break;
		}
	}
	else if(!$edit) {
		$artgroup_id = !empty($form_state['values']['group']['artgroup_id']) ? $form_state['values']['group']['artgroup_id'] : 1;
	}
	else $artgroup_id = !empty($form_state['values']['group']['artgroup_id']) ? $form_state['values']['group']['artgroup_id'] : $observation['observation']['organism_artgroup_id'];
	
	$form = array();
	$form['group'] = array(
			'#id' => 'organism_group',
			'#prefix' => '<div id="organism_replace">',
			'#suffix' => '</div>',
			'#tree' => true,
	);	
	
	//prepare image of organism, only display if organism and artgroup id match
	if($artgroup_id) {
		if(!empty($form_state['values']['group']['organism'])) {
			$artgroup = organism_artgroup_byname($form_state['values']['group']['organism']);
			if($artgroup['organism_artgroup_id'] != $artgroup_id) 
				$organism_forimage = null;
			else
				$organism_forimage = organism_get_byname(filter_xss($form_state['values']['group']['organism']));
		} else if(!empty($observation['observation']['organism_id'])) {
			$organism_forimage = organism_get(filter_xss($observation['observation']['organism_id']));
		}
		else $organism_forimage = null;
		if($organism_forimage) {
			$images = gallery_images(GALLERY_MEDIATYPE_IMAGE, 'organismreference', $organism_forimage['id']);
			if(!empty($images) && $images['count'] > 0) {
				$img = '
							<img src="' . url('gallery/observation/' . $organism_forimage['id'] . '/thumb/' . $images['images'][0]['id'] . '/gallery_presentation') . '" alt="' . $images['images'][0]['title'] . '" title="' . $images['images'][0]['title'] . '" />
						';
			}
			else {
				$img = '
							<img src="/sites/all/modules/datatable/images/no_photo.png" />
						';
			}
		}
		else $img = '';
	} else $img = '';
	
	$checked = isset($form_state['values']['group']['unknown_check']) ? $form_state['values']['group']['unknown_check'] : $observation['observation']['organism_id'] == UNKNOWN_SPECIES ? 1 : 0;

	$form['group']['unknown_check'] = array(
			'#id' => 'unknown_check',
			'#title' => t('Unkown species?'),
			'#description' => t('Do you need help with determination?'),
			'#type' => 'checkbox',
			'#default_value' => $checked,
			'#tree' => true,
			'#ajax' => array(
						'event' => 'change',
						'callback' => 'observation_unknown_check_callback',
						'wrapper' => 'organism_replace',
			),
	);
	
	$json = 'organism/search/json/'.$artgroup_id;
	
	if(!empty($img)) {
		$form['group']['organism_image'] = array(
			'#markup' => '<div class="observation_organism_reference_image">'.$img.'</div>',	
		);
	}
	$form['group']['organism'] = array(
			'#id' => 'organism_name',
			'#title' => t('Search for species'),
			'#type' => 'textfield',
			'#autocomplete_path' => $json,
			'#description' => t('Maximum 30 search results will be showed.'),
			'#attributes' => array(
					'placeholder' => t('Start typing to see the species...'),
			),
			'#ajax' => array(
						'event' => 'change',
						'callback' => 'observation_organism_callback',
						'wrapper' => 'organism_replace',
			),
			'#tree' => true,
			'#required' => $checked ? false : true,
			'#disabled' => $checked ? true : false,
			'#default_value' => isset($form_state['values']['group']['organism']) ? $form_state['values']['group']['organism'] : $observation['organism']['name_lang'],
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
	
	$form['group']['time'] = array(
			'#id' => 'time',
			'#title' => t('Time of the observation'),
			'#attributes' => array('class' => array('timepicker')),
			'#type' => 'textfield',
			'#required' => true,
			'#tree' => true,
			'#default_value' => date('H:i', $timestamp),
	);
	
	$form['#after_build'] = array('observation_form_add_js_tools');
	
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
					'#id' => 'detmethod',
					'#title' => t('Determination method'),
					'#type' => 'select',
					'#options' => 	$options,
					'#description' => t('Determination Method'),
					'#default_value' => $observation['observation']['organism_artgroup_detmethod_id'],
					'#tree' => true,
			);
		}
		
		$observation_attributes = observation_get_attributes($observation['observation']['id'], $artgroup_id);
		foreach ($observation_attributes as $key => $attribute) {
			if($attribute['type_drupal'] == 'select') {
				$form['group']['details']['attributes'][$attribute['id']] = array(
						'#id' => $attribute['id'],
						'#title' => t($attribute['name']),
						'#type' => $attribute['type_drupal'],
						'#weight' =>$attribute['id']+12,
						'#tree' => true,
						'#default_value' => (int)$attribute['value'],
						'#options' => $attribute['options'],
				);
			}
			else {
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
	}

	$form['group']['artgroup_id'] = array(
			'#id' => 'artgroup_id',
			'#title' => t('Filter on species group'),
			'#type' => 'select',
			'#options' => organism_artgroup_get_all_as_array(),
			'#default_value' => $artgroup_id,
			'#ajax' => array(
					'event' => 'change',
					'callback' => 'observation_organism_callback',
					'wrapper' => 'organism_replace'
			),
			'#weight' => -1,
			'#tree' => true,
			'#value' => $artgroup_id
	);
	
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
		if(!isset($inventory_id)) {
			$form['div']['map'] = array(
					'#theme' => 'area',
					'#mapid' => 'observationmap',
					'#coordinate_storage_id' => 'coordinate',
					'#ch1903' => true,
					'#search' => true,
					'#height' => '250px',
					'#action' => 'getcoordinate',
			);
		}
		else {
			//display area of inventory on map
			$inventory = inventory_get($inventory_id);
			$form['div']['map'] = array(
					'#theme' => 'area',
					'#mapid' => 'observationmap',
					'#coordinate_storage_id' => 'coordinate',
					'#ch1903' => true,
					'#search' => true,
					'#height' => '250px',
					'#action' => 'getcoordinate',
					'#geometry_edit_id' => $inventory['area_id'],
					'#infowindow_content_fetch_url_area' => base_path()
					. 'area/{ID}/areaoverview/ajaxform',
					'#geometries_fetch_url' => base_path() . 'area/' . $inventory['area_id'] . '/json',
			);
		}
	} else {
		$form['div']['map'] = array(
				'#theme' => 'area',
				'#mapid' => 'observationmap',
				'#coordinate_storage_id' => 'coordinate',
				'#ch1903' => true,
				'#search' => true,
				'#infowindow_content_fetch_url_observation' => base_path() . 'observation/{ID}/overview/ajaxform',
				'#infowindow_content_fetch_url_area' => base_path() . 'area/{ID}/areaoverview/ajaxform',
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
	
	/* Hidden fields */
	$form['observer'] = array(
			'#type' => 'hidden',
			'#id' => 'observer', 
			'#name' => 'observer', 
			'#value' => $observation['observation']['observer']
	);
	
	
	$form['observation_id'] = array(
			'#type' => 'hidden', 
			'#id' => 'observation_id', 
			'#name' => 'observation_id', 
			'#value' => $observation['observation']['id']
	);
	
	if(isset($inventory_id)) {
		$form['inventory_id'] = array(
			'#type' => 'hidden', 
			'#id' => 'inventory_id', 
			'#value' => $inventory_id
		);
	}
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
	
	if($observationdata['observation']['organism_id'] == UNKNOWN_SPECIES) {
		$species = t('Unknown species');
	} else {
		$species = $observationdata['organism']['name_lat'];
		if (isset($observationdata['organism']['name_lang']))
			$species .= ' / ' . $observationdata['organism']['name_lang'];
	}
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
			'#infowindow_content_fetch_url_observation' => base_path() . 'observation/{ID}/overview/ajaxform',
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
	if($form_state['triggering_element']['#id'] == 'artgroup_id') {
		$form['group']['organism']['#value'] = '';
	} 
	return $form['group'];
}

function observation_unknown_check_callback($form, &$form_state) {
	return $form['group'];
}

function observation_form_add_js_tools($form, &$form_state) {
	//add date and timepicker
	drupal_add_library('system', 'ui.datepicker');
	drupal_add_library('system', 'ui.slider');
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_js($observation_path . '/js/timepicker.js');

	
	drupal_add_css('/* css for timepicker */
					.ui-timepicker-div .ui-widget-header { margin-bottom: 8px; }
					.ui-timepicker-div dl { text-align: left; }
					.ui-timepicker-div dl dt { height: 25px; margin-bottom: -25px; }
					.ui-timepicker-div dl dd { margin: 0 10px 10px 65px; }
					.ui-timepicker-div td { font-size: 90%; }
					.ui-tpicker-grid-label { background: none; border: none; margin: 0; padding: 0; }
			
					.ui-timepicker-rtl{ direction: rtl; }
					.ui-timepicker-rtl dl { text-align: right; }
					.ui-timepicker-rtl dl dd { margin: 0 65px 10px 10px; })', 'inline');
	 
	drupal_add_js($observation_path . '/js/qtip_tooltip.js');
	/* Tooltip texts */
	$tooltip['text']['artgroup_id'] = t('Select a specimen group from the dropdown menu to constrain the search.');
	$tooltip['text']['organism_name'] = t('Search for the specimen by inputing at least 2 chars.');
	$tooltip['text']['date'] = t('Date of observation (dd.mm.yyyy)');
	$tooltip['text']['time'] = t('Time of observation (hh:mm)');
	$tooltip['text']['count'] = t('Number of observed specimens.');
	$tooltip['text']['observer'] = t('Name of observer (You)');
	$tooltip['text']['detmethod'] = t('Select a method for determination from the dropdown menu.');
	$tooltip['text']['Comment'] = t('Provide any additional information.');
	$tooltip['text']['Accuracy'] = t('Precision of place of observation.');
	$tooltip['text']['Verified'] = t('Verified by');
	$tooltip['text']['Determination'] = t('Sureness of classification');
	$tooltip['text']['Origin'] = t('Origin');
	$tooltip['text']['Phenology'] = t('Phenology');
	$tooltip['text']['Occurence'] = t('Occurence');
	$tooltip['text']['Evidence available'] = t('Evidence available');
	$tooltip['text']['Evaluationmethod'] = t('Evaluation Method');
	
	
	$tooltip_options = 'position: {
					      corner: {
					         target: "topRight",
					         tooltip: "bottomLeft"
					      }
				     },
	 				 style: {
	 				 		name: "light",
	 				 		tip: true,
							border: {
						         width: 1,
						         radius: 3,
						         color: "#C0C0C0"
							}
					}';
	

	
	$attributes_tooltips = '';
	$observation_id = isset($form_state['values']['observation_id']) ? $form_state['values']['observation_id'] : 0;
	$artgroup_id = isset($form_state['values']['group']['artgroup_id']) ? $form_state['values']['group']['artgroup_id'] : 0;
	if($artgroup_id) {
		$observation_attributes = observation_get_attributes($observation_id, $artgroup_id);
		foreach ($observation_attributes as $key => $attribute) {
			$attributes_tooltips.= 'jQuery("#'.$attribute['id'].'").live("mouseover", function(event) {
	 			$(this).qtip({
			 	 content: "'.$tooltip['text'][$attribute['name']].'",
	 				'.$tooltip_options.',
				 overwrite: false,
		 			show: {
						event: event.type,
						ready: true
		 			}
				 }, event);
			 });';
		}
	}
	
	 drupal_add_js('jQuery(document).ready(function(){
	 		jQuery( ".datepicker" ).live("focus", function() {
	 				$(this).datepicker({
			      dateFormat: "dd.mm.yy"
    			});
	 		});
	 		
			jQuery( ".timepicker" ).live("focus", function() {
	 			$(this).timepicker({
			 	timeFormat: "HH:mm",
	 			showButtonPanel: false
			 });
	 		});
	 		'.
	 		//tooltips on artgroup_id and organism_name leads to problem with ajax replace..
// 	 		jQuery( "#artgroup_id" ).live("mouseover", function(event) {
// 	 			$(this).qtip({
// 			 	 content: "'.$tooltip['text']['artgroup_id'].'",
// 			 '.$tooltip_options.',
// 				overwrite: false,
// 		 			show: {
// 						event: event.type,
// 						ready: true
// 		 			}
// 				 }, event);
// 	 		});
		
// 	 		jQuery( "#organism_name" ).live("mouseover", function(event) {
// 	 			$(this).qtip({
// 			 	 content: "'.$tooltip['text']['organism_name'].'",
// 	 			'.$tooltip_options.',
// 	 			overwrite: false,
// 	 			show: {
// 					event: event.type,
// 					ready: true
// 	 			}
// 			 }, event);
// 	 		});
'	
	 		jQuery( "#date" ).live("mouseover", function(event) {
	 			$(this).qtip({
			 	 content: "'.$tooltip['text']['date'].'",
	 			'.$tooltip_options.',
				overwrite: false,
		 			show: {
						event: event.type,
						ready: true
		 			}
				 }, event);
	 		});
	 					
	 		jQuery( "#time" ).live("mouseover", function(event) {
	 			$(this).qtip({
			 	 content: "'.$tooltip['text']['time'].'",
			 '.$tooltip_options.',
				overwrite: false,
		 			show: {
						event: event.type,
						ready: true
		 			}
				 }, event);
	 		});
			 		
	 		jQuery( "#count" ).live("mouseover", function(event) {
	 			$(this).qtip({
			 	 content: "'.$tooltip['text']['count'].'",
			 '.$tooltip_options.',
				overwrite: false,
		 			show: {
						event: event.type,
						ready: true
		 			}
				 }, event);
	 		});
			 		
	 		jQuery( "#observer" ).live("mouseover", function(event) {
	 			$(this).qtip({
			 	 content: "'.$tooltip['text']['observer'].'",
			 '.$tooltip_options.',
				overwrite: false,
		 			show: {
						event: event.type,
						ready: true
		 			}
				 }, event);
	 		});		
			 		
	 		jQuery( "#detmethod" ).live("mouseover", function(event) {
	 			$(this).qtip({
			 	 content: "'.$tooltip['text']['detmethod'].'",
			 '.$tooltip_options.',
				overwrite: false,
		 			show: {
						event: event.type,
						ready: true
		 			}
				 }, event);
	 		});'	
			 				
	 		.$attributes_tooltips.
'});'
, 'inline'); 
	
	return $form;
}
