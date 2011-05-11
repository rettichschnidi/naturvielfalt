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

/**
 * Autocomplete field for search by already saved areas
 * 
 * @return a json string with all areas
 */
function get_areas() {
	$areas = db_query('
		SELECT area.id area_id, at.desc at_desc, field_name, altitude, surface_area,locality, township, canton,
			centroid_lat, centroid_lng, comment, lat, lng, seq, habitat_id, label, name_de, ah.id ah_id, ah.habitat_id ah_habitat_id
		FROM area
			LEFT JOIN area_point ap ON ap.area_id = area.id
			LEFT JOIN area_habitat ah ON ah.area_id = area.id
			LEFT JOIN habitat h ON h.id = ah.habitat_id
			LEFT JOIN area_type at ON at.id = area.type_id
		ORDER BY field_name, seq, ah_id asc
	');

	$items = array();
	
	$last_area_id = 0;
	foreach ($areas as $entry){
		if ($last_area_id!=$entry->area_id){
			// we have to reorder the habitats of the last area to make sure the first key of the array starts with 0
			// otherwise json_encode() will treat the array as an object
			if ($items[$last_area_id]['habitats']){
				$items[$last_area_id]['habitats'] = array_values($items[$last_area_id]['habitats']);
			}
		}
			$key = $entry->area_id;
			settype( $key, "integer" );

			$items[$key]['id'] = $key;
			$items[$key]['field_name'] = $entry->field_name;
			$items[$key]['parcel_nr'] = NULL;
			$items[$key]['type'] = $entry->at_desc;
			$items[$key]['altitude'] = $entry->altitude;
			settype($items[$key]['altitude'] ,"integer");
			$items[$key]['surface_area']= $entry->surface_area;
			settype($items[$key]['surface_area'] ,"integer");
			$items[$key]['locality'] = $entry->locality;
			$items[$key]['township'] = $entry->township;
			$items[$key]['canton'] = $entry->canton;
			$items[$key]['centroid']['lat'] = $entry->centroid_lat;
			$items[$key]['centroid']['lng'] = $entry->centroid_lng;
			$items[$key]['comment'] = $entry->comment;
			
			if ($entry->ah_id!=''){
				$ah_id = $entry->ah_id;
				settype($ah_id,"integer");
				
				$items[$key]['habitats'][$ah_id]['id'] = $entry->ah_habitat_id;
				settype($items[$key]['habitats'][$ah_id]['id'],"integer");
				
				$items[$key]['habitats'][$ah_id]['label'] = $entry->label;
				$items[$key]['habitats'][$ah_id]['name_de'] = $entry->name_de;
			} else {
				$items[$key]['habitats'] = array();
			}

			if($entry->seq!=''){
				// if area is of type marker, it has no points --> we have to check for that
				$seq = $entry->seq;
				settype($seq,"integer");
				$items[$key]['area_points'][$seq]['lat'] = $entry->lat;
				$items[$key]['area_points'][$seq]['lng'] = $entry->lng;
			} else {
				$items[$key]['area_points'] = array();
			}
			$last_area_id= $entry->area_id;
	}

	// json_encode wants that keys are sequential and start with 0, otherwise it'll treat it as an object and
	// not as an array. However, we used area_id as $key, so we have to index them numerically
	$items = array_values($items);

	return drupal_json_output($items);
}
?>