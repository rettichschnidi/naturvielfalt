<?php
function show_area($areaId) {
  $areaResult = db_query("SELECT * FROM {area} WHERE id = :id", array(':id' => $areaId));
  $area = $areaResult->fetch();
  drupal_set_title($area->field_name);
  drupal_add_js(drupal_get_path('module', 'area') . '/js/area.datatable.select.js');
  
  //include code for gmap_static_image_path
  module_load_include('inc', 'area', 'area.search');
  
  //$rows = array();
  $row[0][] = t('Area Name');
  $row[0][]= $area->field_name;
  $row[1][] = t('Altitude');
  $row[1][] = $area->altitude;
  $row[2][] = t('Surface area');
  $row[2][] = $area->surface_area;
  $row[3][] = t('Locality');
  $row[3][] = $area->locality;
  $row[4][] = t('Zip');
  $row[4][] = $area->zip;
  $row[5][] = t('Township');
  $row[5][] = $area->township;
  $row[6][] = t('Canton');
  $row[6][] = $area->canton;
  $row[7][] = t('Country');
  $row[7][] = $area->country;
  $row[8][] = t('Comment');
  $row[8][] = $area->comment;
  $row[9][] = t('Create time');
  $row[9][] = date("j.m.Y", strtotime($area->create_time));
  $row[10][] = t('Modify time');
  $row[10][] = date("j.m.Y", strtotime($area->modify_time));
  
  $output['Area']['area_map'] = array(
    '#type' => 'fieldset',
    '#title' => t('Map'),
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
      'class' => array('collapsible'),
    ),
  );

  $imagepath = gmap_static_image_path($areaId,400, 300, 14);
  $output['Area']['area_map']['image'] = array(
    '#markup'=>'<img src="'.$imagepath.'">',
  );
  $variables_map= array(
    'path' => $imagepath,
    'alt' => t('Map'),
    'title' => t('Map'),
    'width' => '400px',
    'height' => '300px',
  );
  $img_map = theme('image', $variables_map);
  
  $output['Area']['area_details'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Area details'), 
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
      'class' => array('collapsible'),
    ),
  );
  
  $output['Area']['area_details']['table'][0][] = array(
    '#theme' => 'table',
    '#rows' => $row,
    '#id_table' => 'area_details',
  );
  
  $output['Inventories'] = drupal_get_form('showAreaInventories', $areaId);
  
  if(function_exists('gallery_list_renderer'))
    $output['Gallery'] = gallery_list_renderer('area', $areaId);
  return $output;
  
}

// Load functions of inventory
require_once DRUPAL_ROOT . '/modules/inventory/inventory.show.inc';

function show_area_gallery($areaId) {
  $area = db_select('area', 'a')->condition('id', $areaId)->fields('a')->execute()->fetch();
  drupal_set_title($area->field_name);
  if(!function_exists('gallery_presentation_renderer'))
    return;
  $query = db_select('head_inventory', 'hi', array('fetch' => PDO::FETCH_ASSOC))->condition('area_id', $areaId);
  $query->join('inventory', 'i', 'i.head_inventory_id = hi.id');
  $query->join('inventory_type', 'it', 'it.id = i.inventory_type_id');
  $query->addField('hi', 'id', 'head_inventory_id');
  $query->addField('i', 'id', 'inventory_id');
  $query->addField('it', 'id', 'inventory_type_id');
  $query->addField('it', 'name', 'inventory_type_name');
  $inventories = $query->execute()->fetchAll();
  
  $page = array();
  $seen = array();
  $groups = array();
  foreach($inventories as $inventory) {
    $show_red = hasRedListPermission($inventory['head_inventory_id']);
    $sql = 'SELECT e.id, e.organism_id, fauna.name_de faunanamede, fauna.genus, fauna.species, o.organism_type, flora.name_de floranamede, flora."Gattung" floragattung, flora."Art" floraart
      FROM inventory_entry e
      LEFT JOIN organism o on e.organism_id = o.id
      LEFT JOIN fauna_organism fauna ON o.organism_id = fauna.id
      LEFT JOIN flora_organism flora ON o.organism_id = flora.id';
    if($show_red) // Show everything
      $sql .= ' WHERE e.inventory_id = :id;';
    else // Show no red list organism
      $sql .= '
        LEFT JOIN flora_red_list flora_rl ON flora_rl.flora_organism_id = flora.id AND o.organism_type = 2
        WHERE e.inventory_id = :id
        AND (
          (fauna.protection_ch != 1 AND o.organism_type = 1)
          OR (
            (flora_rl.red_list_ch != 1 OR flora_rl.red_list_ch IS NULL)
            AND o.organism_type = 2
          )
        )';
    $entries = db_query($sql ,array(':id' => $inventory['inventory_id']))->fetchAll();
    if(!$entries)
      continue;
    if(!array_key_exists($inventory['inventory_type_name'], $groups))
      $groups[$inventory['inventory_type_name']] = array();
    
    foreach($entries as $entry) {
      if(!array_key_exists($entry->organism_id, $seen)) {
        $seen[$entry->organism_id] = count($groups[$inventory['inventory_type_name']]);
        if($entry->organism_type == 1){
          // organism_type 1 is Fauna
          $label = $entry->faunanamede;
          $label_latin = $entry->genus." ".$entry->species;
        } else {
          // organism_type 2 is Flora
          $label = $entry->floranamede;
          $label_latin = $entry->floragattung." ".$entry->floraart;
        }
        $groups[$inventory['inventory_type_name']][$seen[$entry->organism_id]] = array(
          'type' => 'inventory_entry',
          'id' => array(),
          'alternative' => array(
            'type' => 'organism',
            'id' => $entry->organism_id
          ),
          'content' => array(
            'label' => array(
              '#markup' => '<h3>'.l($label, 'organism/'.$entry->organism_id).'</h3>'
            ),
            'label_latin' => array(
              '#markup' => '<h4>'.l($label_latin, 'organism/'.$entry->organism_id).'</h4>'
            )
          )
        );
        array_push($groups[$inventory['inventory_type_name']][$seen[$entry->organism_id]]['id'], $entry->id);
      }
    }
  }
  
  foreach($groups as $group => $items) {
    if($items) {
      $page[$group] = array();
      $page[$group]['title'] = array('#markup' => '<h2>'.$group.'</h2>');
      $page[$group]['gallery'] = gallery_presentation_renderer($items);
    }
  }
  
  if(!$page)
    $page['empty'] = array(
      '#markup' => '<p>'.t('There are no organisms available in this area.').'</p>'
    );
  
  return $page;
}

/**
 * Display area linked inventories
 * @param unknown_type $form
 * @param unknown_type $form_state
 * @param unknown_type $areaId
 */
function showAreaInventories($form, &$form_state, $areaId) {
  global $user;
  $result = db_query('
    SELECT distinct hi.name, hi.id, hi.shared, u.name uname, hi.modify_time, hi.create_time
    FROM area a
    LEFT JOIN head_inventory hi on a.id = hi.area_id
    LEFT JOIN sgroup_inventory sgi ON sgi.hiid = hi.id
    LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
    LEFT JOIN users u ON hi.owner_id = u.uid
    LEFT JOIN sgroup sg ON sg.sgid = sgi.sgid
    WHERE a.id = :aid AND (hi.shared = TRUE OR hi.owner_id = :uid OR (sgu.uid = :uid AND (sgi.read != 0 OR sgi.write != 0)))',
    array(':aid' => $areaId, ':uid' => $user->uid)
  );
  
  foreach($result as $record) {
    $row = array();
    $row[] = $record->name;
    $row[] = date("j.m.Y", strtotime($record->modify_time));
    $row[] = date("j.m.Y", strtotime($record->create_time));
    $row[] = $record->uname;
    $rows[] = array('data' => $row, 'id' => 'inventory_'. $record->id);
  }
  
  $output['linked_inventories'] = array(
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
  
  if($record->id!=''){
    $header = array(t('Inventory name'),t('Modify time'),t('Create time'),t('Owner'));
    $output['linked_inventories']['area_inventory'] = array(
      '#theme' => 'datatable',
      '#header' => $header,
      '#rows' => $rows,
      '#id_table' => 'inventories',
    );
  }
  else{
    $output['linked_organisms']['area_organisms'] = array(
      '#markup' => t("There are no inventories linked to this area"),
    );
  }
  return $output;
}

/**
 * Display area linked organisms
 * @param unknown_type $form
 * @param unknown_type $form_state
 * @param unknown_type $areaId
 */
function showAreaOrganisms($form, &$form_state, $areaId) {
  $organisms = db_query ('
    SELECT DISTINCT o.id oid, o.organism_type, flo."Gattung" gattung, flo."Art" art, flo."Familie" familie, flo.name_de, fao.order, fao.genus, fao.species, fao.name_de fauna_name, hi.id hid, hi.owner_id, hi.shared
    FROM AREA
    LEFT JOIN head_inventory hi ON area.id = hi.area_id
    LEFT JOIN inventory i ON hi.id = i.head_inventory_id
    LEFT JOIN inventory_entry ie ON i.id = ie.inventory_id
    LEFT JOIN organism o ON ie.organism_id = o.id
    LEFT JOIN flora_organism flo ON o.organism_id = flo.id
    LEFT JOIN fauna_organism fao ON o.organism_id = fao.id
    WHERE  area.id = :aid
    ORDER BY oid ASC',
    array(':aid' => $areaId)
  );
  foreach($organisms as $organism) {
    $organismDates = getOrganismDates($organism->oid, $areaId);
    $row = array();
    $permissions = getOrganismPermission($organism->hid, $organism->owner_id);
    $redList = isRedList($organism->oid);
    if($permissions[1] == TRUE OR $organism->shared == TRUE) {
      if(($redList == TRUE AND $permissions[0] == TRUE) OR $redList == FALSE) {
        if($organism->organism_type == '1') {
          //organism is fauna
          $row[] = $organism->fauna_name;
          $row[] = $organism->genus.' '.$organism->species;
          $row[] = getOrganismCount($organism->oid, $areaId);
          $row[] = date('d.m.Y', $organismDates[0]);
          $row[] = date('d.m.Y', end($organismDates));
          $rows[] = array('data' => $row, 'id' => 'organism_'. $organism->oid);
        }
        if($organism->organism_type == '2') {
          //organism is flora
          $row[] = $organism->name_de;
          $row[] = $organism->gattung.' '.$organism->art;
          $row[] = getOrganismCount($organism->oid, $areaId);
          $row[] = date('d.m.Y', $organismDates[0]);
          $row[] = date('d.m.Y', end($organismDates));
          $rows[] = array('data' => $row, 'id' => 'organism_'. $organism->oid);
        }
      }
      else {
        $containsProtected = TRUE;
      }
    }
  }
  
  $output['linked_organisms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Linked organisms'),
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
  
  if($containsProtected == TRUE) {
    $output['linked_organisms']['protected'] = array(
      '#markup' => 'Note: Without the necessary permissions, protected organisms are hidden', 
    );
  }
  
  if($organism->oid!=''){
    $header = array(t('Name'),t('Latin name'),t('Sightings'),t('First Sighting'),t('Last Sighting'));
    $output['linked_organisms']['area_organisms'] = array(
      '#theme' => 'datatable',
      '#header' => $header,
      '#rows' => array_unique_multi($rows),
      '#id_table' => 'organisms',
    );
  }
  else{
    $output['linked_organisms']['area_organisms'] = array(
      '#markup' => t("There are no organisms listed in this area"),
    );
  }
  return $output;
}

/**
 * 
 * Check if organism is protected/member of a red list
 * @param unknown_type $organismId
 */
function isRedList($organismId) {
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
    if($record->protection_ch=='0' OR $record->protection_ch=='') {
      $redlist = FALSE;
    }
    else {
      $redlist = TRUE;
    }
  } else {
    if($record->red_list_ch=='0' OR $record->red_list_ch=='') {
      $redlist = FALSE;
    }
    else {
      $redlist = TRUE;
    }
  }
  return $redlist;
}

/**
 * 
 * Returns count of specific organism in this area
 * @param unknown_type $organismId
 * @param unknown_type $areaId
 */
function getOrganismCount($organismId, $areaId) {
  global $user;
  $count = db_query('
    SELECT COUNT (ie.organism_id)
    FROM inventory_entry ie
    LEFT JOIN inventory i ON ie.inventory_id = i.id
    LEFT JOIN head_inventory hi ON i.head_inventory_id = hi.id
    LEFT JOIN area a ON hi.area_id = a.id
    WHERE ie.organism_id = :oid AND a.id = :aid AND (hi.owner_id = :uid OR hi.shared = TRUE OR hi.id
      IN(SELECT sgu.uid
      FROM sgroup_inventory sgi
      LEFT JOIN sgroup_users sgu ON sgi.sgid = sgu.sgid
      WHERE sgu.uid = :uid)
      )',
    array(':oid' => $organismId, ':aid' => $areaId, ':uid' => $user->uid)
  );
  $count = $count->fetch()->count;
  return $count;
}

/**
 * 
 * Query organism first/last seen dates
 * @param unknown_type $organismId
 * @param unknown_type $areaId
 */
function getOrganismDates($organismId, $areaId) {
  $dates = db_query(
    "SELECT itaie.value
    FROM organism o
    LEFT JOIN inventory_entry ie ON o.id = ie.organism_id
    LEFT JOIN inventory i ON ie.inventory_id = i.id
    LEFT JOIN head_inventory hi ON i.head_inventory_id = hi.id
    LEFT JOIN area a ON hi.area_id = a.id
    LEFT JOIN inventory_type_attribute_inventory_entry itaie ON ie.id = itaie.inventory_entry_id
    LEFT JOIN inventory_type_attribute ita ON itaie.inventory_type_attribute_id = ita.id
    WHERE o.id = :oid AND ita.name = 'Funddatum' AND a.id = :aid",
    array(":oid" => $organismId, ":aid" => $areaId)
  );
  $dateTable = array();
  foreach($dates as $date) {
    $dateTable[] = strtotime($date->value);
  }
  sort($dateTable);
  return $dateTable;
}

/**
 * 
 * Returns if user has necessary inventory permission for organism
 * @param unknown_type $inventoryId
 */
function getOrganismPermission($inventoryId, $ownerId) {
  global $user;
  $displayred = FALSE;
  $access = FALSE;
  if($ownerId == $user->uid) {
    $displayred = TRUE;
    $access = TRUE;
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
      if($permission->read == '1' or $permission->write == '1') {
        $access = TRUE;
      }
    }
  }
  $permissions = array($displayred, $access);
  return $permissions;
}

/**
 * Removes duplicate entries from a multidimensional array
 * @param unknown_type $array
 */
function array_unique_multi($array){
  $copy = $array;
  array_walk($array,create_function('&$v,$k','$v = serialize($v);'));
  $array = array_unique($array);
  return array_intersect_key($copy,$array);
}

?>