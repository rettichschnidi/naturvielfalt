<?php
/**
 * @file organism.show.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'organism');

/**
 * Menu callback. This will be called for visits to /organism.
 * @return array
 */
function organism_show_classifiers() {
	// Make kingdom (Reich) from classifier
	$kingdoms['CSCF'] = 'Animals';
	$kingdoms['CRSF'] = 'Plants';
	$kingdoms['Fungus'] = 'Mushrooms';
	$kingdoms['Swisslichens'] = 'Lichens';
	$kingdoms['NISM'] = 'Mosses';

	// description text at the beginning
	$render_array['helptext'] = array(
			'#type' => 'markup',
			'#markup' => t(
				'Here you can browse all species of naturvielfalt.ch. Please select first the biological kingdom:')
	);

	// query classifier and the amount of species
	$classifiers = db_query(
		'SELECT
	        oc.id AS oc_id,
	        oc.name AS oc_name,
	        organismcount.cnt AS organismcount_cnt
	    FROM
	        {organism_classification} oc
	        JOIN (
	            SELECT
	                oc.prime_father_id AS prime_father_id,
	                COUNT(ocs.id) AS cnt
	            FROM
			{organism_classification_subscription} ocs
	                JOIN {organism_classification} oc ON oc.id = ocs.organism_classification_id
	            GROUP BY
	                prime_father_id
	        )
	        AS organismcount ON organismcount.prime_father_id = oc.prime_father_id
	    WHERE
	        oc.prime_father_id = oc.id
	    GROUP BY
	        oc_id,
	        oc_name,
	        organismcount.cnt;');

	// create list of links
	$list = array(
			'#theme' => 'links',
			'#links' => array(),
	);
	foreach ($classifiers as $classifier) {
		$title = $classifier->oc_name;
		if (isset($kingdoms[$title])) {
			$title = t($kingdoms[$title]);
		}
		$species = format_plural($classifier->organismcount_cnt, '1 Species', '@count Species');
		$list['#links'][] = array(
				'title' => $title . ' (' . $species . ')',
				'href' => 'organism/classification/' . $classifier->oc_id
		);
	}
	$render_array['classifiers'] = $list;

	return $render_array;
}

/**
 * Menu callback. This will be called for visits to /organism/classification/%.
 * @return array
 */
function organism_show_classification($classificationid) {
	assert($classificationid != false);
	// fetch classification data
	$resultClassification = db_query(
		'SELECT
			oc.parent_id AS oc_parent_id,
			oc.prime_father_id AS oc_prime_father_id,
			oc.name AS oc_name,
			ocl.name AS ocl_name
		FROM
			{organism_classification} oc
		LEFT JOIN
			{organism_classification_level} ocl
			ON
				ocl.parent_id = oc.organism_classification_level_id
			AND
				ocl.id != ocl.parent_id
		WHERE
			oc.id = :ocid',
		array(':ocid' => $classificationid));

	$classificationObj = $resultClassification->fetchObject();
	
	drupal_add_css(drupal_get_path('module', 'organism') . '/css/organism.css');
	drupal_add_js(drupal_get_path('module', 'organism') . '/js/organism.js');
	
	// hierarchy level up buttons
	// insert html in title to not change flexigrid.js...
	$hierarchyLevelUpButtons = '</div>'
		.'<div class="mUp mButton" data-cid="' . $classificationObj->oc_parent_id . '"><span /></div>'
		.'<div class="mTop mButton" data-cid="' . $classificationObj->oc_prime_father_id . '"><span />';
	
	// Set the table header
	$classificationTableHeader[] = array(
			'name' => t('Internal id'),
			'idField' => true,
			'dbfield' => 'ds_id',
			'width' => 30,
			'hide' => true
	);
	$classificationTableHeader[] = array(
			'name' => t(
				ucfirst($classificationObj->ocl_name),
				array(),
				array(
					'context' => 'scientific classification'
				)),
			'dbfield' => 'ds_name',
			'dbExactField' => 'ds_name',
			'width' => 760,
			'hide' => false
	);
	$classificationTableHeader[] = array(
			'name' => t('Number of species'),
			'dbfield' => 'cnt',
			'dbExactField' => 'cnt',
			'width' => 100,
			'hide' => false
	);
	
	$render_array = array();
	
	$render_array['classificators'] = array(
			'#theme' => 'datatable',
			'#title' => t('Subclassifications within') . ' ' . $classificationObj->oc_name . $hierarchyLevelUpButtons,
			'#header' => $classificationTableHeader,
			'#id_table' => 'classificators',
			'#options' => array(
					'jsonUrl' => base_path()
							. "organism/classification/$classificationid/classificationjson",
					'rowClick' => 'rowClick',
					'tableWidth' => 900,
					'tableHeight' => 220,
					'rowClickHandler' => "function rowClick(celDiv, id) {
											jQuery(celDiv).click(
												function() {
													window.location.href = Drupal.settings.basePath+'organism/classification/' + id;
												}
											)
										}",
					'onSubmitHandler' => "organism.onClassificatorsSubmit"
			)
	);

	$render_array['organismsTitle'] = array(
			'#type' => 'markup',
			'#markup' => '<h2 class="gridsubsection">' . t('Species') . '</h2>',
	);

	$organismsTableHeader[] = array(
			'name' => t('Organism id'),
			'dbfield' => 'organism_id',
			'idField' => true,
			'hide' => true,
			'width' => 30,
	);

	$classificationleveldata = organism_get_available_classification_levels(
		$classificationid);

	$organismsTableHeader[] = array(
			'name' => t('Picture'),
			'dbfield' => 'thumbnail',
			'width' => 80,
			'noSort' => true
	);
	
	$organismsTableHeader[] = array(
			'name' => t('Common name'),
			'dbfield' => 'ol_name',
			'dbExactField' => 'ol_name',
			'hide' => false,
			'width' => 180
	);
	
	$organismsTableHeader[] = array(
			'name' => t('Scientific name'),
			'dbfield' => 'scientific_name',
			'dbExactField' => 'scientific_name',
			'hide' => false,
			'width' => 300
	);
	
	foreach ($classificationleveldata as $index => $value) {
		if ($index == 0) {
			continue;
		}
		$organismsTableHeader[] = array(
				'name' => t(
					ucfirst($value->name),
					array(),
					array(
							'context' => 'scientific classification'
					)),
				'dbfield' => 'oc_' . $value->name,
				'dbExactField' => 'oc_' . $value->name,
				'hide' => false,
				'width' => 140
		);
	}

	$organismsTableHeader[] = array(
			'name' => t('Number of pictures'),
			'dbfield' => 'imagescount',
			'width' => 20
	);

	$organismsTableHeader[] = array(
			'name' => t('Number of videos'),
			'dbfield' => 'videoscount',
			'width' => 20,
	);

	$organismsTableHeader[] = array(
			'name' => t('Number of sounds'),
			'dbfield' => 'soundscount',
			'width' => 20,
	);

	$organismsTableHeader[] = array(
			'name' => t('Number of documents'),
			'dbfield' => 'documentscount',
			'width' => 20
	);

	$render_array['organisms'] = array(
			'#theme' => 'datatable',
			'#title' => t('Species which are in classification ') . $classificationObj->oc_name,
			'#header' => $organismsTableHeader,
			'#id_table' => 'organisms',
			'#options' => array(
					'jsonUrl' => base_path()
							. "organism/classification/$classificationid/organismjson",
					'rowClick' => 'rowClickOrganism',
					'tableWidth' => 900,
					'preProcessHandler' => 'organism.tablePreProcess',
					'rowClickHandler' => "function rowClickOrganism(celDiv, id){
												jQuery(celDiv).click(
													function() {
														window.location.href = Drupal.settings.basePath+'organism/' + id;
													}
												)
											}",
					'gallery_enabled' => true,
					'gallery_image_sources' => array(
						array(
							'value' => DATATABLE_IMGSRC_BELEG,
							'option' => t('only proof images'), // belegbilder
						),
					 	'selected' => array(
					 		'value' => DATATABLE_IMGSRC_PORTRAIT,
					 		'option' => t('only portrait images'),
					 	),
						array(
							'value' => DATATABLE_IMGSRC_BELEG_PORTRAIT,
							'option' => t('proof images, then portrait images'),
						),
						array(
							'value' => DATATABLE_IMGSRC_PORTRAIT_BELEG,
							'option' => t('portrait images, then proof images')
						),
					)
			)
	);
	return $render_array;
}

/**
 * Menu callback. This will be called for visits to /organism/%.
 * @return array
 */
function organism_show_organism($organismdata) {
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_css($observation_path . '/css/observation.css');
	drupal_add_js($observation_path . '/js/observation.js');
	
	drupal_add_css(drupal_get_path('module', 'organism') . '/css/organism.css');
	drupal_add_js('jQuery(document).ready(function () { jQuery("#files a").attr("target", "_blank"); });', 'inline');
	global $user;
	global $language;
	$current_language = isset($user->language) && !empty($user->language)
			? $user->language : $language->language;

	if (empty($organismdata)) {
		drupal_set_title(t('No such species available'));
		$render_array['nosuchorganism'] = null;
		return $render_array;
	}
	drupal_set_title(organism_get_organism_title($organismdata), PASS_THROUGH);
	
	$render_array = array();
	
	$render_array['attributes'] = array(
				'#type' => 'fieldset',
				'#title' => t('Attributes'),
				'#weight' => 1,
				'#tree' => true,
				'#attributes' => array(
						'id' => 'organism-attributes' // required for CSS
	)
	);
	
	if (isset($organismdata['attributes'])
	&& !empty($organismdata['attributes'])) {
		$attributes = $organismdata['attributes'];
		foreach ($attributes as $attribute_name => $data) {
			$attributevaluetype = $data['valuetype'];
			$attributename = t($attribute_name);
	
			$render_array['attributes'][$attributename] = array(
						'#type' => 'markup',
						'#markup' => "<b>$attributename: </b>",
			);
			$attributevalues = $data['values'];
			/**
			 * This is untested, but should generate a reasonable output
			 * even if there is more than a single value
			 */
			$render_array['attributes'][$attributename]['#markup'] .= implode(
					', ',
			$attributevalues) . '<br/>';
		}
	} else {
		$render_array['attributes']['noattribute'] = array(
					'#type' => 'markup',
					'#markup' => '<b>' . t('No attributes set for this species')
		. '</b>',
		);
	}
	
	/**
	 * Show all synonyms of this organism (but only, if there are any).
	 */
	if ((isset(
	$organismdata['classification_name_translations'][$current_language])
	&& count(
	$organismdata['classification_name_translations'][$current_language])
	>= 1)
	|| (isset($organismdata['synonyms'])
	&& count($organismdata['synonyms']) >= 1)) {
		$render_array['synonyms-container'] = array(
					'#prefix' => '<div id="organism-synonyms-container">',
					'#suffix' => '</div>',
					'#weight' => 2
		);
		$render_array['synonyms-container']['scientific_names'] = array(
					'#type' => 'fieldset',
					'#title' => t('Scientific synonyms'),
					'#attributes' => array(
							'id' => 'organism-synonyms-scientific' // required for CSS
		)
		);
	
		$synonym_markup = (isset($organismdata['synonyms'])) ? implode('<br/>', $organismdata['synonyms']) : '';
	
		$render_array['synonyms-container']['scientific_names']['synonyms'] = array(
					'#markup' => $synonym_markup
		);
	
		$render_array['synonyms-container']['classification_name_translations'] = array(
					'#type' => 'fieldset',
					'#title' => t(
						'Synonyms @language',
		array('@language' => $language->native)),
					'#attributes' => array(
							'id' => 'organism-synonyms-localized' // required for CSS
		)
		);
		if (isset(
		$organismdata['classification_name_translations'][$current_language])) {
			$render_array['synonyms-container']['classification_name_translations']['synonyms'] = array(
						'#markup' => implode(
							'<br/>',
			$organismdata['classification_name_translations'][$current_language])
			);
		} else {
			$render_array['synonyms-container']['classification_name_translations']['synonyms'] = array(
						'#markup' => t('None available')
			);
		}
	}

	if (function_exists('gallery_list_renderer')) {
		$render_array['ImageGallery'] = gallery_list_renderer(
			'images',
			'organismreference',
			$organismdata['id'],
			false,
			3,
			false);
		
		$render_array['div'] = array(
				'#type' => 'fieldset',
				'#title' => t('Map'),
				'#weight' => 4
		);
		$render_array['div']['map'] = array(
				'#theme' => 'area',
				'#ch1903' => true,
				'#search' => true,
				'#mapid' => 'observationmap',
				'#geometries_fetch_url' => base_path()
						. 'observation/geometry/json',
				'#geometries_autoload' => false,
				'#infowindow_content_fetch_url_observation' => base_path()
						. 'observation/{ID}/overview/ajaxform',
				'#height' => '300px'
		);
		$render_array['div']['space'] = array(
				'#type' => 'markup',
				'#markup' => '<p/>'
		);
		$render_array['div']['area_table'] = array(
				'#theme' => 'datatable',
				'#header' => observation_get_standard_datatable_headers(
					9,
					'desc'),
				'#title' => t('Observations'),
				'#id_table' => 'observations',
				'#tableWidth' => 917,
				'#options' => array(
						'jsonUrl' => base_path() . "observation/get/organism/" . $organismdata['id'],
						'rowClick' => 'rowClick',
						'rowClickHandler' => '
						function rowClick(celDiv, id) {
						jQuery(celDiv).click(
						function() {
						observationmap.selectObservation(id);
	}
				)
	}',
						'rpShowAll' => true,
						'preProcessHandler' => 'observation.tablePreProcess',
						'onSuccessHandler' => (($user->uid == 0) ? ''
								: 'observation.onTableSuccess'),
						/* only show the delete and export buttons if user is logged in */
						'gallery_enabled' => true,
						'gallery_image_sources' => array(
								'selected' => array(
										'value' => DATATABLE_IMGSRC_BELEG,
										'option' => t('only proof images')
								),
								array(
										'value' => DATATABLE_IMGSRC_PORTRAIT,
										'option' => t('only portrait images')
								),
								array(
										'value' => DATATABLE_IMGSRC_BELEG_PORTRAIT,
										'option' => t(
											'proof images, then portrait images')
								),
								array(
										'value' => DATATABLE_IMGSRC_PORTRAIT_BELEG,
										'option' => t(
											'portrait images, then proof images')
								)
						),
						// Filters to add to the filterDiv above the table. The gallery image source
						// filter has a weight of 10, the table/gallery buttons 4 and 5 respectively.
						'custom_filter' => array(
								'acl_filter' => array(
										'#type' => 'select',
										'#name' => 'acl_filter',
										'#title' => t('Observations') . ':',
										'#options' => array(
												'all' => t('Public'),
												'own' => t('Own'),
												'writeable' => t('Editables')
										),
										'#attributes' => array(
												'class' => array(
														'acl_filter'
												)
										),
										'#weight' => 8
								)
						)
				)
		);
		$render_array['VideoGallery'] = gallery_list_renderer(
			'videos',
			'organismreference',
			$organismdata['id'],
			false,
			5,
			true);
		$render_array['SoundGallery'] = gallery_list_renderer(
			'sounds',
			'organismreference',
			$organismdata['id'],
			false,
			6,
			true);
	}
	
	$render_array['file_overview'] = organism_files($organismdata['id'], 7, false);

	return $render_array;
}

/**
 * Return the title of an organism. Include the localized name if available and link the text
 * to wikipedia if an article about this organism exists.
 * @param unknown_type $organismdata
 * @return String title, may contain HTML
 */
function organism_get_organism_title($organismdata) {
	global $user;
	global $language;
	// Make sure we get a language
	$langcode = isset($user->language) ? $user->language : $language->language;

	$logo_wikipedia_url = base_path()
			. drupal_get_path('module', 'commonstuff')
			. '/images/wikipedia.png';
	$scientific_name = $organismdata['scientific_name']; 
	$title = $scientific_name;
	if (isset($organismdata['classification_name_translations'][$langcode])
			&& !empty(
				$organismdata['classification_name_translations'][$langcode])) {
		$title .= ' / '
				. reset(
					$organismdata['classification_name_translations'][$langcode]);
	}

	// if wikipedia article existing, linkt to it and show a wikipedialogo
	if ($link = organism_check_if_wikipedia_article_exists($organismdata)) {
		$alt_text = t('Wikipedia article available');
		$title = "<a href='$link' target='_blank' title='$alt_text'>$title <img style='height: 1em' src='$logo_wikipedia_url'/></a>";
	}

	return $title;
}
?>
