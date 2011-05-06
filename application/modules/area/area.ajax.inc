<?php
/**
 * @return a json string which describes the area
 * @param string $areaId
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

/**
 * @param unknown_type $string
 * @return a json string with fetched habitats
 */
function area_habitat_autocomplete($string) {
	if (preg_match('/^[0-9]/', $string) > 0) {
		$habitats = db_query('SELECT id, name_de, label FROM {habitat} WHERE label LIKE :s ORDER BY label', array (':s' => $string . '%'));
	} else {
		$habitats = db_query('SELECT id, name_de, label FROM {habitat} WHERE name_de ILIKE :s ORDER BY label', array (':s' => $string . '%'));
	}

	$items = array();
	foreach ($habitats as $habitat) {
		$value = $habitat->label . ': ' . $habitat->name_de;
		$key = $habitat->id . '|' . $habitat->label . '|' . $habitat->name_de;
		$items[$key] = check_plain($value);
	}
	return drupal_json_output($items);
}

/**
 * Autocomplete field for search by already saved areas
 * 
 * @param string $string
 * @return a json string whith fetched areas
 */
function area_saved_autocomplete($string) {

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
?>