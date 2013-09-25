<?php
module_load_include('inc', 'observation');

drupal_add_js('misc/form.js');
drupal_add_js('misc/collapse.js');
drupal_add_js(drupal_get_path('module', 'vote') . '/js/imageslider.js');
drupal_add_js(drupal_get_path('module', 'vote') . '/js/vote.js');
drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js');
drupal_add_js(drupal_get_path('module', 'vote') . '/js/jquery.xcolor.min.js');
drupal_add_js(drupal_get_path('module', 'vote') . '/js/selectbox.js');
drupal_add_js(drupal_get_path('module', 'observation') . '/js/observation.js');

drupal_add_css(drupal_get_path('module', 'vote') . '/css/vote.css');
drupal_add_css(drupal_get_path('module', 'vote') . '/css/selectbox.css');
drupal_add_css(drupal_get_path('module', 'vote') . '/css/imageslider.css');
drupal_add_css(drupal_get_path('module', 'gallery') . '/css/gallery.css');
drupal_add_css(drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css');

function vote_show_observation($observation_ids = NULL) {
	$vote_observations_filter = array();

	if ($observation_ids != null) {
		$vote_observations_filter = explode(',', $observation_ids);
	}
	
	$_SESSION["vote_observations_filter"] = $vote_observations_filter;
	
	return vote_show_overview();
}

function vote_show_organism($organism_id = NULL) {
	$vote_observations_filter = array();
	
	if ($organism_id != null) {
		$result = observation_get_by_organism($organism_id);
		foreach ($result as $row) {
			$vote_observations_filter[] = $row->id;
		}
	}
	
	$_SESSION["vote_observations_filter"] = $vote_observations_filter;
	
	return vote_show_overview();
}

function vote_show_artgroup($artgroup_id = NULL) {
	$vote_observations_filter = array();
	
	if ($artgroup_id != null) {
		$result = observation_get_by_artgroup($artgroup_id);
		if (count($result) <= 0 && $artgroup_id != 1) {
			drupal_add_js('jQuery(document).ready(function(){
				observation.setMessage(Drupal.t("No observations available in this artgroup. All observations are displayed. Please choose another one."), "warning", 0);
			});', 'inline');
		}
		foreach ($result as $row) {
			$vote_observations_filter[] = $row->id;
		}
	}
	
	$_SESSION["vote_observations_filter"] = $vote_observations_filter;
	
	return vote_show_overview($artgroup_id);
}

function vote_show_overview($artgroup_id = 0) {
	$render_array['message'] = array(
			'#type'   => 'markup',
			'#markup' => '
			<div id="message" style="display: none; height: auto;">
				<div class="messages status"></div>
			</div>',
			'#weight' => 1	);
	
	$render_array['welcome'] = array(
		'#type' => 'fieldset',
		'#title' => t('Welcome'),
		'#weight' => 2
	);
	$render_array['welcome']['description'] = array(
		'#type' => 'markup',
		'#markup' => t('On this page experts can identify species and vote for the correct one. This helps keeping a good identification quality and can help unexperienced users.')
	);
	
	$render_array['artgroup'] = array(
			'#id' => 'artgroup',
			'#type' => 'fieldset',
			'#title' => t('Artgroup Settings'),
			'#weight' => 3,
			'#attributes' => array(
					'class' => array(
							'collapsible'
					)
			)
	);
	
	$artgroup_filter_id = $artgroup_id;
	$render_array['artgroup']['artgroup_filter_id'] = array(
		'#id' => 'artgroup_filter_id',
		'#title' => t('Filter on species group'),
		'#type' => 'select',
		'#options' => organism_artgroup_get_all_as_array(),
		'#default_value' => $artgroup_filter_id,
 		'#attributes' => array(
			'id' => 'artgroup_filter_id'
		),
		'#weight' => 4,
		'#tree' => true,
		'#value' => $artgroup_filter_id,
		'#attributes' => array('onchange' => "document.location='/vote/artgroup/' + document.getElementById('artgroup_filter_id').value;"),
	);
	
	$render_array['imageslider'] = array(
		'#type' => 'fieldset',
		'#title' => t('Next images'),
		'#weight' => 5
	);
	$render_array['imageslider']['images'] = array(
		'#type' => 'markup',
		'#markup' => '<div id="imagesContainer">
			<img id="loadPreviousImages" class="imageSliderNavigation" src="/' . drupal_get_path('module', 'vote') . '/img/previous.png" alt="Previous" onclick="moveImages(-4, false);" />
			<img id="previousImage01" class="imageSliderImage" style="width: 0px;" />
			<img id="previousImage02" class="imageSliderImage" style="width: 0px;" />
			<img id="previousImage03" class="imageSliderImage" style="width: 0px;" />
			<img id="previousImage04" class="imageSliderImage" style="width: 0px;" />
			<img id="currentImage01" src="/' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="imageSliderImage" alt="Image 01" onclick="moveImages(1, true);" />
			<img id="currentImage02" src="/' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="imageSliderImage" alt="Image 02" onclick="moveImages(2, true);" />
			<img id="currentImage03" src="/' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="imageSliderImage" alt="Image 03" onclick="moveImages(3, true);" />
			<img id="currentImage04" src="/' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="imageSliderImage" alt="Image 04" onclick="moveImages(4, true);" />
			<img id="futureImage01" class="imageSliderImage" style="width: 0px;" />
			<img id="futureImage02" class="imageSliderImage" style="width: 0px;" />
			<img id="futureImage03" class="imageSliderImage" style="width: 0px;" />
			<img id="futureImage04" class="imageSliderImage" style="width: 0px;" />
			<img id="loadNextImages" class="imageSliderNavigation" src="/' . drupal_get_path('module', 'vote') . '/img/next.png" alt="Next" onclick="moveImages(4, false);" />
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
		'#weight' => 6
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
			<a class="lightbox" id="mainImageLink" href="/' . drupal_get_path('module', 'vote') . '/img/loading.gif">
			<img src="/' . drupal_get_path('module', 'vote') . '/img/loading.gif" id="mainImage" alt="Main image" />
			<div class="caption"></div>
			</a>'
		. $result . '</div><div id="slideshowImages"></div>'
	);
	
	$render_array['rightDiv']['maps'] = array(
		'#id' => 'googleMapsFieldset',
		'#type' => 'fieldset',
		'#title' => t('Map'),
		'#weight' => 7
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
	
	$render_array['rightDiv']['observation_attributes'] = array(
			'#type' => 'fieldset',
			'#title' => t('Details'),
			'#weight' => 10,
			'#collapsible' => true,
			'#collapsed' => false
	);
	
	$render_array['rightDiv']['observation_attributes']['table'] = array(
		'#type'   => 'markup',
		'#markup' => '<div id="commentTable"></div>'
	);
	
	$render_array['leftDiv']['metaInfo'] = array(
		'#id' => 'metaInfo',
		'#type' => 'fieldset',
		'#title' => t('Observation'),
		'#weight' => 8
	);
	
	$render_array['leftDiv']['metaInfo']["table"] = array(
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
		'#id' => 'ownVerification',
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
	
	$form['#after_build'] = array('own_verification_form_add_js_tools');
	
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
			
			jQuery("#submitVerification").live("click", function() {
				submitVerification();
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