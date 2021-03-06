<?php
/**
 * @file area.show.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');
module_load_include('inc', 'inventory', 'inventory.show');

/**
 * Show all existing areas. Ify $onlyMyAreas is true,
 * restrict to areas where the current user is admin.
 *
 * @return render array
 */
function area_show_all_areas() {
	global $user;
	
	drupal_add_library('system', 'ui.dialog');
	$observation_path = drupal_get_path('module', 'area');
	drupal_add_css($observation_path . '/css/area.css');
	
	$output['message'] = array(
			'#type'   => 'markup',
			'#markup' => '
			<div id="message" style="display: none; height: auto;">
				<div class="messages status"></div>
			</div>'
	);

	$output['map'] = array(
			'#mapid' => 'areamap',
			'#search' => true,
			'#ch1903' => true,
			'#geometries_fetch_url' => base_path() . 'areas/json',
			'#geometries_autoload'  => false,
			'#infowindow_content_fetch_url_area' => base_path()
					. 'area/{ID}/areaoverview/ajaxform',
			'#theme' => 'area'
	);

	$output['space'] = array(
			'#type' => 'markup',
			'#markup' => '<p/>'
	);
	
	if(user_access(AREA_CREATE)) {
		global $base_url;
		$iconBaseUrl = '/' . path_to_theme() . '/images/icons/enabled/';
		
		$output['create'] = array(
				'#type' => 'markup',
				'#markup' => '<a href="'.$base_url.'/area/new"> <img src="' . $iconBaseUrl . 'Add.png" alt="' . t('Create a new area') . '" title="' . t('Create a new area') . '" />'.t('Create a new area').'</a>',
		);
	}
	
	$output['area_table'] = array(
		'#theme'    => 'datatable',
		'#header'   =>  area_get_standard_datatable_headers(12,
		'DESC'),
		'#title'    => t('Areas'),
		'#id_table' => DATATABLE_AREA,
		'#options'  => array(
					'jsonUrl' => base_path() . "areas/get/all",
					'rowClick'              => 'rowClick',
					'rowClickHandler' 		=> "function rowClick(celDiv, id) {
						jQuery(celDiv).click(
							function() {
								areamap.selectGeometry(id);
							}
						)
					}",
			'rpShowAll'             => true,
			'preProcessHandler'     => 'area.tablePreProcess',
 			'onSuccessHandler'      => ( ($user->uid == 0) ? '' : 'area.onTableSuccess' ),	/* only show the delete and export buttons if user is logged in */
			'gallery_enabled'       => true,
			'gallery_image_sources' => false,
			// Filters to add to the filterDiv above the table. The gallery image source
			// filter has a weight of 10, the table/gallery buttons 4 and 5 respectively.
			'custom_filter' => array(
				'acl_filter' => array(
					'#type' => 'select',
					'#name' => 'acl_filter',
					'#title' => t('Areas') . ':',
					'#options' => array(
						'all'       => t('Public'),
						'own'       => t('Own'),
						'writeable' => t('Editables')),
					'#attributes' => array(
						'class' => array('acl_filter')),
					'#weight' => 8
				),
			),
		    'date' => array(
		    	'dbDateField' => "date",
		    	'dbDateFieldType' => DBDateFieldType::TIMESTAMP)					
		)
		
	);	
	
	return $output;
}

/**
 * Show a specific area.
 *
 * @param array $areadata
 * @param int $inventory_id
 * @return array Drupal form
 */
function area_show_specific_area($areadata = NULL, $inventory_id = NULL) {
	assert(!empty($areadata));
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css');
	drupal_add_js('misc/form.js');
	drupal_add_js('misc/collapse.js');
	drupal_set_title(t('area').' '.$areadata['name']);
	
	global $base_url;
	
	//load js for observation table/map
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_js($observation_path . '/js/observation.js');
	drupal_add_library('system', 'ui.dialog');

	$output['message'] = array(
			'#type'   => 'markup',
			'#markup' => '
			<div id="message" style="display: none; height: auto;">
				<div class="messages status"></div>
			</div>'
	);
	
	/* Create a fieldset for the tabular data */
	$output['area'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area details'),
			'#attributes' => array(
					'id' => 'area-show-details' // required for CSS
			),
			'#weight' => 1
	);
	
	/* Build the content of the table, leave out empty fields */
	$output['area']['table'] = area_get_infotable_of_specific_area($areadata);

	/* Create a fieldset for the static google maps */
	$output['area_map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#attributes' => array(
					'id' => 'area-show-map', // required for CSS
					'name' => 'map' //used for anchor
			)
	);
	
	$iconBaseUrl = '/' . path_to_theme() . '/images/icons/enabled/';
	$output['area_map']['zoom'] = array(
			'#type' => 'markup',
			'#markup' => '<img style="cursor:pointer;" src="' . $iconBaseUrl . 'Zoom in.png" alt="' . t('zoomin') . '" title="' . t('zoomin') . '" onclick="javascript:observationmap.zoomIn(event, ' .$areadata['id'] . ');" /> '.t('zoomin') 
	);
	
	//if user has RED_WRITE Permission and area is not a child, user can add a subarea
	if(area_check_access($areadata, 'ACL_RED_WRITE') && empty($areadata['parent_id'])) {
		$output['area_map']['create_sub_area'] = array(
				'#type' => 'markup',
				'#prefix' => '</br>',
				'#markup' => '<img style="cursor:pointer;" onclick="window.location.href=\''.$base_url . '/area/new/'.$areadata['id'].'\'" src="'.$iconBaseUrl.'Site map.png" style="cursor:pointer;" alt="' .t('Create a subarea') . '" title="' . t('Create a subarea') . '"/> '.  t('Create a subarea')
		);
	}
	
	$output['area_map']['map'] = array(
			'#theme' => 'area',
			'#mapid' => 'observationmap',
			'#ch1903' => true,
			'#showall' => false,
			'#scalecontrol' => true,
			'#action' => 'show',
			'#reticle' => false,
			'#geometry_edit_id' => (int) $areadata['id'],
			'#defaultzoom' => '15',
			'#infowindow_content_fetch_url_area' => base_path()
			. 'area/{ID}/areaoverview/ajaxform',
			'#infowindow_content_fetch_url_observation' => base_path()
			. 'observation/{ID}/overview/ajaxform',
			'#geometries_fetch_url' => base_path() . 'area/' . $areadata['id'] . '/json',
	);
	
	/* Create a fieldset for the comment text field */
	$output['area_description'] = array(
			'#type' => 'fieldset',
			'#title' => t('Description'),
			'#collapsible' => true,
			'#collapsed' => false,
			'#attributes' => array(
					'id' => 'area-show-comment',
					'class' => array(
							'collapsible'
					),
			),
			'#weight' => 2
	);
	
	/* add the comments for the area as an editable textarea */
	$output['area_description']['comment'] = array(
			'#type' => 'item',
			'#markup' => "<div class='area-description'>"
			. (empty($areadata['comment']) ? t("None available")
					: check_plain($areadata['comment'])) . "</div>"
	);
	
	/* Create a fieldset for the linked habitats */
	$output['area_inventories'] = array(
			'#type' => 'fieldset',
			'#title' => t('inventories').' & '.t('observations'),
			'#collapsible' => true,
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
					'id' => 'area-show-comment' // required for CSS
			),
			'#weight' => 3
	);
	if(area_check_access($areadata, 'ACL_RED_WRITE') && user_access(CREATE_INVENTORY)) {
		global $base_url;
		$iconBaseUrl = '/' . path_to_theme() . '/images/icons/enabled/';
		
		$output['area_inventories']['add_inventory'] = array(
				'#type' => 'markup',
				'#markup' => '<a href="'.$base_url.'/inventory/area/'.$areadata['id'].'/new"> <img src="' . $iconBaseUrl . 'Add.png" alt="' . t('Add Inventory') . '" title="' . t('Add Inventory') . '" />'.t('Add Inventory').'</a>',
		);
	}
	
	$output['area_inventories']['inventories'] = inventory_show_area_inventories($areadata);
	$output['area_inventories']['space'] = array(
			'#type' => 'markup',
			'#markup' => '<br><br>'
	);
	
	$output['area_inventories']['observations'] = inventory_show_observations($areadata['id'], $inventory_id);
	
	/* Create a fieldset for the strategies text fields */
	$output['area_concept'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area concept'),
			'#collapsible' => true,
			'#collapsed' => false,
			'#attributes' => array(
					'class' => array(
							'collapsible',
							'collapsed'
					),
			),
			'#weight' => 4
	);

	/* add the protectiont target textarea */
	$output['area_concept']['protection_target'] = array(
			'#type' => 'item',
			'#title' => t('Protection target'),
			'#markup' => "<pre>"
					. (empty($areadata['protection_target'])
							? t("None available")
							: check_plain($areadata['protection_target']))
					. "</pre>"
	);

	/* add the tending strategies target textarea */
	$output['area_concept']['tending_strategies'] = array(
			'#type' => 'item',
			'#title' => t('Tending strategies'),
			'#markup' => "<pre>"
					. (empty($areadata['tending_strategies'])
							? t("None available")
							: check_plain($areadata['tending_strategies']))
					. "</pre>"
	);

	/* add the tending strategies target textarea */
	$output['area_concept']['safety_precautions'] = array(
			'#type' => 'item',
			'#title' => t('Safety precautions'),
			'#markup' => "<pre>"
					. (empty($areadata['safety_precautions'])
							? t("None available")
							: check_plain($areadata['safety_precautions'])) . "</pre>"
	);

	/* Create a fieldset for the linked habitats */
	$output['area_habitats'] = array(
			'#type' => 'fieldset',
			'#title' => t('Linked habitats'),
			'#collapsible' => true,
			'#attributes' => array(
					'class' => array(
							'collapsible',
							'collapsed'
					),
			),
			'#weight' => 5
	);

	if (function_exists('gallery_list_renderer')) {
		$output['area_videos'] = gallery_list_renderer(
			'videos',
			'area',
			$areadata['id']);
		$output['area_images'] = gallery_list_renderer(
			'images',
			'area',
			$areadata['id']);
	}

	/* add the linked habitats */
	$results = db_query(
		'SELECT
			label,
			name
		FROM
			{area_habitat} ah
			JOIN {habitat} h ON ah.habitat_id = h.id
		WHERE
			ah.area_id = ?;',
		array($areadata['id']));

	$habitats = array();
	foreach ($results->fetchAll() as $habitat) {
		$habitats[] = (array) $habitat;
	}
	if(!empty($habitats)) {
		$output['area_habitats']['habitats'] = array(
				'#theme' => 'datatable',
				'#header' => array(
						array(
								'name' => t('Label'),
								'width' => 300
						),
						array(
								'name' => t('Name'),
								'width' => 300
						)
				),
				'#tableWidth' => 900,
				'#rows' => $habitats,
				'#id_table' => DATATABLE_HABITATS
		);
	} else {
		$output['area_habitats']['habitats'] = array(
			'#type' => 'item',
			'#markup' => "<pre>".t("None available")."</pre>",
			);
	}

	$output['area_files'] = area_files($areadata);
	$output['area_files']['#weight'] = 6;

	return $output;
}

/**
 * Return a form which shows some important facts about a given area.
 * Intended for usage in a infowindow on a map.
 *
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 * @param array $areadata Area data
 * @return array Drupal form
 */
function area_overview_form($form, &$form_state, $areadata) {
	$form['table'] = area_get_infotable_of_specific_area(
		$areadata,
		array('create_time' => false,
				'modify_time' => false,
				'altitude' => false,
				'coordinates' => false,
				'surface_area' => false
		));
	$form['table']['#attributes'] = array(
			'id' => 'infowindow_tabular_data'
	);
	/**
	 * Display the "edit" button only if user has write permission to this area
	 */
	if (check_permission($areadata['acl_id'], variable_get('ACL_WRITE'))) {
		$form['edit'] = array(
				'#type' => 'markup',
				'#markup' => l(t('Edit'), 'area/' . $areadata['id'] . '/edit')
		);
		$form['whitespace'] = array(
				'#type' => 'markup',
				'#markup' => '&nbsp;/&nbsp;'
		);
		
	}
	$form['show'] = array(
			'#type' => 'markup',
			'#markup' => l(t('Show'), 'area/' . $areadata['id'] . '/show')
	);
	
	$form['whitespace_two'] = array(
			'#type' => 'markup',
			'#markup' => '&nbsp;/&nbsp;'
	);
	
	if (area_check_access($areadata, 'ACL_RED_WRITE') && user_access(CREATE_INVENTORY)) {
		$form['addinventory'] = array(
				'#type' => 'markup',
				'#markup' => l(t('Add Inventory'), 'inventory/area/' . $areadata['id'] . '/new')
		);
	}
	
	
	return $form;
}

function area_habitat_table($habitatId) {
	module_load_include('inc', 'datatable','datatable.user');
	
	// Set the table header
	$tableHeader[] = array(
			'name' => t('ID'),
			'dbfield' => 'id',
			'align' => 'center',
			'hide' => true,
			'width' => 40
	);
	$tableHeader[] = array(
			'name' => t('Area'),
			'dbfield' => 'areaname',
			'width' => 300
	);
	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
			'width' => 200,
	);
	$tableHeader[] = array(
			'name' => t('ZIP'),
			'dbfield' => 'zip',
			'align' => 'center',
			'width' => 40
	);
	$tableHeader[] = array(
			'name' => t('Creator'),
			'dbfield' => 'username',
			'width' => 220
	);
	
		//apply user settings
	datatable_apply_user_settings(DATATABLE_HABITATS, $tableHeader);
	
			
	$render_array['habitats'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#id_table' => DATATABLE_HABITATS,
			'#tableWidth' => 900,
			'#options' => array(
					'jsonUrl' => base_path() . 'area/habitat/'
							. $habitatId . "/datatablejson",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
					window.location.href = Drupal.settings.basePath+'area/'+id + '/show';})}"
			)
	);
	return $render_array;

	$results = db_query(
		'SELECT
			DISTINCT ON (a.id)
			a.id,
			a.name AS areaname,
			a_s.township AS township,
			a_s.zip AS zip,
			u.name AS username
		FROM
			{area_habitat} AS a_h
			JOIN {area} AS a ON a_h.area_id = a.id
			JOIN {area_geometry} AS a_s ON a_s.id = a.area_geometry_id
			JOIN {acl} cacl ON cacl.id = a.acl_id
			JOIN {users} u ON u.uid = cacl.users_id
		WHERE
			a_h.habitat_id=:habitatId',
		array(':habitatId' => $habitatId));

	$habitats = array();
	foreach ($results->fetchAll() as $habitatObject) {
		$habitatArray = array();
		$habitatArray[] = $habitatObject->areaname;
		$habitatArray[] = $habitatObject->township;
		$habitatArray[] = $habitatObject->zip;
		$habitatArray[] = $habitatObject->username;
		$habitats[] = $habitatArray;
	}

	$output = array(
			'#theme' => 'datatable',
			'#header' => array(
					array(
							'name' => t('Area'),
							'width' => 300
					),
					array(
							'name' => t('Township'),
							'width' => 200
					),
					array(
							'name' => t('ZIP'),
							'width' => 80
					),
					array(
							'name' => t('Creator'),
							'width' => 220
					)
			),
			'#tableWidth' => 900,
			'#rows' => $habitats,
			'#id_table' => DATATABLE_HABITATS
	);
	return $output;
}

/**
 * Return an array that contains a table which shows all documents for this area.
 *
 * @param $organism_id
 * 	Integer containing the id of the desired organism.
 */
function area_files($areadata) {
	global $user;
	$render_array = array();
	$result = db_query(
		'SELECT
			file_managed_fid
		FROM
			{area_file_managed} ofm
		WHERE
			area_id = :aid',
		array(':aid' => $areadata['id']),
		array('fetch' => PDO::FETCH_ASSOC));
	return commonstuff_showfiles(
		'area',
		$areadata['id'],
		$result->fetchAllKeyed(0, 0));
}
?>
