<?php
function vote_show_overview($observation_id = NULL) {
	drupal_add_js('misc/form.js');
	drupal_add_js('misc/collapse.js');
	drupal_add_js(drupal_get_path('module', 'vote') . '/js/imageslider.js');
	drupal_add_js(drupal_get_path('module', 'vote') . '/js/vote.js');
	drupal_add_css(drupal_get_path('module', 'vote') . '/css/vote.css');
	drupal_add_css(drupal_get_path('module', 'vote') . '/css/selectbox.css');
	
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
	drupal_add_js(
		drupal_get_path('module', 'vote') . '/js/jquery.xcolor.min.js',
		array(
		'weight' => 101
	));
	drupal_add_js(
		drupal_get_path('module', 'vote') . '/js/selectbox.js',
		array(
		'weight' => 102
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
	$artgroup_filter_id = 0;
	$render_array['artgroup']['artgroup_filter_id'] = array(
			'#id' => 'artgroup_filter_id',
			'#title' => t('Filter on species group'),
			'#type' => 'select',
			'#options' => organism_artgroup_get_all_as_array(),
			'#default_value' => $artgroup_filter_id,
 			'#attributes' => array(
					'id' => 'artgroup_filter_id'
			),
			'#weight' => 3,
			'#tree' => true,
			'#value' => $artgroup_filter_id
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
	
	$render_array['leftDiv'] = array(
			'#type' => 'form',
			"#id" => "leftDiv",
			'#weight' => 10
	);
	$render_array['rightDiv'] = array(
			'#type' => 'form',
			"#id" => "rightDiv",
			'#weight' => 11
	);
	
	$render_array['leftDiv']['mainimage'] = array(
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
				<a id="openImages" class="datatable_gallery_image_type" onclick="$('#mainImage').trigger('click');">
					$proofImageText (<span id="numberOfImages"></span>)
				</a>
			</span>
		</div>
	</div>
EOT;
	
	$render_array['leftDiv']['mainimage']['images'] = array(
			'#type' => 'markup',
			'#markup' => '<div id="mainImageContainer">
				<a class="lightbox" id="mainImageLink" href="' . drupal_get_path('module', 'vote') . '/img/loading.gif"><img src="' . drupal_get_path('module', 'vote') . '/img/loading.gif" id="mainImage" alt="Main image" /></a>'
			. $result . '</div><div id="slideshowImages"></div>'
	);
	
	$render_array['rightDiv']['maps'] = array(
			'#id' => 'googleMapsFieldset',
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#weight' => 6
	);
	
	$render_array['rightDiv']['maps']['map'] = array(
			'#theme'                        => 'area',
			'#ch1903'                       => true,
			'#search'                       => true,
			'#mapid'                        => 'observationmap',
			'#geometries_fetch_url'         => base_path() . 'observation/geometry/json',
			'#geometries_autoload'          => false,
			'#infowindow_content_fetch_url_observation' => base_path() . 'observation/{ID}/overview/ajaxform',
			'#height'                       => '300px'
	);
	
	//drupal_add_css(drupal_get_path('module', 'observation') . '/css/observation.css');
	
	$render_array['rightDiv']['metaInfo'] = array(
			'#id' => 'metaInfo',
			'#type' => 'fieldset',
			'#title' => t('Observation'),
			'#weight' => 8
	);
	
	$render_array['rightDiv']['metaInfo']["table"] = array(
			'#type'   => 'markup',
			'#markup' => '<div id="detailTable"></div>'
	);
	
	$render_array['clearBoth'] = array(
			'#type'   => 'markup',
			'#markup' => '<div style="clear:both;"></div>',
			'#weight' => 12
	);
	
	$render_array['verificationBox'] = array(
			'#type' => 'fieldset',
			'#title' => t('Verifications'),
			'#weight' => 13
	);
	
	$render_array['verificationBox']['chart'] = array(
			'#type' => 'markup',
			'#markup' => '<div id="selectBoxContainer"></div><div id="noVerificationsMessage" style="display: none;"></div>'
	);
	
	$render_array['ownVerification'] = array(
			'#type' => 'fieldset',
			'#title' => t('Own verification'),
			'#weight' => 14
	);
	
	$render_array['ownVerification']['form'] = drupal_get_form('own_verification_form');
	
	return $render_array;
}

function own_verification_form($form, &$form_state) {
	$artgroup_id = 1;
	
	if (isset($form_state['triggering_element'])) {
		if ($form_state['triggering_element']['#id'] == 'artgroup_id') {
			$artgroup_id = $form_state['input']['group']['artgroup_id'];
		}
	}
	
	$json = 'organism/search/json/' . $artgroup_id;
	$timestamp = time();
	
	$form['group'] = array(
			'#id' => 'organism_group',
			'#prefix' => '<div id="organism_replace">',
			'#suffix' => '</div>',
			'#tree' => true,
	);
	
	$form['group']['artgroup_id'] = array(
			'#id' => 'artgroup_id',
			'#title' => t('Filter on species group'),
			'#type' => 'select',
			'#options' => organism_artgroup_get_all_as_array(),
			'#default_value' => $artgroup_id,
			'#ajax' => array(
					'event' => 'change',
					'callback' => 'vote_organism_callback',
					'wrapper' => 'organism_replace'
			),
			'#attributes' => array(
					'id' => 'artgroup_id'
			),
			'#tree' => true,
			'#value' => $artgroup_id
	);
	
	$form['group']['organism'] = array(
			'#id' => 'organism_name',
			'#title' => t('Search for species'),
			'#type' => 'textfield',
			'#autocomplete_path' => $json,
			'#description' => t('Maximum 30 search results will be showed.'),
			'#attributes' => array(
					'placeholder' => t('Start typing to see the species...'),
			),
			'#tree' => true,
			'#required' => true,
			'#disabled' => false,
			'#default_value' => isset($form_state['values']['group']['organism']) ? $form_state['values']['group']['organism'] : ''
	);
	
	$form['group']['date'] = array(
			'#id' => 'date',
			'#title' => t('Date of the verification'),
			'#type' => 'textfield',
			'#attributes' => array('class' => array('datepicker')),
			'#required' => true,
			'#tree' => true,
			'#default_value' => date('d.m.Y', $timestamp),
	);
	
	$form['group']['time'] = array(
			'#id' => 'time',
			'#title' => t('Time of the verification'),
			'#attributes' => array('class' => array('timepicker')),
			'#type' => 'textfield',
			'#required' => true,
			'#tree' => true,
			'#default_value' => date('H:i', $timestamp),
	);
	
	$form['#after_build'] = array('own_verification_form_add_js_tools');
	
	$form['group']['comment'] = array(
			'#id' => 'comment',
			'#title' => t('Comment your choice'),
			'#type' => 'textarea',
			'#description' => t('Add a note to explain your choice so others can learn from you.')
	);
	
	$form['group']['submitVerification'] = array(
			'#type'   => 'markup',
			'#markup' => '<input id="submitVerification" type="button" value="'.t('Submit verification').'" />'
	);
	
	return $form;
}

function vote_organism_callback($form, &$form_state) {
	if ($form_state['triggering_element']['#id'] == 'artgroup_id') {
		$form['group']['organism']['#value'] = "";
	}
	return $form['group'];
}

function own_verification_form_add_js_tools($form, &$form_state) {
	//add date and timepicker
	drupal_add_library('system', 'ui.datepicker');
	drupal_add_library('system', 'ui.slider');
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_js($observation_path . '/js/timepicker.js');
	
	drupal_add_css('/* css for timepicker */
					.ui-timepicker-div .ui-widget-header { margin-bottom: 8px; }
					.ui-timepicker-div dl { text-align: left; }
					.ui-timepicker-div dl dt { height: 25px; margin-bottom: -25px; }
					.ui-timepicker-div dl dd { margin: 0 10px 10px 65px; }
					.ui-timepicker-div td { font-size: 90%; }
					.ui-tpicker-grid-label { background: none; border: none; margin: 0; padding: 0; }
		
					.ui-timepicker-rtl{ direction: rtl; }
					.ui-timepicker-rtl dl { text-align: right; }
					.ui-timepicker-rtl dl dd { margin: 0 65px 10px 10px; })', 'inline');

	drupal_add_js('jQuery(document).ready(function(){
	 		jQuery( ".datepicker" ).live("focus", function() {
	 				$(this).datepicker({
			      dateFormat: "dd.mm.yy"
    			});
	 		});

			jQuery( ".timepicker" ).live("focus", function() {
	 			$(this).timepicker({
			 	timeFormat: "HH:mm",
	 			showButtonPanel: false
			 });
	 		});
			});
	 		', 'inline');

	return $form;
}