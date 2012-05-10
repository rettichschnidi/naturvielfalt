<?php

/**
 * @file
 * Administration pages for gallery category options.
 */

/**
 * Menu callback; Listing of all current gallery category options.
 */
function gallery_category_option_list($category) {
	$page = array();
	$title = t(
		'Edit category options of %name',
		array('%name' => $category['name']));
	drupal_set_title($title, PASS_THROUGH);

	$options = gallery_category_options($category['id']);
	$page['gallery_category_option_list'] = array(
			'#markup' => theme(
				'gallery_category_option_list',
				array('options' => $options,
						'category' => $category
				)),
	);

	$page['back'] = array(
			'#theme' => 'links',
			'#links' => array(
					array(
							'title' => t('Back'),
							'href' => 'admin/config/media/gallery/categories'
					)
			),
			'#attributes' => array(
					'class' => 'action-links'
			)
	);

	return $page;
}

/**
 * Form builder; Edit a category option
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $option
 *   A category option array.
 * @ingroup forms
 * @see gallery_category_option_form_submit()
 */
function gallery_category_option_form($form, &$form_state, $category, $option) {
	if (isset($option['name'])) {
		$title = t('Edit %name', array('%name' => $option['name']));
		drupal_set_title($title, PASS_THROUGH);
	}
	$form_state['gallery_category_option'] = isset($option) ? $option : NULL;
	$form['#tree'] = TRUE;

	$form['name'] = array(
			'#type' => 'textfield',
			'#size' => '128',
			'#title' => t('Name'),
			'#default_value' => isset($option['name']) ? $option['name'] : '',
			'#description' => t('The name of the category option.'),
			'#required' => TRUE,
	);

	// Show the Override or Submit button for this style.
	$form['actions'] = array(
			'#type' => 'actions'
	);
	$form['actions']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Update category option'),
	);

	return $form;
}

/**
 * Submit handler for saving a category option.
 */
function gallery_category_option_form_submit($form, &$form_state) {
	$option = $form_state['gallery_category_option'];
	$option['name'] = $form_state['values']['name'];

	gallery_category_option_save($option);
	if ($form_state['values']['op'] == t('Update category option')) {
		drupal_set_message('Changes to the category option have been saved.');
	}
	$form_state['redirect'] = 'admin/config/media/gallery/categories/'
			. $option['gallery_category_id'] . '/options';
}

/**
 * Form builder; Form for adding a new category option.
 *
 * @ingroup forms
 * @see gallery_category_option_add_form_submit()
 */
function gallery_category_option_add_form($form, &$form_state, $category) {
	$form = gallery_category_option_form(
		$form,
		$form_state,
		$category,
		array('gallery_category_id' => $category['id']));
	$title = t('Create new category option');
	drupal_set_title($title, PASS_THROUGH);

	$form['actions']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Create new category option'),
	);

	return $form;
}

/**
 * Submit handler for adding a new category option.
 */
function gallery_category_option_add_form_submit($form, &$form_state) {
	$option = $form_state['values'];
	$option['gallery_category_id'] = $form_state['gallery_category_option']['gallery_category_id'];
	$option = gallery_category_option_save($option);
	drupal_set_message(
		t(
			'Category option %name was created.',
			array('%name' => $option['name'])));
	$form_state['redirect'] = 'admin/config/media/gallery/categories/'
			. $option['gallery_category_id'] . '/options';
}

/**
 * Form builder; Form for deleting a category option.
 *
 * @param $option
 *   A option array.
 *
 * @ingroup forms
 * @see gallery_category_option_delete_form_submit()
 */
function gallery_category_option_delete_form($form, &$form_state, $category,
		$option) {
	$form_state['gallery_category_option'] = $option;

	return confirm_form(
		$form,
		t('Delete %name?', array('%name' => $option['name'])),
		'admin/config/media/gallery/categories/' . $option['gallery_category_id']
				. '/options',
		t(
			'Are you sure you want to delete the category option %name? All stored categorisation of this option will also be deleted.',
			array('%name' => $option['name'])),
		t('Delete'),
		t('Cancel'));
}

/**
 * Submit handler to delete a category option.
 */
function gallery_category_option_delete_form_submit($form, &$form_state) {
	$option = $form_state['gallery_category_option'];

	gallery_category_option_delete($option);
	drupal_set_message(
		t(
			'Category option %name was deleted.',
			array('%name' => $option['name'])));
	$form_state['redirect'] = 'admin/config/media/gallery/categories/'
			. $option['gallery_category_id'] . '/options';
}

/**
 * Returns HTML for the page containing the list of category options.
 *
 * @param $variables
 *   An associative array containing:
 *   - options: An array of all the category options returned by gallery_category_options().
 *   - category: A category array of the current category
 *
 * @see gallery_category_options()
 * @ingroup themeable
 */
function theme_gallery_category_option_list($variables) {
	$options = $variables['options'];
	$category = $variables['category'];

	$header = array(
			t('Option'),
			array(
					'data' => t('Operations'),
					'colspan' => 2
			)
	);
	$rows = array();
	foreach ($options as $option) {
		$row = array();
		$row[] = l(
			$option['name'],
			'admin/config/media/gallery/categories/' . $option['gallery_category_id']
					. '/options/edit/' . $option['id']);
		$row[] = l(
			t('edit'),
			'admin/config/media/gallery/categories/' . $option['gallery_category_id']
					. '/options/edit/' . $option['id']);
		$row[] = l(
			t('delete'),
			'admin/config/media/gallery/categories/' . $option['gallery_category_id']
					. '/options/delete/' . $option['id']);
		$rows[] = $row;
	}

	if (empty($rows)) {
		$rows[] = array(
				array(
						'colspan' => 3,
						'data' => t(
							'There are currently no options for this category. <a href="!url">Add a new one</a>.',
							array('!url' => url(
										'admin/config/media/gallery/categories/'
												. $category['id']
												. '/options/add')
							)),
				)
		);
	}

	return theme('table', array('header' => $header,
			'rows' => $rows
	));
}
