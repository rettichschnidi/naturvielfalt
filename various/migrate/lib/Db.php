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
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getUserInfo();
			return $results;
		}
		$res = $statement -> execute($valuesArray);
		if (PEAR::isError($res)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $res -> getMessage() . "\n" . $res -> getUserInfo();
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
	 * @param string $query
	 * @param array of types $typesArray
	 * @param array of values $valuesArray
	 * @return array
	 */
	public function select_query($columnNameArray, $fromQuery, $typesArray, $valuesArray, $assoziative = true, $ownselect = false) {
		global $errors;
		$finalArray = array();
		assert(count($typesArray) == count($valuesArray));
		$columns = implode(',', $columnNameArray);
		$query = ($ownselect ? $ownselect : 'SELECT') . ' ' . $columns . ' ' . $fromQuery;
		//print "Query: " . $query . "\n";
		//assoziative array
		if ($assoziative) {
			$statement = &$this -> connection -> prepare($query, $typesArray);
			if (PEAR::isError($statement)) {
				$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getMessage();
				$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getUserInfo();
				return $finalArray;
			}
			$rows = $statement -> execute($valuesArray);
			if (PEAR::isError($rows)) {
				$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $rows -> getMessage() . "\n" . $rows -> getUserInfo();
				return $finalArray;
			}
			$j = 0;
			while ($row = $rows -> fetchRow()) {
				$i = 0;
				foreach ($row as $field) {
					$finalArray[$j][$columnNameArray[$i++]] = $field;
				}
				$j++;
			}
			return $finalArray;
		}
		// plain array
		$rows = $this -> query($query, $typesArray, $valuesArray);
		if ($rows) {
			return $rows;
		}
		return array();
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
		$statement = &$this -> connection -> prepare($query, $typesArray);
		if (PEAR::isError($statement)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getMessage();
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getUserInfo();
			return false;
		}
		$rows = $statement -> execute($valuesArray);
		if (PEAR::isError($rows)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $rows -> getMessage() . "\n" . $rows -> getUserInfo();
			return false;
		}
		return true;
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
		$statement = &$this -> connection -> prepare($query, $typesArray);
		if (PEAR::isError($statement)) {
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getMessage();
			$errors[] = __FILE__ . "@" . __LINE__ . " :  " . $statement -> getUserInfo();
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
			return false;
		}
		return true;
	}

}
?>