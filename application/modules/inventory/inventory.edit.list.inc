<?php
function inventory_modify() {
	drupal_set_title(t('Modify Inventory'));

	$tableHeader[] = t('Inventory');
	$tableHeader[] = t('Area');
	$tableHeader[] = t('Public');
	
	global $user;
	
	$result = db_query("SELECT a.field_name, hi.id, hi.name,hi.owner_id, hi.shared
		FROM head_inventory hi 
		LEFT JOIN area a ON hi.area_id = a.id
		WHERE (hi.owner_id=:id)
		ORDER BY hi.name, a.field_name asc;",
		array(':id' => $user->uid)
	);
	
	foreach ($result as $record) {
		$inventoryGroupId = $record->id;
		$field_name = $record->field_name;
		if(!is_null($record->name))
			$name = $record->name;
		else $name = t("no name");
		if($shared = $record->shared == TRUE)
			$shared = t("Yes");
		else $shared = t("No");
		$row = array();
		$row[] = l($name." (".$inventoryGroupId.")", 'inventory/'.$inventoryGroupId."/edit");
		$row[] = $field_name;
		$row[] = $shared;
		$rows[] = $row;
	}
	
	$render_array['inventories_own'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('My Own Inventories'), 
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
	
	$render_array['inventories_own']['inventories'] = array(
	    '#theme' => 'table', 
	   	'#header' => $tableHeader,
		'#rows' => $rows,
	);
	
	$result = array();
	$record = array();
	$row = array();
	$rows = array();
	$tableHeader = array();
	
	$tableHeader[] = t('Inventory');
	$tableHeader[] = t('Area');
	$tableHeader[] = t('Owner');
	$tableHeader[] = t('Groups');
	$tableHeader[] = t('Public');
	
	$result = db_query("SELECT hi.name hi_name, hi.id, hi.shared, a.field_name, u.name u_name, sg.sgroup_name
		FROM head_inventory hi LEFT JOIN users u ON hi.owner_id = u.uid
		LEFT JOIN sgroup_users sgu ON u.uid = sgu.uid
		LEFT JOIN sgroup sg ON sg.sgid = sgu.sgid
		LEFT JOIN sgroup_users sguMine ON sg.sgid = sguMine.sgid
		LEFT JOIN area a ON hi.area_id = a.id
		LEFT Join sgroups_inventory sgi ON sgi.hiid = hi.id
		WHERE (sguMine.uid = :id) AND (u.uid != :id) AND (write = '1')
		ORDER BY hi.name, hi.id, sg.sgroup_name;",
		array(':id' => $user->uid)
	);
	
	foreach ($result as $record) {
		if($inventoryGroupId != $record->id) {
			// new inventory id => add element to the list
			$inventoryGroupId = $record->id;
			$field_name = $record->field_name;
			if(!is_null($record->hi_name))
				$name = $record->hi_name;
			else $name = t("no name");
			$owner=$record->u_name;
			if($shared = $record->shared == TRUE)
				$shared = t("Yes");
			else $shared = t("No");
			$sgroups = $record->sgroup_name;
			$row = array(
				'Inventory' => l($name." (".$inventoryGroupId.")", 'inventory/'.$inventoryGroupId.'/edit'),
				'Area' => $field_name,
				'Owner' => $owner,
				'Groups' => $sgroups,
				'Public' => $shared,
			);
			$rows[] = $row;
		}
		else {
			// inventory id same as the last one => modify group field of last element in list 
			$sgroups = $record->sgroup_name;
			$rows[count($rows)-1]['Groups'] = $rows[count($rows)-1]['Groups'].', '.$sgroups;
		}
	}
	
	$render_array['inventories_share'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Group Inventories with Write Access'), 
  		'#weight' => 4, 
  		'#collapsible' => TRUE, 
  		'#collapsed' => FALSE,
		'#attached' => array (
	        'js' => array (
	          'misc/form.js',
	          'misc/collapse.js',
        	),
        ),
        '#attributes' => array(
        	'class' => array('collapsible'),	// add 'collapsed' to array, to close by default
        ),
	);
	
	$render_array['inventories_share']['inventories'] = array(
	    '#theme' => 'table', 
	   	'#header' => $tableHeader,
		'#rows' => $rows,
	);
	return $render_array;
}
?>