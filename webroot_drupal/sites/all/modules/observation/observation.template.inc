
<?php
// function observation_new_form($form, $form_state) {
function observation_form($observation_id = false) {
	global $user;

	if($observation_id){
		$observation = observation_get($observation_id);
		print_r($observation);
	}

	if(!isset($observation['observation']['found_as_lang'])) $observation['observation']['found_as_lang'] = 'false';
	if(!isset($observation['observation']['found_as_lat'])) $observation['observation']['found_as_lat'] = 'false';
	if(!isset($observation['observation']['organism_artgroup_id'])) $observation['observation']['organism_artgroup_id'] = '';
	if(!isset($observation['observation']['organism_id'])) $observation['observation']['organism_id'] = '';
	if(!isset($observation['organism']['name_lat'])) $observation['organism']['name_lat'] = '';
	if(!isset($observation['organism']['name_lang'])) $observation['organism']['name_lang'] = '';
	isset($observation['observation']['date']) ? $timestamp = $observation['observation']['date'] : $timestamp = time();
	if(!isset($observation['observation']['count'])) $observation['observation']['count'] = '1';
	if(!isset($observation['observation']['observer'])) $observation['observation']['observer'] = $user->name;
	if(!isset($observation['observation']['organism_artgroup_detmethod_id'])) $observation['observation']['organism_artgroup_detmethod_id'] = false;

	$form_id = "observation_form";

	module_load_include('inc', 'organism', 'organism.artgroup');


	drupal_add_library('system', 'ui.autocomplete');
	drupal_add_library('system', 'ui.datepicker');
	drupal_add_js('misc/form.js');
	drupal_add_js('misc/collapse.js');
	$observation_path= drupal_get_path('module', 'observation');
// 	drupal_add_js('https://maps.google.com/maps/api/js?sensor=false');
// 	drupal_add_js($observation_path . '/js/gmap.js');
	drupal_add_js($observation_path . '/js/observation_autocomplete.js');
	drupal_add_js($observation_path . '/js/observation.js');
	drupal_add_css($observation_path . '/css/observation.css');
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
			'#title' => "Google Map",
			'#attributes' => array(
					'style' => 'width: 95%;'
			)
	);

	$form['div']['hiddenfields'] = area_address_hidden_form();

	$form['div']['map'] = array(
			'#theme' => 'area',
			'#action' => 'getcoordinate',
			'#coordinate_storage_id' => 'coordinate',
			'#search' => true,
			'#ch1903' => true
	);


$formcust .= '<div id="message" style="display: none; height: auto;">
<div class="messages status"></div>
</div>';


	$formcust .= '<div class="form-item form-type-textfield">'
					.'<form onsubmit="javscript:observation.save();return false;" method="post" name="'.$form_id.'" id="'.$form_id.'">'
					.'<input type="text" style="display: none; height: 0px;" id="organismn_id" name="organismn_id" value="'.$observation['observation']['organism_id'].'">'
					.'<input type="text" style="display: none; height: 0px;" id="observation_found_as_lang" name="observation_found_as_lang" value="'.$observation['observation']['found_as_lang'].'">'
					.'<input type="text" style="display: none; height: 0px;" id="observation_found_as_latin" name="observation_found_as_latin" value="'.$observation['observation']['found_as_lat'].'">'
					.'<input type="text" style="display: none; height: 0px;" name="artgroup" id="artgroup" value="'.$observation['observation']['organism_artgroup_id'].'">'

					.'<table class="form-item form-type-textfield">'
						.'<tr><td class="observation_td">'
								.'<span class="observation_title">'.t('Search for organsim or species').'</span><br>'
									.'<input id="organismn_autocomplete" class="ui-autocomplete-input organismn_autocomplete" type="text" maxlength="64" size="40" value="'.$observation['organism']['name_lang'].'">'
									.'<br><span id="species_autocomplete">'.$observation['organism']['name_lat'].'</span>'
							.'</td><td class="observation_td">'
									.'<span class="observation_title">'.t('Filter on species group').'</span><br>'
									.'<select name="artgroup_select" id="artgroup_select" onChange="$(\'#artgroup\').val(this.options[this.selectedIndex].value);">'.organism_artgroup_get_all_as_option(false, 0, $observation['observation']['organism_artgroup_id']).'</select>'

					.'</td></tr></table>'

					.'<table class="form-item form-type-textfield" style="width: auto !important;">'

						.'<tr>'
						.'<td class="observation_subtitle">'.t('Date').'<br><input type="text" id="date" name="date" value="'.date('d.m.Y',$timestamp).'"></td>'
						.'<td class="observation_subtitle">'.t('Count').'<br><input type="text" name="count" value="'.$observation['observation']['count'].'"></td>'
						.'<td class="observation_subtitle">'.t('Observer').'<br><input type="text" name="observer" value="'.$observation['observation']['observer'].'"></td>'
						.'</tr><tr>'
						.'<td class="observation_td">'.t('Please enter date for the observation').'</td>'
						.'<td class="observation_td">'.t('Please enter number of found').'</td>'
						.'<td class="observation_td">'.t('Please enter the username of the observer').'</td>'
						.'</tr></table>';

					$formcust .= drupal_render($form);
					$formcust .=

							'<fieldset id="observation-edit-options" class="collapsible form-wrapper collapsed">
							<legend>
							<span class="fieldset-legend">Add more Details</span>
							</legend>
							<div class="fieldset-wrapper">'

									.'<table><tr>'
									.'<td>'.t('Determination method').':</td><td> <select name="determination_method_id" id="determination_method_id">'
									.organism_artgroup_get_determation_methods(false, true, $observation['observation']['organism_artgroup_detmethod_id'])
									.'</select></td>'
									.'</tr>'

							.'<div id="observation_attributes">'
							.organism_artgroup_get_attributes(false,true,'<tr id="#id#" style="display: none;"><td>','</td>', '<td>', '</td></tr>')
							.'</div>'

							.'</table></div></fieldset>'
							.'<input type="button" class="form-submit" value="'.t('Save').'" name="op" id="edit-submit" onclick="javscript:observation.save();return false;">'
					.'</form>'
					.'<div style="display: none;" id="message"><div class="messages"></div></div>'

			.'</div>';

	/**
	 * Drupal form handling, should be that
	 */
// 	$form['organismn_main'] = array(
// 			'#type' => 'container',
// 			'#attributes' => array(
// 					'style' => array(
// 							//'visibility: hidden;'
// 					))
// 	);

// 	$form['organismn_main']['organismn_autocomplete'] = array(
// 			'#type' => 'textfield',
// 			'#title' => t('Search for organsim or species'),
// 			'#size' => 30,
// 			'#maxlength' => 64,
// 			'#description' => t('Please enter a name to search in our database'),
// 			'#id' => 'organismn_date'
// 	);

// 	$form['organismn_main']['observation_date'] = array(
// 			'#type' => 'textfield',
// 			'#title' => t('Date'),
// 			'#size' => 30,
// 			'#maxlength' => 64,
// 			'#description' => t('Please enter date for the observation'),
// 			'#id' => 'organismn_date'
// 	);

// 	$form['organismn_main']['observation_count'] = array(
// 			'#type' => 'textfield',
// 			'#title' => t('Count'),
// 			'#size' => 30,
// 			'#maxlength' => 64,
// 			'#description' => t('Please enter number of found'),
// 			'#id' => 'organismn_count'
// 	);

// 	$form['organismn_main']['details'] = array(
// 			'#type' => 'fieldset',
// 			'#title' => t('Add more Details'),
// 			'#collapsible' => TRUE,
// 			'#collapsed' => TRUE,
// 	);



// 	$form['organismn_main']['organismn_id'] = array(
// 			'#type' => 'hidden',
// 			'#value' => '',
// 			'#id' => 'organismn_id'
// 	);

// 	$form['organismn_main']['submit'] = array(
// 			'#type' => 'submit',
// 			'#value' => t('Save the Observation')
// 	);


	/**
	 * Area map include
	 */
	$form['test'] = array(
			'#type' => 'textfield',
			'#title' => 'Coordinates as JSON',
			'#attributes' => array(
					'id' => 'meineid'
			),
	);

	$form['div'] = array(
			'#type' => 'fieldset',
			'#title' => "Google Map",
			'#attributes' => array(
					'style' => 'width: 95%;'
			)
	);

	$form['div']['map'] = array(
			'#theme' => 'area',
			'#action' => 'getcoordinate',
			'#coordinate_storage_id' => 'meineid',
			'#search' => true,
			'#ch1903' => true
	);
	$formcust .= drupal_render($form);
// 	return $form;

	return $formcust;
}