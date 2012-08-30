<?php
/**
 * @file organism.show.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'organism', 'organism');

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
				'href' => 'organism/classification/' . $classifier->oc_id,
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
	// Extract the name of the current classification
	$classificationName = 'Classifications';
 {
		$resultClassification = db_query(
			'SELECT
				oc.id AS oc_id,
				oc.name AS oc_name
			FROM
				{organism_classification} oc
			WHERE
				oc.id = :ocid',
			array(':ocid' => $classificationid));

		$render_array = array();

		if ($resultClassification->rowCount() == 1) {
			$resultClassificationObject = $resultClassification->fetchObject();
			$classificationName = $resultClassificationObject->oc_name;
		}
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
			'name' => t('Classificator name'),
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
	$render_array['classificators'] = array(
			'#theme' => 'datatable',
			'#title' => t('Subclassifications within') . " $classificationName",
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
										}"
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
				'width' => 200
		);
	}

	$organismsTableHeader[] = array(
			'name' => t('Scientific name'),
			'dbfield' => 'scientific_name',
			'dbExactField' => 'scientific_name',
			'hide' => false,
			'width' => 300
	);

	$organismsTableHeader[] = array(
			'name' => t('Common name'),
			'dbfield' => 'ol_name',
			'dbExactField' => 'ol_name',
			'hide' => false,
			'width' => 200
	);

	$render_array['organisms'] = array(
			'#theme' => 'datatable',
			'#title' => t('Species which are in classification ')
					. $classificationName,
			'#header' => $organismsTableHeader,
			'#id_table' => 'organisms',
			'#options' => array(
					'jsonUrl' => base_path()
							. "organism/classification/$classificationid/organismjson",
					'rowClick' => 'rowClickOrganism',
					'tableWidth' => 900,
					'rowClickHandler' => "function rowClickOrganism(celDiv, id){
												jQuery(celDiv).click(
													function() {
														window.location.href = Drupal.settings.basePath+'organism/' + id;
													}
												)
											}"
			)
	);
	return $render_array;
}

/**
 * Menu callback. This will be called for visits to /organism/%.
 * @return array
 */
function organism_show_organism($organismdata) {
	drupal_add_css(drupal_get_path('module', 'organism') . '/css/organism.css');
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
	// include the datatable javascript
	$render_array = array();

	if (function_exists('gallery_list_renderer')) {
		$render_array['ImageGallery'] = gallery_list_renderer(
			'images',
			'organismreference',
			$organismdata['id'],
			false,
			-1,
			false);
		$render_array['VideoGallery'] = gallery_list_renderer(
			'videos',
			'organismreference',
			$organismdata['id'],
			false,
			-1,
			true);
		$render_array['SoundGallery'] = gallery_list_renderer(
			'sounds',
			'organismreference',
			$organismdata['id'],
			false,
			-1,
			true);
	}
	$render_array = array_merge(
		$render_array,
		organism_files($organismdata['id']));

	$render_array['attributes'] = array(
			'#type' => 'fieldset',
			'#title' => t('Attributes'),
			'#tree' => TRUE,
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
