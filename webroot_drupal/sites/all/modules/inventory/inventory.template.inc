<?php
module_load_include('inc', 'inventory');
module_load_include('inc', 'inventory', 'inventory.validate');
module_load_include('inc', 'inventory', 'inventory.show');

/**
 * Returns the form for creation and editing of inventories
 *
 * @param array $form
 * @param array $form_state
 * @param array $area
 * @param array $inventory
 */
function inventory_form($form = null, &$form_state = null, $area = NULL, $inventory = false) {
	//load js for observation table/map
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_js($observation_path . '/js/observation.js');
	
	drupal_add_css(drupal_get_path('module', 'inventory') . '/css/inventory.css');
	
	global $user;
	$edit = false;
	
	if($area === NULL && isset($inventory['area_id'])) $area = area_get_full_dataset_of_specific_area($inventory['area_id']);
	
	drupal_set_title(t('Create a new inventory for area <i>@areaname</i>', array('@areaname' => check_plain($area['name']))), PASS_THROUGH);
	if ($inventory) {
		drupal_set_title(t('Edit inventory  <i>@inventoryname</i>', array('@inventoryname' => check_plain($inventory['name']))), PASS_THROUGH);
		$edit = true;
	}
	
	//set default values if inventory is not set
	if (!isset($inventory['id']))
		$inventory['id'] = '';
	
	if(!isset($inventory['name'])) 
		$inventory['name'] = '';
	
	if(!isset($inventory['description'])) 
		$inventory['description'] = '';
	
	if (!isset($inventory['observer'])) 
		$inventory['observer'] = $user->name;
	isset($inventory['create_time'])
		? $timestamp = strtotime($inventory['create_time'])
		: $timestamp = time();
		
	$form = array();
	
	if($edit) {
		//used for error messages
		$form['message'] = array(
				'#type'   => 'markup',
				'#markup' => '
			<div id="message" style="display: none; height: auto;">
				<div class="messages status"></div>
			</div>'
		);
	}
	
	$form['name'] = array(
			'#id' => 'name',
			'#title' => t('Name of inventory'),
			'#type' => 'textfield',
			'#required' => true,
			'#default_value' => $inventory['name'],
	);
	
	
	$form['description'] = array(
			'#id' => 'description',
			'#title' => t('Description of inventory'),
			'#type' => 'textarea',
			'#default_value' => $inventory['description'],
	);
	
	$form['date'] = array(
			'#id' => 'date',
			'#title' => t('Date of the creation'),
			'#type' => 'textfield',
			'#attributes' => array('class' => array('datepicker')),
			'#required' => true,
			'#tree' => true,
			'#default_value' => date('d.m.Y', $timestamp),
	);
	
	$form['time'] = array(
			'#id' => 'time',
			'#title' => t('Time of the creation'),
			'#attributes' => array('class' => array('timepicker')),
			'#type' => 'textfield',
			'#required' => true,
			'#tree' => true,
			'#default_value' => date('H:i', $timestamp),
	);
	
	$form['#after_build'] = array('inventory_form_add_js_tools');
	
	$form['observer'] = array(
			'#id' => 'observer',
			'#title' => t('Creator'),
			'#type' => 'textfield',
			'#disabled' => true,
			'#default_value' => $inventory['observer'],
			'#attributes' => array(
					'placeholder' => t('Some text'),
			),
	);
	
	$form['button'] = array(
			'#type' => 'submit',
			'#value' => t('Save'),
			'#submit' => array(
					'inventory_save'
			),
	);
	
	/* Create a fieldset for the tabular data */
	$form['area'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area details'),
			'#weight' => 1,
			'#attributes' => array(
					'id' => 'inventory-show-area-table' // required for CSS
			),
	);
	
	/* Build the content of the table, leave out empty fields */
	$form['area']['table'] = area_get_infotable_of_specific_area($area);
	
	$form['space'] = array(
			'#type' => 'markup',
			'#markup' => '<br><br>'
	);
	if(!$edit) {
		/* Create a fieldset for the static google maps */
		$form['area_map'] = array(
				'#type' => 'fieldset',
				'#title' => t('Map'),
				'#weight' => 3,
				'#attributes' => array(
						'id' => 'area-show-map', // required for CSS
						'name' => 'map' //used for anchor
				)
		);
		
		$form['area_map']['map'] = array(
				'#theme' => 'area',
				'#mapid' => 'observationmap',
				'#ch1903' => true,
				'#showall' => false,
				'#scalecontrol' => true,
				'#action' => 'show',
				'#reticle' => false,
				'#geometry_edit_id' => (int) $area['id'],
				'#defaultzoom' => '15',
				'#infowindow_content_fetch_url_area' => base_path()
				. 'area/{ID}/areaoverview/ajaxform',
				'#infowindow_content_fetch_url_observation' => base_path()
				. 'observation/{ID}/overview/ajaxform',
				'#geometries_fetch_url' => base_path() . 'area/' . $area['id'] . '/json',
		);
	}
	else {
		global $base_url;
		$iconBaseUrl = '/' . path_to_theme() . '/images/icons/enabled/';
		
		$form['add_observation'] = array(
				'#type' => 'markup',
				'#markup' => '<a href="'.$base_url.'/observation/new/'.$inventory['id'].'"> <img src="' . $iconBaseUrl . 'Add.png" alt="' . t('Add Observation') . '" title="' . t('Add Observation') . '" />'.t('Add Observation').'</a>',
		);
		
		$form['observations'] = inventory_show_observations($area['id'], $inventory['id'], false);
		
		/* Create a fieldset for the static google maps */
		$form['area_map'] = array(
				'#type' => 'fieldset',
				'#title' => t('Map'),
				'#weight' => 3,
				'#attributes' => array(
						'id' => 'inventory-show-area-map', // required for CSS
						'name' => 'map' //used for anchor
				)
		);
		
		$form['area_map']['map'] = array(
				'#theme' => 'area',
				'#mapid' => 'observationmap',
				'#ch1903' => true,
				'#showall' => false,
				'#scalecontrol' => true,
				'#action' => 'show',
				'#reticle' => false,
				'#geometry_edit_id' => (int) $area['id'],
				'#defaultzoom' => '15',
				'#infowindow_content_fetch_url_area' => base_path()
				. 'area/{ID}/areaoverview/ajaxform',
				'#infowindow_content_fetch_url_observation' => base_path()
				. 'observation/{ID}/overview/ajaxform',
				'#geometries_fetch_url' => base_path() . 'area/' . $area['id'] . '/json',
		);
	}
	
	//hidden fields
	$form['area_id'] = array(
			'#id' => 'area_id',
			'#type' => 'hidden',
			'#value' => isset($area['id']) ? $area['id'] : $inventory['area_id'],
			'#attributes' => array(
						'id' => 'area_id'
				)
	);
	
	$form['id'] = array(
			'#type' => 'hidden', 
			'#id' => 'id', 
			'#value' => $inventory['id'],
			'#attributes' => array(
					'id' => 'id'
			)
	);
	
	$form['observer'] = array(
			'#type' => 'hidden', 
			'#id' => 'observer', 
			'#name' => 'observer', 
			'#value' => $inventory['observer']
	);
	
	return $form;
}

/**
 * Adds javascript components to the form (date/timepicker, tooltips)
 *
 * @param array $form
 * @param array $form_state
 */
function inventory_form_add_js_tools($form, &$form_state) {
	//add date and timepicker
	drupal_add_library('system', 'ui.datepicker');
	drupal_add_library('system', 'ui.slider');
	$inventory_path = drupal_get_path('module', 'inventory');
	drupal_add_js($inventory_path . '/js/timepicker.js');

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

	drupal_add_js($inventory_path . '/js/qtip_tooltip.js');
	/* Tooltip texts */
	$tooltip['text']['name'] = t('Name of inventory');
	$tooltip['text']['description'] = t('Description of inventory');
	$tooltip['text']['date'] = t('Date of creation (dd.mm.yyyy)');
	$tooltip['text']['time'] = t('Time of creation (hh:mm)');


	$tooltip_options = 'position: {
					      corner: {
					         target: "center",
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
	 		
	 		jQuery( "#name" ).live("mouseover", function(event) {
	 			$(this).qtip({
			 	 content: "'.$tooltip['text']['name'].'",
			 '.$tooltip_options.',
				overwrite: false,
		 			show: {
						event: event.type,
						ready: true
		 			}
				 }, event);
	 		});
			 jQuery( "#description" ).live("mouseover", function(event) {
	 			$(this).qtip({
			 	 content: "'.$tooltip['text']['description'].'",
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

		});', 'inline');

	return $form;
}
