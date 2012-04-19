<?php
/**
 * @file
 * Administration pages for gallery settings.
 */

/**
 * Menu callback; Edit gallery settings
 * 
 * Show an admin menu where user can set...
 *  - The minimum rating level for pics to show up
 *  - Max size of image file in KByte
 *  - Max size of sound file in KByte
 *  - Max size of video file in KByte
 * @return array
 * 	Drupal form
 */
function gallery_form() {
	$form = array();
	$form['gallery_boundary'] = array(
			'#type' => 'textfield',
			'#size' => '8',
			'#title' => t('Display boundary'),
			'#default_value' => variable_get('gallery_boundary', 0),
			'#description' => t(
				'The minimum rating an media has to receive in order to be displayed. Whereas the rating is the product of the average ratings of a type multiplied by the weight of the type.'),
			'#required' => TRUE,
	);
	$form['gallery_image_size'] = array(
			'#type' => 'textfield',
			'#size' => '8',
			'#title' => t('Image size'),
			'#default_value' => variable_get('gallery_image_size', 2048),
			'#description' => t('Max size of image file in KByte'),
			'#required' => TRUE,
	);
	$form['gallery_sound_size'] = array(
			'#type' => 'textfield',
			'#size' => '8',
			'#title' => t('Sound size'),
			'#default_value' => variable_get('gallery_sound_size', 2048),
			'#description' => t('Max size of sound file in KByte'),
			'#required' => TRUE,
	);
	$form['gallery_video_size'] = array(
			'#type' => 'textfield',
			'#size' => '8',
			'#title' => t('Video size'),
			'#default_value' => variable_get('gallery_video_size', 8192),
			'#description' => t('Max size of video file in KByte'),
			'#required' => TRUE,
	);
	// Pass the form array through system_settings_form()
	return system_settings_form($form);
}

/**
 * Restrict the inserted values to valid integers.
 * 
 * @param array $form
 * 	Drupal form
 * @param array &$form_state
 *  An associative array containing the current state of the form.
 */
function gallery_form_validate($form, &$form_state) {
	if (!preg_match('/^\-?[0-9]+$/', $form_state['values']['gallery_boundary'])) {
		form_error(
			$form['gallery_boundary'],
			t('Specify a valid signed integer value.'));
	}
	if (!preg_match(
		'/^\-?[0-9]+$/',
		$form_state['values']['gallery_image_size'])) {
		form_error(
			$form['gallery_image_size'],
			t('Specify a valid signed integer value.'));
	}
	if (!preg_match(
		'/^\-?[0-9]+$/',
		$form_state['values']['gallery_sound_size'])) {
		form_error(
			$form['gallery_sound_size'],
			t('Specify a valid signed integer value.'));
	}
	if (!preg_match(
		'/^\-?[0-9]+$/',
		$form_state['values']['gallery_video_size'])) {
		form_error(
			$form['gallery_video_size'],
			t('Specify a valid signed integer value.'));
	}
}
