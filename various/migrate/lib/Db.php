<?php
/**
 * @author Reto Schneider, 2011, github@reto-schneider.ch
 */

require_once ('MDB2.php');
require_once (dirname(__FILE__) . '/bootstrap.php');
require_once ($configdir . '/databases.php');

class Db {
	private $connectionUrl = false;
	private $connection = false;

	public function __construct($dbdriver, $dbname, $user, $password, $host) {
		Db::updateDBConnectionSettings($dbdriver, $dbname, $user, $password, $host);
		Db::connectDbLazy();
		$this -> connection -> loadModule('Extended');

	}

	public function __destruct() {
		$this -> connection -> disconnect();
	}

	private function updateDBConnectionSettings($dbdriver, $dbname, $user, $password, $host) {
		$this -> connectionUrl = $dbdriver . '://' . $user . ':' . $password . '@' . $host . '/' . $dbname;
	}

	/**
	 *
	 * Connect to databases, but create instance just when used the first time
	 */
	private function connectDbLazy() {
		$this -> connection = MDB2::factory($this -> connectionUrl);
		if (PEAR::isError($this -> connection)) {
			die("Error while lazy connecting\n" . $this -> connection -> getMessage() . "\n" . "ConnectionURL was: \"" . $this -> connectionUrl . "\"\n");
		}
	}

	/**
	 *
	 * Connect do singleton database
	 */
	private function connectDBSingleton() {
		$this -> connection = MDB2::singleton($this -> connectionUrl);
		if (PEAR::isError($this -> connection)) {
			die("Error while singleton connecting\n: " . $this -> connection -> getMessage() . "\n" . "ConnectionURL was: \"" . $this -> connectionUrl . "\"\n");
		}
	}

	/**
	 *
	 * Returns the query result as array or false if nothing returned
	 *
	 * Example:
	 * 	$sql = "SELECT id, name FROM people WHERE color = ? AND age = ?;";
	 *  $typesArray = array('text', 'integer');
	 *  $valuesArray = array('blue', '25');
	 *  $myDB = new DB();
	 *  $myDB->query($sql, $typesArray, $valuesValue);
	 *
	 * @param string $query
	 * @param array of types $typesArray
	 * @param array of values $valuesArray
	 * @return array
	 */

	public function query($query, $typesArray, $valuesArray) {
		global $errors;
		$results = false;
		$statement = &$this -> connection -> prepare($query, $typesArray);
		if (PEAR::isError($statement)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getMessage();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . " :  " . $statement -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($typesArray, true);
			return $results;
		}
		$res = $statement -> execute($valuesArray);
		if (PEAR::isError($res)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $res -> getMessage() . "\n" . $res -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($typesArray, true);
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Values : " . var_export($valuesArray, true);
			return $results;
		}
		while ($row = $res -> fetchRow()) {
			$array = array();
			foreach ($row as $field) {
				$array[] = $field;
			}
			$results[] = $array;
		}
		return $results;
	}

	/**
	 * Returns the query result as array. Empty array if nothing returned.
	 *
	 * Example:
	 * 	$columns = array('id', 'COUNT(*)', 'name');
	 *  $sql = 'FROM test';
	 *  $typeArray = array();
	 *  $typeValue = array();
	 *  $result = $db->select_query($cols, $sql, $typeArray, $typeValue);
	 *
	 * Return value:
	 *  array (
	 *    array (
	 *      'id' => '1',
	 *      'COUNT(*)' => '7',
	 *      'name' => 'Hostname',
	 *    ),
	 *  )
	 *
	 * @param array of columnnames $columnNameArray
	 * @param string $query
	 * @param array of types $typesArray
	 * @param array of values $valuesArray
	 * @param boolean If true an assoziative, when false a non assoziative array will be returned
	 * @param string If defined, this statement will be used instead of a plain 'SELECT'
	 * @return array
	 */
	public function select_query($columnNameArray, $fromQuery, $typesArray, $valuesArray, $assoziative = true, $ownselect = 'SELECT ') {
		global $errors;
		$finalArray = array();
		assert(count($typesArray) == count($valuesArray));
		assert(is_array($columnNameArray));
		assert(is_array($typesArray));
		assert(is_array($valuesArray));
		$columns = implode(',', $columnNameArray);
		$query = $ownselect . ' ' . $columns . ' ' . $fromQuery;
		 print "SELECTQUERY:  $query\n";
		 print "SELECTTYPES:  " . var_export($typesArray, true) . "\n";
		 print "SELECTVALUES: " . var_export($valuesArray, true) . "\n";
		$statement = &$this -> connection -> prepare($query, $typesArray);
		if (PEAR::isError($statement)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getMessage();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . " :  " . $statement -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($typesArray, true);
			return false;
		}
		$rows = $statement -> execute($valuesArray);
		if (PEAR::isError($rows)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $rows -> getMessage();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . " :  " . $rows -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($typesArray, true);
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Values : " . var_export($valuesArray, true);
			return false;
		}
		if ($assoziative) {
			$this -> connection -> setFetchMode(MDB2_FETCHMODE_ASSOC);
		} else {
			$this -> connection -> setFetchMode(MDB2_FETCHMODE_ORDERED);
		}
		while ($row = $rows -> fetchRow()) {
			$finalArray[] = $row;
		}
		return $finalArray;
	}

	/**
	 * @param string $fromQuery
	 * @param array of types $typesArray
	 * @param array of values $valuesArray
	 * @return integer
	 */
	public function getcount_query($fromQuery, $typesArray, $valuesArray) {
		global $errors;
		assert(count($typesArray) == count($valuesArray));
		$rows = $this -> select_query(array('COUNT(*)'), $fromQuery, $typesArray, $valuesArray, false);
		return $rows[0][0];
	}

	/**
	 * @param string $query
	 * @param array of types $typesArray
	 * @param array of values $valuesArray
	 * @return array
	 */
	public function insert_query($columnArray, $table, $typesArray, $valuesArray) {
		global $errors;
		$finalArray = array();
		assert(count($typesArray) == count($valuesArray));
		$columns = implode(',', $columnArray);
		$questionmarks = '?';
		for ($i = 0; $i < count($columnArray) - 1; $i++) {
			$questionmarks .= ',?';
		}
		$query = 'INSERT INTO ' . $table . ' (' . $columns . ')' . ' VALUES(' . $questionmarks . ')';
		//print "insert_query: $query\n";
		//var_dump($valuesArray);
		$statement = &$this -> connection -> prepare($query, $typesArray, MDB2_PREPARE_MANIP);
		if (PEAR::isError($statement)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getMessage();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . " :  " . $statement -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($typesArray, true);
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Values : " . var_export($valuesArray, true);
			return false;
		}
		$rows = $statement -> execute($valuesArray);
		if (PEAR::isError($rows)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $rows -> getMessage() . "\n" . $rows -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($typesArray, true);
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Values : " . var_export($valuesArray, true);
			return false;
		}
		return $rows;
	}

	/**
	 * @param string $query
	 * @param array of types $typesArray
	 * @param array of values $valuesArray
	 * @return array
	 */
	public function multiinsert_query($columnArray, $table, $typesArray, $valuesArrayArray) {
		global $errors;
		$finalArray = array();
		assert(count($typesArray) == count($typesArray));
		$columns = implode(',', $columnArray);
		$questionmarks = '?';
		for ($i = 0; $i < count($columnArray) - 1; $i++) {
			$questionmarks .= ',?';
		}
		$query = 'INSERT INTO ' . $table . ' (' . $columns . ')' . ' VALUES(' . $questionmarks . ')';
		// print "multiinsert_query: $query\n";
		$statement = &$this -> connection -> prepare($query, $typesArray, MDB2_PREPARE_MANIP);
		if (PEAR::isError($statement)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getMessage();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . " :  " . $statement -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($typesArray, true);
			return false;
		}
		if ($this -> connection -> supports('transactions')) {
			$this -> connection -> beginTransaction();
		}
		$rows = $this -> connection -> extended -> executeMultiple($statement, $valuesArrayArray);
		if ($this -> connection -> in_transaction) {
			$this -> connection -> commit();
		}
		if (PEAR::isError($rows)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $rows -> getMessage() . "\n" . $rows -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($typesArray, true);
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Values : " . var_export($valuesArrayArray, true);
			return false;
		}
		return $rows;
	}

	public function update_query($table, $updateColumnArray, $updateTypesArray, $updateValuesArray, $whereColumnArray, $whereTypesArray, $whereValuesArray) {
		global $errors;
		assert(count($updateColumnArray) == count($updateTypesArray));
		assert(count($updateColumnArray) == count($updateValuesArray));
		assert(count($updateColumnArray) > 0);
		assert(count($whereColumnArray) == count($whereTypesArray));
		assert(count($whereColumnArray) == count($whereValuesArray));
		assert(count($whereColumnArray) > 0);
		$unifiedValuesArray = array_merge($updateValuesArray, $whereValuesArray);
		$unifiedTypesArray = array_merge($updateTypesArray, $whereTypesArray);
		$updateColumns = implode(' = ?, ', $updateColumnArray);
		$updateColumns .= ' = ?';
		$whereColumns = implode(' = ? AND ', $whereColumnArray);
		$whereColumns .= ' = ?';
		$query = 'UPDATE ' . $table . ' SET ' . $updateColumns;
		if (count($whereColumnArray)) {
			$query .= ' WHERE ' . $whereColumns;
		}
		$statement = &$this -> connection -> prepare($query, $unifiedTypesArray, MDB2_PREPARE_MANIP);
		if (PEAR::isError($statement)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getMessage();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . " :  " . $statement -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($unifiedTypesArray, true);
			return false;
		}
		$numrows = $statement -> execute($unifiedValuesArray);
		if (PEAR::isError($numrows)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $numrows -> getMessage() . "\n" . $numrows -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . " :  " . $numrows -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($unifiedTypesArray, true);
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Values : " . var_export($unifiedValuesArray, true);
			return false;
		}
		return $numrows;
	}

	public function getIdArray_query($whereColumnArray, $table, $whereTypesArray, $whereValuesArray) {
		assert(count($whereColumnArray) == count($whereValuesArray));
		assert(count($whereColumnArray) > 0);
		$whereColumns = implode(' = ? AND ', $whereColumnArray);
		$whereColumns .= ' = ?';
		$query = 'SELECT id FROM ' . $table . ' WHERE ' . $whereColumns;
		// print "QUERY: $query\n";
		// print "VALUES: \n";
		// var_dump($whereValuesArray);
		// print "\nTYPES:\n";
		// var_dump($whereTypesArray);
		// print "\n";
		$statement = &$this -> connection -> prepare($query, $whereTypesArray);
		if (PEAR::isError($statement)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getMessage();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . " :  " . $statement -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($whereTypesArray, true);
			return $finalArray;
		}
		$rows = $statement -> execute($whereValuesArray);
		if (PEAR::isError($rows)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $rows -> getMessage() . "\n" . $rows -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . " :  " . $rows -> getUserInfo();
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Query : " . $query;
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Types : " . var_export($whereTypesArray, true);
			$errors[] = basename(__FILE__) . "@" . __LINE__ . ", Values : " . var_export($whereValuesArray, true);
			return false;
		}
		$ids = array();
		while ($row = $rows -> fetchRow(MDB2_FETCHMODE_ASSOC)) {
			$ids[] = $row['id'];
		}
		return $ids;
	}

	public function upsert_query($table, $updateColumnArray, $updateTypesArray, $updateValuesArray, $whereColumnArray, $whereTypesArray, $whereValuesArray) {
		global $errors;
		assert(count($updateColumnArray) == count($updateTypesArray));
		assert(count($updateColumnArray) == count($updateValuesArray));
		assert(count($updateColumnArray) > 0);
		assert(count($whereColumnArray) == count($whereTypesArray));
		assert(count($whereColumnArray) == count($whereValuesArray));
		assert(count($whereColumnArray) > 0);
		$unifiedValuesArray = array_merge($updateValuesArray, $whereValuesArray);
		$unifiedTypesArray = array_merge($updateTypesArray, $whereTypesArray);
		$whereColumns = implode(' = ? AND ', $updateColumnArray);
		$whereColumns .= ' = ?';
		$numrowsupdate = $this -> update_query($table, $updateColumnArray, $updateTypesArray, $updateValuesArray, $whereColumnArray, $whereTypesArray, $whereValuesArray);
		$numrowsinsert = 0;
		$count = 0;
		if ($numrowsupdate == 0) {
			//print "Could not update\n";
			$fromQuery = ' FROM ' . $table;
			if (count($updateColumnArray) > 0) {
				$fromQuery .= ' WHERE ' . $whereColumns;
			}
			$num = $this -> getcount_query($fromQuery, $updateTypesArray, $updateValuesArray);
			if ($num == 0) {
				$numrowsinsert = $this -> insert_query($updateColumnArray, $table, $updateTypesArray, $updateValuesArray);
				assert($numrowsinsert == 1);
			} else {
				print "This should be one: $num\n";
				assert($num == 1);
			}
		}
		$ids = $this -> getIdArray_query($updateColumnArray, $table, $updateTypesArray, $updateValuesArray);
		assert(count($ids) == 1);
		/*
		 print "VARDUMP:\n";
		 var_dump($ids);
		 print "\nERRORDUMP:\n";
		 var_dump($errors);*/
		return $ids[0];
	}

	public function get_nextval($sequenceId) {
		global $errors;
		$nextval = $this -> query("SELECT nextval(?)", array('text'), array($sequenceId));
		if ($nextval && count($nextval) == 1) {
			return $nextval[0][0];
		}
		die("FAILED: " . __FILE__ . "\n");
	}

	public function startTransactionIfPossible() {
		if ($this -> connection -> supports('transactions')) {
			$this -> connection -> beginTransaction();
		}
	}

	public function stopTransactionIfPossible() {
		if ($this -> connection -> supports('transactions') && $this -> connection -> in_transaction) {
			$this -> connection -> commit();
		}
	}

	public function addSubOrganism($subElementId, $parentElementId, $table) {
		$columnNameArray = array('left_value', 'right_value');
		$fromQuery = 'FROM ' . $table . ' WHERE id = ?';
		$typesArray = array('integer');
		$valuesArray = array($parentElementId);
		$parent = $this->select_query($columnNameArray, $fromQuery, $typesArray, $valuesArray, false);
		print "Parent: " . var_export($parent, true) . "\n";
	}

}
?>