<?php 
function inventory_show($head_inventory_id) {
  drupal_set_title(getInventoryTitle($head_inventory_id)." (".$head_inventory_id.")");
  drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');
  
  $render_array = getInventoryBasics($head_inventory_id);
  
  if(inventory_user_has_redlist_permission($head_inventory_id) == FALSE)
    $render_array = array_merge($render_array, getRedListCount($head_inventory_id));
  
  if(function_exists('gallery_list_renderer'))
    $render_array['Gallery'] = gallery_list_renderer('head_inventory', $head_inventory_id, false, 100);
  
  $render_array = array_merge($render_array, getInventories($head_inventory_id), getInventoryFiles($head_inventory_id));

  return $render_array;
}

function getInventoryTitle($head_inventory_id) {
  $inventory_title = '-';

  $inventories = db_query('SELECT name
                FROM head_inventory
                WHERE id = :id;', array(':id' => $head_inventory_id));
  foreach($inventories as $inventory) {
    if(!is_null($inventory->name))
    $inventory_title =  $inventory->name;
  }

  return $inventory_title;
}

function inventory_gallery($head_inventory_id) {
  return _inventory_presentation($head_inventory_id);
}

function inventory_list($head_inventory_id) {
  return _inventory_presentation($head_inventory_id, 'gallery_presentation_list', 'presentation-list', 1000000);
}

function _inventory_presentation($head_inventory_id, $size='gallery_presentation', $cssClass=false, $columns=GALLERY_COLUMNS) {
  if(!function_exists('gallery_presentation_renderer'))
    return;
  drupal_add_css(drupal_get_path('module', 'inventory') . '/css/main.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_set_title(getInventoryTitle($head_inventory_id)." (".$head_inventory_id.")");
  
  $sorting = array('default', 'latin', 'german', 'family');
  $sources = array('default', 'inventory', 'presentation', 'best');
  
  $key = md5('inventory_presentation'.$head_inventory_id);
  if($_GET[$key.'_source'] && in_array($_GET[$key.'_source'], $sources))
    $_SESSION[$key.'_source'] = $_GET[$key.'_source'];
  $source = $_SESSION[$key.'_source'];
  
  if($_GET[$key.'_order'] && in_array($_GET[$key.'_order'], $sorting))
    $_SESSION[$key.'_order'] = $_GET[$key.'_order'];
  $order = $_SESSION[$key.'_order'];
  
  switch($order) {
    case 'latin':
      $sort = 'fauna.genus, fauna.species, flora."Gattung", flora."Art", e.id ASC';
      break;
    case 'german':
      $sort = 'fauna.name_de, flora.name_de, e.id ASC';
      break;
    case 'family':
      $sort = 'fauna.family, flora."Familie", e.id ASC';
      break;
    default:
      $sort = 'e.position, e.id ASC';
      break;
  }
  
  $invArr = getInventoriesData($head_inventory_id, $sort);
  
  $weight = 1;
  $seen = array();
  foreach($invArr as $inventory_group) {
    $items = array();
    
    $invGroupName = $inventory_group['invDesc']['name'];
    $rows = array();
    foreach($inventory_group['invDesc']['cols'] as $invDetail)
      $rows[] = array(array(
        'data' => $invDetail['name'],
        'header' => true
      ));
    foreach($inventory_group as $k => $invDetails) {
      if((is_string($k) && $k == 'invDesc') || in_array($invDetails['orgId'], $seen))
        continue;
      array_push($seen, $invDetails['orgId']);
      $item = array(
        'content' => array(
          'label_family' => array(
            '#markup' => '<h3>'.$invDetails['label_family'].'</h3>'
          ),
          'label_latin' => array(
            '#markup' => '<h4>'.l($invDetails['label_latin'], 'organism/'.$invDetails['orgId']).'</h4>'
          ),
          'label' => array(
            '#markup' => '<h5>'.l($invDetails['label'], 'organism/'.$invDetails['orgId']).'</h5>'
          )
        )
      );
      switch($source) {
        case 'best':
          $item['type'] = 'organism';
          $item['id'] = $invDetails['orgId'];
          break;
        case 'inventory':
          $item['type'] = 'inventory_entry';
          $item['id'] = $invDetails['id'];
          break;
        case 'presentation':
          $item['type'] = 'inventory_entry';
          $item['id'] = $invDetails['id'];
          $item['subtype'] = 'presentation';
          break;
        default:
          $item['type'] = 'inventory_entry';
          $item['id'] = $invDetails['id'];
          $item['alternatives'] = array(
            array(
              'type' => 'organism',
              'id' => $invDetails['orgId']
            )
          );
          break;
      }
      array_push($items, $item);
    }
    if($items) {
      $render_array[$invGroupName] = array('#weight' => $weight++);
      $render_array[$invGroupName]['title'] = array('#markup' => '<h2>'.$invGroupName.'</h2>');
      $render_array[$invGroupName]['gallery'] = gallery_presentation_renderer($items, $size, $cssClass, $columns, (!$source || $source == 'default' ? t('Evidence image') : false));
    }
  }
  
  if(!$render_array)
    $render_array['empty'] = array(
      '#markup' => '<p>'.t('There are no organisms available in this inventory.').'</p>'
    );
  else {
    $url = drupal_get_destination();
    $url = drupal_parse_url($url['destination']);
    $url = $url['path'];
    
    $sselected = array();
    foreach($sources as $s)
      $sselected[$s] = $source == $s ? ' selected="selected"' : '';
    $oselected = array();
    foreach($sorting as $o)
      $oselected[$o] = $order == $o ? ' selected="selected"' : '';
    $render_array['form'] = array(
      '#markup' => 
        '<form class="presentation-settings" method="get" action="'.url($url).'">'.
          '<select size="1" name="'.$key.'_source">'.
            '<option value="default"'.$sselected['default'].'>'.t('Prefer images from inventory').'</option>'.
            '<option value="inventory"'.$sselected['inventory'].'>'.t('Show only images from inventory').'</option>'.
            '<option value="presentation"'.$sselected['presentation'].'>'.t('Show only presentation images from inventory').'</option>'.
            '<option value="best"'.$sselected['best'].'>'.t('Show the highest rated images of the site').'</option>'.
          '</select> '.
          '<select size="1" name="'.$key.'_order">'.
            '<option value="default"'.$oselected['default'].'>'.t('Sort by position in inventory').'</option>'.
            '<option value="german"'.$oselected['german'].'>'.t('Sort by german name').'</option>'.
            '<option value="latin"'.$oselected['latin'].'>'.t('Sort by latin name').'</option>'.
            '<option value="family"'.$oselected['family'].'>'.t('Sort by family').'</option>'.
          '</select> '.
          '<input type="submit" name="submit" value="'.t('apply').'" />'.
        '</form>',
      'weight' => -1
    );
  }
  return $render_array;
}

/**
 * 
 * Returns form containing inventory basic information
 * @param unknown_type $head_inventory_id
 */
function getInventoryBasics($head_inventory_id) {
  //Display Inventory Basics
  $query = db_select('head_inventory', 'hi')
    ->condition('hi.id', $head_inventory_id)
    ->fields('hi');
  $query->leftJoin('users', 'u', 'u.uid = hi.owner_id');
  $query->leftJoin('area', 'a', 'a.id = hi.area_id');
  $query->addField('u', 'uid');
  $query->addField('u', 'name', 'owner');
  $query->addField('a', 'field_name', 'area_name');
  $query->addField('a', 'locality', 'area_locality');
  $query->addField('a', 'zip', 'area_zip');
  $query->addField('a', 'id', 'area_id');
  $query->addField('a', 'owner_id', 'area_owner_id');
  $inventory = $query->execute()->fetch();
  
  $query = db_select('area', 'a')
  ->condition('a.id', $inventory->area_id)
  ->fields('a');
  $query->leftJoin('users', 'u', 'u.uid = a.owner_id');
  $query->addField('u','name','owner');
  $area = $query->execute()->fetch();

  $rows = array();
  $rows[] = array(
    array('data' => t('Observer'), 'header' => true), 
    l($inventory->owner, 'user/'.$inventory->uid.'/contact')
  );
  $rows[] = array(
    array('data' => t('Modified on'), 'header' => true), 
    date("j.m.Y", strtotime($inventory->modify_time))
  );
  $rows[] = array(
    array('data' => t('Created on'), 'header' => true), 
    date("j.m.Y", strtotime($inventory->create_time))
  );
  $rows[] = array(
    array('data' => t('Public'), 'header' => true), 
    $inventory->shared == "t" ? t('Yes') : t('No')
  );
  $rows[] = array(
    array('data' => t('Description'), 'header' => true), 
    $inventory->description
  );
  
  $ownerId = $inventory->uid;
  $render_array['details'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Inventory details'), 
    '#weight' => 1, 
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
  $render_array['details']['table'] = array(
    '#theme' => 'table',
    '#rows' => $rows,
    '#attributes' => array(
        'class' => array('inventory_show'),
      ),
  );
  
  $rows = array();
  $rows[] = array(
    array('data' => t('Area'), 'header' => true), 
    l($inventory->area_name, 'area/'.$inventory->area_id)
  );
  $rows[] = array(
    array('data' => t('City'), 'header' => true), 
    $inventory->area_locality
  );
  $rows[] = array(
    array('data' => t('ZIP'), 'header' => true), 
    $inventory->area_zip
  );
  $rows[] = array(
    array('data' => t('Area creator'), 'header' => true), 
    $area->owner
  );
  
  $render_array['areas'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Area details'), 
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
  $render_array['areas']['table'] = array(
    '#theme' => 'table',
    '#rows' => $rows,
    '#attributes' => array(
      'class' => array('inventory_show'),
    ),
  );
  $rows = '';
  $row = '';

  return $render_array;
}

/**
 * Gets organism table table header
 * @param unknown_type $inventoryId
 */
function getInventoryColumnDescription($inventoryId){
  $invColDesc = array();

  $resInventory = db_query_range("
    SELECT t.name, i.inventory_type_id 
    FROM inventory i 
    LEFT JOIN inventory_type t ON i.inventory_type_id = t.id 
    WHERE i.id = :id", 0, 1, 
    array(':id' => $inventoryId)
  );
  foreach ($resInventory as $inventory) {
    $typeId = $inventory->inventory_type_id;
    $invColDesc["name"] = $inventory->name;
    $invColDesc["id"] = $inventory->inventory_type_id;
  }

  $resAttributes = db_query('SELECT a.id, a.name, f.format
    FROM inventory_type_attribute a 
    LEFT JOIN attribute_format f ON a.attribute_format_id = f.id 
    WHERE inventory_type_id = :typeId', array(':typeId' => $typeId));

  $i = 0;
  foreach ($resAttributes as $attribute) {
    $i++;
    $invColDesc["cols"][$i] = array(
      "name" => $attribute->name,
      "id" => $attribute->id,
      "format" => $attribute->format,
    );

    // If it is a dropdown list
    if($attribute->format == "dropdown") {
      $invColDesc["cols"][$i]["dropdown_values"] = array();
      $dropdownValues = db_query('SELECT * FROM inventory_type_attribute_dropdown_value WHERE inventory_type_attribute_id = :id', array(':id' => $attribute->id));
      foreach($dropdownValues as $dropdownValue) {
        array_push($invColDesc["cols"][$i]["dropdown_values"], array("id" =>$dropdownValue->id, "value" => $dropdownValue->value));
      }
    }

  }
  return $invColDesc;
}

function getInventoriesData($head_inventory_id, $order='e.position, e.id ASC') {
  $invArr = array();
  $inventories = db_query('SELECT id
              FROM inventory 
              WHERE head_inventory_id = :id;', 
              array(':id' => $head_inventory_id)
              );
  
  $protected = inventory_redlisted_entries($head_inventory_id);
  
  foreach($inventories as $inventory) {
    $inventory_id = $inventory->id;
    $invArr[$inventory_id] = array();
    
    // Get the inventory column description
    $invArr[$inventory_id]["invDesc"] = getInventoryColumnDescription($inventory_id);
    
    $sql = 'SELECT e.id, e.organism_id, fauna.name_de faunanamede, fauna.genus, fauna.species, fauna.family, o.organism_type, flora.name_de floranamede, flora."Gattung" floragattung, flora."Art" floraart, flora."Familie" florafamilie
      FROM inventory_entry e
      LEFT JOIN organism o on e.organism_id = o.id
      LEFT JOIN fauna_organism fauna ON o.organism_id = fauna.id
      LEFT JOIN flora_organism flora ON o.organism_id = flora.id
      WHERE e.inventory_id = :id '.
      ($protected?'AND NOT e.id IN ('.implode(', ', $protected).')':'').
      'ORDER BY '.$order.';';
    
    $inventoryEntries = db_query($sql ,array(':id' => $inventory_id));

    foreach($inventoryEntries as $inventoryEntry) {
      $inventoryEntryId = $inventoryEntry->id;
      $entry = array(
        'id' => $inventoryEntryId,
        'orgId' => $inventoryEntry->organism_id
      );

      if($inventoryEntry->organism_type==1){
        // organism_type 1 is Fauna
        $entry["label"] = $inventoryEntry->faunanamede;
        $entry["label_latin"] = $inventoryEntry->genus." ".$inventoryEntry->species;
        $entry["label_family"] = $inventoryEntry->family;
      } else {
        // organism_type 2 is Flora
        $entry["label"] = $inventoryEntry->floranamede;
        $entry["label_latin"] = $inventoryEntry->floragattung." ".$inventoryEntry->floraart;
        $entry["label_family"] = $inventoryEntry->florafamilie;
      }

      // Get inventory columns
      $columns = db_query('
        SELECT * 
        FROM inventory_type_attribute_inventory_entry 
        WHERE inventory_entry_id = :id', array(':id' => $inventoryEntryId));
      foreach ($columns as $column) {
        // Dropdown
        $colVal = "";
        if($column->inventory_type_attribute_dropdown_value_id > 0) {
          $colVal = $column->inventory_type_attribute_dropdown_value_id.'_dropdown';
        } else {
          $colVal = $column->value;
        }
        $entry["col_".$column->inventory_type_attribute_id] = isset($colVal) ? $colVal : "";
      }
      $invArr[$inventory_id][] = $entry;
    }
  }
  return $invArr;
}

/**
 * 
 * Returns form containing inventory organisms
 * @param unknown_type $head_inventory_id
 */
function getInventories($head_inventory_id) {
  $output = array();
  $invArr = getInventoriesData($head_inventory_id);
  
  if(function_exists('gallery_edit_link'))
    $gallery_available = true;
  
  $weight = 4;
  foreach($invArr as $key => $inventory_group) {
    $rows = array();
    $header = array();
    $colIds = array();
    $colId = '';
    $invDetail = '';
    $has_gallery = false;
    
    
    $invGroupName = $inventory_group['invDesc']['name'];
    $header[] = t('Name');
    $header[] = t('Latin name');
    foreach($inventory_group['invDesc']['cols'] as $invDetail) {
      $header[] = $invDetail['name'];
      $colIds[] = $invDetail['id'];
    }
    foreach($inventory_group as $key => $invDetails) {
      if(is_string($key) && $key == 'invDesc')
        continue;
      $row = array();
      $row[] = l($invDetails['label'], 'organism/'.$invDetails['orgId'], array('html'=>true,'attributes'=>array('id'=>'organism_'.$invDetails['orgId'])));
      $row[] = l($invDetails['label_latin'], 'organism/'.$invDetails['orgId'], array('html'=>true,'attributes'=>array('id'=>'organism_'.$invDetails['orgId'])));
      foreach($colIds as $colId) {
        $colValueDropdown = array (explode('_', $invDetails['col_'.$colId]));
        if (isset($colValueDropdown[0][1])) {
          $row[] = getInventoryEntryValue($colValueDropdown[0][0]);
        }
        else {
          $row[] = $invDetails['col_'.$colId];
        }
        $colValueDropdown = NULL; 
      }
      if($gallery_available) {
        $row[] = gallery_edit_link('inventory_entry', $invDetails['id']);
        if($row[(count($row)-1)])
          $has_gallery = true;
      }
      $rows[] = array('data' => $row, 'id' => 'organism_'. $invDetails['orgId']);
    }
    if($has_gallery)
      $header[] = '&nbsp;';
    if($rows[0] != '') {
      $output[$invGroupName] = array(
        '#type' => 'fieldset', 
        '#title' => $invGroupName, 
        '#weight' => $weight++, 
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
    
      $output[$invGroupName]['Details'] = array(
        '#theme' => 'datatable', 
        '#header' => $header,
        '#rows' => $rows,
        '#id_table' => $invGroupName, 
      );
    }
    
  }
  if($output)
    krsort($output);
  return $output;
}

/**
 * Gets inventory attribute values for dropdown fields
 * @param unknown_type $valueId
 */
function getInventoryEntryValue($valueId) {
  $result = db_query('
    SELECT * 
    FROM inventory_type_attribute_dropdown_value itadv
    WHERE itadv.id = :id',
    array(':id' => $valueId)
  );
  $value = $result->fetch()->value;
  
  return $value;
}

function getRedListCount($head_inventory_id) {
  $count = db_query('
    SELECT COUNT (o.organism_id)
    FROM head_inventory hi
    LEFT JOIN inventory i ON hi.id = i.head_inventory_id
    LEFT JOIN inventory_entry e ON i.id = e.inventory_id
    LEFT JOIN organism o on e.organism_id = o.id
    LEFT JOIN fauna_organism fauna ON o.organism_id = fauna.id
    LEFT JOIN flora_organism flora ON o.organism_id = flora.id
    LEFT JOIN flora_red_list flora_rl ON flora_rl.flora_organism_id = flora.id AND o.organism_type = 2
    WHERE hi.id = :hiid
    AND (
      ((fauna.protection_ch IS NOT NULL AND fauna.protection_ch != 0) AND o.organism_type = 1)
    OR (
      (flora_rl.red_list_ch IS NOT NULL AND flora_rl.red_list_ch != 0)
      AND o.organism_type = 2
    ))',
    array(':hiid' => $head_inventory_id)
  );
  
  $count = $count->fetch()->count;
  if($count != '0') {
    $render_array['protection'] = array(
      '#type' => 'fieldset', 
      '#title' => t('Protection information'), 
      '#weight' => 50, 
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
    $owner = db_select('head_inventory', 'hi')->condition('hi.id', $head_inventory_id)->fields('hi', array('owner_id'))->execute()->fetch()->owner_id;
    if($count == '1') {
      $markup = t('This inventory contains a protected organism which is hidden due to your inventory permissions. If you need more details please !contact.', array(
        '!contact' => l(t('contact the observer'), 'user/'.$owner.'/contact', array('html' => true))
      ));
    }
    else {
      $markup = t('This inventory contains !count protected organisms which are hidden due to your inventory permissions. If you need more details please !contact.', array(
        '!count' => $count, 
        '!contact' => l(t('contact the observer'), 'user/'.$owner.'/contact', array('html' => true))
      ));
    }
    $render_array['protection']['status'] = array(
      '#markup' => $markup,
    );
    return $render_array;
  }
}

function getInventoryFiles($head_inventory_id) {
  $render_array = array();
  $result = db_query('
    SELECT * FROM head_inventory_file_managed hifm
    LEFT JOIN file_managed fm ON hifm.file_id = fm.fid
    WHERE head_inventory_id = :hiid',
    array(':hiid' => $head_inventory_id)
  );
  
  foreach($result as $record) {
    $mimeIconMap = array(
      'path' => drupal_get_path('module', 'inventory').'/images/' . getInventoryIconMap($record->filemime).'.png',
      'alt' => $record->filemime,
      'title' => $record->filemime,
    );
    $img_mime = theme('image', $mimeIconMap);
    
    $description = $record->description;
    $filename = l($record->filename, 'sites/default/files/swissmon/inventory/'.$head_inventory_id.'/'.$record->filename);
    $filemime = $img_mime;
    $filesize = ($record->filesize / 1000)." KB";
    $timestamp = date('d.m.Y',$record->timestamp);
    $row = array();
    $row[] = $description;
    $row[] = $filename;
    $row[] = $filemime;
    $row[] = $filesize;
    $row[] = $timestamp;
    $rows[] = $row;
  }
  
  if(empty($rows))
    return $render_array;
  
  $tableHeader[] = t('Description');
  $tableHeader[] = t('Filename');
  $tableHeader[] = t('Typ');
  $tableHeader[] = t('Size');
  $tableHeader[] = t('Date');
  
  $render_array['files'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Files'), 
    '#weight' => '20', 
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
  
  $render_array['files']['data'] = array(
    '#theme' => 'datatable', 
    '#header' => $tableHeader,
    '#rows' => $rows,
    '#id_table' => 'files', 
  );
  return $render_array;
}
function getInventoryIconMap($filemime) {

  switch ($filemime) {
    // Word document types.
    case 'application/msword':
    case 'application/vnd.ms-word.document.macroEnabled.12':
    case 'application/vnd.oasis.opendocument.text':
    case 'application/vnd.oasis.opendocument.text-template':
    case 'application/vnd.oasis.opendocument.text-master':
    case 'application/vnd.oasis.opendocument.text-web':
    case 'application/vnd.openxmlformats-officedocument.wordprocessingml.document':
    case 'application/vnd.stardivision.writer':
    case 'application/vnd.sun.xml.writer':
    case 'application/vnd.sun.xml.writer.template':
    case 'application/vnd.sun.xml.writer.global':
    case 'application/vnd.wordperfect':
    case 'application/x-abiword':
    case 'application/x-applix-word':
    case 'application/x-kword':
    case 'application/x-kword-crypt':
      return 'x-office-document';

      // Spreadsheet document types.
    case 'application/vnd.ms-excel':
    case 'application/vnd.ms-excel.sheet.macroEnabled.12':
    case 'application/vnd.oasis.opendocument.spreadsheet':
    case 'application/vnd.oasis.opendocument.spreadsheet-template':
    case 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet':
    case 'application/vnd.stardivision.calc':
    case 'application/vnd.sun.xml.calc':
    case 'application/vnd.sun.xml.calc.template':
    case 'application/vnd.lotus-1-2-3':
    case 'application/x-applix-spreadsheet':
    case 'application/x-gnumeric':
    case 'application/x-kspread':
    case 'application/x-kspread-crypt':
      return 'x-office-spreadsheet';

      // Presentation document types.
    case 'application/vnd.ms-powerpoint':
    case 'application/vnd.ms-powerpoint.presentation.macroEnabled.12':
    case 'application/vnd.oasis.opendocument.presentation':
    case 'application/vnd.oasis.opendocument.presentation-template':
    case 'application/vnd.openxmlformats-officedocument.presentationml.presentation':
    case 'application/vnd.stardivision.impress':
    case 'application/vnd.sun.xml.impress':
    case 'application/vnd.sun.xml.impress.template':
    case 'application/x-kpresenter':
      return 'x-office-presentation';

      // Compressed archive types.
    case 'application/zip':
    case 'application/x-zip':
    case 'application/stuffit':
    case 'application/x-stuffit':
    case 'application/x-7z-compressed':
    case 'application/x-ace':
    case 'application/x-arj':
    case 'application/x-bzip':
    case 'application/x-bzip-compressed-tar':
    case 'application/x-compress':
    case 'application/x-compressed-tar':
    case 'application/x-cpio-compressed':
    case 'application/x-deb':
    case 'application/x-gzip':
    case 'application/x-java-archive':
    case 'application/x-lha':
    case 'application/x-lhz':
    case 'application/x-lzop':
    case 'application/x-rar':
    case 'application/x-rpm':
    case 'application/x-tzo':
    case 'application/x-tar':
    case 'application/x-tarz':
    case 'application/x-tgz':
      return 'package-x-generic';

      // Script file types.
    case 'application/ecmascript':
    case 'application/javascript':
    case 'application/mathematica':
    case 'application/vnd.mozilla.xul+xml':
    case 'application/x-asp':
    case 'application/x-awk':
    case 'application/x-cgi':
    case 'application/x-csh':
    case 'application/x-m4':
    case 'application/x-perl':
    case 'application/x-php':
    case 'application/x-ruby':
    case 'application/x-shellscript':
    case 'text/vnd.wap.wmlscript':
    case 'text/x-emacs-lisp':
    case 'text/x-haskell':
    case 'text/x-literate-haskell':
    case 'text/x-lua':
    case 'text/x-makefile':
    case 'text/x-matlab':
    case 'text/x-python':
    case 'text/x-sql':
    case 'text/x-tcl':
      return 'text-x-script';

      // HTML aliases.
    case 'application/xhtml+xml':
      return 'text-html';

      // Executable types.
    case 'application/x-macbinary':
    case 'application/x-ms-dos-executable':
    case 'application/x-pef-executable':
      return 'application-x-executable';

      // PDF
    case 'application/pdf':
      return 'application-pdf';

    default:
      return $return;
  }
}
?>