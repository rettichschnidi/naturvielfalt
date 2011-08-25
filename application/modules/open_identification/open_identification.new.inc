<?php



function open_identification_new($form, &$form_state) {
    
	global $user;
    drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
    drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js');
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js');
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.upload.js');
	
    drupal_set_title(t('New open identification'));
	$validate_function = '_gallery_medias_form_upload_validate';
    $form['#tree'] = TRUE;
	
    $form['delete'] = array(
	    '#type' => 'submit',
	    '#ajax' => array(
	      'callback' => 'open_identification_new_delete_file',
	      'name' => 'submit_delete_file',
	    ),
	    '#value' => t('delete file'),
	    '#weight' => 0
	  );
    $form['file'] = array(
      '#type' => 'file',
      '#title' => t('Select'),
      '#size' => 48,
     // '#element_validate' => array($validate_function),
      '#file_required' => false,//!$image['id'],
	  '#weight' => 1
    );
    $form['author'] = array(
      '#type' => 'textfield',
      '#title' => t('Author'),
      '#size' => 35,
      '#maxlength' => 255,
      '#default_value' => 'author',
      '#description' => t('The author of the title'),
      '#required' => FALSE,
      '#weight' => 2
    );
	$form['fileselection'] = array(
	    '#type' => 'markup',
	    '#prefix' => '<div id="fileselection">',
	    '#suffix' => '</div>',
	  	'#weight' => 0
	  );
	
	$form['submit'] = array(
	    '#type' => 'submit',
	    '#ajax' => array(
	      'callback' => 'open_identification_new_add_file',
	      'wrapper' => 'fileselection',
	      'name' => 'submit_add_file',
	    ),
	    '#value' => t('add file'),
	    '#weight' => 3
	  );

    $form['comment'] = array(
      '#type' => 'textarea',
      '#title' => t('Comment'),
      '#default_value' => 'Comment',
      '#description' => t('Give a comment to the open identification'),
      '#required' => FALSE,
      '#weight' => 4
    );
    $form['location'] = array(
      '#type' => 'textfield',
      '#title' => t('Location'),
      '#size' => 100,
      '#maxlength' => 255,
      '#default_value' => 'location',
      '#description' => t('Location of the open identification.'),
      '#required' => FALSE,
      '#weight' => 4
    );
  	$form['type'] = array(
	    '#name' => 'type',
	    '#type' => 'select',
	    '#title' => t('Type'),
	    '#options' => array('habitat' => t('habitat'), 'organism' => t('organism')),
	    '#weight' => 5,
  		'#ajax' => array(
	      'callback' => 'open_identification_new_add_organismgroup',
	      'wrapper' => 'organismgroup',
	    )
  	);

  
	$form['upload_submit'] = array(
	    '#type' => 'submit',
	    '#value' => 'upload',
	    '#submit' => array('open_identification_new_upload_submit'),
	    '#weight' => 7,
	);


	return $form;
}
function open_identification_new_add_file($form, $form_state)
{
	  $header[] = t('Filename');
	  $header[] = t('Author');
      $header[] = t('Delete');
      //print_r($_FILES);
	  $rows = $_SESSION['rows'];
	  if($_FILES['files']['name']['file'] !=""){
		  $row[] = $_FILES['files']['name']['file'];
		  $row[] = $form_state['values']['author'];
		  $row[] = '<input type="checkbox" name="jj" value="1" />';
		  $rows[] = $row;
	      $_SESSION['rows'] = $rows;
		  $form['overview'] = array(
	      '#theme' => 'table',
	      '#header' => $header,
	      '#rows' => $_SESSION['rows'],
		  '#prefix' => '<div id="fileselection">',
		  '#suffix' => '</div>',
	      );
	  }
	  else{
	  	drupal_set_message('no filename','error');
	  }
      //$_FILES['files']['name']['file'] = "";

     return $form['overview'];
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