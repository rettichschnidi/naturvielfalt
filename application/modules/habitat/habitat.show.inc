<?php 
//Display linked areas
function habitat_show($habitatGroupId) {
  drupal_add_js(drupal_get_path('module', 'habitat') . '/js/habitat.datatable.select.js');
  $habitat = db_select('habitat', 'h')
    ->condition('h.id', $habitatGroupId)
    ->fields('h')
    ->execute()
    ->fetch();
  $title = $habitat->label.' '.$habitat->name_de;
  if($habitat->name_lt)
    $title .= ' ['.$habitat->name_lt.']';
  drupal_set_title($title);

  // Linked Areas
  $tableHeader[] = t('Area name');
  $tableHeader[] = t('Locality');
  $tableHeader[] = t('ZIP');
  $tableHeader[] = t('Canton');
  $tableHeader[] = t('Linked inventories');
  $tableHeader[] = t('Author');
  
  $result = db_query("
    SELECT area_habitat.area_id, area_habitat.habitat_id, area.field_name, area.locality, area.zip, area.canton, ua.field_address_first_name || ' ' || ua.field_address_last_name u_name
    FROM area_habitat
    LEFT JOIN area ON area.id = area_habitat.area_id
    LEFT JOIN users ON users.uid = area.owner_id
    LEFT JOIN field_data_field_address ua ON ua.entity_id = users.uid
    WHERE habitat_id = :id AND area.hidden IS NOT TRUE",
    array(':id' => $habitatGroupId)
  );
  
  foreach ($result as $record) {
    $inventories = db_query('
      SELECT COUNT (area_id)
      FROM head_inventory
      WHERE area_id = :aid AND hidden IS NOT TRUE',
      array(':aid' => $record->area_id)
    );
    $row = array();
    $row[] = $record->field_name;
    $row[] = $record->locality;
    $row[] = $record->zip;
    $row[] = $record->canton;
    $row[] = $inventories->fetch()->count;
    $row[] = $record->u_name;
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
    $render_array['linked_areas']['areas'] = array(
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
    $render_array['Gallery'] = gallery_list_renderer('images','habitat', $habitatGroupId);
  return $render_array;  
}

?>