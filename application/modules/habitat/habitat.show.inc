<?php 
//Display linked areas
function habitat_show($habitatGroupId) {
	drupal_set_title(t('Show Habitat Areas'));
	drupal_add_js(drupal_get_path('module', 'habitat') . '/js/habitat.datatable.select.js');

	// Linked Areas
	$tableHeader[] = t('Area name');
	$tableHeader[] = t('Locality');
	$tableHeader[] = t('ZIP');
	$tableHeader[] = t('Canton');
	$tableHeader[] = t('Linked inventories');
	$tableHeader[] = t('Author');
	
	$result = db_query('
		SELECT area_habitat.area_id, area_habitat.habitat_id, area.field_name, area.locality, area.zip, area.canton, users.name
		FROM area_habitat
		LEFT JOIN area ON area.id = area_habitat.area_id
		LEFT JOIN users ON users.uid = area.owner_id
		WHERE habitat_id = :id',
		array(':id' => $habitatGroupId)
	);
	
	foreach ($result as $record) {
		$inventories = db_query('
			SELECT COUNT (area_id)
			FROM head_inventory
			WHERE area_id = :aid',
			array(':aid' => $record->area_id)
		);
		$row = array();
		$row[] = $record->field_name;
		$row[] = $record->locality;
		$row[] = $record->zip;
		$row[] = $record->canton;
		$row[] = $inventories->fetch()->count;
		$row[] = $record->name;
		$rows[] = array('data' => $row, 'id' => 'area_'. $record->area_id);
	}
	
	$render_array['linked_areas'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Linked Areas'), 
  		'#weight' => 11, 
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
		$render_array['linked_areas']['areas]'] = array(
		    '#theme' => 'datatable', 
		   	'#header' => $tableHeader,
			'#rows' => $rows,
			'#id_table' => 'areas',
		);
	}else{
		$render_array['linked_areas']['areas'] = array(
	 	 	'#markup' => t("There are no linked areas yet"),
		);
	}
  if(function_exists('gallery_list_renderer'))
    $render_array['Gallery'] = gallery_list_renderer('habitat', $habitatGroupId);
	return $render_array;	
}

?>