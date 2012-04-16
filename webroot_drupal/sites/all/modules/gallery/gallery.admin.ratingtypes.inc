<?php

/**
 * @file
 * Administration pages for gallery rating types.
 */

/**
 * Menu callback; Listing of all current gallery rating types.
 * 
 * @return
 *   A form array
 */
function gallery_rating_type_list() {
	$page = array();

	$types = gallery_rating_types();
	$page['gallery_rating_type_list'] = array(
			'#markup' => theme(
				'gallery_rating_type_list',
				array('types' => $types)),
	);

	return $page;
}

/**
 * Form builder; Edit a rating type
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $type
 *   A gallery type array.
 * @ingroup forms
 * @see gallery_rating_type_form_submit()
 */
function gallery_rating_type_form($form, &$form_state, $type) {
	$title = t('Edit %name', array('%name' => $type['name']));
	drupal_set_title($title, PASS_THROUGH);

	$form_state['gallery_rating_type'] = $type;
	$form['#tree'] = TRUE;

	$form['name'] = array(
			'#type' => 'textfield',
			'#size' => '128',
			'#title' => t('Name'),
			'#default_value' => $type['name'],
			'#description' => t('The name of the rating type.'),
			'#required' => TRUE,
	);
	$form['description'] = array(
			'#type' => 'textarea',
			'#title' => t('Description'),
			'#default_value' => $type['description'],
			'#description' => t('The description of the rating type.'),
			'#required' => FALSE,
	);
	$form['weight'] = array(
			'#type' => 'select',
			'#title' => t('Weight'),
			'#default_value' => $type['weight'],
			'#options' => array(
					'1' => '1',
					'2' => '2',
					'3' => '3',
					'4' => '4',
					'5' => '5',
					'6' => '6',
					'7' => '7',
					'8' => '8',
					'9' => '9',
					'10' => '10'
			),
			'#description' => t(
				'The weight of this rating type in the global rating.'),
	);
	$roles = user_roles();
	$roles[''] = t('None');
	$form['role_id'] = array(
			'#type' => 'select',
			'#title' => t('Required role'),
			'#default_value' => $type['role_id'],
			'#options' => $roles,
			'#description' => t(
				'The role required to place ratings of this type.'),
	);

	// Show the Override or Submit button for this style.
	$form['actions'] = array(
			'#type' => 'actions'
	);
	$form['actions']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Update rating type'),
			'#access' => $editable,
	);

	return $form;
}

/**
 * Submit handler for saving a rating type.
 */
function gallery_rating_type_form_submit($form, &$form_state) {
	$type = $form_state['gallery_rating_type'];
	$type['name'] = $form_state['values']['name'];
	$type['description'] = $form_state['values']['description'];
	$type['weight'] = $form_state['values']['weight'];
	$type['role_id'] = $form_state['values']['role_id'];

	gallery_rating_type_save($type);
	if ($form_state['values']['op'] == t('Update rating type')) {
		drupal_set_message('Changes to the rating type have been saved.');
	}
	$form_state['redirect'] = 'admin/config/media/gallery/rating-types';
}

/**
 * Form builder; Form for adding a new rating type.
 *
 * @ingroup forms
 * @see gallery_rating_type_add_form_submit()
 */
function gallery_rating_type_add_form($form, &$form_state) {
	$form = gallery_rating_type_form(
		$form,
		$form_state,
		array('weight' => 1,
				'role_id' => ''
		));
	$title = t('Create new rating type');
	drupal_set_title($title, PASS_THROUGH);

	$form['actions']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Create new rating type'),
	);

	return $form;
}

/**
 * Submit handler for adding a new rating type.
 */
function gallery_rating_type_add_form_submit($form, &$form_state) {
	$type = gallery_rating_type_save($form_state['values']);
	drupal_set_message(
		t('Rating type %name was created.', array('%name' => $type['name'])));
	$form_state['redirect'] = 'admin/config/media/gallery/rating-types';
}

/**
 * Form builder; Form for deleting a rating type.
 *
 * @param $type
 *   A rating type array.
 *
 * @ingroup forms
 * @see gallery_rating_type_delete_form_submit()
 */
function gallery_rating_type_delete_form($form, &$form_state, $type) {
	$form_state['gallery_rating_type'] = $type;

	return confirm_form(
		$form,
		t('Delete %name?', array('%name' => $type['name'])),
		'admin/config/media/gallery/rating-types',
		t(
			'Are you sure you want to delete the rating type %name? All ratings of this type will also be deleted.',
			array('%name' => $type['name'])),
		t('Delete'),
		t('Cancel'));
}

/**
 * Submit handler to delete a rating type.
 */
function gallery_rating_type_delete_form_submit($form, &$form_state) {
	$type = $form_state['gallery_rating_type'];

	gallery_rating_type_delete($type);
	drupal_set_message(
		t('Rating type %name was deleted.', array('%name' => $type['name'])));
	$form_state['redirect'] = 'admin/config/media/gallery/rating-types';
}

/**
 * Returns HTML for the page containing the list of rating types.
 *
 * @param $variables
 *   An associative array containing:
 *   - types: An array of all the rating types returned by gallery_rating_types().
 *
 * @see gallery_rating_types()
 * @ingroup themeable
 */
function theme_gallery_rating_type_list($variables) {
	$types = $variables['types'];

	$header = array(
			t('Rating type'),
			t('Weight'),
			t('Required role'),
			array(
					'data' => t('Operations'),
					'colspan' => 2
			)
	);
	$rows = array();
	foreach ($types as $type) {
		$row = array();
		$role = 'None';
		if ($type['role_id'] && $loaded = user_role_load($type['role_id']))
			$role = $loaded->name;
		$row[] = l(
			$type['name'],
			'admin/config/media/gallery/rating-types/edit/' . $type['id']);
		$row[] = l(
			$type['weight'],
			'admin/config/media/gallery/rating-types/edit/' . $type['id']);
		$row[] = l(
			t($role),
			'admin/config/media/gallery/rating-types/edit/' . $type['id']);
		$row[] = l(
			t('edit'),
			'admin/config/media/gallery/rating-types/edit/' . $type['id']);
		$row[] = l(
			t('delete'),
			'admin/config/media/gallery/rating-types/delete/' . $type['id']);
		$rows[] = $row;
	}

	if (empty($rows)) {
		$rows[] = array(
				array(
						'colspan' => 5,
						'data' => t(
							'There are currently no rating types. !add.',
							array('!add' => l(
										t('Add a new one'),
										'admin/config/media/gallery/rating-types/add')
							)),
				)
		);
	}

	return theme('table', array('header' => $header,
			'rows' => $rows
	));
}
