<?php
/**
 * @file organism.show.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'organism');
module_load_include('inc', 'observation');

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
	$output['helptext'] = array(
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
	$links = array();
	foreach ($classifiers as $classifier) {
		$classifier_title = $classifier->oc_name;
		if (isset($kingdoms[$classifier_title])) {
			$title = t($kingdoms[$classifier_title]);
		}
		$species = format_plural($classifier->organismcount_cnt, '1 Species', '@count Species');
		$list[$classifier_title] = array(
				'title' => '<div class="classifier-div"><span class="classifier-title">'.$title . '</span></br> <span class="classifier-amount"> (' . $species . ') </span></div>',
				'href' => 'organism/classification/' . $classifier->oc_id
		);
	}

	// using drupal_get_path() and images is not working when the page is localized
	$path = base_path() . 'sites/all/modules/organism';
    $module_path = drupal_get_path('module', 'organism');
	drupal_add_css($module_path . '/css/organism.css');
	drupal_add_js($module_path . '/js/organism.js');
	
	global $base_url;
	$output['diagram'] = array(
			'#type' => 'markup',
			'#prefix' => '<div id="diagram-wrapper">',
			'#suffix' => '</div>',
			'#markup' => '
				<div id="diagram-container">
						  <img src="'.$path.'/img/gesamt.png" id="diagram-background">
						  <img src="'.$path.'/img/gesamt.png" usemap="#diagram_map" id="diagram-image">
						  <map name="diagram_map" id="diagram_map">
							<area shape="poly" coords="275,10, 276,256,461,92,420,55,375,28,318,12,276,7" href="'.$list['CRSF']['href'].'" id="area_pflanzen">
							<area shape="poly" coords="276,257,461,94,489,132,503,165,519,199,524,242,521,288,522,288" href="'.$list['Fungus']['href'].'" id="area_pilze">
							<area shape="poly" coords="276,258,477,405,501,360,518,313,520,289" href="'.$list['Swisslichens']['href'].'" id="area_flechten">
							<area shape="poly" coords="275,258,383,483,443,442,477,405" href="'.$list['NISM']['href'].'" id="area_moose">
							<area shape="poly" coords="276,10,276,256,384,484,303,506,226,502,149,475,84,417,46,358,31,301,26,261,29,201,61,119,135,44,225,14,262,9,276,9" href="'.$list['CSCF']['href'].'" id="area_tiere">
						  </map>
				<a id="pflanzen-link" href="'.$list['CRSF']['href'].'"> '.$list['CRSF']['title'].'</a>
				<a id="pilze-link" href="'.$list['Fungus']['href'].'"> '.$list['Fungus']['title'].'</a>
				<a id="flechten-link" href="'.$list['Swisslichens']['href'].'"> '.$list['Swisslichens']['title'].'</a>
				<a id="moose-link" href="'.$list['NISM']['href'].'"> '.$list['NISM']['title'].'</a>
				<a id="tiere-link" href="'.$list['CSCF']['href'].'"> '.$list['CSCF']['title'].'</a>
			 	</div>'
	);
	
	return $output;

}

/**
 * Menu callback. This will be called for visits to /organism/classification/%.
 * @return array
 */
function organism_show_classification($classificationid) {
	assert($classificationid != false);
	
	module_load_include('inc', 'datatable', 'datatable.user');
	
	// fetch classification data
	$resultClassification = db_query(
		'SELECT
			oc.parent_id AS oc_parent_id,
			oc.prime_father_id AS oc_prime_father_id,
			(SELECT oc_prime.name 
				FROM {organism_classification} oc_prime 
				WHERE oc_prime.id = oc.prime_father_id) AS prime_father_name,
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
	if($classificationid == $classificationObj->oc_prime_father_id) {
		$hierarchyLevelUpButtons = '</div>'
				.'<div class="mUp mButton" data-cid="navigateToMenu"><span /></div>'
				.'<div class="mTop mButton" data-cid="navigateToMenu"><span />';
	} else {
		$hierarchyLevelUpButtons = '</div>'
				.'<div class="mUp mButton" data-cid="' . $classificationObj->oc_parent_id . '"><span /></div>'
				.'<div class="mTop mButton" data-cid="' . $classificationObj->oc_prime_father_id . '"><span />';
	}
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
	
	//apply user settings
	datatable_apply_user_settings(DATATABLE_CLASSIFIERS, $classificationTableHeader);
	$render_array = array();

	$render_array['classificators'] = array(
			'#theme' => 'datatable',
			'#title' => t('Subclassifications within') . ' ' . $classificationObj->oc_name . $hierarchyLevelUpButtons,
			'#header' => $classificationTableHeader,
			'#id_table' => DATATABLE_CLASSIFIERS,
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
	
	//apply user settings
	$oc_prime_father = strtolower($classificationObj->prime_father_name);
	datatable_apply_user_settings('organisms_'.$oc_prime_father, $organismsTableHeader);

	$render_array['organisms'] = array(
			'#theme' => 'datatable',
			'#title' => t('Species which are in classification ') . $classificationObj->oc_name,
			'#header' => $organismsTableHeader,
			'#id_table' => 'organisms_'.$oc_prime_father,
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
	
	drupal_add_css(drupal_get_path('module', 'organism') . '/css/organism.css');
	drupal_add_js('jQuery(document).ready(function () { jQuery("#files a").attr("target", "_blank"); });', 'inline');
	global $user;
	global $language;
	$current_language = $language->language;

	if (empty($organismdata)) {
		drupal_set_title(t('No such species available'));
		$render_array['nosuchorganism'] = null;
		return $render_array;
	}
	drupal_set_title(organism_get_organism_title($organismdata), PASS_THROUGH);
	
	$render_array = array();
	
	$render_array['message'] = array(
			'#type'   => 'markup',
			'#markup' => '
			<div id="message" style="display: none; height: auto;">
				<div class="messages status"></div>
			</div>'
	);
	
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
		
		$observationCount = get_number_of_observations_for_organism($organismdata['id']);
		if($observationCount > 0) {
			drupal_add_css($observation_path . '/css/observation.css');
			drupal_add_js($observation_path . '/js/observation.js');
			drupal_add_library('system', 'ui.dialog');
			
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
					'#header' => observation_get_organism_datatable_headers(
						9,
						'desc'),
					'#title' => t('Observations'),
					'#id_table' => DATATABLE_OBSERVATION,
					'#tableWidth' => 917,
					'#options' => array(
							'jsonUrl' => base_path() . "observation/get/organism/" . $organismdata['id'] . "/all",
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
							),
							'date' => array(
									'dbDateField' => "o.date",
									'dbDateFieldType' => DBDateFieldType::INTEGER)
					)
			);
		} else {
			$render_array['div']['noObservationsFound'] = array(
					'#type' => 'markup',
					'#markup' => t('No observations found for this species')
			);
		}
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
	$langcode = $language->language;

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