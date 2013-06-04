<?php
module_load_include('inc', 'observation');
module_load_include('inc', 'observation', 'observation.ajax');
module_load_include('inc', 'organism');
module_load_include('inc', 'gallery', 'gallery.implementations');
/**
 * Show the form to add or edit an observation
 * @param array $observation
 */
function observation_form($form = null, &$form_state = null, $observation = false) {
	global $user;
	
	$edit = false;
	
	$hidden_fields_arr = array();
	drupal_set_title(t('Capture a new observation'));
	$url = base_path() . 'observation/save';
	if ($observation) {
		$url = base_path() . 'observation/' . $observation['observation']['id']. '/save';
		$hidden_fields_arr['area_geometry_id'] = $observation['observation']['area_geometry_id'];
		$species = $observation['organism']['name_lat'];
		if (isset($observation['organism']['name_lang']))
			$species .= ' / ' . $observation['organism']['name_lang'];
		drupal_set_title(t('Edit observation of <i>@species</i>', array('@species' => check_plain($species))), PASS_THROUGH);
		$edit = true;
	}

	// if not edit, set standard values
	if (!isset($observation['observation']['id']))
		$observation['observation']['id'] = '';
	$hidden_fields_arr['observation_id'] = $observation['observation']['id'];
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
	if (!isset($observation['detmethods_vis']))
		$observation['detmethods_vis'] = array();
	if (!isset($observation['attributes']))
		$observation['attributes'] = array();

	$form_id = "observation_form";
	$hidden_fields = '';
	foreach ($hidden_fields_arr as $key => $value)
		$hidden_fields .= '<input type="hidden" id="' . $key . '" name="' . $key . '" value="' . $value . '">';

	module_load_include('inc', 'organism', 'organism.artgroup');

// 	drupal_add_library('system', 'ui.autocomplete');
// 	drupal_add_library('system', 'ui.datepicker');
// 	drupal_add_js('misc/form.js');
// 	drupal_add_js('misc/collapse.js');
// 	drupal_add_js('misc/jquery.form.js');
// 	drupal_add_js('misc/ui/jquery.ui.dialog.min.js');
// 	drupal_add_js('misc/ui/jquery.ui.progressbar.min.js');
	$observation_path = drupal_get_path('module', 'observation');
// 	drupal_add_js($observation_path . '/js/observation_autocomplete.js');
	drupal_add_js($observation_path . '/js/observation.js');
// 	drupal_add_css($observation_path . '/css/observation.css');
// 	drupal_add_css('misc/ui/jquery.ui.dialog.css');
// //	drupal_add_js('misc/ui/jquery.ui.progressbar.min.js');
// 	drupal_add_js($observation_path . '/js/tooltip.js');
// 	$formcust = "";
	
	//drupal_add_js(drupal_get_path('module', 'mymodule') .'/mymodule.js');
	
	// Sections
// 	$form['species'] = array(
// 			'#type' => 'fieldset',
// 			'#title' => t('Select species'),
// 			'#weight' => 10,
// 	);

	
	$form = array();
	$json = ($form_state['values']['artgroup_id'] == NULL) ? 'organism/search/json/'.'1' : 'organism/search/json/'.$form_state['values']['artgroup_id'];
	$form['organism'] = array(
			'#id' => 'organism_name',
			'#title' => t('Search for species'),
			'#type' => 'textfield',
			'#autocomplete_path' => $json,
			'#description' => t('Maximum 30 search results will be showed.'),
			'#attributes' => array(
					'placeholder' => t('Start typing to see the species...'),
			),
			'#disabled' => $form_state['values']['artgroup_id'] == NULL ? TRUE : FALSE,
			'#prefix' => '<div id="organism_replace">',
			'#suffix' => '</div>',
			'#ajax' => array(
					'event' => 'change',
					'callback' => 'observation_details_callback',
					'wrapper' => 'replace_details'
			),
			'#default_value' => $observation['organism']['name_lang'],
	);
	
	$observation_id = null;
	
	$artgroup_id = !empty($form_state['values']['artgroup_id']) ? $form_state['values']['artgroup_id'] : 1;
	
	if(!$observation)
		$observation_id = $observation['observation']['id'];
	
	$form['details'] = array(
			'#type' => 'fieldset',
			'#title' => t('Details'),
			'#prefix' => '<div id="replace_details">',
			'#suffix' => '</div>',
	);
	debug($artgroup_id);
	if($artgroup_id > 1) {
		$options = organism_artgroup_get_determation_methods($artgroup_id);
		if(!empty($options)) { // don't show determination method if no options
			$form['details']['determination'] = array(
					'#title' => t('Determination method'),
					'#type' => 'select',
					'#options' => 	organism_artgroup_get_determation_methods($artgroup_id),
					'#weight' => 11,
					'#description' => 'Determination Method'
			);
		}
		
		$observation_attributes = observation_get_attributes($observation_id, $artgroup_id);
		foreach ($observation_attributes as $key => $attribute) {
			$form['details'][$key] = array(
					'#id' => $attribute['id'],
					'#title' => $attribute['name'],
					'#type' => $attribute['type_drupal'],
					'#weight' =>$attribute['id']+12,
			);
		}
		debug($form['details']);
	}

  
	// show artgroup select if new observation, otherwise show the selected artgroup (not changeable)
	if($edit) {
		// get the organism selection box
		
		$artgroup_name = organism_artgroup_get_artgroup($observation['observation']['organism_artgroup_id'],true);
		
		$form['artgroup_text'] = array(
				'#id' => 'artgroup_text',
				'#title' => t('Artgroup'),
				'#type' => 'textfield',
				'#default_value' => $artgroup_name,
				'#weight' => -1,
				'#disabled' => false,
		);
		
		$form['artgroup_id'] = array(
				'#id' => 'area_artgroup',
				'#type' => 'hidden',
				'#default_value' => $observation['observation']['organism_artgroup_id'],
		);
	}
	else {
		// get the organism selection box
		
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
	
	$form['date'] = array(
			'#id' => 'date',
			'#title' => t('Date of the obeservation'),
			'#type' => 'textfield',
			'#attributes' => array('class' => array('datepicker')),
			'#required' => true,
			'#default_value' => date('m/d/Y', $timestamp),
	);
	
	$form['count'] = array(
			'#id' => 'count',
			'#title' => t('Count of found individuals'),
			'#type' => 'textfield',
			'#required' => true,
			'#default_value' => $observation['observation']['count'],
			'#attributes' => array(
					'placeholder' => t('Some text'),
			),
	);
	$form['observer'] = array(
			'#id' => 'observer',
			'#title' => t('Observer'),
			'#type' => 'textfield',
			'#disabled' => true,
			'#default_value' => $observation['observation']['observer'],
			'#attributes' => array(
					'placeholder' => t('Some text'),
			),
	);
	
	$form['#after_build'] = array('observation_form_uidatepicker');
	
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
	}
	$file_extensions = implode(' ', gallery_get_valid_file_extensions());
	
	// format the upload field manually, because it's not possible with drupal forms...
	$image = '<fieldset id="picture" class="form-wrapper">
	<legend>
		<span class="fieldset-legend">'
			. t('Add media for this observation')
			. '</span>
	</legend>
	<div class="fieldset-wrapper">
		<div class="form-item form-type-file form-item-files-">
			<div>
				<div id="picture_upload__0" style="display: none; height: 0px;">
					<input id="file_input" onchange="javascript:checkMimeType_metaData(this);" name="files[]" size="48" class="form-file" type="file">
					<a onclick="observation.galleryMetaDataDialog($(this).parent());return false;" >  '	. t('Click here to add details to this file') . '</a>
					<input type="text" style="display: none; height: 0px;" id="meta_title" name="meta_title[]" value="">
					<input type="text" style="display: none; height: 0px;" id="meta_description" name="meta_description[]" value="">
					<input type="text" style="display: none; height: 0px;" id="meta_location" name="meta_location[]" value="">
					<input type="text" style="display: none; height: 0px;" id="meta_author" name="meta_author[]" value="">
				</div>
				<label for="picture_upload__1">' . t('Upload files')
				. '</label>
				<div id="picture_upload__1">
					<input id="file_input" onchange="javascript:checkMimeType_metaData(this);" name="files[]" size="48" class="form-file" type="file">
					<a onclick="observation.galleryMetaDataDialog($(this).parent());return false;" >  ' . t('Click here to add details to this file') . '</a>
					<input type="text" style="display: none; height: 0px;" id="meta_title" name="meta_title[]" value="">
					<input type="text" style="display: none; height: 0px;" id="meta_description" name="meta_description[]" value="">
					<input type="text" style="display: none; height: 0px;" id="meta_location" name="meta_location[]" value="">
					<input type="text" style="display: none; height: 0px;" id="meta_author" name="meta_author[]" value="">
				</div>
			</div>

			<div class="description">
				<a href="javascript:addUploadSlot();">' . t('Click here to add one more file') . '</a>
				<p>' . t('Allowed files endings are: ') . $file_extensions . '</p>
			</div>
			
		</div>
	</div>
</fieldset><input type="submit" value="' . t('Save') . '" id="picture_upload_submit" /><br />';
				
	$form['media'] = array(
		'#type'=> 'item',	
		'#title'=> t('Media'),	
		'#markup'=> $image,
	);
	
	
	$file_list['files'] = gallery_implementation_files_table(
		'observation',
		$observation['observation']['id'],
		true);

// 	$species_autocomplete = ($observation['organism']['name_lat'] == '')
// 			? t('Maximum 30 search results will be showed')
// 			: $observation['organism']['name_lat'];
	
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
		$aclfieldset =  array(
				'#type' => 'fieldset',
				'#title' => t('Permissions'),
				'#weight' => 99,
		);

		$aclfieldset['content'] = observation_acl_template($observation);
	}
	
	return $form;
}


function ajax_example_simplest_callback($form, $form_state) {
	// The form has already been submitted and updated. We can return the replaced
	// item as it is.
	return $form['replace_textfield'];
}


/**
 * ACL permission template
 * @param array $observation
 */
function observation_acl_template($observation = NULL) {
	$output = array();
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

	$output['acl'] = array(
			'#theme' => 'c_acl',
			'#defaults' => $defaults,
			'#acl_id' => $observation['observation']['acl_id'],
			'#add_element' => true,
	);
	return $output;
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
	
	return $form['organism'];
}

function observation_details_callback($form, &$form_state) {
	return $form['details'];
}

function observation_form_uidatepicker($form, $form_state) {
	drupal_add_library('system', 'ui.datepicker');
	drupal_add_js("(function ($) { $('.datepicker').datepicker(); })(jQuery);", array('type' => 'inline', 'scope' => 'footer', 'weight' => 5));
	return $form;
}
