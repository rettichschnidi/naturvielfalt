<?php

/**
 * @file area.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

/**
 * Create new area.
 * @return render array
 */
function area_new_area() {
	$output = array();
	drupal_set_title(t('Create new area'));

	$output['map'] = array(
			'#search' => true,
			'#create' => true,
			'#theme' => 'area',
	);

	// 	$output['googleMaps']['GoogleMaps'] = array(
	// 			'#type' => 'fieldset',
	// 			'#weight' => 3,
	// 			'#collapsible' => FALSE,
	// 			'searchByMaps' => array(
	// 					'#theme' => 'area.base'
	// 			), // map
	// 	);
	// $output['areaNew'] = drupal_get_form('area_create_form');
	return $output;
}

/**
 * Return a hidden form which can be used to store values handed over by Javascript
 * @param array $form
 * @param array $form_state
 * @return form to submit area properties
 */
function area_create_form($form, &$form_state) {
	// 	This produces an ugyl field. Do we really need it?! 
	$form['AreaNewForm'] = array(
			'#type' => 'fieldset',
			'#weight' => 8,
	);
	$form['AreaNewForm']['field_name'] = array(
			'#type' => 'hidden',
			'#required' => true,
			'#size' => 50,
			'#attributes' => array(
					'id' => 'edit-name'
			),
	);
	$form['AreaNewForm']['zip'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-zip'
			),
	);
	$form['AreaNewForm']['township'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-township'
			),
	);
	$form['AreaNewForm']['canton'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-canton'
			),
	);
	$form['AreaNewForm']['altitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-altitude'
			),
	);
	$form['AreaNewForm']['surface-area'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-surface-area'
			),
	);
	$form['AreaNewForm']['country'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-country'
			),
	);
	$form['AreaNewForm']['latitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-latitude'
			),
	);
	$form['AreaNewForm']['longitude'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-longitude'
			),
	);
	$form['AreaNewForm']['area-coords'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-area-coords'
			),
	);
	$form['AreaNewForm']['area_type'] = array(
			'#type' => 'hidden',
			'#attributes' => array(
					'id' => 'edit-area-type'
			),
	);
	$form['AreaNewForm']['actions-new'] = array(
			'#type' => 'actions'
	);
	$form['AreaNewForm']['actions-new']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Continue'),
	);
	return $form;
}
/**
 * A simple testview to demonstrate the usage of the static google maps API
 */

function area_naturwerkstatic() {
	$form = array();
	// coordinates for  a point in Brugg, near naturwerk
	$params['latitude'] = 47.48135013015885;
	$params['longitude'] = 8.210649490356445;

	$url = area_get_static_image_url($params, 800, 800);
	$form['naturwerk'] = array(
			'#type' => 'markup',
			'#markup' => "<img src='$url' alt='Naturwerk in Brugg'/>"
	);
	return $form;
}
?>
