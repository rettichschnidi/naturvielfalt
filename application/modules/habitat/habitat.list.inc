<?php
//Display all habitats from database
function habitat_list() {
  drupal_add_js(drupal_get_path('module', 'habitat') . '/js/habitat.datatable.select.js');

  // Own Inventories
  $tableHeader[] = t('Label');
  $tableHeader[] = t('Habitat name');
  $tableHeader[] = t('Linked Areas');
  $tableHeader[] = t('id');
  
  $result = db_query('
    SELECT label, name_de, id
    FROM habitat'
  );
  
  $has_gallery = false;
  foreach ($result as $record) {
    $count = db_query('
      SELECT count (area_habitat.area_id)
      FROM area_habitat
      LEFT JOIN area ON area.id = area_habitat.area_id
      WHERE habitat_id = :id',
      array(':id' => $record->id)
    );
    $row = array();
    $row[] = $record->label;
    $row[] = $record->name_de;
    $row[] = $count->fetch()->count;
    $row[] = $record->id;
    if(function_exists('gallery_view_link')) {
      $row[] = gallery_view_link('habitat', $record->id);
      if($row[3])
        $has_gallery = true;
    }
    $rows[] = array('data' => $row, 'id' => 'habitat_'. $record->id);
  }
  
  if($has_gallery)
    $tableHeader[] = '&nbsp;';
  else
    array_walk($rows, create_function('&$row', 'array_pop($row[\'data\']);'));
  
  $render_array['habitats'] = array(
      '#theme' => 'datatable', 
       '#header' => $tableHeader,
    '#rows' => $rows,
    '#id_table' => 'habitats',
  );
  return $render_array;
}
?>