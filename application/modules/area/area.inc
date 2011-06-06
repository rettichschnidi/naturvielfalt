<?php
function show_area($areaId) {
	$areaResult = db_query("SELECT * FROM {area} WHERE id = :id", array(':id' => $areaId));
	$area = $areaResult->fetch();
	drupal_set_title(check_plain($area->field_name));
	drupal_add_js(drupal_get_path('module', 'area') . '/js/area.datatable.select.js');
	
	//include code for gmap_static_image_path
	module_load_include('inc', 'area', 'area.search');
	
	$rows = array();
	$row[0][] = t('Area Name');
	$row[0][]= $area->field_name;
	$row[1][] = t('Altitude');
	$row[1][] = $area->altitude;
	$row[2][] = t('Surface area');
	$row[2][] = $area->surface_area;
	$row[3][] = t('Locality');
	$row[3][] = $area->locality;
	$row[4][] = t('Zip');
	$row[4][] = $area->zip;
	$row[5][] = t('Township');
	$row[5][] = $area->township;
	$row[6][] = t('Canton');
	$row[6][] = $area->canton;
	$row[7][] = t('Country');
	$row[7][] = $area->country;
	$row[8][] = t('Comment');
	$row[8][] = $area->comment;
	$row[9][] = t('Create time');
	$row[9][] = date("j.m.Y", strtotime($area->create_time));
	$row[10][] = t('Modify time');
	$row[10][] = date("j.m.Y", strtotime($area->modify_time));
	
	$output['Area']['area_map'] = array(
	'#type' => 'fieldset', 
	'#title' => t('Map'), 
	'#weight' => 5, 
	'#collapsible' => TRUE,
	'#collapsed' => FALSE,
	'#attached' => array (
		'js' => array (
			'misc/form.js',
			'misc/collapse.js',
		),
	),
	'#attributes' => array(
		'class' => array('collapsible'),
       ),
	);

	$imagepath = gmap_static_image_path($areaId,400, 300, 14);
	$output['Area']['area_map']['image'] = array(
		'#markup'=>'<img src="'.$imagepath.'">',
	);

	$output['Area']['area_details'] = array(
	'#type' => 'fieldset', 
	'#title' => t('Area details'), 
	'#weight' => 5, 
	'#collapsible' => TRUE,
	'#collapsed' => FALSE,
	'#attached' => array (
		'js' => array (
			'misc/form.js',
			'misc/collapse.js',
		),
	),
	'#attributes' => array(
		'class' => array('collapsible'),
       ),
	);
	
	$output['Area']['area_details']['table'] = array(
		'#theme' => 'table',
		'#rows' => $row,
		'#id_table' => 'area_details',
	);
	
	$output['Inventories'] = drupal_get_form('showAreaInventories', $areaId);
	return $output;
}

function showAreaInventories($form, &$form_state, $areaId) {
	global $user;
	$result = db_query('
		SELECT distinct hi.name, hi.id, hi.shared, u.name uname, hi.modify_time, hi.create_time
		FROM area a
		LEFT JOIN head_inventory hi on a.id = hi.area_id
		LEFT JOIN sgroup_inventory sgi ON sgi.hiid = hi.id
		LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
		LEFT JOIN users u ON hi.owner_id = u.uid
		LEFT JOIN sgroup sg ON sg.sgid = sgi.sgid
		WHERE a.id = :aid AND (hi.shared = TRUE OR hi.owner_id = :uid OR (sgu.uid = :uid AND (sgi.read != 0 OR sgi.write != 0)))',
		array(':aid' => $areaId, ':uid' => $user->uid)
	);
	
	foreach($result as $record) {
		$row = array();
		$row[] = $record->name;
		$row[] = date("j.m.Y", strtotime($record->modify_time));
		$row[] = date("j.m.Y", strtotime($record->create_time));
		$row[] = $record->uname;
		$rows[] = array('data' => $row, 'id' => 'inventory_'. $record->id);
	}
	
	$output['linked_inventories'] = array(
		'#type' => 'fieldset', 
		'#title' => t('Linked inventories'), 
		'#weight' => 3, 
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
		'#attached' => array (
			'js' => array (
				'misc/form.js',
				'misc/collapse.js',
			),
		),
		'#attributes' => array(
			'class' => array('collapsible'),
        ),
	);
	
	$header = array(t('Inventory name'),t('Modify time'),t('Create time'),t('Owner'));
		$output['linked_inventories']['area_inventory'] = array(
			    '#theme' => 'datatable', 
				'#header' => $header,
			    '#rows' => $rows,
				'#id_table' => 'inventories', 
				);
	return $output;
}
?>