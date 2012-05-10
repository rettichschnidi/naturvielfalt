<?php

/**
 * @file
 * Administration pages for gallery category conditions.
 */

/**
 * Menu callback; Listing of all current gallery category conditions.
 */
function gallery_category_condition_list($category) {
	$page = array();
	$title = t(
		'Edit category conditions of %name',
		array('%name' => $category['name']));
	drupal_set_title($title, PASS_THROUGH);

	$conditions = gallery_category_conditions($category['id']);
	$page['gallery_category_condition_list'] = array(
			'#markup' => theme(
				'gallery_category_condition_list',
				array('conditions' => $conditions,
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
 * Form builder; Edit a category condition
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $condition
 *   A category condition array.
 * @ingroup forms
 * @see gallery_category_condition_form_submit()
 */
function gallery_category_condition_form($form, &$form_state, $category,
		$condition) {
	$title = t('Edit %name', array('%name' => $condition['name']));
	drupal_set_title($title, PASS_THROUGH);

	$form_state['gallery_category_condition'] = $condition;
	$form['#tree'] = TRUE;

	$conds = _gallery_category_conditions();
	if (!array_key_exists($condition['condition'], $conds)) {
		drupal_set_message(t('Condition not found.'), 'error');
		return gallery_category_condition_list($category);
	}
	$cond = $conds[$condition['condition']];

	$form['condition'] = array(
			'#type' => 'hidden',
			'#default_value' => $condition['condition'],
			'#required' => TRUE,
	);
	$form['name'] = array(
			'#type' => 'textfield',
			'#title' => t('Name'),
			'#size' => 128,
			'#default_value' => $condition['name'],
			'#description' => t('The name of this condition.'),
			'#required' => TRUE,
	);
	$options = _gallery_column_options($cond);
	if ($options) {
		$form['column_value'] = array(
				'#type' => 'select',
				'#title' => t('Column'),
				'#default_value' => $condition['columnname'] . '___'
						. $condition['value'],
				'#options' => $options,
				'#description' => t('The column the value is checked against.'),
				'#required' => TRUE,
		);
	}

	// Show the Override or Submit button for this style.
	$form['actions'] = array(
			'#type' => 'actions'
	);
	$form['actions']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Update category condition'),
			'#access' => isset($editable) ? $editable : false,
	);

	return $form;
}

function _gallery_column_options($cond) {
	$table = isset($cond['table']) ? $cond['table'] : '';
	if (is_array($table))
		$table = $table[0];
	$options = array();
	$columns = isset($cond['columns']) ? $cond['columns'] : array();
	foreach ($columns as $column => $group) {
		$prefix = $column . '___';
		if (is_array($group)) {
			if (function_exists($group[1])) {
				$options[$group[0]] = array();
				$columns = call_user_func($group[1]);
				foreach ($columns as $value => $name)
					$options[$group[0]][$prefix . $value] = $name;
			}
		} else {
			$results = db_query(
				'SELECT DISTINCT ' . $table . '."' . $column . '" FROM '
						. $table . ' WHERE ' . $table . '."' . $column
						. '" IS NOT NULL ORDER BY ' . $table . '."' . $column
						. '";')->fetchCol($column);
			$options[$group] = array();
			// Boolean column
			if (count($results) == 2 && $results[0] == 0 && $results[1] == 1) {
				$options[$group] = array(
						($prefix . '0') => t('No'),
						($prefix . '1') => t('Yes')
				);
				continue;
			}
			foreach ($results as $result)
				$options[$group][($prefix . $result)] = $result;
		}

	}
	return $options;
}

function _create_condition_from_form(&$form_state) {
	$condition = $form_state['gallery_category_condition'];
	$condition['condition'] = $form_state['values']['condition'];
	if ($form_state['values']['name'])
		$condition['name'] = $form_state['values']['name'];
	if (isset($form_state['values']['column_value']) && $form_state['values']['column_value']) {
		$column_value = explode('___', $form_state['values']['column_value'], 2);
		$condition['columnname'] = $column_value[0];
		$condition['value'] = $column_value[1];
	}
	return $condition;
}

/**
 * Submit handler for saving a category condition.
 */
function gallery_category_condition_form_submit($form, &$form_state) {
	$condition = _create_condition_from_form($form_state);
	gallery_category_condition_save($condition);
	if ($form_state['values']['op'] == t('Update category condition')) {
		drupal_set_message(
			t('Changes to the category condition have been saved.'));
	}
	$form_state['redirect'] = 'admin/config/media/gallery/categories/'
			. $condition['gallery_category_id'] . '/conditions';
}

/**
 * Form builder; Form for adding a new category condition.
 *
 * @ingroup forms
 * @see gallery_category_condition_add_form_submit()
 */
function gallery_category_condition_add_form($form, &$form_state, $category) {
	$conds = _gallery_category_conditions();
	if (isset($_POST['condition'])) {
		if (!array_key_exists($_POST['condition'], $conds)) {
			drupal_set_message(t('Condition not found.'), 'error');
			$form_state['redirect'] = 'admin/config/media/gallery/categories/'
					. $category['id'] . '/conditions';
			return $form;
		}
		$cond = $conds[$_POST['condition']];
		$condition = array(
				'name' => $cond['name'],
				'condition' => $_POST['condition'],
				'gallery_category_id' => $category['id']
		);
		$form = gallery_category_condition_form(
			$form,
			$form_state,
			$category,
			$condition);
		$title = t(
			'Create new category condition of type %name',
			array('%name' => $condition['name']));
		drupal_set_title($title, PASS_THROUGH);
		$form['actions']['submit'] = array(
				'#type' => 'submit',
				'#value' => t('Create new category condition'),
		);
	} else {
		drupal_set_title(t('Create new category condition'));
		drupal_set_message(t('Condition not found.'), 'error');
			$form_state['redirect'] = 'admin/config/media/gallery/categories/'
					. $category['id'] . '/conditions';
	}

	return $form;
}

/**
 * Submit handler for adding a new category condition.
 */
function gallery_category_condition_add_form_submit($form, &$form_state) {
	$condition = _create_condition_from_form($form_state);
	$condition = gallery_category_condition_save($condition);
	drupal_set_message(
		t(
			'Category condition %name was created.',
			array('%name' => $condition['name'])));
	$form_state['values']['op'] = false;
	$form_state['redirect'] = 'admin/config/media/gallery/categories/'
			. $condition['gallery_category_id'] . '/conditions';
}

/**
 * Form builder; Form for deleting a category condition.
 *
 * @param $condition
 *   A condition array.
 *
 * @ingroup forms
 * @see gallery_category_condition_delete_form_submit()
 */
function gallery_category_condition_delete_form($form, &$form_state, $category,
		$condition) {
	$form_state['gallery_category_condition'] = $condition;

	return confirm_form(
		$form,
		t('Delete %name?', array('%name' => $condition['name'])),
		'admin/config/media/gallery/categories/' . $condition['gallery_category_id']
				. '/conditions',
		t(
			'Are you sure you want to delete the category condition %name?',
			array('%name' => $condition['name'])),
		t('Delete'),
		t('Cancel'));
}

/**
 * Submit handler to delete a category condition.
 */
function gallery_category_condition_delete_form_submit($form, &$form_state) {
	$condition = $form_state['gallery_category_condition'];

	gallery_category_condition_delete($condition);
	drupal_set_message(
		t(
			'Category condition %name was deleted.',
			array('%name' => $condition['name'])));
	$form_state['redirect'] = 'admin/config/media/gallery/categories/'
			. $condition['gallery_category_id'] . '/conditions';
}

/**
 * Returns HTML for the page containing the list of category conditions.
 *
 * @param $variables
 *   An associative array containing:
 *   - condition_types: An associative array of condition types
 *   - conditions: An array of all the category conditions returned by gallery_category_conditions().
 *   - category: A category array of the current category
 *
 * @see gallery_category_conditions()
 * @ingroup themeable
 */
function theme_gallery_category_condition_list($variables) {
	$conditions = $variables['conditions'];
	$category = $variables['category'];

	$conds = _gallery_category_conditions();

	$header = array(
			t('Name'),
			t('Column'),
			t('Value'),
			array(
					'data' => t('Operations'),
					'colspan' => 2
			)
	);
	$rows = array();
	foreach ($conditions as $condition) {
		$cond = $conds[$condition['condition']];
		if (isset($cond['columns']) && is_array($cond['columns'])
				&& is_array($cond['columns'][$condition['columnname']])) {
			$options = _gallery_column_options(
				array('columns' => array($condition['columnname'] => $cond['columns'][$condition['columnname']])));
			$condition['value'] = $options[$cond['columns'][$condition['columnname']][0]][$condition['columnname']
					. '___' . $condition['value']];
			$condition['columnname'] = $cond['columns'][$condition['columnname']][0];
		}
		$row = array();
		$row[] = l(
			$condition['name'],
			'admin/config/media/gallery/categories/'
					. $condition['gallery_category_id'] . '/conditions/edit/'
					. $condition['id']);
		$row[] = l(
			$condition['columnname'],
			'admin/config/media/gallery/categories/'
					. $condition['gallery_category_id'] . '/conditions/edit/'
					. $condition['id']);
		$row[] = l(
			$condition['value'],
			'admin/config/media/gallery/categories/'
					. $condition['gallery_category_id'] . '/conditions/edit/'
					. $condition['id']);
		$row[] = array_key_exists('columns', $cond) ? l(
					t('edit'),
					'admin/config/media/gallery/categories/'
							. $condition['gallery_category_id'] . '/conditions/edit/'
							. $condition['id']) : '';
		$row[] = l(
			t('delete'),
			'admin/config/media/gallery/categories/'
					. $condition['gallery_category_id'] . '/conditions/delete/'
					. $condition['id']);
		$rows[] = $row;
	}

	if (empty($rows)) {
		$rows[] = array(
				array(
						'colspan' => 6,
						'data' => t(
							'There are currently no conditions for this category.')
				)
		);
	}

	$options = '';
	foreach ($conds as $value => $cond)
		$options .= '<option value="' . $value . '">' . $cond['name']
				. '</option>';
	$add_form = '<form action="'
			. url(
				'admin/config/media/gallery/categories/' . $category['id']
						. '/conditions/add') . '" method="post">'
			. '<select name="condition" size="1">' . $options . '</select>'
			. '<input type="submit" value="' . t('Create condition')
			. '" name="submit" />' . '</form>';
	return $add_form
			. theme(
				'table',
				array('header' => $header,
						'rows' => $rows
				));
}
