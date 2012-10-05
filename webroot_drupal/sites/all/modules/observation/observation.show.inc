<?php
module_load_include('inc', 'observation');
module_load_include('inc', 'datatable');

/**
 * Show all existing observations.
 *
 * @return render array
 */
function observation_show_all_observations() {

	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_css($observation_path . '/css/observation.css');
	drupal_add_js($observation_path . '/js/observation.js');
	
	drupal_set_title(t('Show all observations'));
	
	$output['message'] = array(
		'#type'   => 'markup',
		'#markup' => '
			<div id="message" style="display: none; height: auto;">
				<div class="messages status"></div>
			</div>'
	);
	
	$output['div'] = array(
		'#type'  => 'fieldset',
		'#title' => t('Map'),
	);

	$output['div']['map'] = array(
		'#theme'                        => 'area',
		'#ch1903'                       => true,
		'#search'                       => true,
		'#mapid'                        => 'observationmap',
		'#geometries_fetch_url'         => base_path() . 'observation/geometry/json',
		'#geometries_autoload'          => false,
		'#infowindow_content_fetch_url' => base_path() . 'observation/{ID}/overview/ajaxform',
		'#height'                       => '300px'
	);

	$output['space'] = array(
		'#type'   => 'markup',
		'#markup' => '<p/>'
	);

	$output['area_table'] = array(
		'#theme'    => 'datatable',
		'#header'   => observation_get_standard_datatable_headers(8, 'desc'),
		'#title'    => t('Observations'),
		'#id_table' => 'observations',
		'#options'  => array(
			'jsonUrl'               => base_path() . "observation/get/all",
			'rowClick'              => 'rowClick',
			'rowClickHandler'       => '
				function rowClick(celDiv, id) {
					jQuery(celDiv).click(
						function() {							    
							observationmap.selectObservation(id);
						}
					)
				}',
			'rpShowAll'             => true,
			'preProcessHandler'     => 'observation.tablePreProcess',
			'onSuccessHandler'      => 'observation.onTableSuccess',
			'gallery_enabled'       => true,
			'gallery_image_sources' => array(
				'selected'  => array(
					'value'  => DATATABLE_IMGSRC_BELEG,
					'option' => t('only proof images')),
			 	array(
			 		'value'  => DATATABLE_IMGSRC_PORTRAIT,
			 		'option' => t('only portrait images')),
				array(
					'value'  => DATATABLE_IMGSRC_BELEG_PORTRAIT,
					'option' => t('proof images, then portrait images')),
				array(
					'value'  => DATATABLE_IMGSRC_PORTRAIT_BELEG,
					'option' => t('portrait images, then proof images'))
			),
			// Filters to add to the filterDiv above the table. The gallery image source
			// filter has a weight of 10, the table/gallery buttons 4 and 5 respectively.
			'custom_filter' => array(
				'acl_filter' => array(
					'#type' => 'select',
					'#name' => 'acl_filter',
					'#title' => t('Observations') . ':',
					'#options' => array(
						'all'       => t('Public'),
						'own'       => t('Own'),
						'writeable' => t('Editables')),
					'#attributes' => array(
						'class' => array('acl_filter')),
					'#weight' => 8
				),
				// Artgroup filter is made as markup because of white spaces in the options.
				// Drupal_render converts &nbsp; into &amp;nbsp; and because indenting
				// options with &nbsp; is the most reliable way, it's done with #markup.
				'artgroup' => array(
					'#type'   => 'markup',
					'#markup' => '
						<div class="form-item form-type-select form-item-artgroup-filter">
							<label>' . t('Artgroup') . ':</label>
							<select name="artgroup_filter" class="artgroup_filter form-select">
								' . organism_artgroup_get_all_as_option() . '
							</select>
						</div>',
					'#weight' => 9
				)
			)
		)
	);
	
	return $output;
}

/**
 * Return a form which shows some important facts about a given observation.
 * Intended for usage in a infowindow on a map.
 *
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 * @param array $areadata Area data
 * @return array Drupal form
 */
function observation_map_overview_form($form, &$form_state, $observationdata) {
	$form['table'] = observation_get_table(
		$observationdata,
		true);
	$form['table']['#attributes'] = array(
			'id' => 'infowindow_tabular_data'
	);
	/**
	 * Display the "edit" button only if user has write permission to this area
	 */
	if (observation_write_access($observationdata)) {
		$form['edit'] = array(
				'#type' => 'markup',
				'#markup' => l(
					t('Edit'),
					'observation/'
							. $observationdata['observation']['id'] . '/edit')
		);
		$form['whitespace'] = array(
				'#type' => 'markup',
				'#markup' => '&nbsp/&nbsp;'
		);
	}
	$form['show'] = array(
			'#type' => 'markup',
			'#markup' => l(
				t('Show'),
				'observation/' . $observationdata['observation']['id']
						. '/show')
	);
	return $form;
}
