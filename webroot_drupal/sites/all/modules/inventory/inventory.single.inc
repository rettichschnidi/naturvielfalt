<?php

module_load_include('inc', 'inventory', 'inventory.edit');
module_load_include('inc', 'inventory', 'inventory.show');

/**
 * @see inventory_show
 */
function inventory_single_current_show() {
	global $user;
	return inventory_single_show($user);
}

/**
 * @see inventory_edit
 */
function inventory_single_current_edit() {
	global $user;
	return inventory_single_edit($user);
}

/**
 * @see inventory_delete_entry
 */
function inventory_single_current_delete_entry($entry_id, $plain = false) {
	global $user;
	return inventory_single_delete_entry($user, $entry_id, $plain);
}

/**
 * @see inventory_save_entries
 */
function inventory_single_current_save_entries() {
	global $user;
	return inventory_single_save_entries($user);
}

/**
 * @see inventory_add_inventory_group
 */
function inventory_single_current_add_inventory_group($inventory_type_id, $key) {
	global $user;
	return inventory_single_add_inventory_group($user, $inventory_type_id, $key);
}

/**
 * @see inventory_gallery
 */
function inventory_single_current_gallery() {
	global $user;
	return inventory_single_gallery($user);
}

/**
 * @see inventory_list
 */
function inventory_single_current_list() {
	global $user;
	return inventory_single_list($user);
}

function inventory_all_user_show() {
	global $user;

	$url = "user/" . $user->uid . "/singleobservations";

	drupal_goto($url);
}

/*
 * Show all registered observations
 */
function inventory_all_show() {
	global $user;
	$head_inventory_id = _inventory_single_get_id($user);
	_include_js_and_css(false);
	drupal_set_title(t('All single observations'));

	//�ffentlich Inventories holen
	$qOpen = "SELECT ie.id, ie.organism_id, o.name_de, o.name_it, hi.create_time, hi.id as hi_id, ie.accuracy, ie.id as ie_id, i.inventory_type_id, u.name FROM inventory_entry ie
 		LEFT OUTER JOIN inventory i ON i.id=ie.inventory_id 
		INNER JOIN head_inventory hi ON hi.id=i.head_inventory_id 
		INNER JOIN organism o ON o.id=ie.organism_id 
		inner join users u on u.uid=hi.owner_id
		WHERE (area_id = 155) and hi.shared = false";// and (hi.hidden = false OR hi.hidden is null)";
	$openInventories = db_query($qOpen);

	//Gemeinsame Inventories holen
	$qShared = "SELECT ie.id, ie.organism_id, o.name_de, o.name_it, hi.create_time, hi.id as hi_id, ie.accuracy, ie.id as ie_id, i.inventory_type_id, u.name FROM inventory_entry ie
		 		LEFT OUTER JOIN inventory i ON i.id=ie.inventory_id 
				INNER JOIN head_inventory hi ON hi.id=i.head_inventory_id 
				INNER JOIN organism o ON o.id=ie.organism_id 
				inner join users u on u.uid=hi.owner_id
				WHERE (area_id = 155) and hi.shared = true";// and (hi.hidden = false or hi.hidden is null)";
	$sharedInventories = db_query($qShared);

	area_add_gmap_resources(false);

	// 		if(!$inventory)
	// 		echo 'fail';
	$render_array = array();
	$render_array2 = render_table($openInventories, '&Ouml;ffentliche Einzelbeobachtungen', 'open_inventory');
	$render_array1 = render_table($sharedInventories, 'Gemeinsame Einzelbeobachtungen', 'shared_inventory');
	$render_array = array_merge($render_array, $render_array1);
	$render_array = array_merge($render_array, $render_array2);
	$render_array['map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 1,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
					//, 'collapsed'),
					'id' => 'inventory-map'
			),
	);
	// tlevy Hier wird die Map geladen
	// 		echo $head_inventory_id;
	// 		$render_array['map']['map'] = area_map($inventory->area_id, 'map_canvas');
	// 		$render_array['map']['map'] = area_map($inventory->area_id, 'map_canvas');
	$render_array['map']['map'] = area_map($inventory->area_id, 'map_canvas',
			' data-layers="' . url('inventory/' . $head_inventory_id . '/entry_locations') . '"', false, false);

	unset($render_array['details']); //entfernt die Details (wird in inventory_show gerendert)
	unset($render_array['area']);
	$render_array['map']['#attributes']['id'] = 'singleobservations-map';
	return $render_array;
}

/*
 * Rendert eine Tabelle f�r das Anzeigen von Einzelbeobachtungen
 */
function render_table($result, $tabletitle, $fieldname) {
	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');
	$img_localize = theme('image',
			array('path' => 'sites/all/modules/inventory/images/fatcow_icons/16x16/location_pin.png',
					'alt' => t('Observation location'),
					'title' => t('Observation location'),
			));
	$render_array = array();
	//Localize Image
	// 	$img_localize = theme('image', array(
	// 	    'path' => 'sites/all/modules/inventory/images/fatcow_icons/16x16/world.png',
	// 	    'alt' => t('Localize'),
	// 	    'title' => t('Localize'),
	// 	));
	if (!$headings)
		$headings = array(
				// 	t('id'),
				t('Name'),
				t('Lateinischer Name'),
				t('Familie'),
				t('Funddatum'),
				t('Anzahl'),
				t('Beobachter'),
				// 	t('Identifiziert von'),
				// 	t('Status'),
				t('Map'),
				t('')
		// 	t('Familie')
		);
	$rows = array();
	if ($result) {
		foreach ($result as $record) {

			if ($record->inventory_type_id != 16) {
				$query = db_select('inventory_type_attribute', 'i')->fields('i', array('id'))
						->condition('inventory_type_id', $record->inventory_type_id)->condition('name', "Anzahl");
				//  		echo $query." 1: ".$record->inventory_type_id."<br>";
				$anzId = $query->execute()->fetchField();
				//  		echo '<br>anzid: '.$anzId;
				$qAnz = "select * from inventory_type_attribute_inventory_entry where inventory_entry_id ="
						. $record->id . " and inventory_type_attribute_id = " . $anzId;
				// 		echo $qAnz."<br>";
				if ($anzId != "")
					$anzahlId = db_query($qAnz)->fetchField(3);
			}
			// 		echo $query." r:".$record->inventory_type_id;
			// 		$rows = array($record->id);
			// 		array_push($rows, $record->id);
			$row = array();
			// 		echo $record->id."<br>";
			// 		echo $record->organism_id."<br>";
			// 		$row[] = l($record->id,'organism/'.$record->organism_id, array('html'=>true,'attributes'=>array('id'=>'organism_'.$record->organism_id)));
			$row[] = l($record->name_de, 'organism/' . $record->organism_id,
					array('html' => true,
							'attributes' => array('id' => 'organism_' . $record->organism_id)
					));
			$row[] = l($record->name_it, 'organism/' . $record->organism_id,
					array('html' => true,
							'attributes' => array('id' => 'organism_' . $record->organism_id)
					));
			$row[] = '';
			$date = new DateTime($record->create_time);
			$row[] = $date->format("d-m-Y");
			$row[] = $anzahlId;
			$row[] = $record->name;
			// 		$row[] = '';
			// 		$row[] = '';
			// 		$row['location'] = l($img_localize, 'inventory/'.$head_inventory_id.'/entry_location/'.$invDetails['id'], array('html' => true, 'attributes' => array('class' => array('location'))));
			$head_inventory_id = $record->hi_id;
			$row[] = l($img_localize, 'inventory/' . $head_inventory_id . '/entry_location/' . $record->ie_id,
					array('html' => true,
							'attributes' => array('class' => array('location'))
					));
			$row[] = l(t('Detail'), 'inventory/' . $head_inventory_id . '/entry_location/');
			// 		$row[] = $record->
			$rows[] = array(
					'data' => $row,
					'id' => $record->id
			);
			// 		echo '<br>'.$record->id;
		}
	}
	$render_array[$fieldname] = array(
			'#type' => 'fieldset',
			'#title' => $tabletitle,
			'#weight' => 5,
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					), // add 'collapsed' to array, to close by default
			),
	// 	),
	// 	    '#attributes' => array(
	// 	      'class' => array('collapsible'.($collapsed ? ' collapsed' : '')),
	// 	),
	);

	// 	if($rows)
	$render_array[$fieldname]['inventories'] = array(
			'#theme' => 'datatable',
			'#header' => $headings,
			'#rows' => $rows,
			'#id_table' => $fieldname,
	);
	// 	elseif($msg_empty)
	// 	$render_array['all inv']['inventories'] = array(
	// 	      '#markup' => $msg_empty,
	// 	);
	return $render_array;
}

/**
 * @see inventory_show
 */
function inventory_single_show($user) {
	$head_inventory_id = _inventory_single_get_id($user);
	$render_array = inventory_show($head_inventory_id, false);
	unset($render_array['details']); //entfernt die Details (wird in inventory_show gerendert)
	unset($render_array['area']); //entfernt eine andere Ansicht (wird in inventory_show gerendert)
	$render_array['map']['#attributes']['id'] = 'singleobservations-map'; //�ndert die Position, Ausrichtung und Dimension der Map-Area
	drupal_set_title(t('Single observations by %user', array('%user' => $user->name)), PASS_THROUGH);
	return $render_array;
}

function inventory_single_show_media($user) {
	global $user;
	$head_inventory_id = _inventory_single_get_id($user);
	$render_array = inventory_show_media($head_inventory_id, false);
	unset($render_array['details']);
	unset($render_array['area']);
	$render_array['map']['#attributes']['id'] = 'singleobservations-map';
	drupal_set_title(t('Single observations by %user', array('%user' => $user->name)), PASS_THROUGH);
	return $render_array;
}

/**
 * @see inventory_edit
 */
function inventory_single_edit($user) {
	$head_inventory_id = _inventory_single_get_id($user);
	$render_array = inventory_edit($head_inventory_id, false);
	unset($render_array['basics']);
	drupal_set_title(t('Edit single observations by %user', array('%user' => $user->name)), PASS_THROUGH);
	return $render_array;
}

/**
 * @see inventory_delete_entry
 */
function inventory_single_delete_entry($user, $entry_id, $plain = false) {
	return inventory_delete_entry(_inventory_single_get_id($user), $entry_id, $plain);
}

/**
 * @see inventory_save_entries
 */
function inventory_single_save_entries($user) {
	return inventory_save_entries(_inventory_single_get_id($user));
}

/**
 * @see inventory_add_inventory_group
 */
function inventory_single_add_inventory_group($user, $inventory_type_id, $key) {
	return inventory_add_inventory_group(_inventory_single_get_id($user), $inventory_type_id, $key);
}

/**
 * @see inventory_gallery
 */
function inventory_single_gallery($user) {
	$render_array = inventory_gallery(_inventory_single_get_id($user));
	drupal_set_title(t('Single observations by %user', array('%user' => $user->name)), PASS_THROUGH);
	return $render_array;
}

function inventory_single_media($user) {
	$render_array = inventory_list(_inventory_single_get_id($user));
	drupal_set_title(t('Single observations by %user', array('%user' => $user->name)), PASS_THROUGH);
	return $render_array;
}

/**
 * @see inventory_list
 */
function inventory_single_list($user) {
	$render_array = inventory_list(_inventory_single_get_id($user));
	drupal_set_title(t('Single observations by %user', array('%user' => $user->name)), PASS_THROUGH);
	return $render_array;
}

?>