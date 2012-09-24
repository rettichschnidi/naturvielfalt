<?php
// function observation_new_form($form, $form_state) {
module_load_include('inc', 'observation');
module_load_include('inc', 'gallery', 'gallery.implementations');
/**
 * Show the form to add or edit an observation
 * @param array $observation
 */
function observation_form($observation = false) {
	global $user;
	$hidden_fields_arr = array();
	drupal_set_title(t('Capture a new observation'));
	$url = base_path() . 'observation/save';
	if ($observation) {
		$url = base_path() . 'observation/' . $observation['observation']['id']
				. '/save';
		$hidden_fields_arr['area_geometry_id'] = $observation['observation']['area_geometry_id'];
		$species = $observation['organism']['name_lang'];
		if (!isset($species)) {
			$species = $observation['organism']['name_lat'];
		}
		drupal_set_title(t('Edit observation of <i>@species</i>', array('@species' => check_plain($species))), PASS_THROUGH);
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

	drupal_add_library('system', 'ui.autocomplete');
	drupal_add_library('system', 'ui.datepicker');
	drupal_add_js('misc/form.js');
	drupal_add_js('misc/collapse.js');
	drupal_add_js('misc/jquery.form.js');
	drupal_add_js('misc/ui/jquery.ui.dialog.min.js');
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_js($observation_path . '/js/observation_autocomplete.js');
	drupal_add_js($observation_path . '/js/observation.js');
	drupal_add_css($observation_path . '/css/observation.css');
	drupal_add_css('misc/ui/jquery.ui.dialog.css');
	$formcust = "";

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

	$form['div'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
	);

	$form['div']['hiddenfields'] = area_address_hidden_form();

	if(empty($observation['observation']['id'])) {
		$form['div']['map'] = array(
				'#theme' => 'area',
				'#mapid' => 'observationmap',
				'#coordinate_storage_id' => 'coordinate',
				'#ch1903' => true,
				'#search' => true,
				'#height' => '350px',
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
		$image['picture'] = array(
				'#type' => 'fieldset',
				'#title' => t('Manage existing media for this observation'),
				'#collapsible' => true,
				'#collapsed' => false,
		);
		$image['picture']['media_table'] = gallery_implementation_edit_existing_file_list(
			'observation',
			$observation['observation']['id']);
		$image = drupal_render($image);
	} else {
		$image = '';
	}
	$file_extensions = implode(' ', gallery_get_valid_file_extensions());

	// format the upload field manually, because it's not possible with drupal forms...

	$image .= '<fieldset id="picture" class="form-wrapper">
	<legend>
		<span class="fieldset-legend">'
			. t('Add media for this observation')
			. '</span>
	</legend>
	<div class="fieldset-wrapper">
		<div class="form-item form-type-file form-item-files-">
			<div id="picture_upload__0" style="display: none; height: 0px;">
				<input id="file_input" onchange="javascript:checkMimeType_metaData(this);" name="files[]" size="48" class="form-file" type="file">
				<a onclick="observation.galleryMetaDataDialog($(this).parent());return false;" >  '
			. t('Click here to add details to this file')
			. '</a>
				<input type="text" style="display: none; height: 0px;" id="meta_title" name="meta_title[]" value="">
				<input type="text" style="display: none; height: 0px;" id="meta_description" name="meta_description[]" value="">
				<input type="text" style="display: none; height: 0px;" id="meta_location" name="meta_location[]" value="">
				<input type="text" style="display: none; height: 0px;" id="meta_author" name="meta_author[]" value="">
			</div>
			<label for="picture_upload__1">' . t('Upload files')
			. '</label>
			<div id="picture_upload__1">
				<input id="file_input" onchange="javascript:checkMimeType_metaData(this);" name="files[]" size="48" class="form-file" type="file">
				<a onclick="observation.galleryMetaDataDialog($(this).parent());return false;" >  '
			. t('Click here to add details to this file')
			. '</a>
				<input type="text" style="display: none; height: 0px;" id="meta_title" name="meta_title[]" value="">
				<input type="text" style="display: none; height: 0px;" id="meta_description" name="meta_description[]" value="">
				<input type="text" style="display: none; height: 0px;" id="meta_location" name="meta_location[]" value="">
				<input type="text" style="display: none; height: 0px;" id="meta_author" name="meta_author[]" value="">
			</div>

			<div class="description">
				<a href="javascript:addUploadSlot();">'
			. t('Click here to add one more file') . '</a>
				<p>' . t('Allowed files endings are: ') . $file_extensions
			. '</p>
			</div>
		</div>
	</div>
</fieldset>';

	$file_list['files'] = gallery_implementation_files_table(
		'observation',
		$observation['observation']['id'],
		true);

	$species_autocomplete = ($observation['organism']['name_lat'] == '')
			? t('Maximum 30 search results will be showed')
			: $observation['organism']['name_lat'];

	$formcust .= '<div id="message" style="display: none; height: auto;">
<div class="messages status"></div>
</div>';

	$formcust .= '
		<div class="form-item form-type-textfield">'
			.'<form url="' . $url . '" method="post" name="' . $form_id . '" id="' . $form_id . '" enctype="multipart/form-data">'
				.'<input type="hidden" id="organismn_id" name="organismn_id" value="' . $observation['observation']['organism_id'] . '">'
				.'<input type="hidden" id="observation_found_as_lang" name="observation_found_as_lang" value="' . $observation['observation']['found_as_lang'] . '">'
				.'<input type="hidden" id="observation_found_as_latin" name="observation_found_as_latin" value="' . $observation['observation']['found_as_lat'] . '">'
				.'<input type="hidden" name="artgroup" id="artgroup" value="' . $observation['observation']['organism_artgroup_id'] . '">'
				.@$hidden_fields
				.'<table id="artgroup_art_table" class="form-item form-type-textfield">'
					.'<tr>'
						.'<td class="observation_title">'
							.'<label for="artgroup_select">' . t('Filter on species group') . '</label>'
						.'</td><td>'
							.'<select name="artgroup_select" id="artgroup_select" onChange="$(\'#artgroup\').val(this.options[this.selectedIndex].value);observation.resetOrganismAutomcomplete();">'
								.organism_artgroup_get_all_as_option(
									false,
									0,
									$observation['observation']['organism_artgroup_id'])
							.'</select>'
						.'</td>'
					.'</tr><tr>'
						.'<td class="observation_title">'
							.'<label for="organism_autocomplete">' . t('Search for species') . '</label>'
						.'</td><td>'
							.'<input id="organismn_autocomplete" class="ui-autocomplete-input organismn_autocomplete" type="text" maxlength="64" size="40" value="' . $observation['organism']['name_lang'] . '" /><br />'
							.'<span id="species_autocomplete">' . $species_autocomplete . '</span>'
						.'</td>'
					.'</tr>'
				.'</table>'
				.'<table class="form-item form-type-textfield">'
					.'<tr>'
						.'<td class="observation_subtitle">'
							.'<label for="date">' . t('Date of the obeservation') . '</label>'
						.'</td><td>'
							.'<input type="text" id="date" name="date" value="'	. date('d.m.Y', $timestamp) . '" />'
						.'</td>'
					.'</tr><tr>'
						.'<td class="observation_subtitle">'
							.'<label for="observation_count">' . t('Count of found individuals')	. '</label>'
						.'</td><td>'
							.'<input type="text" id="observation_count" name="count" value="' . $observation['observation']['count'] . '" />'
						.'</td>'
					.'</tr><tr>'
						.'<td class="observation_subtitle">'
							.'<label for="observer">' . t('Observer') . '</label>'
						.'</td><td>'
							.'<input type="text" id="observer" name="observer" value="' . $observation['observation']['observer'] . '" />'
						.'</td>'
					.'</tr>'
				.'</table>';

	$formcust .= '<fieldset id="observation-edit-options" class="collapsible form-wrapper collapsed">
							<legend>
							<span class="fieldset-legend">'
			. t('Add more details')
			. '</span>
							</legend>
							<div class="fieldset-wrapper">'
			. '<div><table id="attributes_table"><tr>' . '<td>'
			. t('Determination method')
			. ':</td><td> <select name="determination_method_id" id="determination_method_id">'
			. organism_artgroup_get_determation_methods(
				false,
				true,
				$observation['observation']['organism_artgroup_detmethod_id'],
				$observation['detmethods_vis']) . '</select></td>' . '</tr>'
			.organism_artgroup_get_attributes($observation['observation']['organism_artgroup_id'], true, '<tr><td>#name#', '</td>','<td>','</td></tr>')
			.observation_get_attributes($observation['observation']['id'], true, '<tr><td>#name#', '</td>', '<td>', '</td></tr>')	
				
				
			. '</table></div></div></fieldset>';	
	
	$formcust .= drupal_render($form);
	$formcust .= '<input type="submit" class="edit-submit" value="' . t('Save')
			. '" name="op" id="edit-submit">'
			. '<div style="display: none;" id="message"><div class="messages"></div></div>'
			. $image . drupal_render($file_list) . '</form>' . '</div>';

	$recent['recent_observation'] = array(
			'#theme' => 'datatable',
			'#header' => observation_get_standard_datatable_headers(8, 'desc'),
			'#title' => t(
				'Recently recorded and saved observations (click on a row, to edit an entry)'),
			'#id_table' => 'recent_observations',
			'#options' => array(
					'jsonUrl' => base_path()
							. "observation/get/writeable",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
									window.location.href = Drupal.settings.basePath+'observation/'+id+'/edit';})}"
			)
	);

	if(observation_admin_access($observation)) {
		$aclfieldset =  array(
				'#type' => 'fieldset',
				'#title' => t('Permissions'),
				'#weight' => 99,
		);

		$aclfieldset['content'] = observation_acl_template($observation);
	}
	
	$formcust .= drupal_render($aclfieldset);
	$formcust .= drupal_render($recent);

	return $formcust;
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
	
	$species = $observationdata['organism']['name_lang'];
	if (!isset($species)) {
		$species = $observationdata['organism']['name_lat'];
	}
	drupal_set_title(t('Show observation of <i>@species</i>', array('@species' => check_plain($species))), PASS_THROUGH);

	/* call area_get_static_image_url() to get an url to the google maps visualization */
	$staticimageurl = area_get_static_image_url(
		array('coordinates' => json_encode(
					$observationdata['geometry']['coordinatejson']),
				'type' => $observationdata['geometry']['geometry_type']
		),
		450,
		320);

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

	/* Create a fieldset for the static google maps */
	$output['observation_static_map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 3,
			'#attributes' => array(
					'id' => 'observation-show-details-map' // required for CSS
			)
	);

	/* add the picture */
	$output['observation_static_map']['image'] = array(
			'#type' => 'markup',
			'#markup' => "<img src='$staticimageurl' alt='Static Map'/>"
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

