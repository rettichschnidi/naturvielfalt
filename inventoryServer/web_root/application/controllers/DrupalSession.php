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
		// Role 1 is "anonymous" and role 2 is "authenticated". They are not represented in the database
		// but must be considered in the query.
		$result = pg_query_params($con, 'SELECT count(*) AS count FROM role_permission p
			LEFT JOIN users_roles r ON r.rid = p.rid
			LEFT JOIN users u ON u.uid = r.uid
			WHERE p.module = $2 AND p.permission = $3
			AND (u.uid = $1 OR p.rid = 1 OR p.rid = 2)', array($this->_userId, $module, $permission));
		$row = pg_fetch_assoc($result, 0);
		pg_close($con);
		return $row['count'] > 0;
	}
	
	public function canEditInventory($head_inventory_id)
	{
		$con = $this->getConnection();
		$result = pg_query_params($con, 'SELECT count(*)
											FROM head_inventory
											WHERE id = $1
											AND owner_id = $2', array($head_inventory_id,$this->_userId));
		
		$row = pg_fetch_assoc($result, 0);
		pg_close($con);
		return $row['count'] > 0;
	}
	
	private function getConnection() {
		include('../../../application/sites/default/settings.php');
		
		$database = $databases['default']['default'];
		$host = $database['host'];
		$dbname = $database['database'];
		$user = $database['username'];
		$password = $database['password'];
		
		return pg_connect('host='.$host.' dbname='.$dbname.' user='.$user.' password='.$password);
	}
}
?>