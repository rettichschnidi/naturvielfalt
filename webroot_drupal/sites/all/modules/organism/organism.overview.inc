<?php
/**
 * @file organism.overview.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

/**
 * Menu callback. This will be called for visits to /organism.
 * @return array
 */
function organism_show_classifiers() {
	drupal_set_title(t('Select classifier'));

	// Set the table header
	$tableHeader[] = array(
			'name' => t('Internal Id'),
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
			'#id_table' => 'classificators',
			'#options' => array(
					'jsonUrl' => "'" . base_path() . "organism/json'",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, oc_id) {
												jQuery(celDiv).click(
													function() {
														window.location.href = Drupal.settings.basePath+'organism/classification/'+celDiv.innerHTML;
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
	drupal_set_title(t('Classifications'));
	// Extract the name of the current classification
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
			drupal_set_title(
				t('Classification') . ' '
						. $resultClassificationObject->oc_name);
		}
	}

	// Set the table header
	$classificationTableHeader[] = array(
			'name' => t('Internal id'),
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
			'#header' => $classificationTableHeader,
			'#id_table' => 'classificators',
			'#options' => array(
					'jsonUrl' => "'" . base_path()
							. "organism/classification/$classificationid/classificationjson'",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
									window.location.href = Drupal.settings.basePath+'organism/classification/'+celDiv.innerHTML;})}"
			)
	);

	// Set the table header
	$organismsTableHeader[] = array(
			'name' => t('Organism scientific id'),
			'dbfield' => 'ABCDS',
			'hide' => false
	);
	$organismsTableHeader[] = array(
			'name' => t('Organism scientific name'),
			'dbfield' => 'osc_name',
			'hide' => false
	);
	$organismsTableHeader[] = array(
			'name' => t('Organism Name'),
			'dbfield' => 'o_id',
			'hide' => false
	);
	$render_array['organisms'] = array(
			'#theme' => 'datatable',
			'#header' => $organismsTableHeader,
			'#id_table' => 'organisms',
			'#options' => array(
					'jsonUrl' => "'" . base_path()
							. "organism/classification/$classificationid/organismjson'",
					'rowClick' => 'rowClick',
					'rowClickHandler' => "function rowClick(celDiv, id){jQuery(celDiv).click(function() {
										window.location.href = Drupal.settings.basePath+'organism/'+celDiv.innerHTML;})}"
			)
	);

	// 	// If this classification has organisms attached, then show those organisms
	//  {
	// 		$resultOrganism = db_query(
	// 			'SELECT
	// 			osc.id AS name_id,
	// 			osc.name AS name,
	// 			o.id AS organism_id
	// 		FROM
	// 			{organism_classification} oc
	// 			JOIN {organism_classification_subscription} ocs ON ocs.organism_classification_id = oc.id
	// 			JOIN {organism} o ON ocs.organism_id = o.id
	// 			JOIN {organism_scientific_name} osc ON osc.organism_id = o.id
	// 		WHERE
	// 			oc.id = :ocid',
	// 			array(':ocid' => $classificationid));

	// 		if ($resultOrganism->rowCount() > 0) {
	// 			$header = array(
	// 					t('Organism'),
	// 					'organism id'
	// 			);

	// 			foreach ($resultOrganism as $record) {
	// 				$row = array();
	// 				$row[] = $record->name;
	// 				$row[] = $record->organism_id;
	// 				$rows[] = array(
	// 						'data' => $row,
	// 						'id' => 'organism_' . $record->organism_id
	// 				);
	// 			}
	// 			$render_array['organismsClassificationViewOrganisms'] = array(
	// 					'#theme' => 'datatable',
	// 					'#header' => $header,
	// 					'#rows' => $rows,
	// 					'#id_table' => 'organisms',
	// 			);
	// 		}
	// 	}
	return $render_array;
}

/**
 * Menu callback. This will be called for visits to /organism/%.
 * @return array
 */
function organism_show_organism($organismid) {
	drupal_set_title(t('Show specific organisms'));
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
		$header = array(
				t("Synonyms"),
				'id'
		);

		$result = db_query(
			'SELECT osc.id, osc.name FROM {organism_scientific_name} osc WHERE osc.organism_id = :oscid LIMIT 1',
			array(':oscid' => $organismid));

		if ($record = $result->fetchObject()) {
			drupal_set_title(t('Show organism') . ' ' . $record->name);
		}
		$result = db_query(
			'SELECT osc.id, osc.name FROM {organism_scientific_name} osc WHERE osc.organism_id = :oscid',
			array(':oscid' => $organismid));

		if ($result->rowCount() > 0) {
			foreach ($result as $record) {
				$row = array();
				$row[] = $record->name;
				$row[] = $organismid;
				$rows[] = array(
						'data' => $row,
						'id' => 'organism_' . $organismid
				);
			}

			$render_array['organismsShowOrganisms'] = array(
					'#theme' => 'datatable',
					'#header' => $header,
					'#rows' => $rows,
					'#id_table' => 'organisms',
			);
		}
	}

	return $render_array;
}
?>
