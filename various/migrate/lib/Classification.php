<?php
/**
 * @copyright Naturwerk
 * @author Reto Schneider, 2012
 */

require_once('MDB2.php');
require_once(dirname(__FILE__) . '/Db.php');

/**
 * A simple class to do an initial import of organisms to the naturvielfalt db.
 * 
 * Example:
 * $organism = array(
 * 		'classificator' => 'CSCF-xy',
 * 		'classifications' => array(
 * 				array(
 * 						'classificationlevelname' => 'family',
 * 						'classificationname' => 'Lycosidae',
 * 				),
 * 				array(
 * 						'classificationlevelname' => 'genus',
 * 						'classificationname' => 'Acantholycosa',
 * 				)
 * 		),
 * 		'species' => 'cordicollis',
 * 		'subspecies' => 'cordicollis',
 * 		'scientific_names' => array(
 * 				'Acantholycosa pedestris',
 * 				'Acantholycosa pedestris v2'
 * 		),
 * 		'classification_name_translations' => array(
 * 				'de' => array(
 * 						'Schattenkraut',
 * 				),
 * 				'en' => array(
 * 						'Shadowherb'
 * 				)
 * 		),
 * 		'attributes' => array(
 * 				'author' => array(
 * 						'valuetype' => 't',
 * 						'values' => array(
 * 								'Reto'
 * 						),
 * 				),
 * 				'NUESP' => array(
 * 						'valuetype' => 'n',
 * 						'values' => array(
 * 								1234
 * 						),
 * 				)
 * 		),
 * );
 * 
 * $importer = new Classification($driver, $name, $user, $password, $host);
 * $importer->addOrganism($organism);
 */
class Classification {
	var $db = NULL;
	var $drupalprefix = '';
	var $attributes = array();
	var $classifier_name = false;
	var $classifier_id = NULL;
	var $classifier_level_id = false;
	var $classification_table = '';
	var $classification_level_table = '';
	var $organism_table = '';
	var $scientific_name_table = '';
	var $organism_lang_table = '';
	var $organism_classification_subscription_table = '';
	var $organism_attribute_table = '';
	var $organism_attribute_value_table = '';
	var $organism_attribute_value_subscription_table = '';

	// Maps
	var $tree;
	var $scientific_names = array();
	var $organism_import_id = array();

	var $outstanding_organisms = array();

	public function selectDb($driver, $name, $user, $password, $host) {
		if ($this->db != NULL) {
			$this->db = NULL;
		}
		$this->db = new Db(
			$driver,
			$name,
			$user,
			$password,
			$host);
	}

	public function __construct($driver, $name, $user, $password, $host) {
		$this->selectDb($driver, $name, $user, $password, $host);
		global $drupalprefix;
		$this->drupalprefix = $drupalprefix;
		$this->classification_table = $drupalprefix . 'organism_classification';
		$this->classification_level_table = $drupalprefix
				. 'organism_classification_level';
		$this->scientific_name_table = $drupalprefix
				. 'organism_scientific_name';
		$this->organism_table = $drupalprefix . 'organism';
		$this->organism_lang_table = $drupalprefix . 'organism_lang';
		$this->organism_classification_subscription_table = $drupalprefix
				. 'organism_classification_subscription';
		$this->organism_attribute_table = $drupalprefix . 'organism_attribute';
		$this->organism_attribute_value_table = $drupalprefix
				. 'organism_attribute_value';
		$this->organism_attribute_value_subscription_table = $drupalprefix
				. 'organism_attribute_value_subscription';
		$this->tree = array();
	}

	public function startTransactionIfPossible() {
		$this->db
			->startTransactionIfPossible();
	}

	public function stopTransactionIfPossible() {
		$this->db
			->stopTransactionIfPossible();
	}

	public function addOrganisms(array $organisms) {
		$i = 0;
		$start = microtime(true);
		$this->db
			->startTransactionIfPossible();
		foreach ($organisms as $organism) {
			if (++$i % 100 == 0) {
				$current = microtime(true);
				print "$i of " . count($organisms) . ", ";
				print "Time: " . ($current - $start) . "s\n";
				$start = $current;
				if ($i % 1000 == 0) {
					// 					print "Committing to db.\n";
					$this->db
						->stopTransactionIfPossible();
					$this->db
						->startTransactionIfPossible();
				}
			}

			if (isset($organism['parent_import_id'])) {
				if (isset(
					$this->organism_import_id[$organism['parent_import_id']])) {
					$this->addOrganism($organism);
				} else {
					print 
						"Parent Id not yet importet: "
								. $organism['parent_import_id'] . "\n";
					$this->outstanding_organisms[$organism['import_id']] = $organism;
					assert(false);
				}
			} else {
				$this->addOrganism($organism);
			}
		}
		$this->db
			->stopTransactionIfPossible();
	}

	private function addOrganism(array $organism) {
		global $errors;
		$this->classifier_id = NULL;
		$this->classifier_name = $organism['classificator'];
		$classifierName = $this->classifier_name;
		if (key_exists($classifierName, $this->tree)) {
			$classifierLevelId = $this->tree[$classifierName]['classificationlevelid'];
			$classifierId = $this->tree[$classifierName]['classificationid'];
		} else {
			$classifierLevelId = $this->getOrCreateClassifierLevel(
					$classifierName);
			$classifierId = $this->getOrCreateClassifier(
					$classifierName,
					$classifierLevelId);
			$this->tree[$classifierName] = array(
					'classificationlevelid' => $classifierLevelId,
					'classificationid' => $classifierId,
					'classificationlevelvalues' => array(),
					'classificationvalues' => array()
			);
		}
		$this->classifier_level_id = $classifierLevelId;
		$this->classifier_id = $classifierId;
		assert($this->classifier_id != NULL);
		assert(empty($errors));

		// add all levels
		$parentClassificationLevelId = $classifierLevelId;
		$parentClassificationId = $classifierId;
		$classificationLevels = &$this->tree[$classifierName]['classificationlevelvalues'];
		$currentClassifications = &$this->tree[$classifierName]['classificationvalues'];
		$classificationId = NULL;

		//create classifications
		foreach ($organism['classifications'] as $classification) {
			$classificationLevelName = $classification['classificationlevelname'];
			// create classification levels //
			if (key_exists($classificationLevelName, $classificationLevels)) {
				$classificationLevelId = $classificationLevels[$classificationLevelName];
			} else {
				print 
					"Miss for classification level: $classificationLevelName\n";
				$classificationLevelId = $this->getOrCreateClassificationLevelName(
						$classificationLevelName,
						$parentClassificationLevelId);
				$classificationLevels[$classificationLevelName] = $classificationLevelId;
			}
			$parentClassificationLevelId = $classificationLevelId;

			// create classification //
			if (isset($classification['classificationname'])) {
				$classificationName = $classification['classificationname'];
				if (key_exists($classificationName, $currentClassifications)) {
					//print "Hit for classification: $classificationName\n";
					$classificationId = $currentClassifications[$classificationName]['id'];
				} else {
					// print "Miss for classification: $classificationName\n";
					$classificationId = $this->getOrCreateClassification(
							$classificationName,
							$parentClassificationId,
							$classificationLevelId);
					$currentClassifications[$classificationName]['id'] = $classificationId;
					$currentClassifications[$classificationName]['values'] = array();
				}
				$currentClassifications = &$currentClassifications[$classificationName]['values'];
				$parentClassificationId = $classificationId;

				if ($classificationId == NULL) {
					print_r($classification);
					assert(false);
				}
			}
		}

		// Create organism
		$organismId = NULL;
		if (isset($organism['scientific_names'])) {
			if ($classificationId == NULL) {
				print_r($organism);
				assert(false);
			}
			$scientificNames = $organism['scientific_names'];
			assert(count($scientificNames) == 1); // synonyms not yet implemented
			if (isset($organism['import_id'])) {
				if (isset($this->organism_import_id[$organism['import_id']])) {
					$parentId = $this->organism_import_id[$organism['import_id']];
				} else {
					assert(false);
				}
			} else {
				$parentId = NULL;
			}
			$organismId = $this->getOrCreateOrganism(
					$scientificNames,
					$classificationId,
					$parentId);

			assert($organismId != NULL);

			if (isset($organism['import_id'])) {
				$this->organism_import_id[$organism['import_id']] = $organismId;
			}

			// create translation for organisms
			if (isset($organism['classification_name_translations'])) {
				foreach ($organism['classification_name_translations'] as $lang => $classificationNameTranslations) {
					foreach ($classificationNameTranslations as $classificationNameTranslation) {
						$this->getOrCreateClassificationNameTranslation(
								$lang,
								$classificationNameTranslation,
								$organismId);
					}
				}
			}

			// create attributes for organism
			if (isset($organism['attributes'])) {
				foreach ($organism['attributes'] as $attributeName => $attributeValues) {
					$attributeValueType = $attributeValues['valuetype'];
					$attributeId = NULL;
					if (isset($this->attributes[$attributeName])) {
						$attributeId = $this->attributes[$attributeName]['id'];
					} else {
						$attributeId = $this->getOrCreateAttribute(
								$attributeName,
								$attributeValueType);
						$this->attributes[$attributeName]['id'] = $attributeId;
						$this->attributes[$attributeName]['values'] = array();
					}

					assert($attributeId != NULL);
					foreach ($attributeValues['values'] as $attributeValuesValue) {
						$attributeValueId = NULL;
						if (isset(
							$this->attributes[$attributeName]['values'][$attributeValuesValue])) {
							$attributeValueId = $this->attributes[$attributeName]['values'][$attributeValuesValue];
						} else {
							$attributeValueId = $this->getOrCreateAttributeValue(
									$attributeValuesValue,
									$attributeValueType,
									$attributeId,
									$organismId);
							$this->attributes[$attributeName]['values'][$attributeValuesValue] = $attributeValueId;
						}
						assert($attributeValueId != NULL);
					}
				}
			}
		}
		assert(empty($errors));
	}

	private function getNextval($tablename) {
		return $this->db
			->get_nextval($tablename . '_id_seq');
	}

	private function getSingleValue($column, $table, $id) {
		assert($column != NULL);
		assert($table != NULL);
		assert($id != NULL);
		$columnNameArray = array(
				$column
		);
		$fromQuery = 'FROM ' . $table . ' WHERE id = ?';
		$typesArray = array(
				'integer'
		);
		$valuesArray = array(
				$id
		);
		$rows = $this->db
			->select_query(
				$columnNameArray,
				$fromQuery,
				$typesArray,
				$valuesArray);
		assert(count($rows) <= 1);
		assert(isset($rows[0][$column]));
		return $rows[0][$column];
	}

	private function getOrCreateClassificationLevelName(
			$classification_level_name, $parent_id) {
		$table = $this->classification_level_table;
		$classifier_level_id = $this->classifier_level_id;
		$fromQuery = 'SELECT id, parent_id FROM ' . $table
				. ' WHERE name = ? AND prime_father_id = ?';
		$typesArray = array(
				'text',
				'integer'
		);
		$valuesArray = array(
				$classification_level_name,
				$this->classifier_level_id
		);
		$result = $this->db
			->query($fromQuery, $typesArray, $valuesArray);
		if (count($result) == 1 && $result[0]['parent_id'] == $parent_id) {
			return $result[0]['id'];
		} else if (count($result) == 1 && $result[0]['parent_id'] != $parent_id) {
			die("You fucked up\n");
		} else {
			$newid = $this->getNextval($table);
			if ($parent_id != NULL) {
				$parentRightValue = $this->getSingleValue(
						'right_value',
						$table,
						$parent_id);
				assert($parentRightValue > 0);

				$query = "UPDATE
							$table
						SET
							right_value = right_value + 2
						WHERE
							prime_father_id = ? AND
							right_value >= ?";
				$typesArray = array(
						'integer',
						'integer'
				);
				$valuesArray = array(
						$classifier_level_id,
						$parentRightValue
				);
				$this->db
					->query($query, $typesArray, $valuesArray, false);

				$query = "UPDATE
							$table
						SET
							left_value = left_value + 2
						WHERE
							prime_father_id = ? AND
							left_value > ?";
				$typesArray = array(
						'integer',
						'integer'
				);
				$valuesArray = array(
						$classifier_level_id,
						$parentRightValue
				);
				$this->db
					->query($query, $typesArray, $valuesArray, false);
				$leftValue = $parentRightValue;
				$rightValue = $parentRightValue + 1;
			} else {
				assert(false);
				$leftValue = 1;
				$rightValue = 2;
				$parent_id = $newid;
			}

			$columnArray = array(
					'id',
					'parent_id',
					'prime_father_id',
					'left_value',
					'right_value',
					'name'
			);
			$typesArray = array(
					'integer',
					'integer',
					'integer',
					'integer',
					'integer',
					'text'
			);
			$valuesArray = array(
					$newid,
					$parent_id,
					$classifier_level_id,
					$leftValue,
					$rightValue,
					$classification_level_name
			);
			$this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			return $newid;
		}
		assert(false);
	}

	private function getOrCreateClassification($classification_name, $parent_id,
			$classification_level_id) {
		$table = $this->classification_table;
		$classifier_id = $this->classifier_id;
		$fromQuery = 'SELECT id FROM ' . $table
				. ' WHERE name = ? AND parent_id = ? AND prime_father_id = ? AND organism_classification_level_id = ?';
		$typesArray = array(
				'text',
				'integer',
				'integer',
				'integer'
		);
		$valuesArray = array(
				$classification_name,
				$parent_id,
				$this->classifier_id,
				$classification_level_id
		);
		$num = $this->db
			->query($fromQuery, $typesArray, $valuesArray);
		if (count($num) == 1) {
			return $num[0]['id'];
		} else {
			$newid = $this->getNextval($table);
			if ($parent_id != NULL) {
				$parentRightValue = $this->getSingleValue(
						'right_value',
						$table,
						$parent_id);
				assert($parentRightValue > 0);

				$query = "UPDATE
							$table
						SET
							right_value = right_value + 2
						WHERE
							prime_father_id = ? AND
							right_value >= ?";
				$typesArray = array(
						'integer',
						'integer'
				);
				$valuesArray = array(
						$classifier_id,
						$parentRightValue
				);
				$this->db
					->query($query, $typesArray, $valuesArray, false);

				$query = "UPDATE
							$table
						SET
							left_value = left_value + 2
						WHERE
							prime_father_id = ? AND
							left_value > ?";
				$typesArray = array(
						'integer',
						'integer'
				);
				$valuesArray = array(
						$classifier_id,
						$parentRightValue
				);
				$this->db
					->query($query, $typesArray, $valuesArray, false);
				$leftValue = $parentRightValue;
				$rightValue = $parentRightValue + 1;
			} else {
				assert(false);
				$leftValue = 1;
				$rightValue = 2;
				$parent_id = $newid;
			}

			$columnArray = array(
					'id',
					'parent_id',
					'prime_father_id',
					'organism_classification_level_id',
					'left_value',
					'right_value',
					'name'
			);
			$typesArray = array(
					'integer',
					'integer',
					'integer',
					'integer',
					'integer',
					'integer',
					'text'
			);
			$valuesArray = array(
					$newid,
					$parent_id,
					$classifier_id,
					$classification_level_id,
					$leftValue,
					$rightValue,
					$classification_name
			);
			$this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			return $newid;
		}
		assert(false);
	}

	private function getOrCreateClassifierLevel($classifier_name) {
		$table = $this->classification_level_table;
		$fromQuery = 'SELECT id FROM ' . $table
				. ' WHERE name = ? AND id = parent_id';
		$typesArray = array(
				'text'
		);
		$valuesArray = array(
				$classifier_name
		);
		$num = $this->db
			->query($fromQuery, $typesArray, $valuesArray);
		if (count($num) == 1) {
			return $num[0]['id'];
		} else if (count($num) == 0) {
			assert(preg_match('/^[a-zA-Z_]+$/', $classifier_name) == 1);
			$newid = $this->getNextval($table);
			$columnArray = array(
					'id',
					'parent_id',
					'prime_father_id',
					'left_value',
					'right_value',
					'name'
			);
			$typesArray = array(
					'integer',
					'integer',
					'integer',
					'integer',
					'integer',
					'text'
			);
			$valuesArray = array(
					$newid,
					$newid,
					$newid,
					1,
					2,
					$classifier_name
			);
			$rowcount = $this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			assert($rowcount == 1);
			return $newid;
		}
		assert(false);
	}

	private function getOrCreateClassifier($classifier_name, $classifierLevelId) {
		$table = $this->classification_table;
		$fromQuery = 'SELECT id FROM ' . $table
				. ' WHERE name = ? AND id = parent_id AND organism_classification_level_id = ?';
		$typesArray = array(
				'text',
				'integer'
		);
		$valuesArray = array(
				$classifier_name,
				$classifierLevelId
		);
		$num = $this->db
			->query($fromQuery, $typesArray, $valuesArray);
		if (count($num) == 1) {
			return $num[0]['id'];
		} else if (count($num) == 0) {
			assert(preg_match('/^[a-zA-Z_]+$/', $classifier_name) == 1);
			$newid = $this->getNextval($table);
			$columnArray = array(
					'id',
					'parent_id',
					'prime_father_id',
					'organism_classification_level_id',
					'left_value',
					'right_value',
					'name'
			);
			$typesArray = array(
					'integer',
					'integer',
					'integer',
					'integer',
					'integer',
					'integer',
					'text'
			);
			$valuesArray = array(
					$newid,
					$newid,
					$newid,
					$classifierLevelId,
					1,
					2,
					$classifier_name
			);
			$rowcount = $this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			assert($rowcount == 1);
			return $newid;
		}
		assert(false);
	}

	private function getOrCreateOrganism($organismNames, $classificationId,
			$parentOrganismId) {
		$primeFatherOrganismId = NULL;
		$table = $this->scientific_name_table;

		// if already existing in cache, return the organism id
		foreach ($organismNames as $organismName) {
			if (key_exists($organismName, $this->scientific_names)) {
				print "$organismName already existing:\n";
				print_r($organismNames);
				assert(false);
				return $this->scientific_names[$organismName];
			}
		}
		// if existing in DB, reaturn organism id
		foreach ($organismNames as $organismName) {
			$columnNameArray = array(
					'organism_id'
			);
			$fromQuery = "FROM $table WHERE name = ?";
			$typesArray = array(
					'text'
			);
			$valuesArray = array(
					$organismName
			);
			$rows = $this->db
				->select_query(
					$columnNameArray,
					$fromQuery,
					$typesArray,
					$valuesArray);
			if (count($rows) == 1) {
				return $rows[0]['organism_id'];
			}
		}

		$table = $this->organism_table;
		$newid = $this->getNextval($table);
		if ($parentOrganismId == NULL) {
			$leftValue = 1;
			$rightValue = 2;
			$parentOrganismId = $newid;
			$primeFatherOrganismId = $newid;
		} else {
			$parentRightValue = $this->getSingleValue(
					'right_value',
					$table,
					$parentOrganismId);
			$primeFatherOrganismId = $this->getSingleValue(
					'prime_father_id',
					$table,
					$parentOrganismId);
			assert($parentPrimeFatherId == $primeFatherOrganismId);

			$query = "UPDATE
							$table
						SET
							right_value = right_value + 2
						WHERE
							prime_father_id = ?
							AND right_value >= ?";
			$typesArray = array(
					'integer',
					'integer'
			);
			$valuesArray = array(
					$primeFatherOrganismId,
					$parentRightValue
			);
			$this->query($query, $typesArray, $valuesArray, false);

			$query = "UPDATE
							$table
						SET
							left_value = left_value + 2
						WHERE
							prime_father_id = ?
							AND left_value > ?";
			$typesArray = array(
					'integer',
					'integer'
			);
			$valuesArray = array(
					$primeFatherOrganismId,
					$parentRightValue
			);
			$this->db
				->query($query, $typesArray, $valuesArray, false);

			$leftValue = $parentRightValue;
			$rightValue = $parentRightValue + 1;
		}

		$columnArray = array(
				'id',
				'parent_id',
				'prime_father_id',
				'left_value',
				'right_value',
		);
		$typesArray = array(
				'integer',
				'integer',
				'integer',
				'integer',
				'integer',
		);
		$valuesArray = array(
				$newid,
				$parentOrganismId,
				$primeFatherOrganismId,
				$leftValue,
				$rightValue
		);
		$rowcount = $this->db
			->insert_query($columnArray, $table, $typesArray, $valuesArray);
		assert(count($rowcount) == 1);
		$organism_id = $newid;

		// Attach all names to organism			
		$table = $this->scientific_name_table;
		foreach ($organismNames as $organismName) {
			$newid = $this->getNextval($table);
			$columnArray = array(
					'organism_id',
					'name',
					'id'
			);
			$typesArray = array(
					'integer',
					'text',
					'integer'
			);
			$valuesArray = array(
					$organism_id,
					$organismName,
					$newid
			);
			$numrow = $this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			assert($numrow == 1);
			$this->scientific_names[$organismName] = $organismId;
		}

		// subscribe organism to classification
		$table = $this->organism_classification_subscription_table;
		$newidsubscription = $this->getNextval($table);
		$columnArray = array(
				'organism_id',
				'organism_classification_id',
				'id'
		);
		$typesArray = array(
				'integer',
				'integer',
				'integer'
		);
		$valuesArray = array(
				$organism_id,
				$classificationId,
				$newidsubscription
		);
		$numrow = $this->db
			->insert_query($columnArray, $table, $typesArray, $valuesArray);
		assert($numrow == 1);

		return $organism_id;

	}

	private function getOrCreateAttributeValue($attributeValuesValue,
			$attributeValueType, $attributeId, $organismId) {
		$table = $this->organism_attribute_value_table;
		$columnNameArray = array(
				'id'
		);
		switch ($attributeValueType) {
		case 'n':
			$fromQuery = "FROM $table WHERE organism_attribute_id = ? AND number_value = ?";
			break;
		case 't':
			$fromQuery = "FROM $table WHERE organism_attribute_id = ? AND text_value = ?";
			break;
		case 'b':
			$fromQuery = "FROM $table WHERE organism_attribute_id = ? AND boolean_value = ?";
			break;
		}
		$typesArray = array(
				'integer',
				'text',
		);
		$valuesArray = array(
				$attributeId,
				$attributeValuesValue
		);
		$rows = $this->db
			->select_query(
				$columnNameArray,
				$fromQuery,
				$typesArray,
				$valuesArray);

		if (count($rows) == 1) {
			return $rows[0]['id'];
		} else if (count($rows) == 0) {
			$newid = $this->getNextval($table);
			switch ($attributeValueType) {
			case 'n':
				$columnArray = array(
						'id',
						'organism_attribute_id',
						'number_value'
				);
				break;
			case 't':
				$columnArray = array(
						'id',
						'organism_attribute_id',
						'text_value'
				);
				break;
			case 'b':
				$columnArray = array(
						'id',
						'organism_attribute_id',
						'boolean_value'
				);
				break;
			default:
				die("$attributeValueType is not a valid attributeValueType\n");
			}
			$typesArray = array(
					'integer',
					'integer',
					'text'
			);
			$valuesArray = array(
					$newid,
					$attributeId,
					$attributeValuesValue
			);
			$num = $this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			assert($num == 1);

			$table = $this->organism_attribute_value_subscription_table;
			$newsubscriptionid = $this->getNextval($table);
			$columnArray = array(
					'id',
					'organism_attribute_value_id',
					'organism_id'
			);

			$typesArray = array(
					'integer',
					'integer',
					'integer'
			);
			$valuesArray = array(
					$newid,
					$newid,
					$organismId
			);
			$num = $this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			assert($num == 1);

			return $newid;
		}
		assert(false);
	}

	private function getOrCreateAttribute($attributeName, $attributeValueType) {
		$table = $this->organism_attribute_table;
		$columnNameArray = array(
				'id'
		);
		$fromQuery = "FROM $table WHERE valuetype = ? AND name = ?";
		$typesArray = array(
				'text',
				'text'
		);
		$valuesArray = array(
				$attributeValueType,
				$attributeName
		);
		$rows = $this->db
			->select_query(
				$columnNameArray,
				$fromQuery,
				$typesArray,
				$valuesArray);

		if (count($rows) == 1) {
			return $rows[0]['id'];
		} else if (count($rows) == 0) {
			$newid = $this->getNextval($table);
			$columnArray = array(
					'id',
					'name',
					'valuetype'
			);
			$typesArray = array(
					'integer',
					'text',
					'text'
			);
			$valuesArray = array(
					$newid,
					$attributeName,
					$attributeValueType
			);
			$num = $this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			assert($num == 1);
			return $newid;
		}
		assert(false);
	}

	private function getOrCreateClassificationNameTranslation($lang,
			$classificationNameTranslation, $organismId) {
		$table = $this->organism_lang_table;
		$columnNameArray = array(
				'id'
		);
		$fromQuery = "FROM $table WHERE languages_language = ? AND name = ? AND organism_id = ?";
		$typesArray = array(
				'text',
				'text',
				'integer'
		);
		$valuesArray = array(
				$lang,
				$classificationNameTranslation,
				$organismId
		);
		$rows = $this->db
			->select_query(
				$columnNameArray,
				$fromQuery,
				$typesArray,
				$valuesArray);

		if (count($rows) == 1) {
			return $rows[0]['id'];
		} else {
			$newid = $this->getNextval($table);
			$columnArray = array(
					'id',
					'languages_language',
					'name',
					'organism_id'
			);
			$typesArray = array(
					'integer',
					'text',
					'text',
					'integer'
			);
			$valuesArray = array(
					$newid,
					$lang,
					$classificationNameTranslation,
					$organismId
			);
			$rowcount = $this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			assert(count($rowcount) == 1);
			return $newid;
		}
		assert(false);
	}
}
?>
