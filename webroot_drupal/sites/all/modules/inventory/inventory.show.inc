<?php

module_load_include('inc', 'inventory');
module_load_include('inc', 'datatable');

/**
 * Datatable template for inventories of an area
 *
 * @param array $area
 * @param boolean $edit
 */
function inventory_show_area_inventories($area, $edit = false) {
	return array(
			'#theme'    => 'datatable',
			'#header'   => inventory_get_standard_datatable_headers(9, 'desc'),
			'#title'    => t('Inventories of @area', array('@area' => $area['name'])),
			'#id_table' => 'inventories',
			'#tableWidth' => 918,
			'#tableHeight' => 120,
			'#options'  => array(
					'jsonUrl'               => base_path() . 'inventory/area/'.$area['id'].'/json',
					'rowClick'              => 'rowClick',
					'rowClickHandler'       => '
				function rowClick(celDiv, id) {
					jQuery(celDiv).click(
						function() {
							window.location.href = Drupal.settings.basePath+"area/"+ '.$area['id'].' + '.($edit ? "/edit/" : "/show/").' + id + "";
						}
					)
				}',
					'rpShowAll'             => true,
					'date' => array(
							'dbDateField' => "i.create_time",
							'dbDateFieldType' => DBDateFieldType::TIMESTAMP
					)
			)
	);
}

/**
 * Datatable template for observations of an inventory
 *
 * @param int $area_id
 * @param int $inventory_id
 * @param boolean $showtoolbar 
 *  True if the gallery and filter toolbar should be displayed (datatable is not working if table is inside a form)
 */
function inventory_show_observations($area_id, $inventory_id = NULL, $showtoolbar = true) {

	global $user;
	$inventory = inventory_get($inventory_id);
	$area = area_get_full_dataset_of_specific_area($area_id);
	$output = array(
			'#theme'    => 'datatable',
			'#header'   => observation_get_standard_datatable_headers(9, 'asc'),
			'#title'    => isset($inventory['name']) ? 
							t('Observations of Inventory @inventory',  array('@inventory' => $inventory['name']))
							: t('Observations of @area', array('@area' => $area['name'])),
			'#id_table' => 'observations',
			'#tableWidth' => 918,
			'#tableHeight' => 160,
			'#options'  => array(
				'jsonUrl'               => base_path() . 'inventory/observations/'.$area_id.'/'.$inventory_id.'/json',
				'rowClick'              => 'rowClick',
				'rowClickHandler'       => '
					function rowClick(celDiv, id) {
						jQuery(celDiv).click(
							function() {							    
								observationmap.selectObservation(id);
							}
						)
					}',
				'rpShowAll'             => true,
				'onSuccessHandler'      => ( ($user->uid == 0 ) ? '' : 'observation.onTableSuccess' ),	/* only show the delete and export buttons if user is logged in */
				'date' => array(
						'dbDateField' => 'o.date',
						'dbDateFieldType' => DBDateFieldType::INTEGER
				)
			)
		);
	
	if(!$showtoolbar) {
		$output['#options']['preProcessHandler']  = 'observation.tablePreProcessWithoutGallery';
		$output['#options']['gallery_enabled']    = false;
	} else {
		$output['#options']['preProcessHandler']  = 'observation.tablePreProcess';
		$output['#options']['gallery_enabled']       = true;
		$output['#options']['gallery_image_sources'] = array(
				'selected'  => array(
						'value'  => DATATABLE_IMGSRC_BELEG,
						'option' => t('only proof images')),
				array(
						'value'  => DATATABLE_IMGSRC_PORTRAIT,
						'option' => t('only portrait images')),
				array(
						'value'  => DATATABLE_IMGSRC_BELEG_PORTRAIT,
						'option' => t('proof images, then portrait images')),
				array(
						'value'  => DATATABLE_IMGSRC_PORTRAIT_BELEG,
						'option' => t('portrait images, then proof images'))
		);
		// Filters to add to the filterDiv above the table. The gallery image source
		// filter has a weight of 10, the table/gallery buttons 4 and 5 respectively.
		$output['#options']['custom_filter']['acl_filter'] = array(
						'#type' => 'select',
						'#name' => 'acl_filter',
						'#title' => t('Observations') . ':',
						'#options' => array(
								'all'       => t('Public'),
								'own'       => t('Own'),
								'writeable' => t('Editables')),
						'#attributes' => array(
								'class' => array('acl_filter')),
						'#weight' => 8
				);
		
				// Artgroup filter is made as markup because of white spaces in the options.
				// Drupal_render converts &nbsp; into &amp;nbsp; and because indenting
				// options with &nbsp; is the most reliable way, it's done with #markup.
		$output['#options']['custom_filter']['artgroup'] = array(
						'#type'   => 'markup',
						'#markup' => '
							<div class="form-item form-type-select form-item-artgroup-filter">
								<label>' . t('Artgroup') . ':</label>
								<select name="artgroup_filter" class="artgroup_filter form-select">
									' . organism_artgroup_get_all_as_option() . '
								</select>
							</div>',
									'#weight' => 9
				);
	}

	return $output;
}

/**
 * Return the inventories standard table headers
 * @param int $init_sort
 * 				which header in the array should be the initsort array[$init_sort]
 * @param String $init_sort2
 * 				Sort directions ex. 'asc' or 'desc'
 * @return array
 */
function inventory_get_standard_datatable_headers($init_sort = false,
		$init_sort2 = false) {

	$tableHeader[] = array(
			'name' => t('Internal Id'),
			'idField' => true,
			'dbfield' => 'id',
			'hide' => true,
			'width' => 16
	);

	global $base_url;
	$iconBaseUrl = $base_url . '/' . path_to_theme() . '/images/icons/';

 	$tableHeader[] = array(
 			'name' => '<img src="' . $iconBaseUrl . 'disabled/Erase.png" />',
			'dbfield' => 'removeColumn',
			'width' => 16,
			'noSort' => true
 	);
	
	$tableHeader[] = array(
			'name' => '<img src="' . $iconBaseUrl . 'disabled/Add.png" />',
			'dbfield' => 'addColumn',
			'width' => 16,
			'noSort' => true
	);
	
	$tableHeader[] = array(
			'name' => '<img src="' . $iconBaseUrl . 'disabled/List.png" />',
			'dbfield' => 'showColumn',
			'width' => 16,
			'noSort' => true
	);

	$tableHeader[] = array(
			'name' => '<img src="' . $iconBaseUrl . 'disabled/Modify.png" />',
			'dbfield' => 'editColumn',
			'width' => 16,
			'noSort' => true
	);

	$tableHeader[] = array(
			'name' => t('Name'),
			'dbfield' => 'name',
			'dbExactField' => 'i.name',
			'width' => 250,
	);
	
	$tableHeader[] = array(
			'name' => t('Description'),
			'dbfield' => 'description',
			'dbExactField' => 'i.description',
			'width' => 352,
	);
	
	$tableHeader[] = array(
			'name' => t('Observer'),
			'dbfield' => 'observer',
			'dbExactField' => 'i.observer',
			'width' => 100,
	);
	
	$tableHeader[] = array(
			'name' => t('Observations'),
			'dbfield' => 'count',
			'width' => 100,
	);
	
	$tableHeader[] = array(
			'name' => t('date'),
			'dbfield' => 'date',
			'width' => 120,
	);
	

	

	if ($init_sort && $init_sort2) {
		$tableHeader[$init_sort--]['initSort'] = true;
		$tableHeader[$init_sort--]['sortOrder'] = $init_sort2;
	}
	
	//apply user settings
	datatable_apply_user_settings(DATATABLE_INVENTORY, $tableHeader);
	
	return $tableHeader;
}

/**
 * Displays an inventory
 * 
 * @param array $inventory
 */
function inventory_show($inventory) {
	//load js for observation table/map
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_js($observation_path . '/js/observation.js');
	
	drupal_add_css(drupal_get_path('module', 'inventory') . '/css/inventory.css');
	
	global $user;
	$area = area_get_full_dataset_of_specific_area($inventory['area_id']);
	drupal_set_title(t('Inventory  <i>@inventoryname</i>', array('@inventoryname' => check_plain($inventory['name']))), PASS_THROUGH);
	
	$output = array();
	
	//used for error messages
	$output['message'] = array(
			'#type'   => 'markup',
			'#markup' => '
		<div id="message" style="display: none; height: auto;">
			<div class="messages status"></div>
		</div>'
	);
	
	$output['inventory'] =  array(
			'#type' => 'fieldset',
			'#title' => t('Inventory details')
	);
			
	
	$output['inventory']['details'] = inventory_get_table($inventory);
	if(!empty($inventory['description'])) {
		$output['inventory']['description'] = array(
				'#type' => 'item',
				'#title' => t('Description'),
				'#markup' => '<div class="inventory-description">'.$inventory['description'].'</div>'
		);
	}
	
	if(inventory_check_access($inventory, 'ACL_RED_WRITE') && user_access(CREATE_OBSERVATION)) {
		global $base_url;
		$iconBaseUrl = '/' . path_to_theme() . '/images/icons/enabled/';
	
		$output['inventory']['add_observation'] = array(
				'#type' => 'markup',
				'#markup' => '<a href="'.$base_url.'/observation/new/'.$inventory['id'].'"> <img src="' . $iconBaseUrl . 'Add.png" alt="' . t('Add Observation') . '" title="' . t('Add Observation') . '" />'.t('Add Observation').'</a>',
		);
	}
	
	$output['inventory']['observations'] = inventory_show_observations($area['id'], $inventory['id'], true);
	
	/* Create a fieldset for the tabular data */
	$output['area'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area details'),
			'#weight' => 1,
			'#attributes' => array(
					'id' => 'inventory-show-area-table' // required for CSS
			),
	);
	
	/* Build the content of the table, leave out empty fields */
	$output['area']['table'] = area_get_infotable_of_specific_area($area);
	
	$output['space'] = array(
			'#type' => 'markup',
			'#markup' => '<br><br>'
	);

	/* Create a fieldset for the static google maps */
	$output['area_map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 3,
			'#attributes' => array(
					'id' => 'inventory-show-area-map', // required for CSS
					'name' => 'map' //used for anchor
			)
	);

	$output['area_map']['map'] = array(
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
	return $output;
}

/**
 * Menu Hook
 * Forwards to area of inventory
 * 
 * @param array $inventory
 */
function inventory_show_area($inventory) {
	drupal_goto('area/'.$inventory['area_id'].'/show');
}

/**
 * Return a formated table with the inventory details
 * @param array $inventory
 */
function inventory_get_table($inventory = NULL) {
	if ($inventory == NULL || !is_array($inventory)) {
		return false;
	}
	$rows = array();

	if (!empty($inventory['name'])) {
		$rows[] = array(
				array(
						'data' => t('Name'),
						'header' => true
				),
				$inventory['name'],
		);
	}

	$observations = inventory_get_observations($inventory['id']);
	$rows[] = array(
			array(
					'data' => t('Observations'),
					'header' => true
			),
			sizeof($observations),
	);

	if (!empty($inventory['observer'])) {
		$rows[] = array(
				array(
						'data' => t('Creator'),
						'header' => true
				),
				l(
						check_plain($inventory['observer']),
						base_path() . "user/" . $inventory['users_uid'] . '/contact',
						array(
								'external' => true
						))
		);
	}

	if (!empty($inventory['create_time'])) {
		$rows[] = array(
				array(
						'data' => t('Date'),
						'header' => true
				),
				date('d.m.Y H:i:s', strtotime($inventory['create_time'])),
		);
	}

	if (!empty($inventory['modify_time'])) {
		$rows[] = array(
				array(
						'data' => t('Last modification'),
						'header' => true
				),
				date('d.m.Y H:i:s', strtotime($inventory['modify_time']))
		);
	}


	/* add the table with the data prepared above */
	$form = array(
			'#theme' => 'table',
			'#rows' => $rows,
			'#id_table' => 'inventory_table_wd',
	);

	return $form;
}
