<?php
function show_area($areaId) {
	$areaResult = db_query("SELECT * FROM {area} WHERE id = :id", array(':id' => $areaId));
	$area = $areaResult->fetch();
	drupal_set_title($area->field_name);
	
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
		
	$imagepath = gmap_static_image_path($areaId,400, 300, 14);
	
	$output['Area']['area_map'] = array(
		'#type' => 'fieldset',
  		'#title' => t('Map'),
  		'#weight' => 5,
	);
	
	$output['Area']['area_map']['image'] = array(
		'#markup'=>'<img src="'.$imagepath.'">',
	);
	
	$output['Area']['area_details'] = array(
		'#type' => 'fieldset',
  		'#title' => t('Area details'),
  		'#weight' => 5,
	);
	
	$output['Area']['area_details']['table'] = array(
		'#theme' => 'table',
		'#rows' => $row,
		'#id_table' => 'area_details',
	);
	
	return $output;
}
?>