<?php

/**
 * @file gallery.admin.categories.inc
 * Administration pages for gallery categories.
 */

/**
 * Menu callback; Listing of all current gallery categories.
 */
function gallery_category_list() {
	$page = array();

	$categories = gallery_categories();
	$page['gallery_category_list'] = array(
			'#markup' => theme(
				'gallery_category_list',
				array('categories' => $categories)),
	);

	return $page;
}

/**
 * Form builder; Edit a category
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $category
 *   A category array.
 * @ingroup forms
 * @see gallery_category_form_submit()
 */
function gallery_category_form($form, &$form_state, $category) {
	if (isset($category['name'])) {
		$title = t('Edit %name', array('%name' => $category['name']));
		drupal_set_title($title, PASS_THROUGH);
	}

	$form_state['gallery_category'] = $category;
	$form['#tree'] = TRUE;

	$form['name'] = array(
			'#type' => 'textfield',
			'#size' => '128',
			'#title' => t('Name'),
			'#default_value' => isset($category['name']) ? $category['name'] : '',
			'#description' => t('The name of the category.'),
			'#required' => TRUE,
	);
	$form['description'] = array(
			'#type' => 'textarea',
			'#title' => t('Description'),
			'#default_value' => isset($category['description']) ? $category['description'] : '',
			'#description' => t('The description of the category.'),
			'#required' => FALSE,
	);
	$form['required'] = array(
			'#type' => 'checkbox',
			'#title' => t('Required'),
			'#default_value' => isset($category['required']) ? $category['required'] : '',
			'#description' => t('Whether this category is required or not.'),
			'#required' => FALSE,
	);
	$form['multiple'] = array(
			'#type' => 'checkbox',
			'#title' => t('Multiple'),
			'#default_value' => isset($category['multiple']) ? $category['multiple'] : '',
			'#description' => t(
				'Whether multiple values may be selected for this category or not.'),
			'#required' => FALSE,
	);

	// Show the Override or Submit button for this style.
	$form['actions'] = array(
			'#type' => 'actions'
	);
	$form['actions']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Update category'),
	);

	return $form;
}

/**
 * Submit handler for saving a category.
 */
function gallery_category_form_submit($form, &$form_state) {
	$category = $form_state['gallery_category'];
	$category['name'] = $form_state['values']['name'];
	$category['description'] = $form_state['values']['description'];
	$category['required'] = $form_state['values']['required'];
	$category['multiple'] = $form_state['values']['multiple'];

	gallery_category_save($category);
	if ($form_state['values']['op'] == t('Update category')) {
		drupal_set_message('Changes to the category have been saved.');
	}
	$form_state['redirect'] = 'admin/config/media/gallery/categories';
}

/**
 * Form builder; Form for adding a new category.
 *
 * @ingroup forms
 * @see gallery_category_add_form_submit()
 */
function gallery_category_add_form($form, &$form_state) {
	$form = gallery_category_form($form, $form_state, array());
	$title = t('Create new category');
	drupal_set_title($title, PASS_THROUGH);

	$form['actions']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Create new category'),
	);

	return $form;
}

/**
 * Submit handler for adding a new category.
 */
function gallery_category_add_form_submit($form, &$form_state) {
	$category = gallery_category_save($form_state['values']);
	drupal_set_message(
		t('Category %name was created.', array('%name' => $category['name'])));
	$form_state['redirect'] = 'admin/config/media/gallery/categories';
}

/**
 * Form builder; Form for deleting a category.
 *
 * @param $category
 *  A category array.
 *
 * @ingroup forms
 * @see gallery_category_delete_form_submit()
 */
function gallery_category_delete_form($form, &$form_state, $category) {
	$form_state['gallery_category'] = $category;

	return confirm_form(
		$form,
		t('Delete %name?', array('%name' => $category['name'])),
		'admin/config/media/gallery/categories',
		t(
			'Are you sure you want to delete the category %name? All stored categorisation of this category will also be deleted.',
			array('%name' => $category['name'])),
		t('Delete'),
		t('Cancel'));
}

/**
 * Submit handler to delete a category.
 */
function gallery_category_delete_form_submit($form, &$form_state) {
	$category = $form_state['gallery_category'];

	gallery_category_delete($category);
	drupal_set_message(
		t('Category %name was deleted.', array('%name' => $category['name'])));
	$form_state['redirect'] = 'admin/config/media/gallery/categories';
}

/**
 * Returns HTML for the page containing the list of categories.
 *
 * @param $variables
 *   An associative array containing:
 *   - categories: An array of all the categories returned by gallery_categories().
 *
 * @see gallery_categories()
 * @ingroup themeable
 */
function theme_gallery_category_list($variables) {
	$categories = $variables['categories'];

	$header = array(
			t('Category'),
			t('Conditions'),
			array(
					'data' => t('Operations'),
					'colspan' => 4
			)
	);
	$rows = array();
	foreach ($categories as $category) {
		$row = array();
		$result = gallery_category_conditions($category['id']);
		$conditions = array();
		foreach ($result as $condition)
			array_push($conditions, $condition['name']);
		$row[] = l(
			$category['name'],
			'admin/config/media/gallery/categories/edit/' . $category['id']);
		$row[] = l(
			implode(', ', $conditions),
			'admin/config/media/gallery/categories/' . $category['id']
					. '/conditions');
		$row[] = l(
			t('edit'),
			'admin/config/media/gallery/categories/edit/' . $category['id']);
		$row[] = l(
			t('delete'),
			'admin/config/media/gallery/categories/delete/' . $category['id']);
		$row[] = l(
			t('options'),
			'admin/config/media/gallery/categories/' . $category['id']
					. '/options');
		$row[] = l(
			t('conditions'),
			'admin/config/media/gallery/categories/' . $category['id']
					. '/conditions');
		$rows[] = $row;
	}

	if (empty($rows)) {
		$rows[] = array(
				array(
						'colspan' => 5,
						'data' => t(
							'There are currently no categories. <a href="!url">Add a new one</a>.',
							array('!url' => url(
										'admin/config/media/gallery/categories/add')
							)),
				)
		);
	}

	return theme('table', array('header' => $header,
			'rows' => $rows
	));
}
