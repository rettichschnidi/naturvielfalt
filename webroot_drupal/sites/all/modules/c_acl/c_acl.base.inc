<?php
/**
 * Copyright Intermesh
 *
 * This file is part of Group-Office. You should have received a copy of the
 * Group-Office license along with Group-Office. See the file /LICENSE.TXT
 *
 * If you have questions write an e-mail to info@intermesh.nl
 *
 * @copyright Copyright Intermesh
 * @version $Id$
 * @author Merijn Schering <mschering@intermesh.nl>
 * @package go.basic
 */

/**
 * This file is used to manage access control lists (ACL).
 *
 * ACL's can be used to secure items in Group-Office like addressbooks, calendars etc.
 *
 * @author Merijn Schering <mschering@intermesh.nl>
 * @version $Id$
 * @copyright Copyright Intermesh
 * @package go.basic
 *
 * @uses db
 */

/**
 * This file has been modified to be integrated in the naturvielfalt project.
 *
 * @author Simon Maurer
 */

variable_set("ACL_RESTRICT", -1);
variable_set("ACL_PERMISSION_NOT_SET", 1);
variable_set("ACL_RED_READ", 10);
variable_set("ACL_READ", 20);
variable_set("ACL_RED_WRITE", 30);
variable_set("ACL_WRITE", 40);
variable_set("ACL_ADMIN", 50);

/**
 * Add's a user group to an Access Control List
 *
 * @param	int			$acl_id		The ID of the Access Control List
 * @param	int			$group_id	The group_id to add to the ACL
 * @param	int			$level		permission level (optional)
 * @access	public
 * @return	bool					True on success
 */
function add_group_to_acl($acl_id, $group_id, $level = -1) {
	$res = false;
	if ($acl_id < 1 || $group_id < 1) {
		return false;
	}
	if ($level < 0) {
		$level = variable_get("ACL_RED_READ");
	}

	$exists = db_select('acl_sgroup', 'ag')->condition('ag.acl_id', $acl_id)
		->condition('ag.sgroup_id', $group_id)
		->fields('ag')
		->execute()
		->fetch();

	if ($exists) {
		// update
		$res = update_group_in_acl($acl_id, $group_id, $level);
	} else {
		// insert
		$res = db_insert('acl_sgroup')->fields(
				array('acl_id' => $acl_id,
						'sgroup_id' => $group_id,
						'level' => $level,
				))
			->execute();
		$res = ($res != NULL) ? TRUE : FALSE;
	}
	return $res;
}

/**
 * Adds a user to an Access Control List
 *
 * @param	int		$acl_id		The ID of the Access Control List
 * @param	int		$user_id	The user_id to add to the ACL
 * @param	int		$level		permission level (optional)
 * @access	public
 * @return	bool				True on success
 */
function add_user_to_acl($acl_id, $user_id, $level = -1) {
	$res = false;
	if ($acl_id < 1 || $user_id < 1) {
		return false;
	}
	if ($level < 0) {
		$level = variable_get("ACL_RED_READ");
	}

	$exists = db_select('acl_users', 'au')->condition('au.acl_id', $acl_id)
		->condition('au.users_id', $user_id)
		->fields('au')
		->execute()
		->fetch();

	if ($exists) {
		// update
		$res = update_user_in_acl($acl_id, $user_id, $level);
	} else {
		// insert
		$res = db_insert('acl_users')->fields(
				array('acl_id' => $acl_id,
						'users_id' => $user_id,
						'level' => $level,
				))
			->execute();
		$res = ($res != NULL) ? TRUE : FALSE;
	}
	return $res;
}

/**
 * Checks if a user is in the special admins group
 *
 * @param	int		$user_id	The user ID
 * @access	public
 * @return	bool				True on success
 */
function check_admin_permission($user_id = -1) {
	global $user;
	$my_user = NULL;
	$res = FALSE;
	if ($user_id < 0) {
		$my_user = $user;
	} else {
		$my_user = user_load($user_id);
	}

	if (in_array('administrator', array_values($my_user->roles))) {
		$res = true;
	}

	return $res;
}

/**
 * Checks presence of a group in an ACL
 *
 * @param	int		$acl_id		The ACL ID
 * @param	int		$group_id	The group ID
 * @access	public
 * @return	int					level of group permission
 */
function check_group_in_acl($acl_id, $group_id) {
	$res = false;
	$level = db_select('acl_sgroup', 'ag')->fields('ag', array('level'))
		->condition('ag.acl_id', $acl_id)
		->condition('ag.sgroup_id', $group_id)
		->execute()
		->fetch();

	if (!$level) {
		$res = $level;
	} else {
		$res = $level->level;
	}
	return $res;
}

/**
 * Check permission of user in acl id (option to check only user in groups)
 * 
 * @param	int		$acl_id			id of acl to check
 * @param	int		$min_level		minimum level to pass check
 * @param	int		$user_id		id of user to check (optional, default: active user)
 * @param	bool	$groups_only	set to true if check on groups only (optional, default: FALSE)
 * @access	public
 * @return	bool					TRUE on pass check
 */
function check_permission($acl_id, $min_level, $user_id = -1,
		$groups_only = FALSE) {
	global $user;
	$res = FALSE;
	$level = 0;
	if ($user_id < 0) {
		$user_id = $user->uid;
	}

	if (check_admin_permission($user_id)) {
		return TRUE;
	}

	// get level of All group
	$query = db_select('acl_sgroup', 'ag')->fields('ag', array('level',))
		->condition('ag.acl_id', $acl_id)
		->condition('ag.sgroup_id', variable_get("ACL_GROUP_ALL_ID"));
	$db_level = $query->execute();
	if ($record = $db_level->fetch()) {
		$level = $record->level;
	}

	if (($user_id > 0) && ($acl_id > 0)) {
		$query = db_select('acl_sgroup', 'ag')->fields('ag', array('level',))
			->condition('ag.acl_id', $acl_id);
		$query->join('sgroup', 'g', 'g.id = ag.sgroup_id');
		$query->join('acl_users', 'au', 'au.acl_id = g.acl_id');
		$query->condition('au.users_id', $user_id);
		$db_level = $query->orderBy('ag.level', 'DESC')
			->execute();
		if ($record = $db_level->fetch()) {
			$temp_level = $record->level;
			if ($temp_level > $level) {
				$level = $temp_level;
			}
		}
		if (!$groups_only) {
			// check user
			$temp_level = 0;
			$query = db_select('acl_users', 'au')->fields('au', array('level',))
				->condition('au.acl_id', $acl_id)
				->condition('au.users_id', $user_id);
			$db_level = $query->execute();
			if ($record = $db_level->fetch()) {
				$temp_level = $record->level;
				if ($temp_level > $level) {
					$level = $temp_level;
				}
			}
		}
	}
	if ($min_level <= $level) {
		$res = TRUE;
	}
	return $res;
}

/**
 * Checks presence of a user in an ACL
 *
 * @param	int		$acl_id		The ACL ID
 * @param	int		$user_id	id of user to check (optional, default: active user)
 * @access	public
 * @return	int					level of user permission
 */
function check_user_in_acl($acl_id, $user_id = -1) {
	global $user;
	$res = FALSE;
	if ($user_id == -1) {
		$user_id = $user->uid;
	}

	if (check_admin_permission($user_id)) {
		// if user is admin, return admin-level
		return variable_get("ACL_ADMIN");
	}

	$level = db_select('acl_users', 'au')->fields('au', array('level'))
		->condition('au.acl_id', $acl_id)
		->condition('au.users_id', $user_id)
		->execute()
		->fetch();

	if (!$level) {
		$res = $level;
	} else {
		$res = $level->level;
	}
	return $res;
}

/**
 * Checks if a user is admin of the Access Control List
 *
 * @param	int		$acl_id		The ID of the Access Control List
 * @param	int		$user_id	id of user to check (optional, default: active user)
 * @access	public
 * @return	bool
 */
function check_admin_acl($acl_id, $user_id = -1) {
	global $user;
	$res = FALSE;
	if ($user_id == -1) {
		$user_id = $user->uid;
	}
	if (check_admin_permission($user_id)) {
		// if user is admin
		$res = TRUE;
	} else if (check_permission($acl_id, variable_get("ACL_ADMIN"), $user_id)) {
		$res = TRUE;
	} else {
		$res = check_user_owns_acl($acl_id, $user_id);
	}
	return $res;
}

/**
 * Checks if a user owns Access Control List
 *
 * @param	int		$acl_id		The ID of the Access Control List
 * @param	int		$user_id	id of user to check (optional, default: active user)
 * @access	public
 * @return	bool
 */
function check_user_owns_acl($acl_id, $user_id = -1) {
	global $user;
	$res = FALSE;
	if ($user_id == -1) {
		$user_id = $user->uid;
	}
	$db_res = db_select('acl', 'a')->condition('id', $acl_id)
		->fields('a', array('users_id'))
		->execute()
		->fetch();
	if ($user_id == $db_res->users_id) {
		// user owns acl
		$res = TRUE;
	}
	return $res;
}

/**
 * Change ownership of an ACL
 *
 * @param	int		$acl_id		The ID of the Access Control List
 * @param	int		$user_id	id of user to set (optional, default: active user)
 * @access	public
 * @return	bool
 */
function chown_acl($acl_id, $user_id = -1) {
	if ($user_id == -1) {
		$user_id = $user->uid;
	}
	$res = db_update('acl')->fields(array('users_id' => $user_id))
		->condition('id', $acl_id)
		->execute();
	$res = ($res != 0) ? TRUE : FALSE;

	if ($res) {
		$res = add_user_to_acl($acl_id, $user_id, variable_get("ACL_ADMIN"));
	}

	return $res;
}

/**
 * Remove all users and user groups from an ACL
 *
 * @param	int		$acl_id		The ID of the Access Control List
 * @access	public
 * @return	bool				True on success
 */
function clear_acl($acl_id) {
	$res = FALSE;
	$num_deleted = db_delete('acl_users')->condition('acl_id', $acl_id)
		->execute();
	if ($num_deleted > 0) {
		$res = TRUE;
		$acl = get_acl($acl_id);
		if ($acl) {
			// add owner to acl
			add_user_to_acl($acl_id, $acl->users_id, variable_get("ACL_ADMIN"));
		}
	}
	$num_deleted = db_delete('acl_sgroup')->condition('acl_id', $acl_id)
		->execute();
	if ($num_deleted > 0) {
		$res = TRUE;
	}
	return $res;
}

/**
 * Copy the user and group permissions of one acl to another
 *
 * @param	int		$sAcl	The source ACL to copy
 * @param	int		$dAcl	The destination ACL to copy to (optional, if none is set, a new acl is created)
 * @param	int		$level	minimal permission level to copy (optional, by default all are copied)
 * @access	public
 * @return	int				The (new) destination ACL
 */
function copy_acl($sAcl, $dAcl = 0, $level = 0) {
	global $GO_CONFIG;

	if ($dAcl > 0) {
		clear_acl($dAcl);
	} else {
		$dAcl = new_acl();
	}

	$query = db_select('acl_users', 'au')->fields('au')
		->condition('au.acl_id', $sAcl)
		->condition('au.level', $level, '>=')
		->execute();

	while ($record = $query->fetch()) {
		add_user_to_acl($dAcl, $record->users_id, $record->level);
	}

	$query = db_select('acl_sgroup', 'ag')->fields('ag')
		->condition('ag.acl_id', $sAcl)
		->condition('ag.level', $level, '>=')
		->execute();

	while ($record = $query->fetch()) {
		add_group_to_acl($dAcl, $record->sgroup_id, $record->level);
	}
	return $dAcl;
}

/**
 * Deletes an Access Control List
 *
 * @param	int		$acl_id	The ID of the Access Control List
 * @access	public
 * @return	bool			True on succces
 */
function delete_acl($acl_id) {
	$res = true;
	$num_deleted = db_delete('acl_users')->condition('acl_id', $acl_id)
		->execute();
	$num_deleted += db_delete('acl_sgroup')->condition('acl_id', $acl_id)
		->execute();
	if ($num_deleted > 0) {
		$num_deleted = db_delete('acl')->condition('id', $acl_id)
			->execute();
		if ($num_deleted > 0) {
			$res = true;
		}
	}
	return $res;
}

/**
 * Deletes a user group from an Access Control List
 *
 * @param	int		$acl_id		The ID of the Access Control List
 * @param	int		$group_id	The group_id to delete from the ACL
 * @access	public
 * @return	bool				True on success
 */
function delete_group_from_acl($acl_id, $group_id) {
	$res = false;
	$num_deleted = db_delete('acl_sgroup')->condition('acl_id', $acl_id)
		->condition('sgroup_id', $group_id)
		->execute();
	if ($num_deleted > 0) {
		$res = true;
	}
	return $res;
}

/**
 * Deletes a user from an Access Control List
 *
 * @param	int		$acl_id		The ID of the Access Control List
 * @param	int		$user_id	The user_id to delete from the ACL
 * @access	public
 * @return	bool				True on success
 */
function delete_user_from_acl($acl_id, $user_id) {
	$res = false;
	$num_deleted = db_delete('acl_users')->condition('acl_id', $acl_id)
		->condition('users_id', $user_id)
		->execute();
	if ($num_deleted > 0) {
		$res = true;
	}
	return $res;
}

/**
 * Gets acl entry by id
 * 
 * @param	int		$acl_id	The ACL ID
 * @access	public
 * @return	Object			ACL entry of specified id
 */
function get_acl($acl_id) {
	$acl = db_select('acl', 'sa')->fields('sa')
		->condition('sa.id', $acl_id)
		->execute()
		->fetch();
	return $acl;
}

/**
 * Gets all groups from an ACL
 *
 * @param	int		$acl_id	The ACL ID
 * @param	int		$level	minimum permission level (optional)
 * @access	public
 * @return	Array			all sgroups from ACL (sg.id, sg.name, sal.level)
 */
function get_groups_in_acl($acl_id, $level = 0) {
	$query = db_select('sgroup', 'g')->fields('g', array('id',
				'name'
		));
	$query->join('acl_sgroup', 'ag', 'g.id = ag.sgroup_id');
	$db_groups = $query->fields('ag', array('level'))
		->condition('ag.acl_id', $acl_id)
		->condition('ag.level', $level, '>')
		->orderBy('g.name')
		->execute();

	$groups = array();
	while ($record = $db_groups->fetch()) {
		$groups[] = $record;
	}
	return $groups;
}

/**
 * Gets all users from an ACL
 *
 * @param	int		$acl_id		The ACL ID
 * @param	int		$level		minimum permission level (optional, by default all levels are selected)
 * @param	bool	$group_user	include all users from groups in acl (optional, default=false)
 * @access	public
 * @return	Array			all users from ACL (u.uid, u.name, sal.level)
 */
function get_users_in_acl($acl_id, $level = 0, $incl_group = false) {
	$query = db_select('users', 'u')->fields(
			'u',
			array('uid',
					'name',
			));
	$query->join('acl_users', 'au', 'au.users_id = u.uid');
	$db_users = $query->fields('au', array('level',))
		->condition('au.acl_id', $acl_id)
		->condition('au.level', $level, '>')
		->condition('u.uid', 0, '!=')
		->orderBy('u.name')
		->execute();

	$users = array();
	while ($record = $db_users->fetch()) {
		$users[] = $record;
	}
	//@todo: add users from group if param is set
	/*
	 $sql = "SELECT go_users_groups.user_id FROM go_users_groups INNER JOIN go_acl ON "
	        . "go_acl.group_id=go_users_groups.group_id WHERE go_acl.acl_id="
	        . intval($acl_id) . " AND go_users_groups.user_id!=0";
	
	if ($level > 0) {
	    $sql .= " AND level>=" . $this->escape($level);
	}
	
	$this->query($sql);
	while ($this->next_record()) {
	    if (!in_array($this->f('user_id'), $users)) {
	        $users[] = $this->f('user_id');
	    }
	} 
	 */
	return $users;
}

/**
 * Creates and returns a new Access Control List to secure an object
 * 
 * @param	string	$description	Description of the ACL (default='')
 * @param	int		$user_id		The owner of the new ACL (optional, default: active user)
 * @access	public
 * @return	int						The ID of the new Access Control List
 */
function new_acl($description = '', $user_id = -1) {
	global $user;
	$res = false;
	if ($user_id == -1) {
		$user_id = $user->uid;
	}

	$id = db_insert('acl')->fields(
			array('description' => $description,
					'users_id' => $user_id,
			))
		->execute();
	if (add_user_to_acl($id, $user_id, variable_get("ACL_ADMIN"))) {
		$res = $id;
	}
	return $res;
}

/**
 * Updates acl group element
 * 
 * @param	int			$acl_id		The ID of the Access Control List
 * @param	int			$group_id	The group_id to add to the ACL
 * @param	int			$level
 * @access	public
 * @return	bool					True on success
 */
function update_group_in_acl($acl_id, $group_id, $level) {
	$res = db_update('acl_sgroup')->fields(array('level' => $level))
		->condition('acl_id', $acl_id)
		->condition('sgroup_id', $group_id)
		->execute();
	$res = ($res != 0) ? TRUE : FALSE;
	return $res;
}

/**
 * Updates acl user element
 * 
 * @param	int			$acl_id		The ID of the Access Control List
 * @param	int			$user_id	The user_id to add to the ACL
 * @param	int			$level
 * @access	public
 * @return	bool					True on success
 */
function update_user_in_acl($acl_id, $user_id, $level) {
	$res = db_update('acl_users')->fields(array('level' => $level))
		->condition('acl_id', $acl_id)
		->condition('users_id', $user_id)
		->execute();
	$res = ($res != 0) ? TRUE : FALSE;
	return $res;
}
?>
