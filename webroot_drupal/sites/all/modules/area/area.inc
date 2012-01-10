<?php
/**
 * 
 */

/**
 * 
 * Enter description here ...
 * @param unknown_type $areaId
 */
function show_area($areaId) {

	$areaResult = db_query(
			"
    SELECT * , substring(ST_AsText(geom) from 1 for 4) as g
    FROM {area}
    LEFT JOIN field_data_field_address fdfa ON fdfa.entity_id = area.owner_id 
    WHERE id = :id", array(':id' => $areaId));
	$area = $areaResult->fetch();
	$isLine = $area->g == "LINE";
	//   drupal_set_title($area->field_name);
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
	drupal_add_js(drupal_get_path('module', 'area') . '/js/area.datatable.select.js');

	$coords = area_convert_coordinates($areaId);

	$tending_strategies = $area->tending_strategies;
	$protection_target = $area->protection_target;
	$safety_precautions = $area->safety_precautions;
	if ($tending_strategies == '') {
		$tending_strategies = t('not defined');
	}
	if ($protection_target == '') {
		$protection_target = t('not defined');
	}
	if ($safety_precautions == '') {
		$safety_precautions = t('not defined');
	}

	$rows = array();
	$rows[] = array(
			array(
					'data' => t('Area creator'),
					'header' => true
			),
			l($area->field_address_first_name . ' ' . $area->field_address_last_name, 'user/' . $area->owner_id
					. '/contact')
	);
	$rows[] = array(
			array(
					'data' => t('Altitude'),
					'header' => true
			),
			$area->altitude
	);
	$rows[] = array(
			array(
					'data' => $isLine ? t('Path Length') : t('Surface Area'),
					'header' => true
			),
			number_format($area->surface_area, '0', '', "'") . ($isLine ? ' m' : ' m2')
	);
	$rows[] = array(
			array(
					'data' => t('Locality'),
					'header' => true
			),
			$area->locality
	);
	$rows[] = array(
			array(
					'data' => t('Zip'),
					'header' => true
			),
			$area->zip
	);
	$rows[] = array(
			array(
					'data' => t('Township'),
					'header' => true
			),
			$area->township
	);
	$rows[] = array(
			array(
					'data' => t('Canton'),
					'header' => true
			),
			$area->canton
	);
	$rows[] = array(
			array(
					'data' => t('Country'),
					'header' => true
			),
			$area->country
	);
	$rows[] = array(
			array(
					'data' => t('Create time'),
					'header' => true
			),
			date("j.m.Y", strtotime($area->create_time))
	);
	$rows[] = array(
			array(
					'data' => t('Modify time'),
					'header' => true
			),
			date("j.m.Y", strtotime($area->modify_time))
	);
	$rows[] = array(
			array(
					'data' => t('Coordinates'),
					'header' => true
			),
			$coords
	);
	$rows[] = array(
			array(
					'data' => t('Parcels'),
					'header' => true
			),
			_area_parcels_table($areaId)
	);

	$comment[] = array(
			array(
					'data' => t('Description'),
					'header' => true
			),
			$area->comment
	);
	$concept[] = array(
			array(
					'data' => t('Protection Target'),
					'header' => true
			),
			$protection_target
	);
	$concept[] = array(
			array(
					'data' => t('Safety precautions'),
					'header' => true
			),
			$safety_precautions
	);
	$concept[] = array(
			array(
					'data' => t('Tending strategies'),
					'header' => true
			),
			$tending_strategies
	);

	$output['area_details'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area details'),
			'#weight' => 1,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
					'id' => 'area-details'
			),
	);

	$output['area_details']['table'] = array(
			'#theme' => 'table',
			'#rows' => $rows,
			'#id_table' => 'area_details',
	);

	$output['area_map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 2,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
					'id' => 'area-map'
			),
	);

	$output['area_comment'] = array(
			'#type' => 'fieldset',
			'#title' => t('Description'),
			'#weight' => 3,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
					'id' => 'area-description'
			),
	);

	$output['area_comment']['table'] = array(
			'#theme' => 'table',
			'#rows' => $comment,
			'#id_table' => 'area_details',
	);

	$output['area_concept'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area concept'),
			'#weight' => 4,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
					'id' => 'area-concept'
			),
	);

	$output['area_concept']['table'] = array(
			'#theme' => 'table',
			'#rows' => $concept,
			'#id_table' => 'area_details',
	);

	$output['area_map']['map'] = area_map($areaId);

	$output = array_merge($output, area_inventories($areaId));
	$output = array_merge($output, area_habitats($areaId));
	$output = array_merge($output, area_files($areaId));

	if (function_exists('gallery_list_renderer')) {
		$output['galleryVideos'] = gallery_list_renderer('videos', 'area', $areaId);
		$output['galleryImages'] = gallery_list_renderer('images', 'area', $areaId);

	}
	return $output;

}

/**
 * 
 * Enter description here ...
 * @param unknown_type $areaId
 */
function area_gallery($areaId) {
	return _area_presentation($areaId);
}

/**
 * 
 * Enter description here ...
 * @param unknown_type $areaId
 */
function area_list($areaId) {
	return _area_presentation($areaId, 'gallery_presentation_list', 'presentation-list', 1000000);
}

/**
 * 
 * Enter description here ...
 * @param unknown_type $areaId
 * @param unknown_type $size
 * @param unknown_type $cssClass
 * @param unknown_type $columns
 */
function _area_presentation($areaId, $size = 'gallery_presentation', $cssClass = false, $columns = GALLERY_COLUMNS) {
	if (!function_exists('gallery_presentation_renderer'))
		return;
	drupal_add_css(drupal_get_path('module', 'inventory') . '/css/main.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
	$area = db_select('area', 'a')->condition('id', $areaId)->fields('a')->execute()->fetch();
	drupal_set_title($area->field_name);
	$query = db_select('head_inventory', 'hi', array('fetch' => PDO::FETCH_ASSOC))->condition('area_id', $areaId)
			->where('hi.hidden IS NOT TRUE');
	$query->join('inventory', 'i', 'i.head_inventory_id = hi.id');
	$query->join('inventory_type', 'it', 'it.id = i.inventory_type_id');
	$query->addField('hi', 'id', 'head_inventory_id');
	$query->addField('i', 'id', 'inventory_id');
	$query->addField('it', 'id', 'inventory_type_id');
	$query->addField('it', 'name', 'inventory_type_name');
	$inventories = $query->execute()->fetchAll();

	$sorting = array(
			'default',
			'latin',
			'german',
			'family'
	);
	$sources = array(
			'default',
			'inventory',
			'presentation',
			'best'
	);

	$key = md5('area_presentation' . $areaId);
	if ($_GET[$key . '_source'] && in_array($_GET[$key . '_source'], $sources))
		$_SESSION[$key . '_source'] = $_GET[$key . '_source'];
	$source = $_SESSION[$key . '_source'];

	if ($_GET[$key . '_order'] && in_array($_GET[$key . '_order'], $sorting))
		$_SESSION[$key . '_order'] = $_GET[$key . '_order'];
	$order = $_SESSION[$key . '_order'];

	switch ($order) {
	case 'latin':
		$sort = 'fauna.genus, fauna.species, flora."Gattung", flora."Art", e.id ASC';
		break;
	case 'german':
		$sort = 'fauna.name_de, flora.name_de, e.id ASC';
		break;
	case 'family':
		$sort = 'fauna.family, flora."Familie", e.id ASC';
		break;
	default:
		$sort = 'e.position, e.id ASC';
		break;
	}

	$page = array();
	$seen = array();
	$groups = array();
	$protected = inventory_redlisted_entries();
	$weight = 1;
	foreach ($inventories as $inventory) {
		$sql = 'SELECT e.id, e.organism_id, fauna.name_de faunanamede, fauna.genus, fauna.species, fauna.family, o.organism_type, flora.name_de floranamede, flora."Gattung" floragattung, flora."Art" floraart, flora."Familie" florafamilie
      FROM inventory_entry e
      LEFT JOIN organism o on e.organism_id = o.id
      LEFT JOIN fauna_organism fauna ON o.organism_id = fauna.id
      LEFT JOIN flora_organism flora ON o.organism_id = flora.id
      WHERE e.inventory_id = :id ' . ($protected ? 'AND NOT e.id IN (' . implode(', ', $protected) . ')' : '')
				. 'ORDER BY ' . $sort . ';';
		$entries = db_query($sql, array(':id' => $inventory['inventory_id']))->fetchAll();
		if (!$entries)
			continue;
		if (!array_key_exists($inventory['inventory_type_name'], $groups))
			$groups[$inventory['inventory_type_name']] = array();

		foreach ($entries as $entry) {
			if (!array_key_exists($entry->organism_id, $seen)) {
				$seen[$entry->organism_id] = count($groups[$inventory['inventory_type_name']]);
				if ($entry->organism_type == 1) {
					// organism_type 1 is Fauna
					$label = $entry->faunanamede;
					$label_latin = $entry->genus . " " . $entry->species;
					$label_family = $entry->family;
				} else {
					// organism_type 2 is Flora
					$label = $entry->floranamede;
					$label_latin = $entry->floragattung . " " . $entry->floraart;
					$label_family = $entry->florafamilie;
				}
				$item = array(
						'content' => array(
								'label_family' => array(
										'#markup' => '<h3>' . $label_family . '</h3>'
								),
								'label_latin' => array(
										'#markup' => '<h4>' . l($label_latin, 'organism/' . $entry->organism_id)
												. '</h4>'
								),
								'label' => array(
										'#markup' => '<h5>' . l($label, 'organism/' . $entry->organism_id) . '</h5>'
								)
						)
				);
				switch ($source) {
				case 'best':
					$item['type'] = 'organism';
					$item['id'] = $entry->organism_id;
					break;
				case 'inventory':
					$item['type'] = 'inventory_entry';
					$item['id'] = $entry->id;
					break;
				case 'presentation':
					$item['type'] = 'inventory_entry';
					$item['id'] = $entry->id;
					$item['subtype'] = 'presentation';
					break;
				default:
					$item['type'] = 'inventory_entry';
					$item['id'] = $entry->id;
					$item['alternatives'] = array(
							array(
									'type' => 'organism',
									'id' => $entry->organism_id
							)
					);
					break;
				}
				$groups[$inventory['inventory_type_name']][$seen[$entry->organism_id]] = $item;
			} else if ($source == 'best')
				continue;
			array_push($groups[$inventory['inventory_type_name']][$seen[$entry->organism_id]]['id'], $entry->id);
		}
	}

	$query = db_select('area_habitat', 'ah', array('fetch' => PDO::FETCH_ASSOC))->condition('area_id', $areaId);
	$query->join('habitat', 'h', 'h.id = ah.habitat_id');
	$query->fields('h');
	$query->addField('ah', 'id', 'ah_id');
	switch ($order) {
	case 'latin':
		$query->orderBy('h.name_lt, h.label', 'ASC');
		break;
	case 'german':
		$query->orderBy('h.name_de, h.label', 'ASC');
		break;
	case 'family':
		$query->orderBy('h.label', 'ASC');
		break;
	default:
		$query->orderBy('h.label', 'ASC');
		break;
	}

	$habitats = $query->execute()->fetchAll();

	foreach ($habitats as $habitat) {
		if (!array_key_exists(t('Habitats'), $groups))
			$groups[t('Habitats')] = array();
		array_push($groups[t('Habitats')],
				array(
						'type' => 'area_habitat',
						'id' => $habitat['ah_id'],
						'alternative' => array(
								'type' => 'habitat',
								'id' => $habitat['id']
						),
						'content' => array(
								'label' => array(
										'#markup' => '<h3>' . $habitat['label'] . '</h3>'
								),
								'label_de' => array(
										'#markup' => '<h4>' . l($habitat['name_de'], 'habitat/' . $habitat['id'])
												. '</h4>'
								),
								'label_latin' => array(
										'#markup' => $habitat['name_lt'] ? '<h5>'
														. l($habitat['name_lt'], 'habitat/' . $habitat['id']) . '</h5>'
												: ''
								)
						)
				));
	}

	foreach ($groups as $group => $items) {
		if ($items) {
			$page[$group] = array(
					'#weight' => $weight++
			);
			$page[$group]['title'] = array(
					'#markup' => '<h2>' . $group . '</h2>'
			);
			$page[$group]['galleryImages'] = gallery_presentation_renderer('images', $items, $size, $cssClass, $columns);
		}
	}

	if (!$page)
		$page['empty'] = array(
				'#markup' => '<p>' . t('There are no organisms and no habitats available in this area.') . '</p>'
		);
	else {
		$url = drupal_get_destination();
		$url = drupal_parse_url($url['destination']);
		$url = $url['path'];

		$sselected = array();
		foreach ($sources as $s)
			$sselected[$s] = $source == $s ? ' selected="selected"' : '';
		$oselected = array();
		foreach ($sorting as $o)
			$oselected[$o] = $order == $o ? ' selected="selected"' : '';
		$page['form'] = array(
				'#markup' => '<form class="presentation-settings" method="get" action="' . url($url) . '">'
						. '<select size="1" name="' . $key . '_source">' . '<option value="default"'
						. $sselected['default'] . '>' . t('Prefer images from inventories') . '</option>'
						. '<option value="inventory"' . $sselected['inventory'] . '>'
						. t('Show only images from inventories') . '</option>' . '<option value="presentation"'
						. $sselected['presentation'] . '>' . t('Show only presentation images from inventories')
						. '</option>' . '<option value="best"' . $sselected['best'] . '>'
						. t('Show the highest rated images of the site') . '</option>' . '</select> '
						. '<select size="1" name="' . $key . '_order">' . '<option value="default"'
						. $oselected['default'] . '>' . t('Sort by position in inventory') . '</option>'
						. '<option value="german"' . $oselected['german'] . '>' . t('Sort by german name')
						. '</option>' . '<option value="latin"' . $oselected['latin'] . '>' . t('Sort by latin name')
						. '</option>' . '<option value="family"' . $oselected['family'] . '>' . t('Sort by family')
						. '</option>' . '</select> ' . '<input type="submit" name="submit" value="' . t('apply')
						. '" />' . '</form>',
				'weight' => -1
		);
	}

	return $page;
}

/**
 * Display area linked inventories
 * @param unknown_type $areaId
 */
function area_inventories($areaId) {
	global $user;
	$output = array();
	$result = db_query(
			"
    SELECT distinct hi.name, hi.id, hi.shared, ua.field_address_first_name || ' ' || ua.field_address_last_name uname, hi.modify_time, hi.create_time
    FROM area a
    LEFT JOIN head_inventory hi on a.id = hi.area_id
    LEFT JOIN sgroup_inventory sgi ON sgi.hiid = hi.id
    LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
    LEFT JOIN users u ON hi.owner_id = u.uid
    LEFT JOIN field_data_field_address ua ON ua.entity_id = u.uid
    LEFT JOIN sgroup sg ON sg.sgid = sgi.sgid
    WHERE a.id = :aid AND hi.hidden IS NOT TRUE AND (hi.shared = TRUE OR hi.owner_id = :uid OR (sgu.uid = :uid AND (sgi.read != 0 OR sgi.write != 0)))",
			array(':aid' => $areaId,
					':uid' => $user->uid
			));

	foreach ($result as $record) {
		$row = array();
		$row[] = $record->name;
		$row[] = date("j.m.Y", strtotime($record->modify_time));
		$row[] = date("j.m.Y", strtotime($record->create_time));
		$row[] = $record->uname;
		$rows[] = array(
				'data' => $row,
				'id' => 'inventory_' . $record->id
		);
	}

	$output['linked_inventories'] = array(
			'#type' => 'fieldset',
			'#title' => t('Linked inventories'),
			'#weight' => 6,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
					'id' => 'area-inventories'
			),
	);

	if ($record->id != '') {
		$header = array(
				t('Inventory name'),
				t('Modify time'),
				t('Create time'),
				t('Owner')
		);
		$output['linked_inventories']['area_inventory'] = array(
				'#theme' => 'datatable',
				'#header' => $header,
				'#rows' => $rows,
				'#id_table' => 'inventories',
		);
	} else {
		$output['linked_inventories']['area_inventory'] = array(
				'#markup' => t("There are no inventories linked to this area"),
		);
	}
	return $output;
}

/**
 * Display area linked habitats
 * @param unknown_type $areaId
 */
function area_habitats($areaId) {
	global $user;
	$output = array();
	$query = db_select('area_habitat', 'ah')->condition('ah.area_id', $areaId);
	$query->join('habitat', 'h', 'h.id = ah.habitat_id');
	$query->addField('ah', 'id', 'ah_id');
	$query->fields('h');

	$result = $query->execute()->fetchAll();

	//Localize Image
	$view_img_localize = theme('image',
			array('path' => 'sites/all/modules/inventory/images/fatcow_icons/16x16/world.png',
					'alt' => t('Details'),
					'title' => t('Details'),
			));

	$header[] = t('Label');
	$header[] = t('Name');
	$header[] = t('Latin');
	$header[] = t('id');

	$has_gallery = false;
	$weight = 10;
	foreach ($result as $record) {
		$row = array();
		$row[] = $record->label;
		$row[] = $record->name_de;
		$row[] = $record->name_lt;
		$row[] = $record->id;
		if (function_exists('gallery_view_link')) {
			$row[] = gallery_view_link('area_habitat', $record->ah_id);
			if ($row[4])
				$has_gallery = true;
		}
		$rows[] = array(
				'data' => $row,
				'id' => 'habitat_' . $record->id
		);
	}

	if ($has_gallery)
		$header[] = '&nbsp;';
	else
		array_walk($rows, create_function('&$row', 'array_pop($row[\'data\']);'));

	if (empty($rows))
		return $output;

	$output['linked_habitats'] = array(
			'#type' => 'fieldset',
			'#title' => t('Linked habitats'),
			'#weight' => 7,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
			),
	);

	$output['linked_habitats']['habitats'] = array(
			'#theme' => 'datatable',
			'#header' => $header,
			'#rows' => $rows,
			'#id_table' => 'habitats',
	);
	return $output;
}

/**
 * Display area linked organisms
 * @param unknown_type $areaId
 */
function showAreaOrganisms($areaId) {
	$organisms = db_query(
			'
    SELECT DISTINCT o.id oid, o.organism_type, flo."Gattung" gattung, flo."Art" art, flo."Familie" familie, flo.name_de, fao.order, fao.genus, fao.species, fao.name_de fauna_name, hi.id hid, hi.owner_id, hi.shared
    FROM AREA
    LEFT JOIN head_inventory hi ON area.id = hi.area_id
    LEFT JOIN inventory i ON hi.id = i.head_inventory_id
    LEFT JOIN inventory_entry ie ON i.id = ie.inventory_id
    LEFT JOIN organism o ON ie.organism_id = o.id
    LEFT JOIN flora_organism flo ON o.organism_id = flo.id
    LEFT JOIN fauna_organism fao ON o.organism_id = fao.id
    WHERE  area.id = :aid AND hi.hidden IS NOT TRUE
    ORDER BY oid ASC', array(':aid' => $areaId));
	foreach ($organisms as $organism) {
		$organismDates = getOrganismDates($organism->oid, $areaId);
		$row = array();
		$permissions = getOrganismPermission($organism->hid, $organism->owner_id);
		$redList = isRedList($organism->oid);
		if ($permissions[1] == TRUE OR $organism->shared == TRUE) {
			if (($redList == TRUE AND $permissions[0] == TRUE) OR $redList == FALSE) {
				if ($organism->organism_type == '1') {
					//organism is fauna
					$row[] = $organism->fauna_name;
					$row[] = $organism->genus . ' ' . $organism->species;
					$row[] = getOrganismCount($organism->oid, $areaId);
					$row[] = date('d.m.Y', $organismDates[0]);
					$row[] = date('d.m.Y', end($organismDates));
					$rows[] = array(
							'data' => $row,
							'id' => 'organism_' . $organism->oid
					);
				}
				if ($organism->organism_type == '2') {
					//organism is flora
					$row[] = $organism->name_de;
					$row[] = $organism->gattung . ' ' . $organism->art;
					$row[] = getOrganismCount($organism->oid, $areaId);
					$row[] = date('d.m.Y', $organismDates[0]);
					$row[] = date('d.m.Y', end($organismDates));
					$rows[] = array(
							'data' => $row,
							'id' => 'organism_' . $organism->oid
					);
				}
			} else {
				$containsProtected = TRUE;
			}
		}
	}

	$output['linked_organisms'] = array(
			'#type' => 'fieldset',
			'#title' => t('Linked organisms'),
			'#weight' => 3,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
			),
	);

	if ($containsProtected == TRUE) {
		$output['linked_organisms']['protected'] = array(
				'#markup' => t('Note: Without the necessary permissions, protected organisms are hidden'),
		);
	}

	if ($organism->oid != '') {
		$header = array(
				t('Name'),
				t('Latin name'),
				t('Sightings'),
				t('First Sighting'),
				t('Last Sighting')
		);
		$output['linked_organisms']['area_organisms'] = array(
				'#theme' => 'datatable',
				'#header' => $header,
				'#rows' => array_unique_multi($rows),
				'#id_table' => 'organisms',
		);
	} else {
		$output['linked_organisms']['area_organisms'] = array(
				'#markup' => t('There are no organisms listed in this area'),
		);
	}
	return $output;
}

/**
 * 
 * Check if organism is protected/member of a red list
 * @param unknown_type $organismId
 */
function isRedList($organismId) {
	$result = db_query(
			'SELECT organism.id, organism.organism_type, flo.name flora_name, flo.name_de, fao.genus fauna_genus, fao.species fauna_species, fao.name_de fauna_namede, frl.red_list_ch, fao.protection_ch
    FROM organism
    LEFT JOIN flora_organism flo ON organism.organism_id = flo.id
    LEFT JOIN fauna_organism fao ON organism.organism_id = fao.id
    LEFT JOIN flora_red_list frl ON flo.id = frl.flora_organism_id
    WHERE organism.id = :id limit 1', array(":id" => $organismId));

	$row = array();
	$record = $result->fetch();
	if ($record->organism_type == 1) {
		// organism is fauna
		if ($record->protection_ch == '0' OR $record->protection_ch == '') {
			$redlist = FALSE;
		} else {
			$redlist = TRUE;
		}
	} else {
		if ($record->red_list_ch == '0' OR $record->red_list_ch == '') {
			$redlist = FALSE;
		} else {
			$redlist = TRUE;
		}
	}
	return $redlist;
}

/**
 * 
 * Returns count of specific organism in this area
 * @param unknown_type $organismId
 * @param unknown_type $areaId
 */
function getOrganismCount($organismId, $areaId) {
	global $user;
	$count = db_query(
			'
    SELECT COUNT (ie.organism_id)
    FROM inventory_entry ie
    LEFT JOIN inventory i ON ie.inventory_id = i.id
    LEFT JOIN head_inventory hi ON i.head_inventory_id = hi.id
    LEFT JOIN area a ON hi.area_id = a.id
    WHERE ie.organism_id = :oid AND hi.hidden IS NOT TRUE AND a.id = :aid AND (hi.owner_id = :uid OR hi.shared = TRUE OR hi.id
      IN(SELECT sgu.uid
      FROM sgroup_inventory sgi
      LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
      WHERE sgu.uid = :uid)
      )', array(':oid' => $organismId,
					':aid' => $areaId,
					':uid' => $user->uid
			));
	$count = $count->fetch()->count;
	return $count;
}

/**
 * 
 * Query organism first/last seen dates
 * @param unknown_type $organismId
 * @param unknown_type $areaId
 */
function getOrganismDates($organismId, $areaId) {
	$dates = db_query(
			"SELECT itaie.value
    FROM organism o
    LEFT JOIN inventory_entry ie ON o.id = ie.organism_id
    LEFT JOIN inventory i ON ie.inventory_id = i.id
    LEFT JOIN head_inventory hi ON i.head_inventory_id = hi.id
    LEFT JOIN area a ON hi.area_id = a.id
    LEFT JOIN inventory_type_attribute_inventory_entry itaie ON ie.id = itaie.inventory_entry_id
    LEFT JOIN inventory_type_attribute ita ON itaie.inventory_type_attribute_id = ita.id
    WHERE o.id = :oid AND ita.name = 'Funddatum' AND a.id = :aid AND hi.hidden IS NOT TRUE",
			array(":oid" => $organismId,
					":aid" => $areaId
			));
	$dateTable = array();
	foreach ($dates as $date) {
		$dateTable[] = strtotime($date->value);
	}
	sort($dateTable);
	return $dateTable;
}

/**
 * 
 * Returns if user has necessary inventory permission for organism
 * @param unknown_type $inventoryId
 */
function getOrganismPermission($inventoryId, $ownerId) {
	global $user;
	$displayred = FALSE;
	$access = FALSE;
	if ($ownerId == $user->uid) {
		$displayred = TRUE;
		$access = TRUE;
	} else {
		$permissions = db_query(
				'SELECT sgi.sgid, sgi.hiid, sgi.read, sgi.write, sgi.show_red, sgu.uid
    FROM sgroup_inventory sgi
    LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
    WHERE sgi.hiid = :invid AND sgu.uid = :userid',
				array(':invid' => $record->head_inventory_id,
						':userid' => $user->uid
				));
		foreach ($permissions as $permission) {
			if ($permission->show_red == '1') {
				$displayred = TRUE;
			}
			if ($permission->read == '1' or $permission->write == '1') {
				$access = TRUE;
			}
		}
	}
	$permissions = array(
			$displayred,
			$access
	);
	return $permissions;
}

/**
 * Removes duplicate entries from a multidimensional array
 * @param unknown_type $array
 */
function array_unique_multi($array) {
	$copy = $array;
	array_walk($array, create_function('&$v,$k', '$v = serialize($v);'));
	$array = array_unique($array);
	return array_intersect_key($copy, $array);
}

/**
 * 
 * Enter description here ...
 * @param unknown_type $area_id
 */
function area_files($area_id) {
	$render_array = array();
	$result = db_query(
			'
    SELECT * 
	FROM area_file_managed afm
	LEFT JOIN file_managed fm ON afm.file_id = fm.fid
	WHERE area_id = :aid', array(':aid' => $area_id));

	foreach ($result as $record) {
		$mimeIconMap = array(
				'path' => drupal_get_path('module', 'inventory') . '/images/' . area_get_icon_map($record->filemime)
						. '.png',
				'alt' => $record->filemime,
				'title' => $record->filemime,
		);
		$img_mime = theme('image', $mimeIconMap);

		$description = $record->description;
		$filename = l($record->filename, 'sites/default/files/swissmon/area/' . $area_id . '/' . $record->filename);
		$filemime = $img_mime;
		$filesize = ($record->filesize / 1000) . " KB";
		$timestamp = date('d.m.Y', $record->timestamp);
		$row = array();
		$row[] = $description;
		$row[] = $filename;
		$row[] = $filemime;
		$row[] = $filesize;
		$row[] = $timestamp;
		$rows[] = $row;
	}

	if (empty($rows))
		return $render_array;

	$tableHeader[] = t('Description');
	$tableHeader[] = t('Filename');
	$tableHeader[] = t('Typ');
	$tableHeader[] = t('Size');
	$tableHeader[] = t('Date');

	$render_array['files'] = array(
			'#type' => 'fieldset',
			'#title' => t('Files'),
			'#weight' => 8,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
			),
	);

	$render_array['files']['data'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#rows' => $rows,
			'#id_table' => 'files',
	);
	return $render_array;
}

/**
 * 
 * Enter description here ...
 * @param unknown_type $area_id
 */
function _area_parcels_table($area_id) {
	$result = db_select('area_parcel')->fields('area_parcel')->condition('area_id', $area_id)->execute();
	foreach ($result as $record) {
		$parcels = $parcels . ' ' . $record->parcel . '<br>';
	}
	return $parcels;
}

?>