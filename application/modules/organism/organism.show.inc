<?php
function organism_show($organismId) {
	
	
	
	$forms['Details'] = drupal_get_form('getOrganismDetails', $organismId);
	$forms['Inventories'] = drupal_get_form('getOrganismInventories', $organismId);
	$forms['Areas'] = drupal_get_form('getOrganismAreas', $organismId);
	
	return $forms;
}

function getOrganismDetails($form, &$form_state, $organismId) {
// display basic organism details	
	$result = db_query(
		"select organism.id, organism.organism_type, flora_organism.name flora_name, flora_organism.name_de, fauna_organism.genus fauna_genus, fauna_organism.species fauna_species, fauna_organism.name_de fauna_namede
			from organism
			left join flora_organism ON organism.organism_id = flora_organism.id
			left join fauna_organism ON organism.organism_id = fauna_organism.id
			where organism.id = :id limit 1",
			array(":id" => $organismId)
	);

	$row = array();
	$record = $result->fetch();

	if($record->organism_type==1){
		// yiiiiiipeee, it is Fauna
		$row[] = $record->fauna_genus;
		$row[] = $record->fauna_species;
		$row[] = $record->fauna_namede;
		$row[] = $record->id;

		$header = array(t('Genus'),t('Species'),t('Name'),t('Id'));
		drupal_set_title( $record->fauna_namede." [".$record->fauna_genus." ".$record->fauna_species."]");
	} else {
		$row[] = $record->flora_name;
		$row[] = $record->id;

		$header = array(t('Name'),t('id'));
		drupal_set_title($record->name_de." [". $record->flora_name."]");
	}
	$rows[] = $row;
	
	$form['organisms'] = array(
    '#theme' => 'table', 
	'#header' => $header,
    '#rows' => $rows, 
	); 
	
	return $form;
}

function getOrganismInventories($form, &$form_state, $organismId) {	
	
	drupal_add_js(drupal_get_path('module', 'organism') . '/js/organism.datatable.select.js');
	
	global $user;
	
//display organism in inventories
	$result = db_query(
			'SELECT distinct o.organism_type, o.organism_id, ie.inventory_id, i.id, i.head_inventory_id, hi.name, hi.id, u.name author, hi.shared, sgi.read, sgi.write
			FROM organism o
			LEFT JOIN inventory_entry ie on o.id = ie.organism_id
			LEFT JOIN inventory i on ie.inventory_id = i.id
			LEFT JOIN head_inventory hi on i.head_inventory_id = hi.id
			LEFT JOIN sgroup_inventory sgi ON sgi.hiid = hi.id
			LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
			LEFT JOIN users u ON hi.owner_id = u.uid
			LEFT JOIN sgroup sg ON sg.sgid = sgi.sgid
			WHERE o.id = :id AND (sgu.uid = :userid OR hi.shared = TRUE OR hi.owner_id = :userid)',
			array(':id' => $organismId, ':userid' => $user->uid)
	);
	
	foreach($result as $record) {
		//query last/first seen dates for organism
		$dates = db_query(
				"SELECT itaie.value
				FROM organism o
				LEFT JOIN inventory_entry ie on o.id = ie.organism_id
				LEFT JOIN inventory i on ie.inventory_id = i.id
				LEFT JOIN head_inventory hi on i.head_inventory_id = hi.id
				LEFT JOIN inventory_type_attribute_inventory_entry itaie on ie.id = itaie.inventory_entry_id
				LEFT JOIN inventory_type_attribute ita on itaie.inventory_type_attribute_id = ita.id
				WHERE o.id = :id AND ita.name = 'Funddatum' AND i.head_inventory_id = :invid",
				array(":id" => $organismId, ":invid" => $record->head_inventory_id)
		);
		
		$dateTable = array();
		foreach($dates as $date) {
			$dateTable[] = strtotime($date->value);		
		} 
		sort($dateTable);
		
		
		
		$row = array();
		$row[] = $record->name;
		$row[] = date('d.m.Y', $dateTable[0]);
		$row[] = date('d.m.Y', end($dateTable));
		$row[] = $record->author;
		$organismsInventory[] = array('data' => $row, 'id' => 'inventory_'. $record->head_inventory_id);	
	}
	
	if($record->head_inventory_id!=''){
			$form['linked_inventories'] = array(
			'#type' => 'fieldset', 
			'#title' => t('Linked inventories'), 
			'#weight' => 3, 
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array (
				'js' => array (
					'misc/form.js',
					'misc/collapse.js',
				),
			),
			'#attributes' => array(
				'class' => array('collapsible'),
	        ),
		);
		
		$header = array(t('Inventory'),t('First seen'),t('Last seen'),t('Author'));
		$form['linked_inventories']['organism_inventory'] = array(
			    '#theme' => 'datatable', 
				'#header' => $header,
			    '#rows' => $organismsInventory,
				'#id_table' => 'inventories', 
				);
		return $form; 
	}
}

function getOrganismAreas($form, &$form_state, $organismId) {
//display organism areas
	$result = db_query(
			'SELECT distinct a.field_name, a.id, u.name author
			FROM organism o
			LEFT JOIN inventory_entry ie on o.id = ie.organism_id
			LEFT JOIN inventory i on ie.inventory_id = i.id
			LEFT JOIN head_inventory hi on i.head_inventory_id = hi.id
			LEFT JOIN area a on hi.area_id = a.id
			LEFT JOIN users u on hi.owner_id = u.uid
			WHERE o.id = :id ORDER BY field_name',
			array(":id" => $organismId)
	);
	foreach($result as $record) {
		//query last/first seen dates for organism
		$dates = db_query(
				"SELECT itaie.value
				FROM organism o
				LEFT JOIN inventory_entry ie on o.id = ie.organism_id
				LEFT JOIN inventory i on ie.inventory_id = i.id
				LEFT JOIN head_inventory hi on i.head_inventory_id = hi.id
				LEFT JOIN area a on hi.area_id = a.id
				LEFT JOIN inventory_type_attribute_inventory_entry itaie on ie.id = itaie.inventory_entry_id
				LEFT JOIN inventory_type_attribute ita on itaie.inventory_type_attribute_id = ita.id
				WHERE o.id = :id AND ita.name = 'Funddatum' AND a.id = :aid",
				array(":id" => $organismId, ":aid" => $record->id)
		);
		
		$dateTable = array();
		foreach($dates as $date) {
			$dateTable[] = strtotime($date->value);		
		} 
		sort($dateTable);
		$row = array();
		$row[] = $record->field_name;
		$row[] = date('d.m.Y', $dateTable[0]);
		$row[] = date('d.m.Y', end($dateTable));
		$row[] = $record->author;
		$organismArea[] = array('data' => $row, 'id' => 'area_'. $record->id);	
	}
	if($record->id!=''){
			$form['linked_areas'] = array(
			'#type' => 'fieldset', 
			'#title' => t('Linked areas'), 
			'#weight' => 3, 
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#attached' => array (
				'js' => array (
					'misc/form.js',
					'misc/collapse.js',
				),
			),
			'#attributes' => array(
				'class' => array('collapsible'),
	        ),
		);
		
		
		$header = array(t('Area'),t('First seen'),t('Last seen'), t('Author'));
		$form['linked_areas']['organism_area'] = array(
			    '#theme' => 'datatable', 
				'#header' => $header,
			    '#rows' => $organismArea, 
				'#id_table' => 'areas',
				);
		return $form; 
	}		
}
?>