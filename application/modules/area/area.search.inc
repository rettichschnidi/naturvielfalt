<?php
/**
 * Chose or create new area
 * @return render array
 */
function show_area_search() {
	drupal_set_title(t('Show Areas'));
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-select.css');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/lib/infobubble.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/autocomplete-select.js');
	drupal_add_js(drupal_get_path('module', 'area') . '/js/habitats.js');
	
	$output['search'] = array(
		'#theme' => 'area.search',
	);

	/*
	 * Last Used areas ist laut Albert nicht mehr erforderlich
	$output['LastUsed'] = array(
		'#type' => 'fieldset',
		'#title' => t('Last Used Areas'),
	);
	$output['LastUsed']['table'] = area_last_used_table();
	*/
	
	$output['googleMaps'] = drupal_get_form('area_google_maps_form');
	$output['FieldName'] = drupal_get_form('area_field_name_form');
	return $output;
}

/**
 * Button with js linker
 * @return a button linking to step 1 of workflow
 */
function area_back_button() {
	$button = array(
		'#type' => 'button',
		'#attributes' => array('onClick' => 'location.replace("../inventory/new"); return false;'),
		'#value' => t('Back'),
  	);
  	return $button;
}

/**
 * Search by Google Maps (City, Street, etc.)
 * @return Google Maps input search field
 */
function area_google_maps_form() {
	$form['GoogleMaps'] = array(
		'#type' => 'fieldset', 
  		'#title' => t('Map'),
  		'#weight' => 3, 
  		'#collapsible' => FALSE, 
		'searchByMaps' => array('#theme' => 'area.search.byMaps.show'), // map
	);
	
	return $form;
}

/**
 * Search by already saved areas
 * @return input search field for saved areas
 */
function area_field_name_form() {
	$area = db_query(
		"SELECT area.id a_id, field_name, zip, locality, canton,country, create_time, u.name u_name, art.desc art_dest FROM area
		LEFT JOIN users u ON u.uid = owner_id
		LEFT JOIN area_type art ON art.id = type_id
		ORDER BY field_name;"
	);
	
	foreach ($area as $record) {
		$row = array();
		$row[] = $record->field_name;
		$row[] = $record->zip;
		$row[] = $record->locality;
		$row[] = $record->canton;
		$row[] = $record->country;
		if($record->create_time){
			$row[] = date("j.m.Y", strtotime($record->create_time));
		} else {
			$row[] = '';
		}
		$row[] = $record->u_name;
		$row[] = $record->art_dest;
		$rows[] = array('data' => $row, 'id' => 'area_'. $record->a_id);
	}
	
	$tableHeader[] = t('Area Name');
	$tableHeader[] = t('Zip');
	$tableHeader[] = t('Locality');
	$tableHeader[] = t('Canton');
	$tableHeader[] = t('Country');
	$tableHeader[] = t('Created on');
	$tableHeader[] = t('Area Creator');
	$tableHeader[] = t('Area Type');
	$form['FieldName'] = array(
		'#type' => 'fieldset',
  		'#title' => t('Search by field name:'),
  		'#weight' => 5,
	);

	$form['FieldName']['show_areas'] = array(
		'#theme' => 'datatable',
		'#header' => $tableHeader,
		'#rows' => $rows,
		'#id_table' => 'show_areas',
		'#len' => 220,
	);

	return $form;
}

 function area_search($form, &$form_state) {
	$form['AreaText'] = array(
		'#title' => t('Search through existing areas'),
		'#type' => 'textfield',   
		'#autocomplete_path' => 'area/area_saved_autocomplete',
	);
	return $form;
}

/**
 * Table with the 5 last used areas
 * @return table element to render
 */
function area_last_used_table() {
	global $user;

	$head_inventories= db_query(
		"SELECT * FROM area 
		WHERE owner_id = :owner_id
		ORDER BY modify_time DESC
		LIMIT 5",
	array(':owner_id'=>$user->uid)
	);

	foreach ($head_inventories as $record) {
		$row = array();
		$row[] = '<a href="#" onclick="AreaSelect.prototype.selectArea('.$record->id.')">'.$record->field_name.'</a>';
		$row[] = $record->zip;
		$row[] = $record->locality;
		$row[] = $record->canton;
		$rows[] = $row;
	}

	$header = array(t('Area Name'),t('Zip Code'),t('Locality'),t('Canton'),);

	if ($rows){
		$table['organisms'] = array(
		    '#theme' => 'table', 
			'#header' => $header,
		    '#rows' => $rows,
		);
	} else {
		$table['noArea'] = array (
			'#markup'=> t('You haven\'t used an area yet')
		);
	}
	return $table;
}

/**
 * Redirects the browser to the Google Maps static image 
 * @param int $area_id
 * @return link to Google Maps static image URL with a 302 header
 */
function googlemaps_static_redirect($area_id){
	$path = gmap_static_image_path($area_id, 200, 200, 14);
	header("Location: ".$path);	
}

/**
 * Generate GoogleMap static image path 
 * @param int $area_id
 * @param int $width
 * @param int $height
 * @param int $zoom
 * 
 * @return link to Google Maps static path
 */
function gmap_static_image_path($area_id, $width, $height, $zoom){
	//define default values if none are passed
	if(!$width){$width = 320;}				// the width of the resulting image
	if(!$height){$height = 320;}				// the width of the resulting image
	if(!$zoom){$zoom = 18;}					// zoomlevel of the map
	
	//other params
	$sensor = 'false';			// we don't need no stinkin sensors
	$fillcolor = '0x0000ff';	// color of the overlay
	$weight = 0;				// how thick should the outer line be? will get set to 2 if there's just two points, to make line visible
	
	//$center and $points get fetched from database entry
	$areas = db_query('
		SELECT * FROM area a
		LEFT JOIN area_point ap ON a.id = ap.area_id
		WHERE a.id = :id
		ORDER BY seq ASC',
	array(":id" => $area_id)
	);
	
	$counter = 0;
	
	foreach ($areas as $entry){
		$points .= '|'.$entry->lat.','.$entry->lng;
		$center = $entry->centroid_lat.",".$entry->centroid_lng;
		$counter++;
	}
	
	//it could be a marker, a path or an area
	if($counter==1){
		$google_map_link = 'http://maps.google.com/maps/api/staticmap?center='.$center.'&zoom='.$zoom.'&size='.$width.'x'.$height.'&sensor='.$sensor.'&maptype=roadmap&markers=color:blue|'.$center.'&sensor='.$sensor;
	} else if ($counter == 2){
		//setting weight to at least 3, otherwise the path will not get drawn (if 0) or be really thin (if 1 or 2)
		if($weight<3){$weight=3;}
		$google_map_link = 'http://maps.google.com/maps/api/staticmap?center='.$center.'&zoom='.$zoom.'&size='.$width.'x'.$height.'&sensor='.$sensor.'&path=fillcolor:'.$fillcolor.'|weight:'.$weight.$points;
	} else {
		//tschenereit si link
		$google_map_link = 'http://maps.google.com/maps/api/staticmap?center='.$center.'&zoom='.$zoom.'&size='.$width.'x'.$height.'&sensor='.$sensor.'&path=fillcolor:'.$fillcolor.'|weight:'.$weight.$points;
	}
	
	return $google_map_link;
}

?>