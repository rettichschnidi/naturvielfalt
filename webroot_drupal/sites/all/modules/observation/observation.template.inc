
<?php
//function observation_new_form($form, $form_state) {
function observation_new_form($action, $name = false) {
	global $user;

	if(!$name) $name = "observation_form";

	module_load_include('inc', 'organism', 'organism.artgroup');
// 	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');

// 	$form['#id'] = 'observation_new_form';

// 	$form['organismn_autocomplete'] = array(
// 			'#type' => 'textfield',
// 			'#title' => t('Species'),
// 			'#size' => 30,
// 			'#maxlength' => 64,
// 			'#description' => t('Please enter the name of the species'),
// 			'#id' => 'organismn_autocomplete'
// 	);

// 	drupal_add_library('system', 'ui.autocomplete');
// 	$areaCSSPath = drupal_get_path('module', 'observation') . '/css/';
// 	$inventoryJsPath = drupal_get_path('module', 'observation') . '/js/';

// 	drupal_add_css($areaCSSPath . 'area-select.css');
// 	drupal_add_js('http://maps.google.com/maps/api/js?sensor=false&libraries=geometry,places',array('group' => JS_LIBRARY));
// 	drupal_add_js($inventoryJsPath . 'lib/jquery/ui.geo_autocomplete.js',array('group' => JS_LIBRARY));
// 	drupal_add_js($inventoryJsPath . 'geometry.js', array('group' => JS_LIBRARY));
// 	drupal_add_js($inventoryJsPath . 'lib/rwo_gmaps/v3_epoly_sphericalArea.js');
// 	drupal_add_js($inventoryJsPath . 'lib/geoxml3/geoxml3.js', array('group' => JS_LIBRARY));
// 	drupal_add_js($inventoryJsPath . 'gmap.js');
// 	drupal_add_js($inventoryJsPath . 'observation.js');
// 	if ($bubble)
// 		drupal_add_js($inventoryJsPath . 'lib/infobubble.js');
// 	drupal_add_js($areaCSSPath . 'overlay-style.js');
// 	drupal_add_js($inventoryJsPath . 'util.js');


	//// ORIG!!!

// 	drupal_add_library('system', 'ui.autocomplete');

// 	$areaJsPath = drupal_get_path('module', 'area') . '/js/';
// 	$areaCSSPath = drupal_get_path('module', 'area') . '/css/';
// 	$inventoryJsPath = drupal_get_path('module', 'inventory') . '/js/';

// 	drupal_add_css($areaCSSPath . 'area-select.css');
// 	drupal_add_js(
// 			'http://maps.google.com/maps/api/js?sensor=false&libraries=geometry,places',
// 			array('group' => JS_LIBRARY));
// 	drupal_add_js(
// 			$inventoryJsPath . 'lib/jquery/ui.geo_autocomplete.js',
// 			array('group' => JS_LIBRARY));
// 	drupal_add_js($areaJsPath . 'geometry.js', array('group' => JS_LIBRARY));
// 	drupal_add_js($areaJsPath . 'lib/rwo_gmaps/v3_epoly_sphericalArea.js');
// 	drupal_add_js(
// 			$areaJsPath . 'lib/geoxml3/geoxml3.js',
// 			array('group' => JS_LIBRARY));
// 	if ($bubble)
// 		drupal_add_js($areaJsPath . 'lib/infobubble.js');
// 	drupal_add_js($areaCSSPath . 'overlay-style.js');
// 	drupal_add_js($inventoryJsPath . 'util.js');
// 	drupal_add_js(drupal_get_path('module', 'observation') . '/js/'. 'area-select.js');
// 	drupal_add_library('system', 'ui.dialog');
// 	drupal_add_library('system', 'ui.datepicker');
// 	drupal_add_library('system', 'ui.autocomplete');
// 	drupal_add_library('system', 'ui.sortable');

// 	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/sgroup.js');
// 	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/lib/jquery/jquery-texotela-numeric.js');

// 	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/util.js');
// 	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.js');

// 	if ($usemain)
// 		drupal_add_css(drupal_get_path('module', 'inventory') . '/css/main.css',
// 				array(
// 						'group' => CSS_DEFAULT,
// 						'every_page' => TRUE
// 				));
// 	drupal_add_css(drupal_get_path('module', 'inventory') . '/css/sgroup.edit.css',
// 			array(
// 					'group' => CSS_DEFAULT,
// 					'every_page' => TRUE
// 			));
	drupal_add_library('system', 'ui.autocomplete');
	drupal_add_library('system', 'ui.datepicker');
	$observation_path= drupal_get_path('module', 'observation');
	drupal_add_js('https://maps.google.com/maps/api/js?sensor=false');
	drupal_add_js($observation_path . '/js/gmap.js');
	drupal_add_js($observation_path . '/js/observation_autocomplete.js');
	drupal_add_js($observation_path . '/js/observation.js');
	drupal_add_css($observation_path . '/css/observation.css');
// 	print_r(organism_artgroup_get_attributes());
	$imgpath= base_path().drupal_get_path('module', 'observation');
	$formcust = "
					<style type='text/css'>
						.organismn_autocomplete {
							background-image: url('".$imgpath."/img/search.gif');
							background-repeat: no-repeat;
							background-position: right;
							border-width: 1px;
							padding: 0.5em;
						}
						.searching {
							background-image: url('".$imgpath."/img/searching.gif');
						}
						.notfound {
							background-image: url('".$imgpath."/img/notfound.gif');
						}
					</style>";

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
									.'<td class="observation_title">'.t('Coordinates').'</td>'
									.'</tr><tr>'
									.'<td class="observation_td" align="right" colspan=2>'
										.'X: <input id="coordinate_x" type="text" name="coordinate_x"> Y: <input id="coordinate_y" type="text" name="coordinate_x"><input type="button" onclick="setMarker();" value="'.t('Set position').'"></td>'
									.'</tr><tr>'
									.'<td class="observation_td" align="right" colspan=2>'.t('Optionaly you can enter the coordinates manually').'</div></td>'
									.'</tr><tr>'
									.'<td colspan=1></td>'
									.'</tr><tr>'
									.'<td colspan=3><div id="map" style="width: 600px; height: 400px;"></div></td>'
									.'</tr>'

									.'<tr><td>&nbsp;</td></tr>'

									.'<script type="text/javascript">initialize_map();</script>'

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

// 	echo $formcust;

// 	$form['organismn_hidden'] = array(
// 			'#type' => 'container',
// 			'#attributes' => array(
// 					'style' => array(
// 							//'visibility: hidden;'
// 					))
// 	);

// 	$form['organismn_hidden']['observation_date'] = array(
// 			'#type' => 'textfield',
// 			'#title' => t('Date'),
// 			'#size' => 30,
// 			'#maxlength' => 64,
// 			'#description' => t('Please enter date for the observation'),
// 			'#id' => 'organismn_date'
// 	);

// 	$form['organismn_hidden']['observation_count'] = array(
// 			'#type' => 'textfield',
// 			'#title' => t('Count'),
// 			'#size' => 30,
// 			'#maxlength' => 64,
// 			'#description' => t('Please enter number of found'),
// 			'#id' => 'organismn_count'
// 	);






// 	$form['organismn_hidden']['details'] = array(
// 			'#type' => 'fieldset',
// 			'#title' => t('Add more Details'),
// 			'#collapsible' => TRUE,
// 			'#collapsed' => TRUE,
// 	);



// 	$form['organismn_hidden']['organismn_id'] = array(
// 			'#type' => 'hidden',
// 			'#value' => '',
// 			'#id' => 'organismn_id'
// 	);

// 	$form['organismn_hidden']['submit'] = array(
// 			'#type' => 'submit',
// 			'#value' => t('Save the Observation')
// 	);


	return $formcust;
	return $form;
}