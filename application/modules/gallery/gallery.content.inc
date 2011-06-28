<?php

/**
 * @file
 * Generic gallery pages.
 */

define('GALLERY_COLUMNS', 5);
define('GALLERY_IMAGE_COUNT', 15);

/**
 * Menu callback; Listing of all images for a given context.
 */
function gallery_list($type, $id, $backlink=true) {
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  $page = array();
  $editable = gallery_edit_access_callback($type, $id);
  $params = _gallery_list_parameters($type, $id);
  $images = gallery_images($type, $id, $params['crosslinked'], true, $params['show_all'] ? false : GALLERY_IMAGE_COUNT, $params['subtype']);
  $links = _gallery_list_links($params, $images['count']);
  if($images['count']) {
    $page['anchor'] = array(
      '#markup' => '<a name="'.$params['key'].'"></a>'
    );
    $page['gallery_list'] = array(
      '#theme' => 'gallery_list',
      '#images' => $images['images'],
      '#rating_types' => gallery_available_rating_types()
    );
  }
  else if($editable) {
    $page['gallery_list'] = array(
      '#markup' => '<p>'.t('There are no images available yet.').'</p>',
    );
  }
  if($editable)
    array_push($links, array(
      'title' => t('Manage images'),
      'href' => 'gallery/'.$type.'/'.$id.'/edit'
    ));
  if($backlink) {
    $back = _gallery_back_link($type, $id);
    if($back)
      array_push($links, $back);
  }
  if($links)
    $page['actions'] = array(
      '#theme' => 'links',
      '#links' => $links,
      '#attributes' => array('class' => 'gallery-action-links'),
      '#weight' => 9
    );
  
  return $page;
}

/**
 * Renders the images of a given object type and id
 * 
 * @return 
 * @param object $type
 * @param object $id
 * @param object $title[optional]
 * @param object $weight[optional]
 * @param object $collapsed[optional]
 */
function gallery_list_renderer($type, $id, $title=false, $weight=10, $collapsed=false) {
  if(!gallery_access_callback($type))
    return '';
  $gallery = gallery_list($type, $id, false);
  if(!$gallery['gallery_list'])
    return '';
  return _gallery_list_fieldset($gallery, $title, $weight, $collapsed && !array_key_exists(md5($type.$id).'_show_all', $_GET) && !array_key_exists(md5($type.$id).'_crosslinked', $_GET));
}

/**
 * Renders the images of a given object type and multiple ids
 * 
 * @return 
 * @param object $type
 * @param object $ids
 * @param object $title[optional]
 * @param object $weight[optional]
 * @param object $collapsed[optional]
 */
function gallery_multiple_list_renderer($type, $ids, $title=false, $weight=10, $collapsed=false) {
  if(!gallery_access_callback($type))
    return '';
  $params = _gallery_list_parameters($type, $id);
  $images = gallery_images($type, $ids, $params['crosslinked'], true, $params['show_all'] ? false : GALLERY_IMAGE_COUNT, $params['subtype']);
  $links = _gallery_list_links($params, $images['count']);
  if($images['count']) {
    $page = _gallery_list_fieldset(array(
      'anchor' => array(
        '#markup' => '<a name="'.$params['key'].'"></a>'
      ),
      'list' => array(
        '#theme' => 'gallery_list',
        '#images' => $images['images'],
        '#rating_types' => gallery_available_rating_types()
      )
    ), $title, $weight, $collapsed && !array_key_exists($params['key'].'_show_all', $_GET) && 
                                      !array_key_exists($params['key'].'_crosslinked', $_GET) && 
                                      !array_key_exists($params['key'].'_subtype', $_GET));
    if(isset($info))
      $page['gallery_info'] = $info;
    if($links)
      $page['actions'] = array(
        '#theme' => 'links',
        '#links' => $links,
        '#attributes' => array('class' => 'gallery-action-links'),
        '#weight' => 9
      );
    return $page;
  }
  return '';
}

function _gallery_list_parameters($type, $id) {
  $params = array();
  $types = _gallery_types();
  $key = md5($type.(is_array($id) ? implode('.', $id) : $id));
  if(array_key_exists($key.'_show_all', $_GET))
    $_SESSION[$key.'_show_all'] = $_GET[$key.'_show_all'] ? true : false;
  $params['show_all'] = array_key_exists($key.'_show_all', $_SESSION) ? $_SESSION[$key.'_show_all'] : false;
  $params['crosslinking'] = array_key_exists('crosslinked', $types[$type]);
  if($params['crosslinking'] && array_key_exists($key.'_crosslinked', $_GET))
    $_SESSION[$key.'_crosslinked'] = $_GET[$key.'_crosslinked'] ? true : false;
  $params['crosslinked'] = array_key_exists($key.'_crosslinked', $_SESSION) ? $_SESSION[$key.'_crosslinked'] : null;
  $params['subtypes'] = _gallery_get_subtypes($type, $crosslinked);
  if($params['subtypes'] && array_key_exists($key.'_subtype', $_GET))
    $_SESSION[$key.'_subtype'] = $_GET[$key.'_subtype'];
  $params['subtype'] = array_key_exists($key.'_subtype', $_SESSION) ? $_SESSION[$key.'_subtype'] : '';
  $params['key'] = $key;
  return $params;
}

function _gallery_list_links($params, $count) {
  $url = drupal_get_destination();
  $url = drupal_parse_url($url['destination']);
  $url = $url['path'];
  $links = array();
  if($count > GALLERY_IMAGE_COUNT) {
    if(!$params['show_all']) {
      $info = array(
        '#markup' => '<p class="info">'.t('Showing first !showing images of !total', array('!showing' => GALLERY_IMAGE_COUNT, '!total' => $count)),
        '#weight' => 10
      );
      array_push($links, array(
        'title' => t('Show all images'),
        'href' => $url,
        'query' => array($params['key'] => '1'),
        'fragment' => $params['key']
      ));
    } else
      array_push($links, array(
        'title' => t('Show top !number', array('!number' => GALLERY_IMAGE_COUNT)),
        'href' => $url,
        'query' => array($params['key'] => ''),
        'fragment' => $params['key']
      ));
  }
  if($params['crosslinking']) {
    if($params['crosslinked'])
      array_push($links, array(
        'title' => t('Hide linked images'),
        'href' => $url,
        'query' => array($params['key'].'_crosslinked' => ''),
        'fragment' => $params['key']
      ));
    else
      array_push($links, array(
        'title' => t('Show linked images'),
        'href' => $url,
        'query' => array($params['key'].'_crosslinked' => '1'),
        'fragment' => $params['key']
      ));
  }
  if($params['subtypes']) {
    $links[(!$params['subtype'] ? 'selected' : count($links))] = array(
      'title' => t('Show all image types'),
      'href' => $url,
      'query' => array($params['key'].'_subtype' => ''),
        'fragment' => $params['key']
    );
    foreach($params['subtypes'] as $value => $label)
      $links[($value == $params['subtype'] ? 'selected' : count($links))] = array(
        'title' => $label,
        'href' => $url,
        'query' => array($params['key'].'_subtype' => $value),
        'fragment' => $params['key']
      );
  }
  return $links;
}

function _gallery_list_fieldset($list, $title=false, $weight=10, $collapsed=false) {
  $output = array(
    '#type' => 'fieldset', 
    '#title' => $title ? $title : t('Images'), 
    '#weight' => $weight, 
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
    '#attached' => array (
      'js' => array (
        'misc/form.js',
        'misc/collapse.js',
      ),
    ),
    '#attributes' => array(
      'class' => array('collapsible gallery'.($collapsed ? ' collapsed' : '')),
    ),
  );
  $output['edit'] = $list;
  return $output;
}

/**
 * Renders the highest rated image of the given items 
 * 
 * @return 
 * @param array $items
 *   Array of arrays having the following keys:
 *   - "type": The type of object to display
 *   - "id": The id of the object to display
 *   - "subtype": [Optional] The subtype of the image to display
 *   - "content": [Optional] drupal render array of the content to be displayed for the given item
 *   - "alternatives": [Optional] List of arrays with the following keys defining an alternative way of getting the image
 *     - "type": The type of object to display
 *     - "id": The id of the object to display
 * @param string $imgsize
 *   The size of the images to be displayed
 * @param string $cssClass
 *   Additional css classes to be set on the container
 * @param int $columns
 *   The number of columns to show the items in
 * @param string $msgPrimary
 *   Additional message to be displayed if an image is found by using the primary source
 * @param string $msgAlternative
 *   Additional message to be displayed if an image is found by using the alternative source
 */
function gallery_presentation_renderer($items, $imgsize='gallery_presentation', $cssClass='', $columns=GALLERY_COLUMNS, $msgPrimary=false, $msgAlternative=false) {
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js', array('weight' => 100));
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js', array('weight' => 110));
  $page = array();
  
  $i = 1;
  $images = array();
  foreach($items as $weight => $item) {
    $image = gallery_images($item['type'], $item['id'], true, false, 1, $item['subtype']);
    $msg = $msgPrimary;
    if(!$image['images'] && $item['alternatives']) {
      $msg = $msgAlternative;
      foreach($item['alternatives'] as $alternative) {
        $image = gallery_images($alternative['type'], $alternative['id'], true, false, 1);
        if($image['images'])
          break;
      }
    }
    $image = $image['count'] ? $image['images'][0] : false;
    $cls = _gallery_get_class($i-1, $columns);
    if(!$image)
      array_push($cls, 'noimage');
    
    $images['image'.$i]['start'] = array('#markup' => '<li class="'.join(' ', $cls).'">');
    if($image) {
      $images['image'.$i]['image'] = array(
        '#markup' => 
          '<a class="image" href="'.url('gallery/'.$image['item_type'].'/'.$image['item_id'].'/thumb/'.$image['id'].'/gallery_large').'">'.
            '<img src="'.url('gallery/'.$image['item_type'].'/'.$image['item_id'].'/thumb/'.$image['id'].'/'.$imgsize).'" alt="'.$image['title'].'" />'.
          '</a>'
      );
    } else {
      $content = t('No image available');
      $ids = $item['id'];
      if(!is_array($ids))
        $ids = array($ids);
      foreach($ids as $id) {
        if(gallery_edit_access_callback($item['type'], $id)) {
          $content .= '<br>'.l(t('Add one'), 'gallery/'.$item['type'].'/'.$id.'/edit');
          break;
        }
      }
      $images['image'.$i]['image'] = array(
        '#markup' => 
          '<span class="image">'.
            $content.
          '</span>'
      );
    }
    if($item['content']) {
      $images['image'.$i]['content_start'] = array('#markup' => '<div class="caption">');
      if($image) {
        $caption = '';
        if($image['author'] || $image['location'])
          $caption .= '<span class="author">&copy; '.$image['author'].($image['location'] ? ', '.$image['location'] : '').'</span>';
        if($msg)
          $caption .= '<small class="msg">'.$msg.'</small>';
        if($caption)
          $images['image'.$i]['caption'] = array('#markup' => '<p class="author-msg">'.$caption.'</p>');
      }
      $images['image'.$i]['content'] = $item['content'];
      $images['image'.$i]['content_end'] = array('#markup' => '</div>');
    }
    $images['image'.$i]['end'] = array('#markup' => '</li>');
    
    $i++;
  }
  if($images) {
    $page['start'] = array('#markup' => '<ul class="gallery presentation clearfix '.$cssClass.'">');
    $page['images'] = $images;
    $page['end'] = array('#markup' => '</ul>');
  }
  return $page;
}

/**
 * Renders an edit link to manage the images of a given object type and id
 * if the user has edit permissions
 * 
 * @return 
 * @param object $type
 * @param object $id
 */
function gallery_edit_link($type, $id) {
  if(!gallery_edit_access_callback($type, $id))
    return false;
  $gallery_img_localize = theme('image', array(
    'path' => drupal_get_path('module', 'gallery').'/images/image.png',
    'alt' => t('Manage images'),
    'title' => t('Manage images'),
  ));
  return l($gallery_img_localize, 'gallery/'.$type.'/'.$id.'/edit', array('html'=>true));
}

/**
 * Menu callback; Serving a gallery image
 */
function gallery_image($type, $id, $image) {
  $uri = explode('://', $image['uri']);
  return image_style_deliver(image_style_load('gallery_large'), $uri[0], $uri[1]);
}


/**
 * Menu callback; Serving a gallery image thumb
 */
function gallery_thumb($type, $id, $image, $size) {
  $uri = explode('://', $image['uri']);
  return image_style_deliver(image_style_load($size), $uri[0], $uri[1]);
}

/**
 * Menu callback; Gallery edit view
 */
function gallery_images_edit($type, $id) {
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.upload.js');
  
  $back = _gallery_back_link($type, $id);
  if($back)
    $page['back'] = array(
      '#theme' => 'links',
      '#links' => array($back),
      '#attributes' => array('class' => 'action-links')
    );
  
  $page['upload'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload new images'),
    '#weight' => 1,
    '#collapsible' => true,
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
  $page['upload']['edit'] = drupal_get_form('gallery_image_form', $type, $id, array(), 1);
  
  $page['edit'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit existing images'),
    '#weight' => 2,
    '#collapsible' => true,
    '#attached' => array (
      'js' => array (
        'misc/form.js',
        'misc/collapse.js',
      ),
    ),
    '#attributes' => array(
      'class' => array('collapsible'.($_GET['saved'] ? '' : ' collapsed')),
    ),
  );
  $page['edit']['edit'] = drupal_get_form('gallery_images_form', $type, $id);
  
  $title = t('Manage images');
  drupal_set_title($title, PASS_THROUGH);
  
  $info = _gallery_get_title_description($type, $id);
  if($info['title'])
    drupal_set_title($info['title'], PASS_THROUGH);
  if($info['description'])
    $page['description'] = array(
      '#markup' => '<p>'.$info['description'].'</p>',
      '#weight' => -1
    );
  
  return $page;
}

function _gallery_get_title_description($type, $id) {
  $types = _gallery_types();
  $type = $types[$type];
  $info = array(
    'title' => false,
    'description' => false
  );
  if($type['manage_title'] && function_exists($type['manage_title']))
    $info['title'] = call_user_func_array($type['manage_title'], array($id));
  if($type['manage_description'] && function_exists($type['manage_description']))
    $info['description'] = call_user_func_array($type['manage_description'], array($id));
  return $info;
}

/**
 * Form builder; Edit gallery images
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $type
 *   The type of the object the images are to be edited.
 * @param $id
 *   The id of the object the images are to be edited
 * @ingroup forms
 * @see gallery_images_form_submit()
 */
function gallery_images_form($form, &$form_state, $type, $id) {
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js');
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js');
  $form_state['gallery_type'] = $type;
  $form_state['gallery_id'] = $id;
  $subtypes = _gallery_get_subtypes($type, false);
  
  $header = array('', t('Filename'), t('Title'));
  if($subtypes)
    $header[] = t('Type');
  $header[] = t('Author');
  $header[] = t('Location');
  $header[] = t('Operations');
  $header[] = t('Delete');

  $images = gallery_images($type, $id, false);
  
  $rows = array();
  foreach($images['images'] as $image) {
    $editable = gallery_edit_access_callback($type, $id, $image);
    $row = array();
    $row[] = '<a rel="lightbox" href="'.url('gallery/'.$type.'/'.$id.'/thumb/'.$image['id'].'/gallery_large').'"><img src="'.url('gallery/'.$type.'/'.$id.'/thumb/'.$image['id'].'/gallery_mini').'" alt="'.$image['title'].'" /></a>';
    $row[] = $image['filename'];
    $row[] = $image['title'];
    if($subtypes) {
      $types = array();
      foreach($image['subtypes'] as $subtype)
        $types[] = $subtypes[$subtype];
      $row[] = implode(', ', $types);
    }
    $row[] = $image['author'];
    $row[] = $image['location'];
    $row[] = $editable ? l(t('edit'), 'gallery/'.$type.'/'.$id.'/edit/'.$image['id']) : '&nbsp;';
    $row[] = $editable ? '<input type="checkbox" name="delete_'.$image['id'].'" value="1" />' : '&nbsp;';
    $rows[] = $row;
  }
  
  if(!$rows)
    $form['empty'] = array(
      '#markup' => '<p>'.t('There are no images available yet.').'</p>',
    );
  else
    $form['overview'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    );
  
  
  if(array_key_exists('overview', $form)) {
    // Show the Override or Submit button for this style.
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['delete_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Delete selected images'),
      '#submit' => array('gallery_images_form_delete_submit'),
    );
  }

  return $form;
}

/**
 * Form builder; Edit image
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $type
 *   The type of the object the images are to be edited.
 * @param $id
 *   The id of the object the images are to be edited
 * @param $image
 *   An image array.
 * @param $i
 *   Integer to mark the fields with
 * @ingroup forms
 * @see gallery_image_form_submit()
 */
function gallery_image_form($form, &$form_state, $type, $id, $image, $i=1) {
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js');
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js');
  if($image['id']) {
    $title = t('Edit %name', array('%name' => ($image['title'] ? $image['title'] : $image['filename'])));
    $form_state['gallery_image_id'] = $image['id'];
  } else
    $title = t('Add new image');
  drupal_set_title($title, PASS_THROUGH);
  
  $subtypes = _gallery_get_subtypes($type, false);
  $form_state['gallery_type'] = $type;
  $form_state['gallery_id'] = $id;
  $form_state['gallery_image'] = $image;
  $form['#tree'] = TRUE;
  
  if($image['fid'])
    $form['thumb'] = array(
      '#markup' => '<a class="user-picture" rel="lightbox" href="'.url('gallery/'.$type.'/'.$id.'/thumb/'.$image['id'].'/gallery_large').'"><img src="'.url('gallery/'.$type.'/'.$id.'/thumb/'.$image['id'].'/gallery_mini').'" alt="'.$image['title'].'" /></a>'
    );
  
  $available = gallery_available_categories($type, $id);
  $categories = array();
  foreach($available as $category) {
    if(!$category['options']) // no options available for this category
      continue;
    $options = array();
    foreach($category['options'] as $option)
      $options[$option['id']] = $option['name'];
    $categories[$category['id']] = array(
      '#type' => $category['multiple'] ? 'checkboxes' : 'select',
      '#title' => $category['name'],
      '#options' => $options,
      '#description' => $category['description'],
      '#required' => $category['required'],
    );
    
    if($category['multiple'])
      $categories[$category['id']]['#default_value'] = array();
    foreach($options as $value => $option) {
      if(is_array($image['categories']) && array_key_exists($value, $image['categories']))
        if($category['multiple'])
          array_push($categories[$category['id']]['#default_value'], $value);
        else {
          $categories[$category['id']]['#default_value'] = $value;
          break;
        }
    }
    $c++;
  }

  $num = 1;
  while(array_key_exists('images_'.($i+$num), $form_state['input']))
    $num++;
  for($n=$i;$n<$i+$num;$n++) {
    $prefix = 'images_'.$n;
    
    $form[$prefix.'_file'] = array(
      '#type' => 'file',
      '#title' => t('Image'),
      '#size' => 48,
      '#element_validate' => array('_gallery_images_form_upload_validate'),
      '#file_required' => false,//!$image['id'],
    );
    
    if($subtypes)
      $form[$prefix.'_subtypes'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Type'),
        '#options' => $subtypes,
        '#default_value' => $image['subtypes'],
        '#required' => false,
      );
    
    $form[$prefix] = array();
    if($categories) {
      $form[$prefix]['categories'] = array(
        '#type' => 'fieldset',
        '#title' => t('Categories'),
        '#weight' => 1,
        '#collapsible' => true,
        '#collapsed' => $image['id'] ? false : true,
        '#attached' => array (
          'js' => array (
            'misc/form.js',
            'misc/collapse.js',
          ),
        ),
        '#attributes' => array(
          'class' => array('categories collapsible'),
        ),
      );
      foreach($categories as $id => $category)
        $form[$prefix]['categories'][$id] = $category;
    }
    
    $form[$prefix]['metadata'] = array(
      '#type' => 'fieldset',
      '#title' => t('Metadata'),
      '#weight' => 2,
      '#collapsible' => true,
      '#collapsed' => $image['id'] ? false : true,
      '#attached' => array (
        'js' => array (
          'misc/form.js',
          'misc/collapse.js',
        ),
      ),
      '#attributes' => array(
        'class' => array('metadata collapsible'),
      ),
    );
    $form[$prefix]['metadata']['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#size' => 100,
      '#maxlength' => 128,
      '#default_value' => $image['title'],
      '#description' => t('The title of the image.'),
      '#required' => FALSE,
    );
    $form[$prefix]['metadata']['description'] = array(
      '#type' => 'textarea',
      '#title' => t('Description'),
      '#default_value' => $image['description'],
      '#description' => t('The description of the image.'),
      '#required' => FALSE,
    );
    $form[$prefix]['metadata']['author'] = array(
      '#type' => 'textfield',
      '#title' => t('Author'),
      '#size' => 100,
      '#maxlength' => 255,
      '#default_value' => $image['author'],
      '#description' => t('The author of the image.'),
      '#required' => FALSE,
    );
    $form[$prefix]['metadata']['location'] = array(
      '#type' => 'textfield',
      '#title' => t('Location'),
      '#size' => 100,
      '#maxlength' => 255,
      '#default_value' => $image['location'],
      '#description' => t('The location the image was taken at.'),
      '#required' => FALSE,
    );
  }
  
  // Show the Override or Submit button for this style.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['upload_submit'] = array(
    '#type' => 'submit',
    '#value' => $image['id'] ? t('Change image') : t('Upload images'),
    '#access' => $editable,
    '#submit' => array('gallery_images_form_upload_submit'),
    '#attributes' => array('data-add' => t('Add another image'), 'data-remove' => t('Remove image'))
  );

  return $form;
}

/**
 * Validator for gallery image files
 */
function _gallery_images_form_upload_validate($element, &$form_state, $form) {
  $name = substr($element['#name'], 6, strlen($element['#name'])-7);
  if(!array_key_exists($name, $_FILES['files']['name']) || !$_FILES['files']['name'][$name])
    if($element['#file_required'])
      return form_error($element, t('!name field is required.', array('!name' => $element['#title'])));
    else
      return;
  if(array_key_exists($name, $_FILES['files']['name']) && $_FILES['files']['name'][$name] && !$_FILES['files']['type'][$name])
      return form_error($element, t('The file in !name is exceeding the maximum allowed size of !size.', array(
        '!name' => $element['#title'],
        '!size' => format_size(parse_size(file_upload_max_size())))));
  if(!in_array($_FILES['files']['type'][$name], array('image/jpeg', 'image/gif', 'image/png')))
    return form_error($element, t('!name requires a file of type jpg, png or gif. (!type)', array('!name' => $element['#title'], '!type' => $_FILES['files']['type'][$name])));
}

/**
 * Submit handler for saving multiple gallery images.
 */
function gallery_images_form_upload_submit($form, &$form_state) {
  global $GALLERY_MAX_SIZE;
  $type = $form_state['gallery_type'];
  $id = $form_state['gallery_id'];
  $orig_image = $form_state['gallery_image'];
  
  $validators = array(
    'file_validate_extensions' => array('jpg jpeg gif png'),
    'file_validate_size' => array(parse_size(file_upload_max_size()), 0),
  );

  $destination = 'public://swissmon/gallery/'.$type.'/'.$id;
  $categories = gallery_available_categories($type, $id);

  $saved = false;
  file_prepare_directory($destination, FILE_CREATE_DIRECTORY);
  foreach($form_state['values'] as $prefix => $values) {
    if(!is_array($values['metadata']) || !array_key_exists('title', $values['metadata']))
      continue;
    $name = $prefix.'_file';
    if(!array_key_exists($name, $_FILES['files']['name']) || !$_FILES['files']['name'][$name])
      continue;
    $image = array(
      'item_type' => $type,
      'item_id' => $id,
      'title' => $values['metadata']['title'],
      'description' => $values['metadata']['description'],
      'author' => $values['metadata']['author'],
      'location' => $values['metadata']['location'],
      'categories' => array(),
      'subtypes' => $form_state['values'][$prefix.'_subtypes']
    );
    if($orig_image['id'])
      $image['id'] = $orig_image['id'];
    foreach($categories as $category) {
      if(!$category['options']) // no options available for this category
        continue;
      if(!$values['categories'][$category['id']])
        continue;
      $options = $values['categories'][$category['id']];
      if(!$category['multiple'])
        $options = array($options);
      foreach($options as $value) {
        if(!$value)
          continue;
        $cat = array(
          'option_id' => $value,
        );
        if(is_array($orig_image['categories']) && array_key_exists($value, $orig_image['categories']))
            $cat['value_id'] = $orig_image['categories'][$value]['value_id'];
        array_push($image['categories'], $cat);
      }
    }
    $file = file_save_upload($name, $validators, $destination);
    if ($file) {
      $file->status |= FILE_STATUS_PERMANENT;
      $file = file_save($file);
      $image['fid'] = $file->fid;
    } else if (!$orig_image['id'])
      return drupal_set_message(t('Image %file could not be saved!', array('%file' => $file->filename)), 'error');
    $image = gallery_image_save($image);
    drupal_set_message(t('Image %file saved', array('%file' => $file->filename)));
    $saved = true;
    $orig_image = array();
  }
  drupal_goto('gallery/'.$type.'/'.$id.'/edit', ($saved ? array('query' => array('saved' => '1')) : array()));
}

/**
 * Submit handler for deleting multiple gallery images.
 */
function gallery_images_form_delete_submit($form, &$form_state) {
  $type = $form_state['gallery_type'];
  $id = $form_state['gallery_id'];
  
  $images = gallery_images($type, $id, false);
  
  foreach($images['images'] as $image) {
    if(!$form_state['input']['delete_'.$image['id']])
      continue;
    if(!gallery_edit_access_callback($type, $id, $image)) {
      drupal_set_message(t('You do not have the permission to delete the image %file', array('%file' => $image['filename']), 'error'));
      continue;
    }
    gallery_image_delete($image);
    drupal_set_message(t('File %file deleted', array('%file' => $image['filename'])));
  }
  drupal_goto('gallery/'.$type.'/'.$id.'/edit', array('query' => array('saved' => '1')));
}

/**
 * Menu callback; Show a gallery image
 */
function gallery_image_show($image) {
  gallery_image_visibility_save($image, '1');
  if($_REQUEST['ajax'])
    return drupal_json_output(array(
      'label' => t('Hide'), 
      'action' => url('gallery/'.$image['item_type'].'/'.$image['item_id'].'/hide/'.$image['id'])
    ));
  $camefrom = $_REQUEST['camefrom'];
  return drupal_goto($camefrom);
}

/**
 * Menu callback; Hide a gallery image
 */
function gallery_image_hide($image) {
  gallery_image_visibility_save($image, '0');
  if($_REQUEST['ajax'])
    return drupal_json_output(array(
      'label' => t('Show'), 
      'action' => url('gallery/'.$image['item_type'].'/'.$image['item_id'].'/show/'.$image['id'])
    ));
  $camefrom = $_REQUEST['camefrom'];
  return drupal_goto($camefrom);
}

/**
 * Menu callback; Rate a gallery image
 */
function gallery_image_rate($image) {
  $type_id = $_POST['type'];
  $types = gallery_available_rating_types();
  foreach($types as $type) {
    if($type['id'] != $type_id)
      continue;
    gallery_image_rating_save($image['id'], $type['id'], $_POST['rating_'.$type['id']]);
    break;
  }
  if($_REQUEST['ajax']) {
    $sql = 'SELECT '.
            'gir.rating_type_id AS id, '.
            'AVG(gir.rating) AS average, '.
            'COUNT(gir.rating) AS number '.
           'FROM gallery_image_rating AS gir '.
           'WHERE '.
            'gir.image_id = :id '.
           'GROUP BY gir.rating_type_id';
    $result = db_query($sql, array(':id' => $image['id']));
    $image['ratings'] = array();
    foreach($result as $record) {
      $image['ratings'][$record->id] = array(
        'rating_type' => $record->id,
        'average' => $record->average,
        'count' => $record->number);
    }
    
    return drupal_json_output(array(
      'rating' => theme('gallery_image_rating', array(
        'image' => $image,
        'rating_types' => gallery_available_rating_types()
      )
    )));
  }
  drupal_set_message(t('Your rating was successfully placed.'));
  $camefrom = $_POST['camefrom'];
  return drupal_goto($camefrom);
}

/**
 * Returns HTML for the page containing the list of gallery images
 *
 * @param $variables
 *   An associative array containing:
 *   - images: An array of the images to display
 *   - rating_types: An array of rating types
 *
 * @ingroup themeable
 */
function theme_gallery_list($variables) {
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js', array('weight' => 100));
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js', array('weight' => 110));
  $images = $variables['images'];
  $rating_types = $variables['rating_types'];
  $types = _gallery_types();
  
  if(!$images)
    return;
  
  $url = drupal_get_destination();
  $url = drupal_parse_url($url['destination']);
  $url = $url['path'];
  $items = array();
  $i = 0;
  foreach ($images as $image) {
    $i++;
    $editable = gallery_edit_access_callback($image['item_type'], $image['item_id'], $image);
    $caption = '';
    if($image['title'])
      $caption .= '<strong>'.$image['title'].'</strong>';
    if($image['description'])
      $caption .= ' <small>'.$image['description'].'</small>';
    if($caption)
      $caption = '<span class="title">'.$caption.'</span>';
    if($image['location'] || $image['author'])
      $caption .= '<span class="author">&copy; '.$image['author'].($image['location'] ? ', '.$image['location'] : '').'</span>';
    if(array_key_exists($image['item_type'], $types) && array_key_exists('image_caption', $types[$image['item_type']]))
      $caption .= ' <em>'.call_user_func_array($types[$image['item_type']]['image_caption'], array($image['item_id'])).'</em>';
    array_push($items, 
      '<li class="'.join(' ', _gallery_get_class($i-1)).'">'.
        '<a href="'.url('gallery/'.$image['item_type'].'/'.$image['item_id'].'/thumb/'.$image['id'].'/gallery_large').'">'.
          '<img src="'.url('gallery/'.$image['item_type'].'/'.$image['item_id'].'/thumb/'.$image['id'].'/gallery_thumbnail').'" alt="'.$image['title'].'" />'.
        '</a>'.
        (user_access('manage gallery images') ? 
          '<form class="show_hide" method="post" action="'.url('gallery/'.$image['item_type'].'/'.$image['item_id'].'/'.($image['visible'] ? 'hide' : 'show').'/'.$image['id']).'">'.
            '<input type="hidden" name="camefrom" value="'.$url.'" />'.
            '<input type="submit" name="submit" value="'.($image['visible'] ? t('Hide') : t('Show')).'" />'.
          '</form>' : '').
        '<div class="caption">'.
          ($caption ? '<p class="captionText">'.$caption.'</p>' : '').
          ($rating_types ? theme('gallery_image_rating', array(
                'image' => $image,
                'rating_types' => $rating_types
              )
            ) : ''
          ).
        '</div>'.
      '</li>');
  }
  return '<ul class="gallery clearfix">'.join('', $items).'</ul>';
}

function _gallery_get_class($i, $columns=GALLERY_COLUMNS) {
  $cls = array();
  $i = $i % $columns;
  if($i == 0)
    array_push($cls, 'first');
  if($i == $columns-1)
    array_push($cls, 'last');
  if($i % 2 == 0)
    array_push($cls, 'odd');
  if($i % 2 == 1)
    array_push($cls, 'even');
  return $cls;
}

/**
 * Returns HTML for an images rating types
 *
 * @param $variables
 *   An associative array containing:
 *   - image: A gallery image array
 *   - rating_types: An array of rating types
 *
 * @ingroup themeable
 */
function theme_gallery_image_rating($variables) {
  global $base_url;
  drupal_add_css($base_url . '/misc/ui/jquery.ui.core.min.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.ui.stars.min.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
  drupal_add_js($base_url . '/misc/ui/jquery.ui.core.min.js');
  drupal_add_js($base_url . '/misc/ui/jquery.ui.widget.min.js');
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.ui.stars.min.js', array('weight' => 101));
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js', array('weight' => 110));
  drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.rating.js', array('weight' => 111));
  $image = $variables['image'];
  $rating_types = $variables['rating_types'];
  if(!$rating_types)
    return '';
  $url = drupal_get_destination();
  $url = drupal_parse_url($url['destination']);
  $url = $url['path'];
  $ratings = '';
  foreach($rating_types as $rating_type) {
    $rating = is_array($image['ratings']) && array_key_exists($rating_type['id'], $image['ratings']) ? $image['ratings'][$rating_type['id']] : array('count' => 0, 'average' => 0);
    $avg = intval($rating['count']) ? (floatval($rating['average'])+3.0) : 0;
    $ratings .= '<form method="post" action="'.url('gallery/'.$image['item_type'].'/'.$image['item_id'].'/rate/'.$image['id']).'" data-average="'.$avg.'">'.
      '<input type="hidden" name="type" value="'.$rating_type['id'].'" />'.
      '<input type="hidden" name="camefrom" value="'.$url.'" />'.
      '<label for="rating_'.$rating_type['id'].'" title="'.$rating_type['description'].'">'.$rating_type['name'].' <small>('.t('Average: %avg; %num votes', array('%avg' => $avg, '%num' => intval($rating['count']))).')</small></label>'.
      '<select name="rating_'.$rating_type['id'].'" id="rating_'.$rating_type['id'].'" size="1">'.
        '<option value="-2">'.t('Poor').'</option>'.
        '<option value="-1">'.t('Not that bad').'</option>'.
        '<option value="0" selected="selected">'.t('Average').'</option>'.
        '<option value="1">'.t('Good').'</option>'.
        '<option value="2">'.t('Perfect').'</option>'.
      '</select>'.
      '<input type="submit" name="rate" value="'.t('Rate').'" />'.
    '</form>';
  }
  return '<div class="rating">'.$ratings.'</div>';
}

function _gallery_back_link($type, $id) {
  global $base_url;
  if((strlen($_SERVER['HTTP_REFERER']) < strlen($base_url)+8 ||
      substr($_SERVER['HTTP_REFERER'], strlen($base_url)+1, 7) != 'gallery') &&
     substr($_SERVER['HTTP_REFERER'], 0, strlen($base_url)) == $base_url)
    $_SESSION['gallery_back_url_'.$type.'_'.$id] = substr($_SERVER['HTTP_REFERER'], strlen($base_url)+1);
  $url = drupal_parse_url($_SESSION['gallery_back_url_'.$type.'_'.$id]);
  $_SESSION['gallery_back_url_'.$type.'_'.$id] = $url['path'];
  if($_SESSION['gallery_back_url_'.$type.'_'.$id])
    return array(
      'title' => t('Back'),
      'href' => $_SESSION['gallery_back_url_'.$type.'_'.$id]
    );
  return false;
}
