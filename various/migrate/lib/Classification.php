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
 * 		'scientific_name' => 'Acantholycosa pedestris',
 *      'synonyms' => array(
 * 				'Acantholycosa pedestris v2'
 * 		),
 * 		'classification_name_translations' => array(
 * 				'de' => array(
 * 						'Eichen',
 * 				),
 * 				'en' => array(
 * 						'Oak'
 * 				)
 * 		),
 * 		'attributes' => array(
 * 				'author' => array(
 *						'comment' => 'The guy who named it.',
 * 						'valuetype' => 't',
 * 						'values' => array(
 * 								'Reto'
 * 						),
 * 				),
 * 				'NUESP' => array(
 *						'comment' => 'Number which got assigned to this organism by XY.',
 * 						'valuetype' => 'n',
 * 						'values' => array(
 * 								1234
 * 						),
 * 				)
 * 		),
 * 		'artgroups' => array(
 * 				'Flora',
 * 		),
 * );
 * 
 * $importer = new Classification($driver, $name, $user, $password, $host);
 * $importer->addOrganism($organism);
 * 
 * @note It is possible to specifiy just some classifications for an organism. 
 * 	However, the very first organism for any classificator has to contain ALL
 * 	classificationlevelnames (in descending order, as always required).
 *  Example:
 *  $makeSureItWorksOrganism = array(
 *  		'classificator' => $classificator,
 *  		'classifications' => array(
 * 				array(
 *  						'classificationlevelname' => 'family'
 *  				),
 *  				array(
 *  						'classificationlevelname' => 'genus'
 *  				),
 *  		)
 *  );
 *  $importer->addOrganism($makeSureItWorksOrganism);
 * @note Please make sure that you are sorting the input by its classification (ordered by e.g. class, order, family, genus).
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
	var $synonym_table = '';
	var $organism_lang_table = '';
	var $organism_classification_subscription_table = '';
	var $organism_attribute_table = '';
	var $organism_attribute_value_table = '';
	var $organism_attribute_value_subscription_table = '';
	var $organism_artgroup = '';
	var $organism_artgroup_subscription = '';

	/* Caches the structure of the current classifier.
	 * Saves us many database requests.
	 * 
	 * Example:
	 * Array
	 * (
	 *     [CRSF] => Array
	 *         (
	 *             [classificationlevelid] => 7
	 *             [classificationid] => 6725
	 *             [classificationlevelvalues] => Array
	 *                 (
	 *                     [family] => 8
	 *                     [genus] => 9
	 *                 )
	 *             [classificationvalues] => Array
	 *                 (
	 *                     [Aceraceae] => Array
	 *                         (
	 *                             [id] => 6726
	 *                             [values] => Array
	 *                                 (
	 *                                    [Acer] => Array
	 *                                         (
	 *                                             [id] => 6727
	 *                                             [values] => Array
	 *                                                 (
	 *                                                 )
	 *                                         )
	 *                                 )
	 *                         )
	 * 
	 *                     [Alismataceae] => Array
	 *                         (
	 *                             [id] => 6734
	 *                             [values] => Array
	 *                                 (
	 *                                     [Alisma] => Array
	 *                                         (
	 *                                             [id] => 6735
	 *                                             [values] => Array
	 *                                                 (
	 *                                                 )
	 *                                         )
	 *                                     [Baldellia] => Array
	 *                                         (
	 *                                             [id] => 6736
	 *                                             [values] => Array
	 *                                                 (
	 *                                                 )
	 *                                         )
	 *                                     [Caldesia] => Array
	 *                                         (
	 *                                             [id] => 6737
	 *                                             [values] => Array
	 *                                                 (
	 *                                                 )
	 *                                         )
	 *                                     [Sagittaria] => Array
	 *                                         (
	 *                                             [id] => 6738
	 *                                             [values] => Array
	 *                                                 (
	 *                                                 )
	 *                                         )
	 *                                 )
	 *                         )
	 *                 )
	 *         )
	 * )
	 */
	var $tree;

	/*
	 * Cache all existing scientific names with their organism id.
	 * 
	 * Example:
	 *  Array
	 *  (
	 *      [Acipenser sturio Linnaeus] => 1
	 *      [Anguilla anguilla (Linnaeus, 1758)] => 2
	 *      [Alosa agone (Fatio, 1890)] => 3
	 *      [Alosa alosa (Linnaeus, 1758)] => 4
	 *      [Alosa fallax (Geoffroy, 1827)] => 5
	 *      [Barbatula barbatula (Linnaeus, 1758)] => 6
	 *      [Cobitis taenia (Linnaeus, 1758)] => 7
	 *      [Misgurnus fossilis (Linnaeus, 1758)] => 8
	 *  )
	 */
	var $scientific_names = array();

	/**
	 * Connect to the given database.
	 * @param string $driver
	 * @param string $name
	 * @param string $user
	 * @param string $password
	 * @param string $host
	 */
	private function selectDb($driver, $name, $user, $password, $host) {
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

	/**
	 * Set up a database connection and populate the "shortcuts" for tablenames.
	 * 
	 * @param string $driver
	 * @param string $name
	 * @param string $user
	 * @param string $password
	 * @param string $host
	 */
	public function __construct($driver, $name, $user, $password, $host) {
		$this->selectDb($driver, $name, $user, $password, $host);
		global $drupalprefix;
		$this->drupalprefix = $drupalprefix;
		$this->classification_table = $drupalprefix . 'organism_classification';
		$this->classification_level_table = $drupalprefix
				. 'organism_classification_level';
		$this->synonym_table = $drupalprefix . 'organism_synonym';
		$this->organism_table = $drupalprefix . 'organism';
		$this->organism_lang_table = $drupalprefix . 'organism_lang';
		$this->organism_classification_subscription_table = $drupalprefix
				. 'organism_classification_subscription';
		$this->organism_attribute_table = $drupalprefix . 'organism_attribute';
		$this->organism_attribute_value_table = $drupalprefix
				. 'organism_attribute_value';
		$this->organism_attribute_value_subscription_table = $drupalprefix
				. 'organism_attribute_value_subscription';
		$this->organism_artgroup_subscription = $drupalprefix
				. 'organism_artgroup_subscription';
		$this->organism_artgroup = $drupalprefix . 'organism_artgroup';
		$this->tree = array();
	}

	/**
	 * If possible, start a transaction on the database.
	 */
	public function startTransactionIfPossible() {
		$this->db
			->startTransactionIfPossible();
	}

	/**
	 * If possible, close a transaction on the database.
	 */
	public function stopTransactionIfPossible() {
		$this->db
			->stopTransactionIfPossible();
	}

	/**
	 * Add all given organisms to the database.
	 * 
	 * @param array $organisms Array of organisms
	 */
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
				// Commit to database (stop current transaction) and create a new transaction
				if ($i % 1000 == 0) {
					$this->db
						->stopTransactionIfPossible();
					$this->db
						->startTransactionIfPossible();
				}
			}
			$this->addOrganism($organism);
		}
		$this->db
			->stopTransactionIfPossible();
	}

	/**
	 * creates or finds the classificationId for the given organism.
	 * @param array $organism
	 */
	public function getClassificationId(array $organism){
		$this->current_classifier_id = NULL;
		$this->classifier_name = $organism['classificator'];
		$classifierName = $this->classifier_name;
		
		// get classifier and classifier level id (and create them if not yet existing)
		{
			if (key_exists($classifierName, $this->tree)) {
				// is in cache
				$classifierLevelId = $this->tree[$classifierName]['classificationlevelid'];
				$classifierId = $this->tree[$classifierName]['classificationid'];
			} else {
				// get from database (and create if needed) and add to cache
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
			$this->current_classifier_level_id = $classifierLevelId;
			$this->current_classifier_id = $classifierId;
			assert($this->current_classifier_id != NULL);
		}
		
		// add all classifications
		$parentClassificationLevelId = $this->current_classifier_level_id;
		$parentClassificationId = $this->current_classifier_id;
		$classificationLevels = &$this->tree[$classifierName]['classificationlevelvalues'];
		$currentClassifications = &$this->tree[$classifierName]['classificationvalues'];
		$classificationId = NULL;
		
		// create all classifications
		foreach ($organism['classifications'] as $classification) {
			$classificationLevelName = $classification['classificationlevelname'];
			// create classification levels //
			if (key_exists($classificationLevelName, $classificationLevels)) {
				$classificationLevelId = $classificationLevels[$classificationLevelName];
			} else {
				print
				"Cache miss for classification level: $classificationLevelName ["
				. $this->classifier_name . "]\n";
				$classificationLevelId = $this->getOrCreateClassificationLevelName(
						$classificationLevelName,
						$parentClassificationLevelId);
						$classificationLevels[$classificationLevelName] = $classificationLevelId;
			}
			$parentClassificationLevelId = $classificationLevelId;
		
			// create classification
			if (isset($classification['classificationname'])) {
			$classificationName = $classification['classificationname'];
			if (key_exists($classificationName, $currentClassifications)) {
			//print "Hit for classification: $classificationName\n";
					$classificationId = $currentClassifications[$classificationName]['id'];
				} else {
				print
				"Cache miss for classification: $classificationName ["
				. $this->classifier_name . "]\n";
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
		return $classificationId;
	}
	
	/**
	 * Add a single organism to the database, including its attributes and classification.
	 * 
	 * @param $organism
	 * 	Array with organism information. Have a look at the top of this file for further
	 *  information.
	 */
	private function addOrganism(array $organism) {
		$classificationId = $this->getClassificationId($organism);

		// Create organism (only if scientific name is given)
		$organismId = NULL;
		if (isset($organism['scientific_name'])) {
			if ($classificationId == NULL) {
				print_r($organism);
				assert(false);
			}
			$synonyms = $organism['synonyms'];
			$organismId = $this->getOrCreateOrganism(
					$organism['scientific_name'],
					$synonyms,
					$classificationId,
					NULL);

			assert($organismId != NULL);

			// create translation for organisms (if given)
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

			// create attributes for organism (if given)
			if (isset($organism['attributes'])) {
				foreach ($organism['attributes'] as $attributeName => $attributeValues) {
					$attributeValueType = $attributeValues['valuetype'];
					$attributeComment = $attributeValues['comment'];
					$attributeId = NULL;
					if (isset($this->attributes[$attributeName])) {
						$attributeId = $this->attributes[$attributeName]['id'];
					} else {
						$attributeId = $this->getOrCreateAttribute(
								$attributeName,
								$attributeValueType,
								$attributeComment);
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
									$attributeId);
							$this->attributes[$attributeName]['values'][$attributeValuesValue] = $attributeValueId;
						}
						assert($attributeValueId != NULL);
						$attributeValueSubscriptionId = $this->getOrCreateAttributeValueSubscription(
								$attributeValueId,
								$organismId);
						assert($attributeValueSubscriptionId != NULL);
					}
				}
			}

			// subscribe organism to artgroup (if artgroup given)
			if (isset($organism['artgroups'])) {
				foreach ($organism['artgroups'] as $artgroupName) {
					$artgroupId = $this->getOrCreateArtgroup($artgroupName);
					assert($artgroupId != NULL);
					$artgroupSubscriptionId = $this->getOrCreateArtgroupSubscription(
							$organismId,
							$artgroupId);
					assert($artgroupSubscriptionId != NULL);
				}
			}
		}
	}

	/**
	 * Return next id of id field in given table.
	 * @param string $tablename
	 */
	private function getNextval($tablename) {
		return $this->db
			->get_nextval($tablename . '_id_seq');
	}

	/**
	 * Return a single field of a record.
	 * 
	 * @param string $column
	 * @param string $table
	 * @param integer $id
	 */
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

	/**
	 * If existing: return existing ClassificationLevel
	 * If not: create classification level and return it
	 * 
	 * @param string $classification_level_name
	 * @param string $parent_id
	 * @return id
	 */
	private function getOrCreateClassificationLevelName(
			$classification_level_name, $parent_id) {
		$table = $this->classification_level_table;
		$classifier_level_id = $this->current_classifier_level_id;
		$fromQuery = 'SELECT id, parent_id FROM ' . $table
				. ' WHERE name = ? AND prime_father_id = ?';
		$typesArray = array(
				'text',
				'integer'
		);
		$valuesArray = array(
				$classification_level_name,
				$this->current_classifier_level_id
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
		$classifier_id = $this->current_classifier_id;
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
				$this->current_classifier_id,
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

	/**
	 * Create organism, and it's synonyms (flora) and subscribe to its classifier.
	 * 
	 * @param array of strings $organismNames
	 * @param integer $classificationId
	 * @param integer $parentOrganismId
	 */
	private function getOrCreateOrganism($organismName, $synonyms,
			$classificationId, $parentOrganismId = NULL) {
		$primeFatherOrganismId = NULL;
		$organism_id = NULL;
		$table = $this->synonym_table;

		// if already existing in cache, return the organism id
		if (key_exists($organismName, $this->scientific_names)) {
			print "$organismName already existing in cache.\n";
			print_r($this->scientific_names);
			assert(false);
			return $this->scientific_names[$organismName];
		}

		// the same check for the synonyms
		foreach ($synonyms as $synonym) {
			if (key_exists($synonym, $this->scientific_names)) {
				print "$synonym already existing in cache.\n";
				print_r($this->scientific_names);
				assert(false);
				return $this->scientific_names[$synonym];
			}
		}

		// search for existing synonyms in DB, return organism id
		foreach ($synonyms as $synonym) {
			$columnNameArray = array(
					'organism_id'
			);
			$fromQuery = "FROM $this->synonym_table WHERE name = ?";
			$typesArray = array(
					'text'
			);
			$valuesArray = array(
					$synonym
			);
			$rows = $this->db
				->select_query(
					$columnNameArray,
					$fromQuery,
					$typesArray,
					$valuesArray);
			if (count($rows) == 1) {
				// print "$organismName already existing.\n";
				$this->scientific_names[$synonym] = $rows[0]['organism_id'];
				$organism_id = $rows[0]['organism_id'];
			}
		}

		// search for the give scientific name
		$columnNameArray = array(
				'id'
		);
		$fromQuery = "FROM $this->organism_table WHERE scientific_name = ?";
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
			// print "$organismName already existing.\n";
			$this->scientific_names[$organismName] = $rows[0]['id'];
			$organism_id = $rows[0]['id'];
		}
		
		// If organism not found, create it
		if ($organism_id == NULL) {
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
					'scientific_name'
			);
			$typesArray = array(
					'integer',
					'integer',
					'integer',
					'integer',
					'integer',
					'text',
			);
			$valuesArray = array(
					$newid,
					$parentOrganismId,
					$primeFatherOrganismId,
					$leftValue,
					$rightValue,
					$organismName
			);
			$rowcount = $this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			assert(count($rowcount) == 1);
			$organism_id = $newid;

			// Attach all synonyms to this organism			
			$table = $this->synonym_table;
			foreach ($synonyms as $synonym) {
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
						$synonym,
						$newid
				);
				$numrow = $this->db
					->insert_query(
						$columnArray,
						$table,
						$typesArray,
						$valuesArray);
				assert($numrow == 1);
				$this->scientific_names[$synonym] = $organism_id;
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
		}
		return $organism_id;
	}

	private function getOrCreateAttributeValue($attributeValuesValue,
			$attributeValueType, $attributeId) {
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
			return $newid;
		}
		assert(false);
	}

	private function getOrCreateAttributeValueSubscription($attributeValueId,
			$organismId) {
		$table = $this->organism_attribute_value_subscription_table;
		$columnNameArray = array(
				'id'
		);
		$fromQuery = "FROM $table WHERE organism_attribute_value_id = ? AND organism_id = ?";
		$typesArray = array(
				'integer',
				'integer',
		);
		$valuesArray = array(
				$attributeValueId,
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
		} else if (count($rows) == 0) {
			$table = $this->organism_attribute_value_subscription_table;
			$newsubscriptionId = $this->getNextval($table);
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
					$newsubscriptionId,
					$attributeValueId,
					$organismId
			);
			$num = $this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			assert($num == 1);
			return $newsubscriptionId;
		}
		assert(false);
	}

	private function getOrCreateAttribute($attributeName, $attributeValueType,
			$attributeComment) {
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
					'valuetype',
					'comment'
			);
			$typesArray = array(
					'integer',
					'text',
					'text',
					'text'
			);
			$valuesArray = array(
					$newid,
					$attributeName,
					$attributeValueType,
					$attributeComment
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

	/**
	 * Return the argroupid for the given artgroupname (and create it if needed).
	 * 
	 * @note This is a very simple implemenation. It does not support hierarchical
	 * artgroups. If we ever need them, this function has to be extended.
	 * 
	 * @param string $artgroupName
	 */
	private function getOrCreateArtgroup($artgroupName) {
		$table = $this->organism_artgroup;
		$columnNameArray = array(
				'id'
		);
		$fromQuery = "FROM $table WHERE name = ?";
		$typesArray = array(
				'text',
		);
		$valuesArray = array(
				$artgroupName
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
					'name',
					'parent',
			);
			$typesArray = array(
					'integer',
					'text',
					'integer',
			);
			$valuesArray = array(
					$newid,
					$artgroupName,
					$newid,
			);
			$rowcount = $this->db
				->insert_query($columnArray, $table, $typesArray, $valuesArray);
			assert(count($rowcount) == 1);
			return $newid;
		}
		assert(false);
	}

	/**
	 * Subscribe an organism to an artgroup and return the id of the subscription.
	 * If the subscription already exists, just return its id.
	 * 
	 * @param integer $organismId
	 * @param integer $artgroupId
	 */
	private function getOrCreateArtgroupSubscription($organismId, $artgroupId) {
		$table = $this->organism_artgroup_subscription;
		$columnNameArray = array(
				'id'
		);
		$fromQuery = "FROM $table WHERE organism_id = ? AND organism_artgroup_id = ? ";
		$typesArray = array(
				'integer',
				'integer',
		);
		$valuesArray = array(
				$organismId,
				$artgroupId
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
					'organism_id',
					'organism_artgroup_id'
			);
			$typesArray = array(
					'integer',
					'integer',
					'integer'
			);
			$valuesArray = array(
					$newid,
					$organismId,
					$artgroupId
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
