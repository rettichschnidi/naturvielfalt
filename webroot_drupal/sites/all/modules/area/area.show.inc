<?php
/**
 * @file area.show.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');

/**
 * Show all existing areas. Ify $onlyMyAreas is true,
 * restrict to areas where the current user is admin.
 *
 * @return render array
 */
function area_show_all_areas($onlyMyAreas = false) {
	if ($onlyMyAreas) {
		$output['map'] = array(
				'#mapid' => 'areamap',
				'#search' => true,
				'#ch1903' => true,
				'#geometries_fetch_url' => base_path() . 'areas/own/json',
				'#infowindow_content_fetch_url' => base_path() . 'area/{ID}/areaoverview/ajaxform',
				'#theme' => 'area'
		);
	} else {
		$output['map'] = array(
				'#mapid' => 'areamap',
				'#search' => true,
				'#ch1903' => true,
				'#geometries_fetch_url' => base_path() . 'areas/json',
				'#infowindow_content_fetch_url' => base_path() . 'area/{ID}/areaoverview/ajaxform',
				'#theme' => 'area'
		);
	}

	$tableHeader[] = array(
			'name' => t('Internal Id'),
			'idField' => true,
			'dbfield' => 'id',
			'hide' => true,
			'width' => 30
	);

	$tableHeader[] = array(
			'name' => t('Name'),
			'dbfield' => 'name',
			'dbExactField' => 'a.name',
			'width' => 310,
	);

	$tableHeader[] = array(
			'name' => t('Altitude [m]'),
			'dbfield' => 'altitude',
			'dbExactField' => 'a_s.altitude',
			'width' => 60
	);

	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
			'dbExactField' => 'a_s.township',
			'width' => 150
	);

	$tableHeader[] = array(
			'name' => t('Canton'),
			'dbfield' => 'canton',
			'dbExactField' => 'a_s.canton',
			'width' => 40
	);

	$tableHeader[] = array(
			'name' => t('Country'),
			'dbfield' => 'country',
			'dbExactField' => 'a_s.country',
			'width' => 180
	);

	$tableHeader[] = array(
			'name' => t('User'),
			'dbfield' => 'username',
			'dbExactField' => 'u.name',
			'width' => 120
	);

	$output['space'] = array(
			'#type' => 'markup',
			'#markup' => '<p/>'
	);

	$output['area_table'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#title' => t('Areas'),
			'#id_table' => 'areas',
			'#options' => array(
					'jsonUrl' => "'" . base_path()
							. ($onlyMyAreas ? "areas/own/datatablejson'"
									: "areas/all/datatablejson'"),
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id) {
						jQuery(celDiv).click(
							function() {
								areamap.selectGeometry(id);
							}
						)
					}"
			)
	);
	return $output;
}

/**
 * Show a specific area.
 *
 * @param integer $area_id
 * @return array Drupal form
 */
function area_show_specific_area($areadata = NULL) {
	assert(!empty($areadata));
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css');
	drupal_add_js('misc/form.js');
	drupal_add_js('misc/collapse.js');
	drupal_set_title($areadata['name']);

	/* call area_get_static_image_url() to get an url to the google maps visualization */
	$staticimageurl = area_get_static_image_url(
		array('coordinates' => json_encode($areadata['coordinatejson']),
				'type' => $areadata['geometry_type']
		),
		450,
		320);

	/* Create a fieldset for the tabular data */
	$output['area'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area details'),
			'#weight' => 1,
			'#attributes' => array(
					'id' => 'area-show-details' // required for CSS
			),
	);

	/* Create a fieldset for the static google maps */
	$output['area_static_map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 3,
			'#attributes' => array(
					'id' => 'area-show-map' // required for CSS
			)
	);

	/* add the picture */
	$output['area_static_map']['image'] = array(
			'#type' => 'markup',
			'#markup' => "<img src='$staticimageurl' alt='Static Map'/>"
	);

	/* Create a fieldset for the comment text field */
	$output['area_description'] = array(
			'#type' => 'fieldset',
			'#title' => t('Description'),
			'#weight' => 10,
			'#collapsible' => true,
			'#collapsed' => false,
			'#attributes' => array(
					'id' => 'area-show-comment',
					'class' => array(
							'collapsible',
							'collapsed'
					),
			)
	);

	/* add the comments for the area as an editable textarea */
	$output['area_description']['comment'] = array(
			'#type' => 'item',
			'#markup' => "<pre>"
					. (empty($areadata['comment']) ? t("None available")
							: $areadata['comment']) . "</pre>"
	);

	/* Create a fieldset for the strategies text fields */
	$output['area_concept'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area concept'),
			'#weight' => 11,
			'#collapsible' => true,
			'#collapsed' => false,
			'#attributes' => array(
					'class' => array(
							'collapsible',
							'collapsed'
					),
			)
	);

	/* add the protectiont target textarea */
	$output['area_concept']['protection_target'] = array(
			'#type' => 'item',
			'#title' => t('Protection target'),
			'#markup' => "<pre>"
					. (empty($areadata['protection_target'])
							? t("None available")
							: $areadata['protection_target']) . "</pre>"
	);

	/* add the tending strategies target textarea */
	$output['area_concept']['tending_strategies'] = array(
			'#type' => 'item',
			'#title' => t('Tending strategies'),
			'#markup' => "<pre>"
					. (empty($areadata['tending_strategies'])
							? t("None available")
							: $areadata['tending_strategies']) . "</pre>"
	);

	/* add the tending strategies target textarea */
	$output['area_concept']['safety_precautions'] = array(
			'#type' => 'item',
			'#title' => t('Safety precautions'),
			'#markup' => "<pre>"
					. (empty($areadata['safety_precautions'])
							? t("None available")
							: $areadata['safety_precautions']) . "</pre>"
	);

	/* Create a fieldset for the linked habitats */
	$output['area_habitats'] = array(
			'#type' => 'fieldset',
			'#title' => t('Linked habitats'),
			'#weight' => 12,
			'#collapsible' => true,
			'#attributes' => array(
					'class' => array(
							'collapsible',
							'collapsed'
					),
			)
	);

	if (function_exists('gallery_list_renderer')) {
		$output['area_videos'] = gallery_list_renderer(
			'videos',
			'area',
			$areadata['id']);
		$output['area_videos']['#weight'] = 14;
		$output['area_images'] = gallery_list_renderer(
			'images',
			'area',
			$areadata['id']);
		$output['area_images']['#weight'] = 13;
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

	$output['area_habitats']['habtiats'] = array(
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
			'#weight' => 15,
			'#id_table' => 'habitats'
	);

	/* Build the content of the table, leave out empty fields */
	$output['area']['table'] = area_get_infotable_of_specific_area($areadata);

	$output['area_files'] = area_files($areadata);
	$output['area_files']['#weight'] = 16;

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
	$form['addressdata'] = array(
			'#type' => 'fieldset',
			'#title' => $areadata['name'],
	);
	$form['addressdata']['table'] = area_get_infotable_of_specific_area(
		$areadata,
		array('create_time' => false,
				'modify_time' => false,
				'altitude' => false,
				'coordinates' => false,
				'surface_area' => false
		));
	/**
	 * Display the "edit" button only if user has write permission to this area
	 */
	if (check_permission($areadata['acl_id'], variable_get('ACL_WRITE'))) {
		$form['addressdata']['edit'] = array(
				'#type' => 'markup',
				'#markup' => l(t('Edit'), 'area/' . $areadata['id'] . '/edit')
		);
		$form['addressdata']['whitespace'] = array(
				'#type' => 'markup',
				'#markup' => '&nbsp;/&nbsp;'
		);
	}
	$form['addressdata']['show'] = array(
			'#type' => 'markup',
			'#markup' => l(t('Show'), 'area/' . $areadata['id'] . '/show')
	);
	return $form;
}

function area_habitat_table($habitatId) {
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

	$render_array['habitats'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#id_table' => 'habitats',
			'#tableWidth' => 900,
			'#options' => array(
					'jsonUrl' => "'" . base_path() . 'area/habitat/'
							. $habitatId . "/datatablejson'",
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
			'#id_table' => 'habitats'
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
