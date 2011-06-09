<?php


function inventory_list() {
	drupal_set_title(t('Show Inventory'));
	drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');

	// Own Inventories
	$tableHeader[] = t('Inventory name');
	$tableHeader[] = t('Area name');
	$tableHeader[] = t('Created on');
	$tableHeader[] = t('Modified on');
	$tableHeader[] = t('Public');
	$tableHeader[] = t('Action');
	
	global $user;
	
	//Edit Image
	$variables_edit= array(
		'path' => 'misc/fatcow_icons/16x16/application_edit.png',
		'alt' => t('Edit'),
		'title' => t('Edit'),
		'width' => '14px',
		'height' => '14px',
      );
	$img_edit = theme('image', $variables_edit);
	
	//Localize Image
	$variables_localize= array(
		'path' => 'misc/fatcow_icons/16x16/world.png',
		'attributes' => array('class' => 'show_static_image'),
		'alt' => t('Localize'),
		'title' => t('Localize'),
		'width' => '14px',
		'height' => '14px',
      );
	$img_localize = theme('image', $variables_localize);
	
	//Remove Image
	$variables_remove= array(
		'path' => 'modules/inventory/images/can_delete.png',
		'alt' => t('Delete'),
		'title' => t('Delete'),
		'width' => '14px',
		'height' => '14px',
      );
	$img_remove = theme('image', $variables_remove);
	
	$result = db_query("SELECT a.field_name, a.id area_id, hi.id, hi.name,hi.owner_id, hi.shared, hi.create_time, hi.modify_time
		FROM head_inventory hi 
		LEFT JOIN area a ON hi.area_id = a.id
		WHERE (hi.owner_id=:id)
		ORDER BY hi.name, a.field_name asc;",
		array(':id' => $user->uid)
	);
	
	foreach ($result as $record) {
		$inventoryGroupId = $record->id;
		$field_name = $record->field_name;
		if(!is_null($record->name)){
			$name = $record->name;
		} else {
			$name = t("no name");
		}
		if($shared = $record->shared == TRUE){
			$shared = t("Yes");
		} else {
			$shared = t("No");
		}
		// ("j.m.Y H:i P", strtotime($record->create_time));
		$row = array();
		$row[] = $name.' ('.$inventoryGroupId.')';
		$row[] = $field_name;
		$row[] = date("j.m.Y", strtotime($record->create_time));
		$row[] = date("j.m.Y", strtotime($record->modify_time));
		$row[] = $shared;
		$row[] = l($img_localize, 'area/'.$record->area_id, array('html'=>true,'attributes'=>array('id'=>'area_'.$record->area_id))).' '.l($img_edit, 'inventory/'.$inventoryGroupId.'/edit',array('html' => TRUE, ));		
		$rows[] = array('data' => $row, 'id' => 'inventory_'. $inventoryGroupId);
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
	
	//render only if there are any records returned
	if($rows){
		$render_array['inventories_own']['inventories'] = array(
		    '#theme' => 'datatable', 
		   	'#header' => $tableHeader,
			'#rows' => $rows,
			'#id_table' => 'inventories_own',
		);
	}else{
		$render_array['inventories_own']['inventories'] = array(
	 	 	'#markup' => t("You haven't created any inventories so far"),
		);
	}
	
	// Group Inventories
	
	$result = array();
	$record = array();
	$row = array();
	$rows = array();
	$tableHeader = array();
	
	$tableHeader[] = t('Inventory name');
	$tableHeader[] = t('Area name');
	$tableHeader[] = t('Owner');
	$tableHeader[] = t('Groups');
	$tableHeader[] = t('Created on');
	$tableHeader[] = t('Modified on');
	$tableHeader[] = t('Public');
	$tableHeader[] = t('Action');

	$result = db_query("SELECT hi.name hi_name, hi.id, a.field_name,  a.id area_id, u.name u_name, sg.sgroup_name, hi.shared, write, hi.create_time, hi.modify_time FROM head_inventory hi
		LEFT JOIN sgroup_inventory sgi ON sgi.hiid = hi.id
		LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
		LEFT JOIN users u ON hi.owner_id = u.uid
		LEFT JOIN sgroup sg ON sg.sgid = sgi.sgid
		LEFT JOIN area a ON hi.area_id = a.id
		WHERE (u.uid != :id AND sgu.uid = :id AND sgi.read = 1)
		ORDER BY hi.name, hi.id, sg.sgroup_name;",
		array(':id' => $user->uid)
	);
	
	foreach ($result as $record) {
		if($inventoryGroupId != $record->id) {
			// new inventory id => add element to the list
			$inventoryGroupId = $record->id;
			$field_name = $record->field_name;
			if(!is_null($record->hi_name)){
				$name = $record->hi_name;
			} else {
				$name = t("no name");
			}
			$owner=$record->u_name;
			if($shared = $record->shared == TRUE) {
				$shared = t("Yes");
			} else {
				$shared = t("No");
			}
			$sgroups = $record->sgroup_name;
			if($record->write == true){
				$write = l($img_localize, 'area/'.$record->area_id, array('html'=>true,'attributes'=>array('id'=>'area_'.$record->area_id))).' '.l($img_edit, 'inventory/'.$inventoryGroupId.'/edit',array('html' => TRUE, ));
			} else {
				$write = l($img_localize, 'area/'.$record->area_id, array('html'=>true,'attributes'=>array('id'=>'area_'.$record->area_id)));
			}
			$row = array(
				'Inventory' => $name.' ('.$inventoryGroupId.')',
				'Area' => $field_name,
				'Owner' => $owner,
				'Groups' => $sgroups,
				'CreatedOn' => date("j.m.Y", strtotime($record->create_time)),
				'ModifiedOn' => date("j.m.Y", strtotime($record->modify_time)),
				'Public' => $shared,
				'Edit' => $write,
			);
			$rows[] = array('data' => $row, 'id' => 'inventory_'. $inventoryGroupId);
		}
		else {
			// inventory id same as the last one => modify group field of last element in list 
			$sgroups = $record->sgroup_name;
			$rows[count($rows)-1]['Groups'] = $rows[count($rows)-1]['Groups'].', '.$sgroups;
		}
	}
	
	$render_array['inventories_group'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Group Inventories'), 
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
	
	//render only if there are any records returned
	if($rows){
		$render_array['inventories_group']['inventories'] = array(
		    '#theme' => 'datatable',
		   	'#header' => $tableHeader,
			'#rows' => $rows,
			'#id_table' => 'inventories_group',
		);
	}else{
		$render_array['inventories_group']['inventories'] = array(
	  		'#markup' => t("No shared inventories"),
		);
	}
	
	// Public Inventories
	
	$result = array();
	$record = array();
	$row = array();
	$rows = array();
	$tableHeader = array();
	
	$tableHeader[] = t('Inventory name');
	$tableHeader[] = t('Area name');
	$tableHeader[] = t('Created on');
	$tableHeader[] = t('Modified on');
	$tableHeader[] = t('Owner');
	$tableHeader[] = t('Action');

	$result = db_query("
		SELECT hi.id, hi.name hi_name, u.name u_name, a.field_name, a.id area_id, hi.create_time, hi.modify_time
		FROM head_inventory hi
			LEFT JOIN users u ON hi.owner_id = u.uid
			LEFT JOIN area a ON hi.area_id = a.id
		WHERE shared = true AND hi.id NOT IN (
			SELECT sgiEx.hiid FROM sgroup_inventory sgiEx
			LEFT JOIN sgroup_users sguEx ON sgiEx.sgid = sguEx.sgid
			WHERE sguEx.uid = :id AND (sgiEx.read != 0 OR sgiEX.write != 0)
		)
		ORDER BY hi.name, hi.id;",
		array(':id' => $user->uid)
	);
	
	foreach ($result as $record) {
		$inventoryGroupId = $record->id;
		$field_name = $record->field_name;
		if(!is_null($record->hi_name))
			$name = $record->hi_name;
		else $name = t("no name");
		$owner=$record->u_name;
		$row = array(
			'Inventory' => $name.' ('.$inventoryGroupId.')',
			'Area' => $field_name,
			'CreatedOn' => date("j.m.Y", strtotime($record->create_time)),
			'ModifiedOn' => date("j.m.Y", strtotime($record->modify_time)),
			'Owner' => $owner,
			'Action' => l($img_localize, 'area/'.$record->area_id, array('html'=>true,'attributes'=>array('id'=>'area_'.$record->area_id)))
		);
		$rows[] = array('data' => $row, 'id' => 'inventory_'. $inventoryGroupId);
	}
	
	$render_array['inventories_share'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Shared Inventories'), 
  		'#weight' => 5, 
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
	
	//render only if there are any records returned
	if($rows){
		$render_array['inventories_share']['inventories'] = array(
		    '#theme' => 'datatable',
		   	'#header' => $tableHeader,
			'#rows' => $rows,
			'#id_table' => 'inventories_share',
		);
	}else{
		$render_array['inventories_share']['inventories'] = array(
	  		'#markup' => t("No public inventories have been created so far"),
		);
	}
	
	$render_array['inventories']['static_image'] = array(
		'#markup' => '<div id="static_image" style="position: absolute; height: 200px; width: 200px;  display: none; z-index: 100; border: solid 1px grey; background-color: white"><img src="modules/area/images/ajax-loader.gif"></div>',
		'html'=> true,
	);

	return $render_array;
}
?>