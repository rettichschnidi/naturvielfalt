<?php
/**
 * This class works as a bridge from Zend to Drupal. It is used to check user rights.
 * The connection to Drupal is realised by the session id, which is sent as cookie to both frameworks (Zend and Drupal)
 *
 * @author roman
 *
 */
class DrupalSession {

	protected $_userId = -1;
	protected $_isLoggedIn = false;

	function __construct() {
		// find the session
		foreach ($_COOKIE as $key => $value) {
			if (substr($key, 0, 4) == 'SESS') {
				// found the session id
				// get the drupal session record
				$con = $this->getConnection();
				$result = pg_query_params($con, 'SELECT * FROM sessions WHERE sid = $1', array($value));
				$row = pg_fetch_assoc($result, 0);
				$this->_userId = $row['uid'];
				pg_close($con);
				$this->_isLoggedIn = true;
			}
		}
	}

	/**
	 * Returns true if the user is logged in. Otherwise false.
	 */
	public function isLoggedIn() {
		return $this->_isLoggedIn;
	}

	/**
	 * Returns the user id or -1 if no id could be found.
	 */
	public function getUserId() {
		return $this->_userId;
	}

	/**
	 * Checks if the user has the appropriate permission.
	 * 
	 * @param String $module The name of the module in which the permission is defined 
	 * @param String $permission The specific permission
	 * @return boolean true if permission is granted. Otherwise false.
	 */
	public function hasPermission($module, $permission) {
		if (!$this->_isLoggedIn) {
			return false;
		}
		$con = $this->getConnection();
		$result = pg_query_params($con, 'SELECT count(*) AS count FROM drupal_users u
			LEFT JOIN users_roles r ON u.uid = r.uid
			LEFT JOIN role_permission p ON r.rid = p.rid
			WHERE u.uid = $1 AND p.module = $2 AND p.permission = $3', array($this->_userId, $module, $permission));
		$row = pg_fetch_assoc($result, 0);
		pg_close($con);
		return $row['count'] > 0;		
	}
	
	private function getConnection() {
		return pg_connect('host=localhost dbname=swissmon user=postgres password=postgres');
	}
}
?>