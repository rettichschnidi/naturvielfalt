<?php
/**
 * @author Reto Schneider, 2011, github@reto-schneider.ch
 */

require_once('MDB2.php');
require_once(dirname(__FILE__) . '/Db.php');
require_once($configdir . '/databases.php');

class NaturvielfaltDb extends Db {
	public function __construct() {
		global $config;
		parent::__construct(
			$config['naturvielfalt_dev']['driver'],
			$config['naturvielfalt_dev']['name'],
			$config['naturvielfalt_dev']['user'],
			$config['naturvielfalt_dev']['password'],
			$config['naturvielfalt_dev']['host']);
	}

	function createScientificName($organism_id, $scientific_name_name) {// update the organism_scientific_name table
		global $drupalprefix;
		$table = $drupalprefix . 'organism_scientific_name';
		$columnArray = array(
				'organism_id',
				'name'
		);
		$typesArray = array(
				'integer',
				'text'
		);
		$valuesArray = array(
				$organism_id,
				$scientific_name_name
		);
		$numrow = $this->insert_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert($numrow == 1);
		$ids = $this->getIdArray_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0];
	}

	function haveScientificName($scientific_name_name) {
		global $drupalprefix;
		$fromQuery = 'FROM ' . $drupalprefix
				. 'organism_scientific_name WHERE name = ?';
		$typesArray = array(
				'text'
		);
		$valuesArray = array(
				$scientific_name_name
		);
		$num = $this->getcount_query($fromQuery, $typesArray, $valuesArray);
		assert($num <= 1);
		return $num;
	}

	function getScientificNameId($scientific_name_name) {
		global $drupalprefix;
		$columnNameArray = array(
				'id'
		);
		$fromQuery = 'FROM ' . $drupalprefix
				. 'organism_scientific_name WHERE name = ?';
		$typesArray = array(
				'text'
		);
		$valuesArray = array(
				$scientific_name_name
		);
		$rows = $this->select_query(
				$columnNameArray,
				$fromQuery,
				$typesArray,
				$valuesArray);
		assert(count($rows) == 1);
		return $num[0]['id'];
	}

	function getScientificNameOrganismId($scientific_name_name) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_scientific_name';
		$columnNameArray = array(
				'organism_id'
		);
		$fromQuery = 'FROM ' . $table . ' WHERE name = ?';
		$typesArray = array(
				'text'
		);
		$valuesArray = array(
				$scientific_name_name
		);
		$rows = $this->select_query(
				$columnNameArray,
				$fromQuery,
				$typesArray,
				$valuesArray);
		if (count($rows) != 1) {
			global $errors;
			$errors[] = "Failed to get scientific_name_id for '$scientific_name_name'";
			assert(false);
		}
		return $rows[0]['organism_id'];
	}

	function haveClassifier($classifier_name) {
		global $drupalprefix;
		$fromQuery = 'FROM ' . $drupalprefix
				. 'organism_classifier
						WHERE
							name = ?';
		$typesArray = array(
				'text'
		);
		$valuesArray = array(
				$classifier_name
		);
		$num = $this->getcount_query($fromQuery, $typesArray, $valuesArray);
		assert($num <= 1);
		return $num;
	}

	function createClassifier($classifier_name, $scientific_classification) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_classifier';
		$columnArray = array(
				'name',
				'scientific_classification'
		);
		$typesArray = array(
				'text',
				'integer'
		);
		$valuesArray = array(
				$classifier_name,
				$scientific_classification
		);
		$rowcount = $this->insert_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert($rowcount == 1);
		$ids = $this->getIdArray_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0];
	}

	function getClassifierId($classifier_name) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_classifier';
		$columnNameArray = array(
				'id'
		);
		$fromQuery = 'FROM ' . $table . '
						WHERE
							name = ?';
		$typesArray = array(
				'text'
		);
		$valuesArray = array(
				$classifier_name
		);
		$rows = $this->select_query(
				$columnNameArray,
				$fromQuery,
				$typesArray,
				$valuesArray);
		assert(count($rows) == 1);
		return $rows[0]['id'];
	}

	function haveClassificationLevel($classification_level_name, $classifier_id) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_classification_level';
		$fromQuery = 'FROM ' . $table
				. ' WHERE name = ? AND organism_classifier_id = ?';
		$typesArray = array(
				'text',
				'integer'
		);
		$valuesArray = array(
				$classification_level_name,
				$classifier_id
		);
		$num = $this->getcount_query($fromQuery, $typesArray, $valuesArray);
		assert($num <= 1);
		return $num;
	}

	function haveClassificationLevelWithSpecialParentId(
			$classification_level_name, $classification_parent_id,
			$classifier_id) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_attribute';
		$fromQuery = 'FROM ' . $table
				. ' WHERE name = ? AND parent_id = ? AND organism_classifier_id = ?';
		$typesArray = array(
				'text',
				'integer',
				'integer'
		);
		$valuesArray = array(
				$classification_level_name,
				$classification_parent_id,
				$classifier_id
		);
		$num = $this->getcount_query($fromQuery, $typesArray, $valuesArray);
		assert($num <= 1);
		return $num;
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
		$rows = $this->select_query(
				$columnNameArray,
				$fromQuery,
				$typesArray,
				$valuesArray);
		assert(count($rows) <= 1);
		assert(isset($rows[0][$column]));
		return $rows[0][$column];
	}

	function createClassificationLevel($classification_level_name,
			$classifier_id, $parent_id) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_classification_level';
		$newid = $this->get_nextval(
				$drupalprefix . 'organism_classification_level_id_seq');
		if ($parent_id != NULL) {
			$parentLeftValue = $this->getSingleValue(
					'left_value',
					$table,
					$parent_id);
			$parentRightValue = $this->getSingleValue(
					'right_value',
					$table,
					$parent_id);
			assert($parentLeftValue > 0);
			assert($parentRightValue > 0);

			$query = "UPDATE 
							$table 
						SET
							right_value = right_value + 2
						WHERE
							organism_classifier_id = ? AND
							right_value > ?";
			$typesArray = array(
					'integer',
					'integer'
			);
			$valuesArray = array(
					$classifier_id,
					$parentLeftValue
			);
			$this->query($query, $typesArray, $valuesArray, false);

			$query = "UPDATE 
							$table
						SET
							left_value = left_value + 2
						WHERE
							organism_classifier_id = ? AND
							left_value > ?";
			$typesArray = array(
					'integer',
					'integer'
			);
			$valuesArray = array(
					$classifier_id,
					$parentLeftValue
			);
			$this->query($query, $typesArray, $valuesArray, false);
			$leftValue = $parentLeftValue + 1;
			$rightValue = $parentLeftValue + 2;
		} else {
			$leftValue = 1;
			$rightValue = 2;
			$parent_id = $newid;
		}

		$columnArray = array(
				'id',
				'parent_id',
				'organism_classifier_id',
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
				$classifier_id,
				$leftValue,
				$rightValue,
				$classification_level_name
		);
		$this->insert_query($columnArray, $table, $typesArray, $valuesArray);
		return $newid;
	}

	function getClassificationLevelId($classification_level_name,
			$classifier_id) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_classification_level';
		$columnArray = array(
				'name',
				'organism_classifier_id'
		);
		$typesArray = array(
				'text',
				'integer'
		);
		$valuesArray = array(
				$classification_level_name,
				$classifier_id
		);
		$ids = $this->getIdArray_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0];
	}

	function haveClassification($classification_name, $classification_level_id) {
		assert($classification_name != NULL);
		assert($classification_level_id != NULL);
		global $drupalprefix;
		$table = $drupalprefix . 'organism_classification';
		$fromQuery = 'FROM ' . $table
				. ' WHERE name = ? AND organism_classification_level_id = ?';
		$typesArray = array(
				'text',
				'integer'
		);
		$valuesArray = array(
				$classification_name,
				$classification_level_id
		);
		$num = $this->getcount_query($fromQuery, $typesArray, $valuesArray);
		assert($num <= 1);
		return $num;
	}

	function getClassificationId($classification_name, $classification_level_id) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_classification';
		$columnArray = array(
				'name',
				'organism_classification_level_id'
		);
		$typesArray = array(
				'text',
				'integer'
		);
		$valuesArray = array(
				$classification_name,
				$classification_level_id
		);
		$ids = $this->getIdArray_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0];
	}

	function createClassification($classification_name, $classifier_id,
			$classification_level_id, $parent_id) {
		assert($classification_name != NULL && $classification_name != '');
		assert($classification_level_id != NULL);
		global $drupalprefix;
		$table = $drupalprefix . 'organism_classification';
		$newid = $this->get_nextval(
				$drupalprefix . 'organism_classification_id_seq');
		if ($parent_id != NULL) {
			$parentLeftValue = $this->getSingleValue(
					'left_value',
					$table,
					$parent_id);
			$parentRightValue = $this->getSingleValue(
					'right_value',
					$table,
					$parent_id);

			$query = "UPDATE 
							$table 
						SET
							right_value = right_value + 2
						WHERE
							organism_classifier_id = ? AND right_value > ?";
			$typesArray = array(
					'integer',
					'integer'
			);
			$valuesArray = array(
					$classifier_id,
					$parentLeftValue
			);
			$changed = $this->query($query, $typesArray, $valuesArray, false);
			if (TRUE) {
				print 
					"Had to change $changed records to update the right values.\n";
			}
			$query = "UPDATE
							$table
						SET
							left_value = left_value + 2
						WHERE
							organism_classifier_id = ? AND left_value > ?";
			$typesArray = array(
					'integer',
					'integer'
			);
			$valuesArray = array(
					$classifier_id,
					$parentLeftValue
			);
			$changed = $this->query($query, $typesArray, $valuesArray, false);
			if (TRUE) {
				print 
					"Had to change $changed records to update the left values.\n";
			}
			$leftValue = $parentLeftValue + 1;
			$rightValue = $parentLeftValue + 2;
		} else {
			$leftValue = 1;
			$rightValue = 2;
			$parent_id = $newid;
		}
		$columnArray = array(
				'id',
				'parent_id',
				'organism_classification_level_id',
				'organism_classifier_id',
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
				$classification_level_id,
				$classifier_id,
				$leftValue,
				$rightValue,
				$classification_name,
		);
		$num = $this->insert_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert($num == 1);
		$ids = $this->getIdArray_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0];
	}

	function getPrimeFatherId($prime_father_id) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism';
		$columnNameArray = array(
				'prime_father_id'
		);
		$fromQuery = 'FROM ' . $table . ' WHERE id = ?';
		$typesArray = array(
				'integer'
		);
		$valuesArray = array(
				$prime_father_id
		);
		$ids = $this->select_query(
				$columnNameArray,
				$fromQuery,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0]['prime_father_id'];
	}

	function createOrganism($prime_father_id, $parent_id) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism';
		$newid = $this->get_nextval($drupalprefix . 'organism_id_seq');
		if ($prime_father_id == NULL) {
			$prime_father_id = $newid;
		}
		if ($parent_id != NULL) {
			$parentLeftValue = $this->getSingleValue(
					'left_value',
					$table,
					$parent_id);
			$parentRightValue = $this->getSingleValue(
					'right_value',
					$table,
					$parent_id);
			$parentPrimeFatherId = $this->getSingleValue(
					'prime_father_id',
					$table,
					$parent_id);
			assert($parentPrimeFatherId == $prime_father_id);

			$query = 'UPDATE ' . $table
					. '
					SET
						right_value = right_value + 2
					WHERE
						prime_father_id = ? AND right_value > ?';
			$typesArray = array(
					'integer',
					'integer'
			);
			$valuesArray = array(
					$parentPrimeFatherId,
					$parentLeftValue
			);
			$this->query($query, $typesArray, $valuesArray, false);

			$query = 'UPDATE ' . $table
					. '
					SET
						left_value = left_value + 2
					WHERE
						prime_father_id = ? AND left_value > ?';
			$typesArray = array(
					'integer',
					'integer'
			);
			$valuesArray = array(
					$parentPrimeFatherId,
					$parentLeftValue
			);
			$this->query($query, $typesArray, $valuesArray, false);

			$leftValue = $parentLeftValue + 1;
			$rightValue = $parentLeftValue + 2;
		} else {
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
				$parent_id,
				$prime_father_id,
				$leftValue,
				$rightValue
		);
		$this->insert_query($columnArray, $table, $typesArray, $valuesArray);
		return $newid;
	}

	function haveAttributeName($attribute_name) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_attribute';
		$fromQuery = 'FROM ' . $table . ' WHERE name = ?';
		$typesArray = array(
				'text'
		);
		$valuesArray = array(
				$attribute_name
		);
		$num = $this->getcount_query($fromQuery, $typesArray, $valuesArray);
		assert($num <= 1);
		return $num;
	}

	function createAttribute($attribute_name, $attribute_valuetype) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_attribute';
		$columnArray = array(
				'name',
				'valuetype'
		);
		$typesArray = array(
				'text',
				'text'
		);
		$valuesArray = array(
				$attribute_name,
				$attribute_valuetype
		);
		$num = $this->insert_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert($num == 1);
		$ids = $this->getIdArray_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0];
	}

	function getAttributeId($attribute_name) {
		global $drupalprefix;
		$table = $drupalprefix . 'organism_attribute';
		$columnArray = array(
				'name'
		);
		$typesArray = array(
				'text'
		);
		$valuesArray = array(
				$attribute_name
		);
		$ids = $this->getIdArray_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0];
	}

	/**
	 * @note We are screwed it $number_value should _really_ be 0
	 */
	function haveAttributeValueNumber($attribute_id, $number_value) {
		assert($number_value != null);
		assert($attribute_id != null && $attribute_id > 0);
		global $drupalprefix;
		$table = $drupalprefix . 'organism_attribute_value';
		$fromQuery = 'FROM ' . $table . ' WHERE number_value = ?';
		$typesArray = array(
				'integer'
		);
		$valuesArray = array(
				$number_value
		);
		$num = $this->getcount_query($fromQuery, $typesArray, $valuesArray);
		assert($num <= 1);
		return $num;
	}

	function createAttributeValueNumber($attribute_id, $number_value) {
		assert($number_value != NULL);
		assert($attribute_id != NULL);
		global $drupalprefix;
		$table = $drupalprefix . 'organism_attribute_value';
		$newid = $this->get_nextval(
				$drupalprefix . 'organism_attribute_value_id_seq');
		$columnArray = array(
				'id',
				'organism_attribute_id',
				'number_value'
		);
		$typesArray = array(
				'integer',
				'integer',
				'integer'
		);
		$valuesArray = array(
				$newid,
				$attribute_id,
				$number_value
		);
		$rowcount = $this->insert_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert($rowcount == 1);
		$ids = $this->getIdArray_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0];
	}

	function createAttributeValueSubscription($organism_id, $attribute_value_id) {
		assert($organism_id != NULL);
		assert($attribute_value_id != NULL);
		global $drupalprefix;
		$table = $drupalprefix . 'organism_attribute_value_subscription';
		$newid = $this->get_nextval(
				$drupalprefix . 'organism_attribute_value_subscription_id_seq');
		$columnArray = array(
				'id',
				'organism_id',
				'organism_attribute_value_id'
		);
		$typesArray = array(
				'integer',
				'integer',
				'integer'
		);
		$valuesArray = array(
				$newid,
				$organism_id,
				$attribute_value_id
		);
		$rowcount = $this->insert_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert($rowcount == 1);
		$ids = $this->getIdArray_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0];
	}

	function createOrganismClassificationSubscription($organism_id,
			$classification_id) {
		assert($organism_id != NULL);
		assert($classification_id != NULL);
		global $drupalprefix;
		$table = $drupalprefix . 'organism_classification_subscription';
		$newid = $this->get_nextval(
				$drupalprefix . 'organism_classification_id_seq');
		$columnArray = array(
				'id',
				'organism_id',
				'organism_classification_id'
		);
		$typesArray = array(
				'integer',
				'integer',
				'integer'
		);
		$valuesArray = array(
				$newid,
				$organism_id,
				$classification_id
		);
		$rowcount = $this->insert_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert($rowcount == 1);
		$ids = $this->getIdArray_query(
				$columnArray,
				$table,
				$typesArray,
				$valuesArray);
		assert(count($ids) == 1);
		return $ids[0];
	}

	function haveOrganismClassificationSubscription($organism_id,
			$classification_id) {
		assert($organism_id != NULL);
		assert($classification_id != NULL);
		global $drupalprefix;
		$table = $drupalprefix . 'organism_classification_subscription';
		$fromQuery = 'FROM ' . $table
				. ' WHERE organism_id = ? AND organism_classification_id = ?';
		$typesArray = array(
				'integer',
				'integer'
		);
		$valuesArray = array(
				$organism_id,
				$classification_id
		);
		$num = $this->getcount_query($fromQuery, $typesArray, $valuesArray);
		assert($num <= 1);
		return $num;
	}

}
?>
