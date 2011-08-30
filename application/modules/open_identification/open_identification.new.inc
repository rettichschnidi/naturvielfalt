<?php



function open_identification_new($form, &$form_state) {
    
	global $user;
    drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
    drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js');
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js');
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.upload.js');
	
    drupal_set_title(t('New open identification'));
    
    $render_array['description'] = array(
      '#type' => 'textarea',
      '#title' => t('Comment'),
      '#default_value' => t('no comment'),
      '#description' => t('Comment to the open identification'),
      '#required' => false,
      '#weight' =>1,
    );
   $render_array['type'] = array(
	    '#name' => 'type',
	    '#type' => 'select',
	    '#title' => t('Type'),
	    '#options' => array('habitat' => t('habitat'),'organism' => t('organism')),
	    '#weight' =>2,
	    '#required' => false,
  		/*'#ajax' => array(
	      'callback' => 'open_identification_new_add_organismgroup',
	      'wrapper' => 'organismgroup',
	    )*/
  	);
	$result = db_select('inventory_type', 'it')
    ->fields('it', array('id', 'name'))
    ->orderBy('it.id', 'DESC')
    ->execute();
  
  	$options = array('' => t('- Please choose -'));
  	$options['unknown'] = t('unknown');
  	foreach($result as $record)
    	$options[$record->name] = $record->name;
  	$render_array['organismgroup'] = array(
	    '#name' => 'organismgroup',
	    '#type' => 'select',
	    '#title' => t('Organismgroup'),
	    '#options' => $options,
  		'#weight' =>3,
  		'#required' => false,
  	);
    $render_array['media_upload'] = drupal_get_form('gallery_image_form','open_identification',false, 'medias',false);
	return $render_array;
}
function open_identification_new_add_organismgroup($form,$form_state){
	
	if($form_state['values']['open_identification_data']['type'] == 'organism'){
		print_r($form_state);
		$result = db_select('inventory_type', 'it')
	    ->fields('it', array('id', 'name'))
	    ->orderBy('it.id', 'DESC')
	    ->execute();
	  
	  	$options = array('' => t('- Please choose -'));
	  	$options['unknown'] = t('unknown');
	  	foreach($result as $record)
	    	$options[$record->id] = $record->name;
	  	$form['organismgroup'] = array(
		    '#name' => 'add',
		    '#type' => 'select',
		    '#title' => t('Organismgroup'),
		    '#options' => $options,
		    '#attributes' => array(
		      'id' => 'add-inventory-group'
	    	)
	  	);
	  	return $form['organismgroup'];
	}
    else{
    	
	$form['organismgroup'] = array(
	    '#type' => 'markup',
	    '#prefix' => '<div id="organismgroup">',
	    '#suffix' => '</div>',
		'#weight' => 6,
	  );
	$form['dropdown_organismgroup'] = array(
	    '#type' => 'markup',
	    '#prefix' => '<div id="organismgroup">',
	    '#suffix' => '</div>',
		'#weight' => 6,
	  );
    	return $form['dropdown_organismgroup'];
    }
  
}
function open_identification_new_upload_submit($form,&$form_state){
	
	unset($_SESSION['rows']);
	return $form_state['redirect'] = 'open_identification/gallery';
	
}
function open_identification_new_delete_file($form, &$form_state){
	
	$rows = $_SESSION['rows'];
	
	foreach($rows as $row){
		
		$check = $row[2];
		print($check);
	}
}
?>