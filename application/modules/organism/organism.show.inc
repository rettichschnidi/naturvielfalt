<?php
function organism_show($organismId) {
  if(function_exists('gallery_list_renderer'))
    $forms['Gallery'] = gallery_list_renderer('organism', $organismId, false, -1);
	$forms['Details'] = drupal_get_form('getOrganismDetails', $organismId);
	$forms['Inventories'] = drupal_get_form('getOrganismInventories', $organismId);
	return $forms;
}

// display basic organism details	
function getOrganismDetails($form, &$form_state, $organismId) {
	$result = db_query(
		'SELECT organism.id, organism.organism_type, flo.name flora_name, flo.name_de, fao.genus fauna_genus, fao.species fauna_species, fao.name_de fauna_namede, frl.red_list_ch, fao.protection_ch
		FROM organism
		LEFT JOIN flora_organism flo ON organism.organism_id = flo.id
		LEFT JOIN fauna_organism fao ON organism.organism_id = fao.id
		LEFT JOIN flora_red_list frl ON flo.id = frl.flora_organism_id
		WHERE organism.id = :id limit 1',
		array(":id" => $organismId)
	);

	$row = array();
	$record = $result->fetch();
	
	if($record->organism_type==1){
		// organism is fauna
		drupal_set_title(check_plain($record->fauna_namede." [".check_plain($record->fauna_genus)." ".check_plain($record->fauna_species)."]"));
		if($record->protection_ch=='0' OR $record->protection_ch=='') {
			$redlist = FALSE;
		}
		else {
			$redlist = TRUE;
		}
	} else {
		drupal_set_title(check_plain($record->name_de." [". check_plain($record->flora_name)."]"));
		if($record->red_list_ch=='0' OR $record->red_list_ch=='') {
			$redlist = FALSE;
		}
		else {
			$redlist = TRUE;
		}
	}
	
	if($redlist==TRUE) {
	$form['redlist'] = array (
			'#markup'=> t('Protection: This organism is part of a red list, for more information about red lists please ').l(t('click here'),'http://www.bafu.admin.ch/biodiversitaet/10372/10393', array('attributes' => array('target'=>'_blank'))).t('. It is possible that some information about this organism is not visible.'));
	}
	else {
	$form['redlist'] = array (
			'#markup'=> t('Protection: Not member of a red list'));		
	}
	$GLOBALS['organismtype'] = $record->organism_type;
	$GLOBALS['protected'] = $redlist;
	return $form;
}

//display organism in inventories
function getOrganismInventories($form, &$form_state, $organismId) {	
	drupal_add_js(drupal_get_path('module', 'organism') . '/js/organism.datatable.select.js');
	global $user;
	if($GLOBALS['organismtype']== 1) {
		$result = db_query(
			'SELECT distinct o.organism_type, o.organism_id, ie.inventory_id, i.id, i.head_inventory_id, a.field_name, hi.name, hi.id, u.name author, hi.shared, fo.protection_ch, hi.owner_id as uid
			FROM organism o
			LEFT JOIN inventory_entry ie on o.id = ie.organism_id
			LEFT JOIN inventory i on ie.inventory_id = i.id
			LEFT JOIN head_inventory hi on i.head_inventory_id = hi.id
			LEFT JOIN area a ON hi.area_id = a.id
			LEFT JOIN sgroup_inventory sgi ON sgi.hiid = hi.id
			LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
			LEFT JOIN users u ON hi.owner_id = u.uid
			LEFT JOIN sgroup sg ON sg.sgid = sgi.sgid
			LEFT JOIN fauna_organism fo ON o.organism_id = fo.id
			WHERE o.id = :id AND (sgu.uid = :userid OR hi.shared = TRUE OR hi.owner_id = :userid)',
			array(':id' => $organismId, ':userid' => $user->uid)
		);
	}
	else {
		$result = db_query(
			'SELECT distinct o.organism_type, o.organism_id, ie.inventory_id, i.id, i.head_inventory_id, a.field_name, hi.name, hi.id, u.name author, hi.shared, u.uid
			FROM organism o
			LEFT JOIN inventory_entry ie on o.id = ie.organism_id
			LEFT JOIN inventory i on ie.inventory_id = i.id
			LEFT JOIN head_inventory hi on i.head_inventory_id = hi.id
			LEFT JOIN area a ON hi.area_id = a.id
			LEFT JOIN sgroup_inventory sgi ON sgi.hiid = hi.id
			LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
			LEFT JOIN users u ON hi.owner_id = u.uid
			LEFT JOIN sgroup sg ON sg.sgid = sgi.sgid
			LEFT JOIN flora_organism fo ON o.organism_id = fo.id
			LEFT JOIN flora_red_list frl ON fo.id = frl.flora_organism_id
			WHERE o.id = :id AND (sgu.uid = :userid OR hi.shared = TRUE OR hi.owner_id = :userid)',
			array(':id' => $organismId, ':userid' => $user->uid)
		);
	}
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
		
		//check if user is allowed to display red list entries, only show if either author or necessary rights
		if($GLOBALS['protected']== TRUE) {
			if($record->uid == $user->uid) {
				$displayred = TRUE;
			}
			else {
				$permissions = db_query(
				'SELECT sgi.sgid, sgi.hiid, sgi.read, sgi.write, sgi.show_red, sgu.uid
				FROM sgroup_inventory sgi
				LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
				WHERE sgi.hiid = :invid AND sgu.uid = :userid',
				array(':invid' => $record->head_inventory_id, ':userid' => $user->uid)
				);
				foreach($permissions as $permission) {
					if($permission->show_red == '1') {
						$displayred = TRUE;	
					}
				}				
			}
		}
		//build table array, hide inventory details if red list organism
		$row = array();
		if($displayred == TRUE or $GLOBALS['protected']==FALSE) {
			$row[] = $record->name;
			$row[] = $record->field_name;
			$row[] = date('d.m.Y', $dateTable[0]);
			$row[] = date('d.m.Y', end($dateTable));
			$row[] = $record->author;
			$organismsInventory[] = array('data' => $row, 'id' => 'inventory_'. $record->head_inventory_id);
		}
		else {
			$row[] = t('Inventory details hidden');
			$row[] = '';
			$row[] = '';
			$row[] = '';
			$row[] = $record->author;
			$organismsInventory[] = array('data' => $row, 'id' => 'inventoryprotected_'.$record->uid);
		}
		$displayred = FALSE;			
	}	
	
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
	
	if($record->head_inventory_id!=''){
		$header = array(t('Inventory name'),t('Area name'),t('First seen'),t('Last seen'),t('Author'));
		$form['linked_inventories']['organism_inventory'] = array(
			    '#theme' => 'datatable', 
				'#header' => $header,
			    '#rows' => $organismsInventory,
				'#id_table' => 'inventories', 
				);
	}
	else{
		$form['linked_inventories']['organism_inventory'] = array(
	  		'#markup' => t("The organism is not part of any inventory so far"),
		);			
	}		
	return $form; 

}

?>