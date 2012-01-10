<?php

/**
 * Set Title and invoke drupal form
 */
function inventory_new($area_id) {
	if ($area_id) {
		$area = db_query('SELECT * FROM {area} WHERE id = :id', array(':id' => $area_id))->fetch();

		if ($area->field_name) {
			//area_id exists
			drupal_set_title(
					t('Create New Inventory for Area "!areaname"', array('!areaname' => check_plain($area->field_name))));
			$_SESSION['inventory']['area_id'] = $area_id;
			return drupal_get_form('inventory_new_form');
		} else {
			//non existing area_id
			drupal_set_title(t('not existing area ID specified'));
			return drupal_get_form('inventory_invalid_id');
		}
	} else {
		//we need an area ID to assign the new inventory to
		drupal_set_title(t('no area ID specified'));
		return drupal_get_form('inventory_invalid_id');
	}
}

/**
 * Form which gets displayed if invalid area_id was specified
 * 
 * @return form of error page
 */
function inventory_invalid_id() {
	//@TODO
	//error handling
}

/**
 * Form to create new inventory
 * 
 * @param unknown_type $form
 * @param unknown_type $form_state
 * @return form of new inventory
 */
function inventory_new_form($form, &$form_state) {
	drupal_add_css(drupal_get_path('module', 'inventory') . '/css/main.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
	// creating forms
	$form['new'] = array(
			'#type' => 'fieldset',
			'#weight' => 5,
	);

	$question_mark = theme('image',
			array('path' => 'misc/question_mark_small.png',
					'alt' => t('Questionmark'),
					'attributes' => array(
							'align' => 'left'
					),
			));

	/*$form['new']['suggestion']['picture'] = array(
	  '#markup'=> $question_mark,
	);*/
	$form['new']['suggestion']['help'] = array(
			'#prefix' => '<p>',
			'#markup' => t(
					'On this site you may create a new inventory. Please specify a name and optionally a description.'),
			'#suffix' => '</p>'
	);

	$form['new']['name'] = array(
			'#type' => 'textfield',
			'#title' => t('Inventory Name'),
			'#size' => 100,
			'#maxlength' => 128,
			'#required' => TRUE,
			'#default_value' => $name,
	);

	$form['new']['share'] = array(
			'#type' => 'checkbox',
			'#title' => t('Allow read access to all users'),
			'#default_value' => true,
	);

	// Export selection
	$options = inventory_export_types();
	if ($options) {
		$form['new']['export'] = array(
				'#type' => 'checkboxes',
				'#title' => t('Allow export to the following platforms'),
				'#options' => $options,
				'#default_value' => array_keys($options),
		);
	}

	$form['new']['description'] = array(
			'#type' => 'text_format',
			'#title' => t('Description of Inventory'),
			'#default_value' => $description,
			'#format' => filter_default_format(),
			'#widget' => array(
					'#type' => 'text_textarea',
			),
			'#settings' => array(
					'#text_processing' => '0',
			),
	);

	$form['new']['actions'] = array(
			'#type' => 'actions'
	);
	$form['new']['actions']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Continue')
	);

	return $form;
}

/**
 * Submit function of new inventory form
 * 
 * @param unknown_type $form
 * @param unknown_type $form_state
 * @return redirect-string to inventory/edit
 */
function inventory_new_form_submit($form, &$form_state) {
	global $user;
	$owner = $user->uid;

	$hiid = db_insert('head_inventory')
			->fields(
					array('area_id' => $_SESSION['inventory']['area_id'],
							'name' => $form_state['values']['name'],
							'shared' => $form_state['values']['share'] == 1 ? '1' : '0',
							'owner_id' => $owner,
							'description' => $form_state['values']['description']['value'],
							'create_time' => 'now()',
							'modify_time' => 'now()',
					))->execute();

	// update export type
	inventory_save_export_type($hiid, $form_state['input']['export']);

	return $form_state['redirect'] = 'inventory/' . $hiid . '/edit';
}
?>