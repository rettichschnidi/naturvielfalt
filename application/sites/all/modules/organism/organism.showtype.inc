<?php
function organism_showtype($typeId) {
  $inventory_type= db_query(
    "SELECT name FROM inventory_type WHERE id=:id",
    array(':id'=> $typeId)
  );
  drupal_set_title( check_plain($inventory_type->fetch()->name) );
  drupal_add_js(drupal_get_path('module', 'organism') . '/js/organism.datatable.select.js');
  
  if( $typeId <> 16){
    $header = array(t('Family'),t('Genus'),t('Species'),t('Name'),t('Sightings'),t('Red List'));  
  } else {
    $header = array(t('Family'),t('Name'),t('Latin name'),t('Author'),t('Sightings'),t('Red List'));
  }
  
  if($typeId <> 16){
    $result = db_query(
      "select organism.id, organism.organism_type, fauna_organism.genus, fauna_organism.species, fauna_organism.name_de, fauna_organism.family, fauna_organism.protection_ch, COUNT(ie.inventory_id) as count,
        (SELECT COUNT(*) FROM gallery_image 
        	INNER JOIN file_managed ON file_managed.fid = gallery_image.fid AND gallery_image.item_id = organism.id  
			WHERE (file_managed.filemime = 'image/jpeg' OR 
			file_managed.filemime = 'image/png' OR 
			file_managed.filemime = 'image/jpg' OR 
			file_managed.filemime = 'image/gif') LIMIT 1) as count_image,
		(SELECT COUNT(*) FROM gallery_image 
			INNER JOIN file_managed ON file_managed.fid = gallery_image.fid AND gallery_image.item_id = organism.id  
			WHERE (file_managed.filemime = 'audio/mpeg') LIMIT 1) as count_sound,
		(SELECT COUNT(*) FROM gallery_image 
			INNER JOIN file_managed ON file_managed.fid = gallery_image.fid AND gallery_image.item_id = organism.id  
			WHERE (file_managed.filemime = 'video/mp4') LIMIT 1) as count_video 
      from organism
        left join fauna_organism ON organism.organism_id = fauna_organism.id
        left join inventory_type ON organism.inventory_type_id = inventory_type.id
        LEFT JOIN inventory_entry ie ON ie.organism_id = organism.id
        LEFT JOIN gallery_image AS gi ON gi.item_id = organism.id
        where inventory_type.id = :id AND organism.organism_type = 1
        GROUP BY organism.id, organism.organism_type, fauna_organism.genus, fauna_organism.species, fauna_organism.name_de, fauna_organism.family, fauna_organism.protection_ch
        order by fauna_organism.genus",
        array(':id' => $typeId)
    );
  } else {
    $result = db_query(
      'SELECT organism.id, organism.organism_type, flora_organism.name, flora_organism."Familie", flora_organism."Gattung", flora_organism."Art", flora_organism."Autor", flora_organism.name_de, frl.red_list_ch, COUNT(ie.inventory_id) as count,
        (SELECT COUNT(*) FROM gallery_image 
        	INNER JOIN file_managed ON file_managed.fid = gallery_image.fid AND gallery_image.item_id = organism.id  
			WHERE (file_managed.filemime = \'image/jpeg\' OR 
			file_managed.filemime = \'image/png\' OR 
			file_managed.filemime = \'image/jpg\' OR 
			file_managed.filemime = \'image/gif\') LIMIT 1) as count_image,
		(SELECT COUNT(*) FROM gallery_image 
			INNER JOIN file_managed ON file_managed.fid = gallery_image.fid AND gallery_image.item_id = organism.id  
			WHERE (file_managed.filemime = \'audio/mpeg\') LIMIT 1) as count_sound,
		(SELECT COUNT(*) FROM gallery_image 
			INNER JOIN file_managed ON file_managed.fid = gallery_image.fid AND gallery_image.item_id = organism.id  
			WHERE (file_managed.filemime = \'video/mp4\') LIMIT 1) as count_video  
      FROM organism
        LEFT JOIN flora_organism ON organism.organism_id = flora_organism.id
        LEFT JOIN inventory_type ON organism.inventory_type_id = inventory_type.id
        LEFT JOIN flora_red_list frl ON flora_organism.id = frl.flora_organism_id
        LEFT JOIN inventory_entry ie ON ie.organism_id = organism.id
        LEFT JOIN gallery_image AS gi ON gi.item_id = organism.id
        WHERE inventory_type.id = :id AND organism.organism_type = 2
        GROUP BY organism.id, organism.organism_type, flora_organism.name, flora_organism."Familie", flora_organism."Gattung", flora_organism."Art", flora_organism."Autor", flora_organism.name_de, frl.red_list_ch
        ORDER BY flora_organism.name',
        array(':id' => $typeId)
    );
  }
  $has_gallery = false;
  $gallery_available = function_exists('gallery_view_link');
  foreach ($result as $record) {
    if ($typeId <> 16) {
      $row = array();
      $row[] = $record->family;
      $row[] = $record->genus;
      $row[] = $record->species;
      $row[] = $record->name_de;
      $row[] = $record->count;
      $row[] = ($record->protection_ch == '0' || $record->protection_ch == '') ? t('No') : t('Yes');
    } else {
      $row = array();
      $row[] = $record->familie;
      $row[] = $record->name_de;
      $row[] = $record->gattung.' '.$record->art;
      $row[] = $record->autor;
      $row[] = $record->count;
      $row[] = ($record->red_list_ch == '0' || $record->red_list_ch == '') ? t('No') : t('Yes');
    }
    if($gallery_available) {
      if($record->count_image > 0 || $record->count_sound > 0 || $record->count_video > 0) {
        $row[] = gallery_view_link('organism', $record->id, 'organism/'.$record->id, false,$record->count_image,$record->count_sound,$record->count_video);
        $has_gallery = true;
      } else {
        $row[] = false;
      }
    }
    $rows[] = array('data' => $row, 'id' => 'organism_'. $record->id);
  }
  if($has_gallery)
    $header[] = '&nbsp';
  else
    array_walk($rows, create_function('&$row', 'array_pop($row[\'data\']);'));
  $render_array['organisms'] = array(
    '#theme' => 'datatable', 
    '#header' => $header,
    '#rows' => $rows,
    '#id_table' => 'organisms',
  );
  if(function_exists('gallery_list_renderer')){
    
    $render_array['GallerySounds'] = gallery_list_renderer('sounds','inventory_type', $typeId);
    $render_array['GalleryVideos'] = gallery_list_renderer('videos','inventory_type', $typeId);
    $render_array['GalleryImages'] = gallery_list_renderer('images','inventory_type', $typeId);
  }
  return $render_array;
}
?>