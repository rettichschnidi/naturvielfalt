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

	$output['LastUsed'] = array(
		'#type' => 'fieldset',
		'#title' => t('Last Used Areas'),
	);
	$output['LastUsed']['table'] = area_last_used_table();
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
		"SELECT area.id a_id, field_name, zip, locality, canton, create_time, u.name u_name, art.desc art_dest FROM area
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
		if($record->create_time){
			$row[] = date("j.m.Y H:i P", strtotime($record->create_time));
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
?>