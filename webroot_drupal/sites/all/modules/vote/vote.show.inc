<?php
function vote_show_overview($observation_id = NULL) {
	drupal_add_js('misc/form.js');
	drupal_add_js('misc/collapse.js');
	drupal_add_js(drupal_get_path('module', 'vote') . '/js/imageslider.js');
	drupal_add_css(drupal_get_path('module', 'vote') . '/css/vote.css');
	
	//start
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/gallery.css',
		array(
		'group' => CSS_DEFAULT,
		'every_page' => TRUE
	));
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css',
		array(
		'group' => CSS_DEFAULT,
		'every_page' => TRUE
	));
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js',
		array(
		'weight' => 100
	));
	//end
	
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
	
	$render_array['imageslider'] = array(
			'#type' => 'fieldset',
			'#title' => t('Next images'),
			'#weight' => 4
	);
	$render_array['imageslider']['images'] = array(
			'#type' => 'markup',
			'#markup' => '<div id="nextImagesContainer">
				<img id="loadPreviousImages" class="navigationImages" src="' . drupal_get_path('module', 'vote') . '/img/previous.png" alt="Previous" onclick="moveImages(-4, false);" />
				<img id="previousImage01" class="nextImages" style="width: 0px;" />
				<img id="previousImage02" class="nextImages" style="width: 0px;" />
				<img id="previousImage03" class="nextImages" style="width: 0px;" />
				<img id="previousImage04" class="nextImages" style="width: 0px;" />
				<img id="image01" src="' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="nextImages" alt="Image 01" onclick="moveImages(1, true);" />
				<img id="image02" src="' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="nextImages" alt="Image 02" onclick="moveImages(2, true);" />
				<img id="image03" src="' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="nextImages" alt="Image 03" onclick="moveImages(3, true);" />
				<img id="image04" src="' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="nextImages" alt="Image 04" onclick="moveImages(4, true);" />
				<img id="futureImage01" class="nextImages" style="width: 0px;" />
				<img id="futureImage02" class="nextImages" style="width: 0px;" />
				<img id="futureImage03" class="nextImages" style="width: 0px;" />
				<img id="futureImage04" class="nextImages" style="width: 0px;" />
				<img id="loadNextImages" class="navigationImages" src="' . drupal_get_path('module', 'vote') . '/img/next.png" alt="Next" onclick="moveImages(4, false);" />
			</div>'
	);
	
	$render_array['mainimage'] = array(
			'#id' => 'mainImageFieldset',
			'#type' => 'fieldset',
			'#title' => t('Current image'),
			'#weight' => 5
	);
	
	$proofImageText = t('Proof image');
	
	$result = <<<EOT
	<div class="datatable_gallery_image_container">
		<div style="position: relative;">
			<span class="mainImageType">
				<a id="openImages" class="datatable_gallery_image_type">
					$proofImageText (<span id="numberOfImages"></span>)
				</a>
			</span>
		</div>
	</div>
EOT;
	
	$render_array['mainimage']['images'] = array(
			'#type' => 'markup',
			'#markup' => '<div id="mainImageContainer">
				<a class="lightbox" id="mainImageLink" href="' . drupal_get_path('module', 'vote') . '/img/loading.gif"><img src="' . drupal_get_path('module', 'vote') . '/img/loading.gif" id="mainImage" alt="Main image" /></a>'
			. $result . '</div>'
	);
	
	$render_array['maps'] = array(
			'#id' => 'googleMapsFieldset',
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 6
	);
	
	$render_array['maps']['map'] = array(
			'#theme'                        => 'area',
			'#ch1903'                       => true,
			'#search'                       => true,
			'#mapid'                        => 'observationmap',
			'#geometries_fetch_url'         => base_path() . 'observation/geometry/json',
			'#geometries_autoload'          => false,
			'#infowindow_content_fetch_url_observation' => base_path() . 'observation/{ID}/overview/ajaxform',
			'#height'                       => '300px'
	);
	
	$render_array['clear'] = array(
			'#type'   => 'markup',
			'#markup' => '<div style="clear: both;"></div>'
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