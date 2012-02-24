<?php
/**
 * @file organism.overview.inc
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
			'hide' => false,
	);
	$tableHeader[] = array(
			'name' => t('Classificator name'),
			'dbfield' => 'oc_name'
	);
	$tableHeader[] = array(
			'name' => t('# of levels'),
			'dbfield' => 'levelcount',
	);

	$render_array['classificators'] = array(
			'#theme' => 'datatable',
			'#header' => $tableHeader,
			'#title' => 'Classifiers',
			'#id_table' => 'classificators',
			'#options' => array(
					'jsonUrl' => "'" . base_path() . "organism/json'",
					'rowClick' => 'rowClick',
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
 * @todo Find a better title
 */
function organism_show_classification($classificationid) {
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
			'dbfield' => 'oc_id',
			'hide' => false
	);
	$classificationTableHeader[] = array(
			'name' => t('Classificator name'),
			'dbfield' => 'oc_name',
			'hide' => false
	);
	$classificationTableHeader[] = array(
			'name' => t('Classification level'),
			'dbfield' => 'classification_level_id',
			'hide' => false
	);
	$render_array['classificators'] = array(
			'#theme' => 'datatable',
			'#title' => $classificationName,
			'#header' => $classificationTableHeader,
			'#id_table' => 'classificators',
			'#options' => array(
					'jsonUrl' => "'" . base_path()
							. "organism/classification/$classificationid/classificationjson'",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id) {
											jQuery(celDiv).click(
												function() {
													window.location.href = Drupal.settings.basePath+'organism/classification/' + id;
												}
											)
										}"
			)
	);

	// Set the table header
	$organismsTableHeader[] = array(
			'name' => t('Organism id'),
			'dbfield' => 'o_id',
			'idField' => true,
			'hide' => false
	);
	$organismsTableHeader[] = array(
			'name' => t('Organism scientific name id'),
			'dbfield' => 'osn_id',
			'hide' => false
	);
	$organismsTableHeader[] = array(
			'name' => t('Organism scientific name'),
			'dbfield' => 'osn_name',
			'hide' => false
	);
	$render_array['organisms'] = array(
			'#theme' => 'datatable',
			'#title' => 'Organisms with classification ' . $classificationName,
			'#header' => $organismsTableHeader,
			'#id_table' => 'organisms',
			'#options' => array(
					'jsonUrl' => "'" . base_path()
							. "organism/classification/$classificationid/organismjson'",
					'rowClick' => 'rowClickOrganism',
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
 * @todo Refactor this stuff into its own function
 * @return array
 */
function organism_show_organism($organismid) {
	$organism = organism_get_single_name_of_organism($organismid);
	drupal_set_title(t('Show attributes and synonyms of ' . $organism['name']));
	// include the datatable javascript
	drupal_add_js(
		drupal_get_path('module', 'organism')
				. '/js/organism.datatable.select.js');
	$render_array = array();

	// show all attributes of this organism
 {
		$attributeresult = db_query(
			'SELECT
					DISTINCT oa.name, oa.valuetype, oa.id
				FROM
					drupal_organism_attribute oa
					JOIN {organism_attribute_value} oav ON oav.organism_attribute_id = oa.id
					JOIN {organism_attribute_value_subscription} oavs ON oavs.organism_attribute_value_id = oav.id
					JOIN {organism_classification_subscription} ocs ON oavs.organism_id = ocs.organism_id
					JOIN {organism_classification} oc ON oc.id = ocs.organism_classification_id
				WHERE
					oavs.organism_id = :organismid
				GROUP BY
					oa.name,
					oa.valuetype,
					oa.id;',
			array(':organismid' => $organismid));

		if ($attributeresult->rowCount() > 0) {
			$render_array['attributes'] = array(
					'#type' => 'fieldset',
					'#title' => t('Attributes'),
					'#tree' => TRUE
			);
			foreach ($attributeresult as $attribute) {
				$attributevaluetype = $attribute->valuetype;
				$attributeid = $attribute->id;
				$attributename = $attribute->name;

				$render_array['attributes'][$attributename] = array(
						'#type' => 'markup',
						'#markup' => "<b>$attributename</b>: ",
				);

				$attributevalueresult = db_query(
					'SELECT
							oav.text_value,
							oav.boolean_value,
							oav.number_value
						FROM
							drupal_organism_attribute oa
							JOIN {organism_attribute_value} oav ON oav.organism_attribute_id = oa.id
							JOIN {organism_attribute_value_subscription} oavs ON oavs.organism_attribute_value_id = oav.id
							JOIN {organism_classification_subscription} ocs ON oavs.organism_id = ocs.organism_id
							JOIN {organism_classification} oc ON oc.id = ocs.organism_classification_id
						WHERE
							oavs.organism_id = :organismid AND oa.id = :attributeid',
					array(':organismid' => $organismid,
							':attributeid' => $attributeid
					));
				foreach ($attributevalueresult as $attributevalue) {
					if ($attributevaluetype == 'n') {
						$render_array['attributes'][$attributename]['#markup'] .= $attributevalue->number_value;
					} else if ($attributevaluetype == 'b') {
						$render_array['attributes'][$attributename]['#markup'] .= $attributevalue->boolean_value;
					} else if ($attributevaluetype == 't') {
						$render_array['attributes'][$attributename]['#markup'] .= $attributevalue->text_value;
					}
				}
			}
		}
	}

	// Show all synonyms of this organism
 {
		// Set the table header
		$tableHeader[] = array(
				'name' => t('Organism id'),
				'dbfield' => 'o_id',
				'idField' => true,
				'hide' => false
		);
		$tableHeader[] = array(
				'name' => t('Organism scientific name id'),
				'dbfield' => 'osn_id',
				'idField' => false,
				'hide' => false
		);
		$tableHeader[] = array(
				'name' => t('Organism scientific name'),
				'dbfield' => 'osn_name',
				'idField' => false,
				'hide' => false
		);
		$render_array['organisms'] = array(
				'#theme' => 'datatable',
				'#title' => t('Synonyms for') . ' ' . $organism['name'],
				'#header' => $tableHeader,
				'#id_table' => 'organisms',
				'#options' => array(
						'jsonUrl' => "'" . base_path()
								. "organism/$organismid/json'"
				)
		);
	}
	return $render_array;
}
?>
