<?php
function show_area_search() {
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-select.css');
	drupal_set_title(t('Areas'));
	drupal_add_js(drupal_get_path('module', 'area') . '/js/lib/infobubble.js');
	drupal_add_js('modules/area/js/autocomplete-select.js');

	$output['test'] = array(
 	'#theme' => 'progress_done',
 	'#elements' =>array(t('Select or create an area'), t('Create Inventory'), t('Completion')), // List of elements
 	'#active' => 'Select or create an area', 		// Indicate active Element
 	);
	
	$output['areaSearch'] = array(
		'#theme' => 'area.search',
	);
	$output['LastUsed'] = drupal_get_form('lastUsed');
	$output['FieldName'] = drupal_get_form('fieldNameForm');
	$output['googleMaps'] = drupal_get_form('googleMapsForm');

	$output['aaa']= array(
 	'#markup' => "<a href='#' onclick=\"
 		jumpto(79);
 	
            \">jumpto</a>
            ",
 	);
	
	return $output;
}

function googleMapsForm() {
	$form['GoogleMaps'] = array(
		'#type' => 'fieldset', 
  		'#title' => t('Search by Google Maps:'), 
  		'#weight' => 3, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => FALSE,
		'searchByMaps' => array('#theme' => 'area.search.byMaps'),
	);
	return $form;
}

function fieldNameForm() {
	$form['FieldName'] = array(
		'#type' => 'fieldset',
  		'#title' => t('Search by field name:'), 
  		'#weight' => 5, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => TRUE,
	);

	$form['FieldName']['searchByField'] = array(
		'search' => array('#theme' => 'area.search.byField'),
	);

	return $form;
}

/**
 * Returns a json string which describes the area
 * @param String $area
 */
function area_json($areaId) {
	// information about area
	$area = db_query('SELECT * FROM {area} WHERE id = :id', array(':id' => $areaId))->fetch();
	$output['field_name'] = $area->field_name;
	$output['locality'] = $area->locality;
	$output['zip'] = $area->zip;
	$output['township'] = $area->township;
	$output['comment'] = $area->comment;
	$output['altitude'] = t('Altitude: !alt m', array('!alt' => $area->altitude));
	$output['surface_area'] = t('Surface area: !surface m<sup>2</sup>', array('!surface' => $area->surface_area));

	// information about containing inventories
	$records = db_query('SELECT hi.id AS hiid, hi.name AS hiname, i.id AS iid, it.name AS itname, count(ie.id) AS count FROM {head_inventory} hi
		LEFT JOIN area a ON hi.area_id = a.id
		LEFT JOIN inventory i ON i.head_inventory_id = hi.id
		LEFT JOIN inventory_type it ON i.inventory_type_id = it.id
		LEFT JOIN inventory_entry ie ON ie.inventory_id = i.id
		WHERE a.id = :id
		GROUP BY hi.id, hi.name, i.id, it.name
		ORDER BY hi.id', array(':id' => $areaId));
	$countInventories = 0;
	$output['inventories']['invs'] = array();
	foreach ($records as $record) {
		if ($record->hiid != $hiid) {
			$hiid = $record->hiid;
			$countInventories++;
			$output['inventories']['invs'][] = array('name' => $record->hiname, 'id' => $record->hiid);
		}
		$type = array('name' => $record->itname, 'count' => $record->count);
		$output['inventories']['invs'][$countInventories-1][types][] = $type;
	}
	$invName = t('Inventories (!count)', array('!count' => $countInventories));
	$output['inventories']['title'] = $invName;

	// information about habitats
	$output['habitats']['title'] = t('Habitats');
	$habitats = db_query('SELECT name_de, label FROM area_habitat ah
		LEFT JOIN habitat h ON ah.habitat_id = h.id
		WHERE ah.area_id = :areaId', array(':areaId' => $areaId));
	foreach ($habitats as $habitat) {
		$output['habitats']['habs'][] = $habitat->name_de;
	}

	return drupal_json_output($output);
}

function area_autocomplete($string) {

	$areas = db_query('
		SELECT id, field_name, zip, locality, create_time, modify_time FROM area
		WHERE field_name ILIKE :term
			OR zip ILIKE :term
			OR locality ILIKE :term
		ORDER BY field_name
	', array(":term" => "%".$string."%")
	);

	$items = array();
	foreach ($areas as $record){		
		$value = $record->field_name.", ".$record->zip." ".$record->locality."<br>Erstellt am ".$record->create_time;
		$key = $record->id;
		$items[$key]=$value; 
	}

	return drupal_json_output($items);
}

function lastUsed() {
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
		$row[] = $record->id;
		$row[] = $record->field_name;
		$row[] = $record->zip;
		$row[] = $record->locality;
		$row[] = $record->canton;
		$row[] = $record->create_time;
		$row[] = $record->modify_time;
		$rows[] = $row;
	}

	$header = array(t('id'),t('field_name'),t('zip'),t('locality'),t('canton'),t('create_time'),t('modify_time'),);

	$form['AreaText'] = array(
	'#title' => t('Search through existing areas'),
	'#type' => 'textfield',   
    '#autocomplete_path' => 'area/area_autocomplete',
	'#suffix' => '<div id="habitatSelected"><table></table></div>',
	);

	if ($rows){
		$form['aaaa'] = array (
			'#markup'=> t('Als letztes verwendete Gebiete')
		);
		$form['organisms'] = array(
		    '#theme' => 'table', 
			'#header' => $header,
		    '#rows' => $rows,
		);
	} else {
		$form['aaaa'] = array (
			'#markup'=> t('Sie haben noch kein Gebiet verwendet')
		);
	}

	return $form;
}
?>