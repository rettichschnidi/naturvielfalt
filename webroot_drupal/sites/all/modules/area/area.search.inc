<?php
/**
 * 
 */

/**
 * Chose or create new area
 * @return render array
 */
function show_area_search() {

	$output['search'] = array(
			'#theme' => 'area.search',
	);

	// tlevy Hier wird die Map erstellt
	$output['googleMaps'] = drupal_get_form('area_google_maps_form');
	//   $output['googleMaps'] = area_google_maps_form(false);
	//   	$output['FieldName'] = drupal_get_form('area_field_name_form');
	$output['FieldName'] = area_field_name_form(false);
	return $output;
}

/**
 * 
 * Enter description here ...
 */
function show_area_search_userspecified() {

	$output['search'] = array(
			'#theme' => 'area.search',
	);

	// tlevy Hier wird die Map erstellt
	$output['googleMaps'] = drupal_get_form('area_google_maps_form');
	// 	$output['FieldName'] = drupal_get_form('area_field_name_form');

	$output['FieldName'] = area_field_name_form(true);
	return $output;
}

/**
 * Button with js linker
 * @return a button linking to step 1 of workflow
 */
function area_back_button() {
	$button = array(
			'#type' => 'button',
			'#attributes' => array(
					'onClick' => 'location.replace("../inventory/new"); return false;'
			),
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
			'searchByMaps' => array(
					'#theme' => 'area.search.byMaps.show'
			), // map
	);

	return $form;
}

/**
 * Search by already saved areas
 * @return input search field for saved areas
 */
function area_field_name_form($userspecified = false) {
	global $user;
	if ($userspecified) {
		$area = db_query(
			"SELECT
				area.id a_id,
				field_name,
				zip,
				locality,
				canton,
				country,
				create_time,
				u.name u_name,
				GeometryType(area.geom) AS art_dest
	    	FROM
	    		area
	    		LEFT JOIN users u ON u.uid = owner_id
	    	WHERE
	    		hidden IS NOT TRUE 
	    		AND u.uid = :userid
	    	ORDER BY
				field_name;",
			array(':userid' => $user->uid));
	} else {
		$area = db_query(
			"SELECT 
				a.id a_id,
				a.field_name,
				a.zip,
				a.locality,
				a.canton,
				a.country,
				a.create_time,
				u.name u_name,
				GeometryType(a.geom) AS art_dest
    		FROM
    			{area} a
    			LEFT JOIN {users} u ON u.uid = owner_id
    		WHERE
    			hidden IS NOT TRUE
    		ORDER BY
				field_name;");
	}

	//Localize Image
	$view_img_localize = theme(
		'image',
		array('path' => 'sites/all/modules/inventory/images/fatcow_icons/16x16/world.png',
				'alt' => t('Details'),
				'title' => t('Details'),
		));

	$rows = array();
	foreach ($area as $record) {
		$row = array();
		$row[] = $record->field_name;
		$row[] = $record->zip;
		$row[] = $record->locality;
		$row[] = $record->canton;
		$row[] = $record->country;
		if ($record->create_time) {
			$row[] = date("j.m.Y", strtotime($record->create_time));
		} else {
			$row[] = '';
		}
		$row[] = $record->name;
		$row[] = $record->art_dest;
		$row[] = array(
				'data' => l(
					$view_img_localize,
					'area/' . $record->a_id,
					array('html' => true)),
				'class' => 'show_static_image'
		);
		$rows[] = array(
				'data' => $row,
				'id' => 'area_' . $record->a_id
		);
	}

	$tableHeader[] = t('Area Name');
	$tableHeader[] = t('Zip');
	$tableHeader[] = t('Locality');
	$tableHeader[] = t('Canton');
	$tableHeader[] = t('Country');
	$tableHeader[] = t('Created on');
	$tableHeader[] = t('Area Creator');
	$tableHeader[] = t('Area Type');
	$tableHeader[] = '&nbsp;';
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

/**
 * 
 * Enter description here ...
 * @param unknown_type $form
 * @param unknown_type $form_state
 */
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

	$head_inventories = db_query(
		"SELECT * FROM area 
    WHERE owner_id = :owner_id AND hidden IS NOT TRUE
    ORDER BY modify_time DESC
    LIMIT 5",
		array(':owner_id' => $user->uid));

	foreach ($head_inventories as $record) {
		$row = array();
		$row[] = '<a href="#" onclick="AreaSelect.prototype.selectArea('
				. $record->id . ')">' . $record->field_name . '</a>';
		$row[] = $record->zip;
		$row[] = $record->locality;
		$row[] = $record->canton;
		$rows[] = $row;
	}

	$header = array(
			t('Area Name'),
			t('Zip Code'),
			t('Locality'),
			t('Canton'),
	);

	if ($rows) {
		$table['organisms'] = array(
				'#theme' => 'table',
				'#header' => $header,
				'#rows' => $rows,
		);
	} else {
		$table['noArea'] = array(
				'#markup' => t('You haven\'t used an area yet')
		);
	}
	return $table;
}

?>
