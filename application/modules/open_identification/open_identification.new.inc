<?php
function open_identification_new() {

	drupal_set_title(t('Create new open identification'));
	
	$prefix = 'Upload';
    $form[$prefix.'_file'] = array(
      '#type' => 'file',
      '#title' => t('Upload'),
      '#size' => 48,
      '#element_validate' => array($validate_function),
      '#file_required' => false,//!$image['id'],
    );
    $form[$prefix.'_file']['author'] = array(
      '#type' => 'textfield',
      '#title' => t('Author'),
      '#size' => 100,
      '#maxlength' => 255,
      '#default_value' => 'author',
      '#description' => t('The author of the title'),
      '#required' => FALSE,
    );
    
	$form[$prefix] = array();
    $form[$prefix]['metadata'] = array(
      '#type' => 'fieldset',
      '#title' => t('Metadata'),
      '#weight' => 2,
      '#collapsible' => true,
      '#collapsed' =>   true,
      '#attached' => array (
        'js' => array (
          'misc/form.js',
          'misc/collapse.js',
        ),
      ),
    );
    $form[$prefix]['metadata']['comment'] = array(
      '#type' => 'textarea',
      '#title' => t('Comment'),
      '#default_value' => 'Comment',
      '#description' => t('Give a comment for the open identification'),
      '#required' => FALSE,
    );
    $form[$prefix]['metadata']['location'] = array(
      '#type' => 'textfield',
      '#title' => t('Location'),
      '#size' => 100,
      '#maxlength' => 255,
      '#default_value' => 'location',
      '#description' => t('The location the location was taken at.'),
      '#required' => FALSE,
    );
  	$form[$prefix]['metadata']['type'] = array(
	    '#name' => 'add',
	    '#type' => 'select',
	    '#title' => t('Type'),
	    '#options' => array('habitat' => t('habitat'), 'organism' => t('organism')),
	    '#weight' => 5
  	);
    $result = db_select('inventory_type', 'it')
    ->fields('it', array('id', 'name'))
    ->orderBy('it.id', 'DESC')
    ->execute();
  
  	$options = array('' => t('- Please choose -'));
  	$options['unknown'] = t('unknown');
  	foreach($result as $record)
    	$options[$record->id] = $record->name;
  	$form[$prefix]['metadata']['organismgroup'] = array(
	    '#name' => 'add',
	    '#type' => 'select',
	    '#title' => t('Organismgroup'),
	    '#options' => $options,
	    '#weight' => 6,
	    '#attributes' => array(
	      'id' => 'add-inventory-group'
    )
  );
  
	$form['action']['upload_submit'] = array(
	    '#type' => 'submit',
	    '#value' => 'upload',
	    '#submit' => 'test',
	    '#attributes' => array('data-add' => t('Add another test'), 'data-remove' => t('Remove test')),
	);
	return $form;
}

?>