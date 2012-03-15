<?php
/**
 * 
 * 
 */


/**
 * Return JSON of the area details
 * @param string $areaId
 * @return a json string which describes the area
 */
function area_json($areaId) {
	// information about area
	$area = db_query(
			"SELECT *, ua.field_address_first_name || ' ' || ua.field_address_last_name u_name FROM {area} 
      LEFT JOIN users u ON u.uid = owner_id
      LEFT JOIN field_data_field_address ua ON ua.entity_id = u.uid
      WHERE id = :id", array(':id' => $areaId))->fetch();
	$output['field_name'] = $area->field_name;
	$output['locality'] = $area->locality;
	$output['zip'] = $area->zip;
	$output['township'] = $area->township;
	$output['comment'] = $area->comment;
	$output['creator'] = t('Creator: !creator', array('!creator' => $area->u_name));
	$output['altitude'] = t('Altitude: !alt m', array('!alt' => $area->altitude));
	$output['surface_area'] = t('Surface area: !surface m<sup>2</sup>', array('!surface' => $area->surface_area));

	// information about containing inventories
	$records = db_query(
			'SELECT hi.id AS hiid, hi.name AS hiname, i.id AS iid, it.name AS itname, u.name AS u_name, count(ie.id) AS count FROM {head_inventory} hi
    LEFT JOIN area a ON hi.area_id = a.id
    LEFT JOIN inventory i ON i.head_inventory_id = hi.id
    LEFT JOIN inventory_type it ON i.inventory_type_id = it.id
    LEFT JOIN inventory_entry ie ON ie.inventory_id = i.id
    LEFT JOIN users u ON hi.owner_id = u.uid
    WHERE a.id = :id AND hi.hidden IS NOT TRUE
    GROUP BY hi.id, hi.name, i.id, it.name, u.name
    ORDER BY hi.id', array(':id' => $areaId));
	$countInventories = 0;
	$output['inventories']['invs'] = array();
	foreach ($records as $record) {
		if ($record->hiid != $hiid) {
			$hiid = $record->hiid;
			$countInventories++;
			$output['inventories']['invs'][] = array(
					'name' => $record->hiname,
					'id' => $record->hiid,
					'owner' => $record->u_name
			);
		}
		$type = array(
				'name' => $record->itname,
				'count' => $record->count
		);
		$output['inventories']['invs'][$countInventories - 1][types][] = $type;
	}
	$invName = t('Inventories (!count)', array('!count' => $countInventories));
	$output['inventories']['title'] = $invName;

	// information about habitats
	$countHabitats = 0;
	$habitats = db_query(
			'SELECT h.id, h.name_de, h.label FROM area_habitat ah
    LEFT JOIN habitat h ON ah.habitat_id = h.id
    WHERE ah.area_id = :areaId', array(':areaId' => $areaId));
	foreach ($habitats as $habitat) {
		$output['habitats']['habs'][] = array(
				'name' => '<small>' . $habitat->label . '</small> ' . $habitat->name_de,
				'id' => $habitat->id
		);
		$countHabitats++;
	}
	$output['habitats']['title'] = t('Habitats (!count)', array('!count' => $countHabitats));
	$output['edit'] = area_edit_access($areaId);
	return drupal_json_output($output);
}

/**
 * Return search result of habitats from DB for the autocomplete form as JSON
 * @param String $string
 * @return a json string with fetched habitats
 */
function area_habitat_autocomplete($string) {
	if (preg_match('/^[0-9]/', $string) > 0) {
		$habitats = db_query('SELECT id, name_de, label FROM {habitat} WHERE label LIKE :s ORDER BY label',
				array(':s' => $string . '%'));
	} else {
		$habitats = db_query('SELECT id, name_de, label FROM {habitat} WHERE name_de ILIKE :s ORDER BY label',
				array(':s' => $string . '%'));
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
 * @param string $string
 * @return a json string whith fetched areas
 */
function area_saved_autocomplete($string) {

	$areas = db_query(
			'
    SELECT id, field_name, zip, locality, create_time, modify_time FROM area
    WHERE (field_name ILIKE :term
      OR zip ILIKE :term
      OR locality ILIKE :term) AND hidden IS NOT TRUE
    ORDER BY field_name
  ', array(":term" => "%" . $string . "%"));

	$items = array();
	foreach ($areas as $record) {
		$value = $record->field_name . ", " . $record->zip . " " . $record->locality . "<br>" . t("Created on ")
				. date("j.m.Y H:i P", strtotime($record->create_time));
		$key = $record->id;
		$items[$key] = $value;
	}
	return drupal_json_output($items);
}

/**
 * Return JSON of all areas that aren't hidden
 * @param int area_id
 * @return a json string with all areas
 */
function get_areas($area_id = false) {
	$areas = db_query(
			'
    SELECT id AS area_id, field_name, altitude, surface_area, locality, township, canton, ST_AsGeoJSON(area.geom) AS geom, comment
    FROM area
    WHERE area.hidden IS NOT TRUE
    ' . ($area_id ? 'AND area.id = ' . $area_id : '') . '
    ORDER BY area_id
  ');

	$items = array();
	foreach ($areas as $entry) {
		$item = array();
		$item['id'] = (int) $entry->area_id;
		$item['field_name'] = $entry->field_name;
		$item['parcel_nr'] = NULL;
		$item['altitude'] = (int) $entry->altitude;
		$item['surface_area'] = (int) $entry->surface_area;
		$item['locality'] = $entry->locality;
		$item['township'] = $entry->township;
		$item['canton'] = $entry->canton;
		$item['comment'] = $entry->comment;
		$item['habitats'] = array();

		$habitats = db_query(
				'
          SELECT ah.habitat_id, h.label, h.name_de
          FROM habitat h
            INNER JOIN area_habitat ah ON h.id = ah.habitat_id
          WHERE ah.area_id = ' . $entry->area_id . '
          ORDER BY ah.id asc
      ');
		foreach ($habitats as $habitat) {
			$item['habitats'][] = array(
					'id' => (int) $habitat->habitat_id,
					'label' => $habitat->label,
					'name_de' => $habitat->name_de
			);
		}

		$item['type'] = '';
		$item['area_points'] = array();
		$json = json_decode($entry->geom);
		if ($json) {
			if ('Polygon' == $json->type) {
				$item['type'] = 'polygon';
				foreach ($json->coordinates[0] as $coordinate) {
					$item['area_points'][] = array(
							'lat' => $coordinate[1],
							'lng' => $coordinate[0]
					);
				}
			} else if ('LineString' == $json->type) {
				$item['type'] = 'polyline';
				foreach ($json->coordinates as $coordinate) {
					$item['area_points'][] = array(
							'lat' => $coordinate[1],
							'lng' => $coordinate[0]
					);
				}
			} else if ('Point' == $json->type) {

				$item['type'] = 'marker';
				$item['area_points'][] = array(
						'lat' => $json->coordinates[1],
						'lng' => $json->coordinates[0]
				);
			}
		}

		$item['edit'] = area_edit_access($item['id']);

		//       global $UseAreaInMap;
		//       echo $UseAreaInMap;
		//       if($UseAreaInMap)
		$items[] = $item;
	}

	// json_encode wants that keys are sequential and start with 0, otherwise it'll treat it as an object and
	// not as an array. However, we used area_id as $key, so we have to index them numerically
	$items = array_values($items);
	return drupal_json_output($items);
}

/**
 * ACHTUNG: MUSS �BERARBEITET WERDEN!
 * [[http://swissmon.origo.ethz.ch/issues/17]] --> Security; SQL Injection edit Area 
 */
function area_datatable() {
	/*
	 * Script:    DataTables server-side script for PHP and MySQL
	 * Copyright: 2010 - Allan Jardine
	 * License:   GPL v2 or BSD (3-point)
	 */

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	 * If you just want to use the basic configuration for DataTables with PHP server-side, there is
	 * no need to edit below this line
	 */
	$aColumns = array(
			'field_name',
			'zip',
			'locality',
			'canton',
			'u_name',
			'art_dest',
	);
	/* 
	 * Paging
	 */
	$sLimit = "";
	if (isset($_REQUEST['iDisplayStart']) && $_REQUEST['iDisplayLength'] != '-1') {
		$sLimit = " LIMIT " . $_REQUEST['iDisplayStart'] . ", " . $_REQUEST['iDisplayLength'];
	}

	/*
	 * Ordering
	 */
	if (isset($_REQUEST['iSortCol_0'])) {
		$sOrder = " ORDER BY ";
		for ($i = 0; $i < intval($_REQUEST['iSortingCols']); $i++) {
			if ($_REQUEST['bSortable_' . intval($_REQUEST['iSortCol_' . $i])] == "true") {
				$sOrder .= $aColumns[intval($_REQUEST['iSortCol_' . $i])] . " " . $_REQUEST['sSortDir_' . $i] . ", ";
			}
		}
		$sOrder = substr_replace($sOrder, "", -2);
		if ($sOrder == " ORDER BY") {
			$sOrder = "";
		}
	}

	/* 
	 * Filtering
	 * NOTE this does not match the built-in DataTables filtering which does it
	 * word by word on any field. It's possible to do here, but concerned about efficiency
	 * on very large tables, and MySQL's regex functionality is very limited
	 */
	$sWhere = "";
	if ($_REQUEST['sSearch'] != "") {
		$sWhere = " WHERE (";
		for ($i = 0; $i < count($aColumns); $i++) {
			$sWhere .= $aColumns[$i] . " LIKE '%" . $_REQUEST['sSearch'] . "%' OR ";
		}
		$sWhere = substr_replace($sWhere, "", -3);
		$sWhere .= ')';
	}

	/* Individual column filtering */
	for ($i = 0; $i < count($aColumns); $i++) {
		if ($_REQUEST['bSearchable_' . $i] == "true" && $_REQUEST['sSearch_' . $i] != '') {
			if ($sWhere == "") {
				$sWhere = " WHERE ";
			} else {
				$sWhere .= " AND ";
			}
			$sWhere .= $aColumns[$i] . " LIKE '%" . $_REQUEST['sSearch_' . $i] . "%' ";
		}
	}

	/*
	 * SQL queries
	 * Get data to display
	 */
	$countArea = db_query("SELECT COUNT(*) FROM area
    LEFT JOIN users u ON u.uid = owner_id;");

	foreach ($countArea as $record) {
		$row = $record->count;
	}
	$iTotal = $row;

	$area = db_query(
			"SELECT area.id a_id, field_name, zip, locality, canton, ua.field_address_first_name || ' ' || ua.field_address_last_name u_name, art.desc art_dest FROM area
    LEFT JOIN users u ON u.uid = owner_id
    LEFT JOIN field_data_field_address ua ON ua.entity_id = u.uid" . $sWhere . $sOrder . $sLimit . ";");

	foreach ($area as $record) {
		$row = array();
		$row[] = $record->field_name;
		$row[] = $record->zip;
		$row[] = $record->locality;
		$row[] = $record->canton;
		$row[] = $record->u_name;
		$row[] = $record->art_dest;
		//$rows[] = array('data' => $row, 'id' => 'area_'. $record->a_id);
		$rows[] = $row;
	}

	/*
	 * Output
	 */

	$output = array(
			"sEcho" => intval($_REQUEST['sEcho']),
			"iTotalRecords" => $iTotal,
			"iTotalDisplayRecords" => count($rows),
			"aaData" => $rows,
	);

	echo json_encode($output);
}
?>