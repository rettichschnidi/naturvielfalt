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
	// Set the table header
	$tableHeader[] = array(
			'name' => t('Internal Id'),
			'idField' => true,
			'dbfield' => 'oc_id',
			'hide' => true,
			'width' => 100,
	);
	$tableHeader[] = array(
			'name' => t('Classificator name'),
			'dbfield' => 'oc_name',
			'width' => 400,
	);
	$tableHeader[] = array(
			'name' => t('# of levels'),
			'dbfield' => 'levelcount',
			'hide' => true,
			'width' => 100,
	);
	$tableHeader[] = array(
			'name' => t('# of organisms'),
			'dbfield' => 'cnt',
			'width' => 100,
	);

	$render_array['classificators'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#title' => 'Classifiers',
			'#id_table' => 'classificators',
			'#options' => array(
					'jsonUrl' => "'" . base_path() . "organism/json'",
					'rowClick' => 'rowClick',
					'tableWidth' => 900,
					'tableHeight' => 200,
					'rowClickHandler' => "function rowClick(celDiv, id) {
												jQuery(celDiv).click(
													function() {
														window.location.href = Drupal.settings.basePath+'organism/classification/' + id;
													}
												)
											}"
			)
	);
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
			'width' => 760,
			'hide' => false
	);
	$classificationTableHeader[] = array(
			'name' => t('Number of organism'),
			'dbfield' => 'cnt',
			'width' => 100,
			'hide' => false
	);
	$render_array['classificators'] = array(
			'#theme' => 'datatable',
			'#title' => t('Subclassifications within') . " $classificationName",
			'#header' => $classificationTableHeader,
			'#id_table' => 'classificators',
			'#options' => array(
					'jsonUrl' => "'" . base_path()
							. "organism/classification/$classificationid/classificationjson'",
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
			'#markup' => '<h1>' . t('Organisms') . '</h1>',
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
				'name' => t(ucfirst($value->name)),
				'dbfield' => 'oc_' . $value->name,
				'hide' => false,
				'width' => 200
		);
	}

	$organismsTableHeader[] = array(
			'name' => t('Latin Name'),
			'dbfield' => 'osn_name',
			'hide' => false,
			'width' => 300
	);

	$organismsTableHeader[] = array(
			'name' => t('Localized Name'),
			'dbfield' => 'ol_name',
			'hide' => false,
			'width' => 200
	);

	$render_array['organisms'] = array(
			'#theme' => 'datatable',
			'#title' => 'Organisms which are in classification '
					. $classificationName,
			'#header' => $organismsTableHeader,
			'#id_table' => 'organisms',
			'#options' => array(
					'jsonUrl' => "'" . base_path()
							. "organism/classification/$classificationid/organismjson'",
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

function organism_attributes_form($form, &$form_state, $organism) {
}

/**
 * Menu callback. This will be called for visits to /organism/%.
 * @return array
 */
function organism_show_organism($organism) {
	if (empty($organism)) {
		drupal_set_title(t('No such organism available'));
		$render_array['nosuchorganism'] = null;
		return $render_array;
	}
	drupal_set_title(
		t('Show attributes and synonyms of ' . $organism['scientific_names'][0]));
	// include the datatable javascript
	$render_array = array();

	if (function_exists('gallery_list_renderer')) {
		$render_array['ImageGallery'] = gallery_list_renderer(
			'images',
			'organismreference',
			$organism['id'],
			false,
			-1,
			false);
		$render_array['VideoGallery'] = gallery_list_renderer(
			'videos',
			'organismreference',
			$organism['id'],
			false,
			-1,
			true);
		$render_array['SoundGallery'] = gallery_list_renderer(
			'sounds',
			'organismreference',
			$organism['id'],
			false,
			-1,
			true);
	}
	// 	$render_array = array_merge($render_array, organism_attributes($organism));
	// 	$render_array = array_merge($render_array, getOrganismInventories($organism));
	$render_array = array_merge($render_array, organism_files($organism['id']));

	$attributes = $organism['attributes'];

	$render_array['attributes'] = array(
			'#type' => 'fieldset',
			'#title' => t('Attributes'),
			'#tree' => TRUE
	);

	if (!empty($attributes)) {
		foreach ($attributes as $attribute_name => $data) {
			$attributevaluetype = $data['valuetype'];
			$attributename = $attribute_name;

			$render_array['attributes'][$attributename] = array(
					'#type' => 'markup',
					'#markup' => "<b>$attributename</b>: ",
			);
			$attributevalues = $data['values'];
			/**
			 * This is untested, but should generate a reasonable output 
			 * even if there is more than a single value
			 */
			$render_array['attributes'][$attributename]['#markup'] .= implode(
				', ',
				$attributevalues) . "<br/>";
		}
	} else {
		$render_array['attributes']['noattribute'] = array(
				'#type' => 'markup',
				'#markup' => '<b>' . t("No attributes set for this organism")
						. '</b>',
		);
	}

	/**
	 * Show all synonyms of this organism.
	 */
	if (count($organism['scientific_names']) > 1) {
		// Set the table header
		$tableHeader[] = array(
				'name' => t('Organism id'),
				'dbfield' => 'o_id',
				'idField' => true,
				'hide' => true,
				'width' => 30
		);
		$tableHeader[] = array(
				'name' => t('Organism scientific name'),
				'dbfield' => 'osn_name',
				'idField' => false,
				'hide' => false,
				'width' => 800
		);

		foreach ($organism['scientific_names'] as $synonym) {
			$row = array();
			$row[] = $organism['id'];
			$row[] = $synonym;
			$rows[] = $row;
		}

		$render_array['organisms'] = array(
				'#theme' => 'datatable',
				'#title' => t('Synonyms for') . ' '
						. $organism['scientific_names'][0],
				'#header' => $tableHeader,
				'#id_table' => 'organism_synonyms',
				'#rows' => $rows
		);
	}
	return $render_array;
}
?>
