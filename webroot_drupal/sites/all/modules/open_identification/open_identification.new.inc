<?php

/**
 * Create a new identification request
 * 
 * @note Code does not get used anywhere - candidate for deletion
 * @param assoziative array $form
 * @param assoziative array $form_state
 */
function open_identification_new($form, &$form_state) {
	global $user;
	drupal_set_title(t('New open identification'));
	$render_array['description'] = array(
			'#type' => 'textarea',
			'#title' => t('Comment'),
			'#default_value' => t('no comment'),
			'#description' => t('Comment to the open identification'),
			'#required' => false,
			'#weight' => 1
	);
	$render_array['type'] = array(
			'#name' => 'type',
			'#type' => 'select',
			'#title' => t('Type'),
			'#options' => array(
					'habitat' => t('habitat'),
					'organism' => t('organism')
			),
			'#weight' => 2,
			'#required' => false
	// FIXME delete this?
	/*'#ajax' => array(
	                                   'callback' => 'open_identification_new_add_organismgroup',
	                                   'wrapper' => 'organismgroup',
	                                 )*/
	);
	$result = db_select('inventory_type', 'it')->fields('it', array('id',
					'name'
			))->orderBy('it.id', 'DESC')->execute();

	$options = array(
			'' => t('- Please choose1 -') // FIXME ASCII art or what?
	);
	
	$options['0'] = t('unknown');
	foreach ($result as $record)
		$options[$record->id] = $record->name;

	$render_array['organismgroupid'] = array(
			'#name' => 'organismgroup',
			'#type' => 'select',
			'#title' => t('Organismgroup'),
			'#options' => $options,
			'#weight' => 3,
			'#required' => false,
	);
	$render_array['media_upload'] = drupal_get_form('gallery_image_form', 'open_identification', false, 'medias', false);
	return $render_array;
}
?>