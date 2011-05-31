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
	
	$result = db_query('
		SELECT area_habitat.area_id, area_habitat.habitat_id, area.field_name, area.locality, area.zip, area.canton
		FROM area_habitat
		LEFT JOIN area ON area.id = area_habitat.area_id
		WHERE habitat_id = :id',
		array(':id' => $habitatGroupId)
	);
	
	foreach ($result as $record) {
		$row = array();
		$row[] = $record->field_name;
		$row[] = $record->locality;
		$row[] = $record->zip;
		$row[] = $record->canton;
		$rows[] = array('data' => $row, 'id' => 'area_'. $record->area_id);
	}
	
	$render_array['linked_areas'] = array(
  		'#type' => 'fieldset', 
  		'#title' => t('Linked Areas'), 
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
		$render_array['linked_areas']['areas]'] = array(
		    '#theme' => 'datatable', 
		   	'#header' => $tableHeader,
			'#rows' => $rows,
			'#id_table' => 'areas',
		);
	}else{
		$render_array['linked_area']['area'] = array(
	 	 	'#markup' => t("There are no linked areas yet"),
		);
	}
	return $render_array;	
}

?>