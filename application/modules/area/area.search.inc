<?php
function show_area_search() {
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area-select.css');
	drupal_set_title(t('Areas'));
	drupal_add_js(drupal_get_path('module', 'area') . '/js/lib/infobubble.js');

	$output['areaSearch'] = array(
		'#theme' => 'area.search',
	);
	$output['FieldName'] = drupal_get_form('fieldNameForm');
	$output['googleMaps'] = drupal_get_form('googleMapsForm');

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
	return drupal_json_output($output);
}
?>