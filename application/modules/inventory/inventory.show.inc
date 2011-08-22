<?php

/**
 * Menu callback; View of an inventory
 * 
 * @param int $head_inventory_id
 *  The ID of the inventory
 * @return array
 *  Drupal render array
 */
function inventory_show($head_inventory_id, $redirect=true) {
  if($redirect) {
    $hidden = db_select('head_inventory', 'hi')
      ->condition('hi.id', $head_inventory_id)
      ->fields('hi', array('hidden', 'owner_id'))
      ->execute()
      ->fetch();
    if($hidden->hidden) {
      global $user;
      if($user->uid == $hidden->owner_id)
        return drupal_goto('singleobservations');
      else
        return drupal_goto('user/'.$hidden->owner_id.'/singleobservations');
    }
  }
  _include_js_and_css(false);
  area_add_gmap_resources(false);
  drupal_set_title(inventory_title($head_inventory_id)." (".$head_inventory_id.")");
  
  $render_array = inventory_basics($head_inventory_id);
  
  if(inventory_user_has_redlist_permission($head_inventory_id) == FALSE)
    $render_array = array_merge($render_array, inventory_redlist_count($head_inventory_id));
  
  if(function_exists('gallery_list_renderer')){
    $render_array['GalleryImages'] = gallery_list_renderer('images','head_inventory', $head_inventory_id, false, 100);
    $render_array['GallerySounds'] = gallery_list_renderer('sounds','head_inventory', $head_inventory_id, false, 101,true);
    $render_array['GalleryVideos'] = gallery_list_renderer('videos','head_inventory', $head_inventory_id, false, 102,true);
    
  }
  
  $render_array = array_merge($render_array, inventory_inventories($head_inventory_id), inventory_files($head_inventory_id));

  return $render_array;
}

/**
 * Menu callback; Renders the KML for displaying all located observations of an inventory
 * in google maps
 * 
 * @param int $head_inventory_id
 *  The ID of the inventory
 * @return
 */
function inventory_entry_locations($head_inventory_id) {
  $xml = '';
  $data = inventory_data($head_inventory_id, false, true, false, false, false);
  $entries = array();
  foreach($data as $group) {
    foreach($group as $k => $entry) {
      if(is_string($k) || !$entry['location'])
        continue;
      $location = explode(',', $entry['location']);
      $lat = floatval($location[0]);
      $lng = floatval($location[1]);
      $image = false;
      if(function_exists('gallery_images')) {
        $images = gallery_images('organism', $entry['orgId'], true, false, 1);
        if($images['count'])
          $image = $images['images'][0]['id'];
      }
      $xml .= '
    <Placemark>
      <name><![CDATA[]]></name>
      <description><![CDATA[
        <a href="'.url('organism/'.$entry['orgId']).'">
          <img style="float:left; margin:0 1em 0 0;" src="'.url('gallery/organism/'.$entry['orgId'].'/thumb/'.$image.'/inventory_mini').'" />
          <big>'.utf8_decode($entry['label']).'</big>
          <br />
          <em>'.utf8_decode($entry['label_latin']).'</em>
        </a>
        <span style="clear:both"></span>
        
      ]]></description>
      <Point>
        <extrude>1</extrude>
        <altitudeMode>relativeToGround</altitudeMode>
        <coordinates>'.$lng.','.$lat.',0</coordinates>
      </Point>
    </Placemark>';
    }
  }
  if(!$xml)
    return drupal_exit();
  
  $xml = '<?xml version="1.0" encoding="UTF-8"?>
<kml xmlns="http://www.opengis.net/kml/2.2">
  <Document>
    <name>'.t('Observations').'</name>
    '.$xml.'
  </Document>
</kml>';
  drupal_add_http_header('Content-Length', strlen($xml));
  drupal_add_http_header('Content-Type', 'text/xml');
  echo $xml;
  drupal_exit();
}

/**
 * Menu callback; View of the observation location of an inventory entry
 * 
 * @param int $head_inventory_id
 * @param int $entry_id
 * @return array
 *  Drupal render array
 */
function inventory_entry_location($head_inventory_id, $entry_id) {
  $ajax = $_REQUEST['ajax'];
  $render_array['map'] = drupal_get_form('inventory_entry_map', $head_inventory_id, $entry_id);
  if($ajax)
    return drupal_json_output(array(
      'form' => drupal_render($render_array),
      'title' => t('Observation location')
    ));
  return $render_array;
}

/**
 * Menu callback; Gallery view of an inventory
 * 
 * @param object $head_inventory_id
 * @return array
 *  Drupal render array
 */
function inventory_gallery($head_inventory_id) {
  return _inventory_presentation($head_inventory_id);
}

/**
 * Menu callback; List view of an inventory
 * 
 * @param object $head_inventory_id
 * @return array
 *  Drupal render array
 */
function inventory_list($head_inventory_id) {
  return _inventory_presentation($head_inventory_id, 'gallery_presentation_list', 'presentation-list', 1000000);
}

/**
 * Private helper function to render an inventory in presentation mode
 *  used by inventory_gallery and inventory_list
 * 
 * @param object $head_inventory_id
 *  The ID of the inventory
 * @param object $size[optional]
 *  The image style to be used for the images
 * @param object $cssClass[optional]
 *  Additional CSS class to be added to the container
 * @param object $columns[optional]
 *  The number of columns used to determine the CSS classes of the items
 * @return array
 *  Drupal render array
 */
function _inventory_presentation($head_inventory_id, $size='gallery_presentation', $cssClass=false, $columns=GALLERY_COLUMNS) {
  if(!function_exists('gallery_presentation_renderer'))
    return;
  drupal_add_css(drupal_get_path('module', 'inventory') . '/css/main.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_set_title(inventory_title($head_inventory_id)." (".$head_inventory_id.")");
  
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
  $result = inventory_data_gallery_list($head_inventory_id,$sort);
  //print_r($result);
  $weight = 1;
  $html  = "";	
  $invGroupName = "title";
  
  foreach($result as $record){

  	$items = array();
  	foreach($record as $infos){
  		
  	   $html = "";
  	   $invGroupName = $infos['basic_infos']['inventory_type_name'];
  	   if($size != 'gallery_presentation_list'){

	        $item = array(
	          'content' => array(
	          'label_family' => array('#markup' => '<h3>'.$infos['basic_infos']['family'].'</h3>'),
	          'label_latin' => array('#markup' => '<h4>'.l($infos['basic_infos']['genus'].' '.$infos['basic_infos']['species'], 
	          											 'organism/'.$infos['basic_infos']['organism_id']).'</h4>'),
	          'label' => array('#markup' => '<h5>'.l($infos['basic_infos']['name_de'], 'organism/'.$infos['basic_infos']['organism_id']).'</h5>')));
      }
      else{
      	$html .= '<div class="image_attributes">';
      	$html .= '<div class="family_title">'.$infos['basic_infos']['family'].'</div>';
      	$html .= '<div class="attributes_value">'.l($infos['basic_infos']['genus'].' '.$infos['basic_infos']['species'], 
      												'organism/'.$infos['basic_infos']['organism_id']).'<br>'.l($infos['basic_infos']['name_de'], 
      												'organism/'.$infos['basic_infos']['organism_id']).'</div>';
      	$html .= '</div>';
      	foreach($infos['fields'] as $fieldname){
      		
      	    $html .= '<div class="image_attributes">';
      	  	$html .= '<div class="attributes_name">'.$fieldname.'</div>';
      	  	if($infos['field_values'][$fieldname]=="")
      	  		$value =t("no data");
      	  	else
      	  		$value = $infos['field_values'][$fieldname];
      	  	$html .= '<div class="attributes_value">'.$value.'</div><div>&nbsp;</div>';
      	  	$html .= '</div>';
      	}
 
  	$item = array('content' => array(
                  'label' => array(
                  '#markup' => '<div class="gallery_listing">'.$html.'</div>')));

    }

      switch($source) {
        case 'best':
          $item['type'] = 'organism';
          $item['id'] = $infos['basic_infos']['organism_id'];
          break;
        case 'inventory':
          $item['type'] = 'inventory_entry';
          $item['id'] = $infos['basic_infos']['inventory_entry_id']; 
          break;
        case 'presentation':
          $item['type'] = 'inventory_entry';
          $item['id'] = $infos['basic_infos']['inventory_entry_id']; 
          $item['subtype'] = 'presentation';
          break;
        default:
          $item['type'] = 'inventory_entry';
          $item['id'] = $infos['basic_infos']['inventory_entry_id']; 
          $item['alternatives'] = array(
            array(
              'type' => 'organism',
              'id' => $infos['basic_infos']['organism_id']
            )
          );
          break;
      }
      array_push($items, $item);

    }
    if($items) {
      //print_r($items);
      $render_array[$invGroupName] = array('#weight' => $weight++);
      $render_array[$invGroupName]['title'] = array('#markup' => '<h2>'.$invGroupName.'</h2>');
      $render_array[$invGroupName]['galleryImages'] = gallery_presentation_renderer('images',$items, $size, $cssClass, $columns, (!$source || $source == 'default' ? t('Evidence image') : false));
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
 * Basic information about an inventory
 * 
 * @param int $head_inventory_id
 *  The ID of the inventory
 * @return array
 *  Drupal render array
 */
function inventory_basics($head_inventory_id) {
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
  $owner = inventory_username($inventory->uid);
  $rows = array();
  $rows[] = array(
    array('data' => t('Observer'), 'header' => true), 
    l($owner, 'user/'.$inventory->uid.'/contact')
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
    '#weight' => 2, 
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
      'id' => 'inventory-details'
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
  
  $render_array['area'] = array(
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
      'id' => 'inventory-area'
    ),
  );
  $render_array['area']['table'] = array(
    '#theme' => 'table',
    '#rows' => $rows,
    '#attributes' => array(
      'class' => array('inventory_show'),
    ),
  );
  
  $render_array['map'] = array(
    '#type' => 'fieldset',
    '#title' => t('Map'),
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
      'id' => 'inventory-map'
    ),
  );
  
  $render_array['map']['map'] = area_map($inventory->area_id, 'map_canvas', ' data-layers="'.url('inventory/'.$head_inventory_id.'/entry_locations').'"');

  return $render_array;
}

/**
 * Information about red listed organisms in an inventory
 * 
 * @param int $head_inventory_id
 *  The ID of the inventory
 * @return array
 *  Drupal render array
 */
function inventory_redlist_count($head_inventory_id) {
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
        'class' => array('collapsible', 'inventory-protection'),
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

/**
 * The organisms contained in this inventory grouped by type
 * 
 * @param int $head_inventory_id
 *  The ID of the inventory
 * @return array
 *  Drupal render array
 */
function inventory_inventories($head_inventory_id, $invArr=false, $gallery=true) {
  drupal_add_js(drupal_get_path('module', 'inventory') . '/js/inventory.datatable.select.js');
  $img_localize = theme('image', array(
    'path' => 'misc/fatcow_icons/16x16/location_pin.png',
    'alt' => t('Observation location'),
    'title' => t('Observation location'),
  ));
  $output = array();
  if(!$invArr)
    $invArr = inventory_data($head_inventory_id, false, false, false, false, true, true);
  
  $gallery_available = $gallery && function_exists('gallery_view_link');
  
  $weight = 4;
  foreach($invArr as $key => $inventory_group) {
    $rows = array();
    $header = array();
    $colIds = array();
    $colId = '';
    $invDetail = '';
    $has_location = false;
    $has_gallery = false;
    
    $invGroupName = $inventory_group['invDesc']['name'];
    $header['name'] = t('Name');
    $header['name_latin'] = t('Latin name');
    $header['family'] = t('Family');
    foreach($inventory_group['invDesc']['cols'] as $invDetail) {
      $header['col'.$invDetail['id']] = $invDetail['name'];
      $colIds[] = $invDetail['id'];
    }
    foreach($inventory_group as $k => $invDetails) {
      if(is_string($k) && $k == 'invDesc')
        continue;
      $row = array();
      $row['name'] = l($invDetails['label'], 'organism/'.$invDetails['orgId'], array('html'=>true,'attributes'=>array('id'=>'organism_'.$invDetails['orgId'])));
      $row['name_latin'] = l($invDetails['label_latin'], 'organism/'.$invDetails['orgId'], array('html'=>true,'attributes'=>array('id'=>'organism_'.$invDetails['orgId'])));
      $row['family'] = l($invDetails['label_family'], 'organism/'.$invDetails['orgId'], array('html'=>true,'attributes'=>array('id'=>'organism_'.$invDetails['orgId'])));
      foreach($colIds as $colId) {
        $colValueDropdown = explode('_', array_key_exists('col_'.$colId, $invDetails) ? $invDetails['col_'.$colId] : '');
        if (isset($colValueDropdown[1])) {
          if($header['col0'] == 'Protection level') {
          	if(!empty($colValueDropdown[0])) {
          	  $row['col'.$colId] = _inventory_protection_entry_value($colValueDropdown[0]);
          	} else {
          		$row['col'.$colId] = t('');
          	}
          } else {
            $row['col'.$colId] = _inventory_entry_value($colValueDropdown[0]);
          }
        }
        else
          $row['col'.$colId] = array_key_exists('col_'.$colId, $invDetails) ? $invDetails['col_'.$colId] : '';
        $colValueDropdown = NULL; 
      }
      if(array_key_exists('location', $invDetails) && $invDetails['location']) {
        $row['location'] = l($img_localize, 'inventory/'.$head_inventory_id.'/entry_location/'.$invDetails['id'], array('html' => true, 'attributes' => array('class' => array('location'))));
        $has_location = true;
      } else
        if($header['col0'] != 'Protection level') {
          $row['location'] = '';
        }
        
      if($gallery_available) {
        $row['gallery'] = gallery_view_link('inventory_entry', $invDetails['id'],true);
        if($row['gallery'])
          $has_gallery = true;
      }
      $rows[] = array('data' => $row, 'id' => 'organism_'. $invDetails['orgId']);
    }
    if($has_gallery)
      $header['gallery'] = '&nbsp;';
    else if($gallery_available)
      array_walk($rows, create_function('&$row', 'unset($row[\'data\'][\'gallery\']);'));
    if($has_location)
      $header['location'] = '&nbsp;';
    else if($gallery_available)
      array_walk($rows, create_function('&$row', 'unset($row[\'data\'][\'location\']);'));
    if(!empty($rows)) {
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
          'class' => array('collapsible', 'invgroup'),
        ),
      );
    
      $output[$invGroupName]['Details'] = array(
        '#theme' => 'datatable', 
        '#header' => $header,
        '#rows' => $rows,
        '#id_table' => 'invgroup'.$key
      );
    }
    
  }
  if($output)
    krsort($output);
  return $output;
}

/**
 * Private helper function to get inventory attribute values for dropdown fields
 * 
 * @param int $value_id
 * @return string
 */
function _inventory_entry_value($value_id) {
  $result = db_query('
    SELECT * 
    FROM inventory_type_attribute_dropdown_value itadv
    WHERE itadv.id = :id',
    array(':id' => $value_id)
  );
  $value = $result->fetch()->value;
  
  return $value;
}

function _inventory_protection_entry_value($entry_id) {
	$result = db_query('
	SELECT *
	FROM inventory_custom_protection_protection_level
	WHERE id = :id',
	array(':id' => $entry_id)
	);
	
	$result = $result->fetch();
	$value = $result->description.' ('.$result->protection_radius.'x'.$result->protection_radius.' km)';
	return $value;
}

/**
 * The files attached to an inventory
 * 
 * @param int $head_inventory_id
 *  The ID of the inventory
 * @return array
 *  Drupal render array
 */
function inventory_files($head_inventory_id) {
  $render_array = array();
  $result = db_query('
    SELECT * FROM head_inventory_file_managed hifm
    LEFT JOIN file_managed fm ON hifm.file_id = fm.fid
    WHERE head_inventory_id = :hiid',
    array(':hiid' => $head_inventory_id)
  );
  
  foreach($result as $record) {
    $mimeIconMap = array(
      'path' => drupal_get_path('module', 'inventory').'/images/' . inventory_icon_map($record->filemime).'.png',
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

?>
