<?php

/**
 * Menu callback; View of open identification
 * 
 * @param int $id The ID of the inventory
 * @return array Drupal render array
 */
function open_identification_edit_form($form, &$form_state, $id) {
	drupal_set_title(t('Open Identification '));
	$open_identification = open_identification_get_entry($id);

	$render_array['open_identification_id'] = array(
			'#type' => 'hidden',
			'#value' => $id
	);

	$render_array['comment'] = array(
			'#name' => 'comment',
			'#type' => 'textarea',
			'#title' => t('Comment'),
			'#value' => $open_identification['comment'],
			'#description' => t('Comment to the open identification'),
			'#required' => false,
			'#weight' => 1,
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
			'#required' => false,
			'#value' => trim($open_identification['type']), /*'#ajax' => array(
															'callback' => 'open_identification_new_add_organismgroup',
															'wrapper' => 'organismgroup',
															)*/
	);
	if ($open_identification['organismgroup']) {
		$result = db_select('inventory_type', 'it')->fields('it', array('id',
						'name'
				))->orderBy('it.id', 'DESC')->execute();

		$options = array(
				'0' => t('- Please choose -')
		);
		$options['unknown'] = t('unknown');
		foreach ($result as $record)
			$options[$record->id] = $record->name;
		$render_array['organismgroup'] = array(
				'#name' => 'organismgroup',
				'#type' => 'select',
				'#title' => t('Organismgroup'),
				'#options' => $options,
				'#weight' => 3,
				'#required' => false,
				'#value' => $open_identification['organismgroupid'],
		);
	}

	$render_array['submit'] = array(
			'#type' => 'submit',
			'#value' => t('change'),
			'#submit' => array(
					'open_identification_edit_submit'
			),
			'#weight' => 4,
	);
	return $render_array;
}

/**
 * Gets the edited values from the website and passes them to open_identification_details_update
 * 
 * @param unknown_type $form
 * @param assoziative array $form_state
 * @return assoziative array $form_state (modified)
 */
function open_identification_edit_submit($form, &$form_state) {
	$open_identification['open_identification_id'] = $form_state['input']['open_identification_id'];
	$open_identification['comment'] = $form_state['input']['comment'];
	$open_identification['type'] = $form_state['input']['type'];
	$open_identification['organismgroup'] = $form_state['input']['organismgroup'];
	open_identification_details_update($open_identification);
	return $form_state['redirect'] = 'open_identification/' . $open_identification['open_identification_id'];
}

/**
 * Shows a form to delete a identification request
 *
 * @param unknown_type $form
 * @param assoziative array $form_state
 * @param integer $open_identification_id
 * @return Configured form
 */
function open_identification_delete_form($form, &$form_state, $open_identification_id) {
	$form_state['open_identification_id'] = $open_identification_id;
	return confirm_form($form, t('Delete open identification ?'), 'open_identification/' . $open_identification_id,
			t('Are you sure you want to delete the open identification?'), t('Delete'), t('Cancel'));
}

/**
 * Retrieves a identification delete request and passes its Id to open_identification_delete
 * @see open_identification_delete
 * 
 * @param unknown_type $form
 * @param assoziative array $form_state
 */
function open_identification_delete_form_submit($form, &$form_state) {
	open_identification_delete($form_state['open_identification_id']);
	$form_state['redirect'] = 'open_identification/';
}
?>