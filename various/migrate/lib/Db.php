<?php
/**
 * @author Reto Schneider, 2011, github@reto-schneider.ch
 */

require_once('MDB2.php');
require_once(dirname(__FILE__) . '/bootstrap.php');
require_once($configdir . '/databases.php');

/**
 * A simple class to access a database.
 * Using the MDB2 internally.
 */
class Db {
	private $dsn = false;
	private $connection = false;

	/**
	 * Connect to the database with the given credentials.
	 * 
	 * @param unknown_type $dbdriver
	 * @param unknown_type $dbname
	 * @param unknown_type $user
	 * @param unknown_type $password
	 * @param unknown_type $host
	 * @param unknown_type $port
	 */
	public function __construct($dbdriver, $dbname, $user, $password, $host, $port = 5432) {
		$this->dsn = array(
			'phptype'  => $dbdriver,
			'username' => $user,
			'password' => $password,
			'hostspec' => $host,
			'database' => $dbname,
			'port' => $port
		);
		Db::connectDbLazy();
		$this->connection
			->loadModule('Extended');
	}

	/**
	 * Disconnect before object gets destroyed.
	 */
	public function __destruct() {
		$this->connection
			->disconnect();
	}

	/**
	 * Connect to databases, but create instance not before the first time
	 * a query gets executed.
	 */
	private function connectDbLazy() {
		$this->connection = MDB2::factory($this->dsn);
		if (PEAR::isError($this->connection)) {
			die(
				"Error while lazy connecting: "
						. $this->connection
							->getMessage() . "\ndsn was: "
						. print_r($this->dsn) . "\n");
		}
	}

	/**
	 * Execute an arbritary query.
	 * Returns the query result as an array or false if nothing returned.
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
	 * @param boolean $isSelect Wether this query is a select query or not
	 * @param boolean $assoziativeResult Wethere the result should be returned
	 * 	as assoziative array (or as object)
	 * @return array
	 */
	public function query($query, $typesArray = array(), $valuesArray = array(),
			$isSelect = true, $assoziativeResult = true) {
		$results = false;
		if ($isSelect) {
			$statement = &$this->connection
				->prepare($query, $typesArray);
		} else {
			$statement = &$this->connection
				->prepare($query, $typesArray, MDB2_PREPARE_MANIP);
		}
		if (PEAR::isError($statement)) {
			$errors[] = $statement->getMessage();
			$errors[] = $statement->getUserInfo();
			$errors[] = "Query : " . $query;
			$errors[] = "Types : " . var_export($typesArray, true);
			$errors[] = "Values : " . var_export($valuesArray, true);
			throw new Exception(implode("\n", $errors));
		}
		$res = $statement->execute($valuesArray);
		if (PEAR::isError($res)) {
			$errors[] = $res->getMessage();
			$errors[] = $res->getUserInfo();
			$errors[] = "Query : " . $query;
			$errors[] = "Types : " . var_export($typesArray, true);
			$errors[] = "Values : " . var_export($valuesArray, true);
			throw new Exception(implode("\n", $errors));
		}
		$statement->free();
		if ($isSelect) {
			// return the results set
			if ($assoziativeResult) {
				$this->connection
					->setFetchMode(MDB2_FETCHMODE_ASSOC);
			} else {
				$this->connection
					->setFetchMode(MDB2_FETCHMODE_ORDERED);
			}
			$results = array();
			while ($row = $res->fetchRow()) {
				$results[] = $row;
			}
			return $results;
		} else {
			// return the number of affected rows
			return $res;
		}
		die(__FILE__ . '@' . __LINE__ . ": WTF?\n");
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
	public function select_query($columnNameArray, $fromQuery, $typesArray,
			$valuesArray, $assoziative = true, $ownselect = 'SELECT ') {
		$finalArray = array();
		assert(count($typesArray) == count($valuesArray));
		assert(is_array($columnNameArray));
		assert(is_array($typesArray));
		assert(is_array($valuesArray));
		$columns = implode(',', $columnNameArray);
		$query = $ownselect . ' ' . $columns . ' ' . $fromQuery;
		if (FALSE) {
			print "SELECTQUERY:  $query\n";
			print "SELECTTYPES:  " . var_export($typesArray, true) . "\n";
			print "SELECTVALUES: " . var_export($valuesArray, true) . "\n";
		}
		$statement = &$this->connection
			->prepare($query, $typesArray);
		if (PEAR::isError($statement)) {
			$errors[] = $statement->getMessage();
			$errors[] = $statement->getUserInfo();
			$errors[] = "Query : " . $query;
			$errors[] = "Types : " . var_export($typesArray, true);
			throw new Exception(implode("\n", $errors));
		}
		$rows = $statement->execute($valuesArray);
		if (PEAR::isError($rows)) {
			$errors[] = $rows->getMessage();
			$errors[] = $rows->getUserInfo();
			$errors[] = "Query : " . $query;
			$errors[] = "Types : " . var_export($typesArray, true);
			$errors[] = "Values : " . var_export($valuesArray, true);
			throw new Exception(implode("\n", $errors));
		}
		if ($assoziative) {
			$this->connection
				->setFetchMode(MDB2_FETCHMODE_ASSOC);
		} else {
			$this->connection
				->setFetchMode(MDB2_FETCHMODE_ORDERED);
		}
		$statement->free();
		while ($row = $rows->fetchRow()) {
			$finalArray[] = $row;
		}
		$statement->free();
		return $finalArray;
	}

	/**
	 * 
	 * Exectue an insert SQL statement.
	 * 
	 * @param unknown_type $columnArray
	 * @param unknown_type $table
	 * @param unknown_type $typesArray
	 * @param unknown_type $valuesArray
	 * @throws Exception
	 * @return unknown
	 */
	public function insert_query($columnArray, $table, $typesArray,
			$valuesArray) {
		$finalArray = array();
		assert(count($typesArray) == count($valuesArray));
		$columns = implode(',', $columnArray);
		$questionmarks = '?';
		for ($i = 0; $i < count($columnArray) - 1; $i++) {
			$questionmarks .= ', ?';
		}
		$query = 'INSERT INTO ' . $table . ' (' . $columns . ')' . ' VALUES('
				. $questionmarks . ')';
		if (FALSE) {
			print "QUERY: $query\n";
			print "TYPES: " . var_export($typesArray, true) . "\n";
			print "VALUES:" . var_export($valuesArray, true) . "\n";
		}
		$statement = &$this->connection
			->prepare($query, $typesArray, MDB2_PREPARE_MANIP);
		if (PEAR::isError($statement)) {
			$errors[] = $statement->getMessage();
			$errors[] = $statement->getUserInfo();
			$errors[] = "Query : " . $query;
			$errors[] = "Types : " . var_export($typesArray, true);
			throw new Exception(implode("\n", $errors));
		}
		$rows = $statement->execute($valuesArray);
		if (PEAR::isError($rows)) {
			$errors[] = $rows->getMessage();
			$errors[] = $rows->getUserInfo();
			$errors[] = "Query : " . $query;
			$errors[] = "Types : " . var_export($typesArray, true);
			$errors[] = "Values : " . var_export($valuesArray, true);
			throw new Exception(implode("\n", $errors));
		}
		$statement->free();
		return $rows;
	}

	/**
	 * Raise a given id_seq atomic and return the value.
	 * @param string Name of the sequence (usually $TABLENAME_id_seq)
	 * @throws Exception
	 */
	public function get_nextval($sequenceId) {
		$sql = "SELECT nextval(?) as nextval";
		$nextval = $this->query(
				$sql,
				array('text'),
				array($sequenceId),
				true,
				true);
		if ($nextval && count($nextval) == 1) {
			return $nextval[0]['nextval'];
		}
		$errors[] = "Nextval: " . var_export($nextval, true) . "\n";
		$errors[] = "SQL: $sql\n";
		$errors[] = "SequenceId:" . $sequenceId . "\n";
		$errors[] = "Errors: " . var_export($errors, true) . "\n";
		throw new Exception(implode("\n", $errors));
	}

	/**
	 * Start a transaction if possible.
	 * If not possible, print an errormessage on stdout and return.
	 */
	public function startTransactionIfPossible() {
		if ($this->connection
			->supports('transactions')
				&& !$this->connection
					->in_transaction) {
			$this->connection
				->beginTransaction();
			// 			print "Started transaction sucessfully.\n";
		} else {
			print "Starting transaction failed.\n";
		}
	}

	/**
	 * End a transaction if possible.
	 * If not possible, print an errormessage on stdout and return.
	 */
	public function stopTransactionIfPossible() {
		if ($this->connection
			->supports('transactions')
				&& $this->connection
					->in_transaction) {
			$this->connection
				->commit();
			// 			print "Stopped transaction sucessfully.\n";
		} else {
			print "Stopping transaction failed.\n";
		}
	}

}
?>
