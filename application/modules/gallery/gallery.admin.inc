<?php

/**
 * @file
 * Administration pages for gallery settings.
 */

/**
 * Menu callback; Edit gallery settings
 */
function gallery_form() {
  $form = array();
  $form['gallery_boundry'] = array(
    '#type' => 'textfield',
    '#size' => '8',
    '#title' => t('Display boundry'),
    '#default_value' => variable_get('gallery_boundry', 0),
    '#description' => t('The minimum rating an image has to receive in order to be displayed. Whereas the rating is the product of the average ratings of a type multiplied by the weight of the type.'),
    '#required' => TRUE,
  );

  // Pass the form array through system_settings_form()
  return system_settings_form($form);
}

function gallery_form_validate($form, &$form_state) {
  if (!preg_match('/^\-?[0-9]+$/', $form_state['values']['gallery_boundry'])) {
    form_error($form['gallery_boundry'], t('Specify a valid signed integer value.'));
  }
}
