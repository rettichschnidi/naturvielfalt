<?php
/**
 * form to edit basic attributes of inventory
 *
 * @param unknown_type $form
 * @param unknown_type $form_state
 * @param unknown_type $head_inventory_id
 * @param unknown_type $new
 * @return array to render
 */
function inventory_edit_basics($form, &$form_state, $head_inventory_id, $new) {
	if (!$new) {
		$result = db_query(
				'SELECT hi.name, hi.shared, hi.description
        FROM head_inventory hi, area a
        WHERE a.id = hi.area_id
        AND hi.id = :id;', array(':id' => $head_inventory_id));

		if (!empty($result)) {
			foreach ($result as $record) {
				$name = $record->name;
				$shared = $record->shared;
				$description = $record->description;
			}
		}
	}

	// creating forms
	$form['name'] = array(
			'#type' => 'textfield',
			'#title' => t('Name'),
			'#size' => 100,
			'#maxlength' => 128,
			'#required' => TRUE,
			'#value' => $name,
	);

	$form['description'] = array(
			'#type' => 'text_format',
			'#title' => t('Description'),
			'#value' => $description,
			'#format' => filter_default_format(),
			'#widget' => array(
					'#type' => 'text_textarea',
			),
			'#settings' => array(
					'#text_processing' => '0',
			),
	);

	if ($new) {
		$form['save'] = array(
				'#type' => 'submit',
				'#value' => t("Create new Inventory"),
				'#submit' => array(
						'inventory_save'
				),
		);
	} else {
		$form['update'] = array(
				'#type' => 'submit',
				'#value' => t('Save'),
				'#submit' => array(
						'inventory_update_basics'
				),
		);
	}

	// Saveing something to the form_state makes it accessible in the submit function.
	// We do this to make sure the sgid is taken from the url which has passed security checkpoints.
	$form_state['storage']['hi_id'] = $head_inventory_id;

	return $form;
}

?>