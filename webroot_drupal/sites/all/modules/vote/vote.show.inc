<?php
function vote_show_overview($observation_id = NULL) {
	drupal_add_js('misc/form.js');
	drupal_add_js('misc/collapse.js');
	drupal_add_css(drupal_get_path('module', 'vote') . '/css/vote.css');
	
	$render_array['welcome'] = array(
			'#type' => 'fieldset',
			'#title' => t('Welcome'),
			'#weight' => 1
	);
	$render_array['welcome']['description'] = array(
			'#type' => 'markup',
			'#markup' => t('On this page experts can identify species and vote for the correct one. This helps keeping a good identification quality and can help unexperienced users.')
	);
	
	$render_array['artgroup'] = array(
			'#id' => 'artgroup',
			'#type' => 'fieldset',
			'#title' => t('Artgroup Settings'),
			'#weight' => 2,
			'#attributes' => array(
					'class' => array(
							'collapsible',
							'collapsed'
					)
			)
	);
	$artgroup_id = 0;
	$render_array['artgroup']['artgroup_id'] = array(
			'#id' => 'artgroup_id',
			'#title' => t('Filter on species group'),
			'#type' => 'select',
			'#options' => organism_artgroup_get_all_as_array(),
			'#default_value' => $artgroup_id,
			'#ajax' => array(
					'event' => 'change',
					'callback' => 'observation_organism_callback',
					'wrapper' => 'organism_replace'
			),
			'#attributes' => array(
					'id' => 'artgroup_id'
			),
			'#weight' => 3,
			'#tree' => true,
			'#value' => $artgroup_id
	);
	
	
	
	
// 	$render_array['own_proposal']['organism'] = array(
// 			'#id' => 'organism_name',
// 			'#title' => t('Search for species'),
// 			'#type' => 'textfield',
// 			'#autocomplete_path' => $json,
// 			'#description' => t('Maximum 30 search results will be showed.'),
// 			'#attributes' => array(
// 					'placeholder' => t('Start typing to see the species...'),
// 			),
// 			'#ajax' => array(
// 					'event' => 'change',
// 					'callback' => 'observation_organism_callback',
// 					'wrapper' => 'organism_replace',
// 			),
// 			'#weight' => 4,
// 			'#tree' => true,
// 			'#required' => $checked ? false : true,
// 			'#disabled' => $checked ? true : false,
// 			'#default_value' => isset($form_state['values']['group']['organism']) ? $form_state['values']['group']['organism'] : $observation['organism']['name_lang'],
// 	);
	
	return $render_array;
}