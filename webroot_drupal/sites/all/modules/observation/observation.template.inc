
<?php
// function observation_new_form($form, $form_state) {
function observation_new_form($action, $name = false) {
	global $user;

	if(!$name) $name = "observation_form";

	module_load_include('inc', 'organism', 'organism.artgroup');


	drupal_add_library('system', 'ui.autocomplete');
	drupal_add_library('system', 'ui.datepicker');
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
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'coordinate',
					'name' => 'coordinate'
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
			'#coordinate_storage_id' => 'coordinate',
			'#search' => true,
			'#ch1903' => true
	);





	$formcust .= '<div class="form-item form-type-textfield">'

					.'<table class="form-item form-type-textfield">'

						.'<tr>'
								.'<td class="observation_td"><span class="observation_title">'.t('Search for organsim or species').'</span><br>'
							.'<input id="organismn_autocomplete" class="ui-autocomplete-input organismn_autocomplete" type="text" maxlength="64" size="40">'
							.'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="species_autocomplete"></span>'
							.'<select name="artgroup_select" id="artgroup_select" onChange="$(\'#artgroup\').val(this.options[this.selectedIndex].value);">'.organism_artgroup_get_all_as_option().'</select>'

						.'</td></tr>'

						.'<tr><td>&nbsp;</td></tr>'

						.'<tr><td class="observation_td">'

							.'<table class="form-item form-type-textfield">'

							.'<form onsubmit="javscript:observation.save();return false;" method="post" name="'.$name.'" id="'.$name.'">'
								.'<input type="hidden" id="organismn_id" name="organismn_id" value="">'
								.'<input type="hidden" id="observation_found_as_lang" name="observation_found_as_lang" value="false">'
								.'<input type="hidden" id="observation_found_as_latin" name="observation_found_as_latin" value="false">'
								.'<input type="hidden" name="artgroup" id="artgroup" value="">'

									.'<tr>'
									.'<td class="observation_title">'.t('Date').'<br><input type="text" id="date" name="date" value="'.date('d.m.Y').'"></td>'
									.'<td class="observation_title">'.t('Count').'<br><input type="text" name="count" value="1"></td>'
									.'<td class="observation_title">'.t('Observer').'<br><input type="text" name="observer" value="'.$user->name.'"></td>'
									.'</tr><tr>'
									.'<td><div class="observation_td">'.t('Please enter date for the observation').'</div></td>'
									.'<td><div class="observation_td">'.t('Please enter number of found').'</div></td>'
									.'<td><div class="observation_td">'.t('Please enter the username of the observer').'</div></td>'
									.'</tr>'

									.'<tr><td>&nbsp;</td></tr>'

									.'<tr>'
									.'<td colspan="3">';




					$formcust .= drupal_render($form);





					$formcust .=	'</td>'
									.'</tr>'

									.'<tr><td>&nbsp;</td></tr>'


								.'</table>'

							.'</td></tr></table>'

							.'<fieldset class="collapsible form-wrapper collapse-processed">
							<legend>
							<span class="fieldset-legend">Add more Details</span>
							</legend>
							<div class="fieldset-wrapper">'

									.'<table border=1><tr>'
									.'<td>'.t('Determination method').':</td><td> <select name="determination_method_id">'
									.'<div id="observation_determation_options">'
									.organism_artgroup_get_determation_methods(false, true)
									.'</div></select></td>'
									.'</tr>'

							.'<tr><td>'.t('Attributes').'.</td></tr>.'
							.'<div id="observation_attributes">'
							.organism_artgroup_get_attributes(false,true,'<tr><td>','</td>', '<td>', '</td></tr>')
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