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
				'#search' => true,
				'#show' => 'myareas',
				'#ch1903' => true,
				'#theme' => 'area'
		);
	} else {
		$output['map'] = array(
				'#search' => true,
				'#show' => 'allareas',
				'#ch1903' => true,
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
			'width' => 310,
	);

	$tableHeader[] = array(
			'name' => t('Altitude [m]'),
			'dbfield' => 'altitude',
			'width' => 60
	);

	$tableHeader[] = array(
			'name' => t('Township'),
			'dbfield' => 'township',
			'width' => 150
	);

	$tableHeader[] = array(
			'name' => t('Canton'),
			'dbfield' => 'canton',
			'width' => 40
	);

	$tableHeader[] = array(
			'name' => t('Country'),
			'dbfield' => 'country',
			'width' => 180
	);

	$tableHeader[] = array(
			'name' => t('User'),
			'dbfield' => 'username',
			'width' => 120
	);

	$output['space'] = array(
			'#type' => 'markup',
			'#markup' => '<p/>'
	);

	$output['area_table'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#title' => 'Areas',
			'#id_table' => 'areas',
			'#options' => array(
					'jsonUrl' => "'" . base_path()
							. ($onlyMyAreas ? "areas/own/datatablejson'"
									: "areas/all/datatablejson'"),
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id) {
						jQuery(celDiv).click(
							function() {
								areabasic.selectArea(id);
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
					'disabled' => true
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
			'#collapsed' => true
	);

	/* add the protectiont target textarea */
	$output['area_concept']['protection_target'] = array(
			'#type' => 'item',
			'#title' => t('Protection target'),
			'#markup' => "<pre>"
					. (empty($areadata['protection_target']) ? t(
								"None available")
							: $areadata['protection_target']) . "</pre>"
	);

	/* add the tending strategies target textarea */
	$output['area_concept']['tending_strategies'] = array(
			'#type' => 'item',
			'#title' => t('Tending strategies'),
			'#markup' => "<pre>"
					. (empty($areadata['tending_strategies']) ? t(
								"None available")
							: $areadata['tending_strategies']) . "</pre>"
	);

	/* add the tending strategies target textarea */
	$output['area_concept']['safety_precautions'] = array(
			'#type' => 'item',
			'#title' => t('Safety precautions'),
			'#markup' => "<pre>"
					. (empty($areadata['safety_precautions']) ? t(
								"None available")
							: $areadata['safety_precautions']) . "</pre>"
	);

	/* Create a fieldset for the linked habitats */
	$output['area_habitats'] = array(
			'#type' => 'fieldset',
			'#title' => t('Linked habitats'),
			'#weight' => 12,
			'#collapsible' => true
	);

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
	foreach($results->fetchAll() as $habitat) {
		$habitats[] = (array) $habitat;
	}
	
	$output['area_habitats']['habtiats'] = array(
			'#theme' => 'datatable',
			'#header' => array(
					array('name' => t('Label'), 'width' => 300),
					array('name' => t('Name'), 'width' => 300)
			),
			'#tableWidth' => 900,
			'#rows' => $habitats,
			'#id_table' => 'habitats'
	);

	/* Build the content of the table, leave out empty fields */
	$output['area']['table'] = area_get_infotable_of_specific_area($areadata);

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
				'#markup' => '&nbsp/&nbsp;'
		);
	}
	$form['addressdata']['show'] = array(
			'#type' => 'markup',
			'#markup' => l(t('Show'), 'area/' . $areadata['id'] . '/show')
	);
	return $form;
}
?>
