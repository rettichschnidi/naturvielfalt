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

global $PERMISSION_DEF;
$PERMISSION_DEF = Array(
	"PARTIAL_READ" => 1,
	"READ" => 1,
	
);

/**
 * Add's a user group to an Access Control List
 *
 * @param	int			$acl_id		The ID of the Access Control List
 * @param	int			$group_id	The group_id to add to the ACL
 * @param	int			$level
 * @access	public
 * @return	bool					True on success
 */
function add_group_to_acl($acl_id, $group_id, $level = 1) {
	$res = false;
	if ($acl_id < 1 || $group_id < 1) {
		return false;
	}

	$exists = db_select('sgroup_acl_link', 'sal')->condition('sal.acl_id',
			$acl_id)
		->condition('sal.sgroup_id', $group_id)
		->fields('sal')
		->execute()
		->fetch();

	if ($exists) {
		// update
		$res = update_group_in_acl($acl_id, $group_id, $level);
	}
	else {
		// insert
		$res = db_insert('sgroup_acl_link')->fields(
				array(
					'acl_id' => $acl_id,
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
 * @param	int		$level
 * @access	public
 * @return	bool				True on success
 */
function add_user_to_acl($acl_id, $user_id, $level = 1) {
	$res = false;
	if ($acl_id < 1 || $user_id < 1) {
		return false;
	}

	$exists = db_select('sgroup_acl_link', 'sal')->condition('sal.acl_id',
			$acl_id)
		->condition('sal.users_id', $user_id)
		->fields('sal')
		->execute()
		->fetch();

	if ($exists) {
		// update
		$res = update_user_in_acl($acl_id, $user_id, $level);
	}
	else {
		// insert
		$res = db_insert('sgroup_acl_link')->fields(
				array(
					'acl_id' => $acl_id,
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
/*
 * admin permission is checked y each module
function check_admin_permission($user_id) {
    global $GO_CONFIG;
    
    require_once($GLOBALS['GO_CONFIG']->class_path
            . 'base/groups.class.inc.php');
    $GO_GROUPS = new GO_GROUPS();
    
    if (!isset($this->is_admin))
        $this->is_admin = $GO_GROUPS
            ->is_in_group($user_id, $GLOBALS['GO_CONFIG']->group_root);
    
    return $this->is_admin;
     
}
 */

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
	$level = db_select('sgroup_acl_link', 'sal')->fields('sal',
			array(
				'level'
			))
		->condition('sal.acl_id', $acl_id)
		->condition('sal.sgroup_id', $group_id)
		->execute()
		->fetch();

	if (!$level) {
		$res = $level;
	}
	else {
		$res = $level->level;
	}
	return $res;
}

/**
 * Check permission of user in acl id (option to check only user in groups)
 * 
 * @param	int		$acl_id			id of acl to check
 * @param	int		$user_id		id of user to check
 * @param	bool	$groups_only	set to true if check on groups only
 * @access	public
 * @return	int						level of permission
 */
function check_permission($acl_id, $user_id = 0, $groups_only = false) {
	if ($user_id == 0)
		$user_id = $user->uid;

	if ($user_id > 0 && $acl_id > 0) {
		$query = db_select('sgroup_acl_link', 'sal')->fields('sal',
				array(
					'acl_id',
					'level'
				))
			->condition('sal.acl_id', $acl_id);
		$query->leftJoin('sgroup_users', 'su', 'su.sgroup_id = sal.sgroup_id');

		if (!$groups_only) {
			$query->condition(
					db_or()->condition('su.users_id', $user_id)
						->condition('sal.users_id', $user_id))
				->orderBy('sal.level', 'DESC');
		}
		else {
			$query->condition('su.users_id', $user_id);
		}

		$db_level = $query->execute();

		if ($record = $db_level->fetch()) {
			return $record->level;
		}
	}
	return false;
}

/**
 * Checks presence of a user in an ACL
 *
 * @param	int		$acl_id		The ACL ID
 * @param	int		$user_id	The user ID
 * @access	public
 * @return	int					level of user permission
 */
function check_user_in_acl($acl_id, $user_id = -1) {
	global $user;
	$res = false;
	if ($user_id == -1) {
		$user_id = $user->uid;
	}
	$level = db_select('sgroup_acl_link', 'sal')->fields('sal',
			array(
				'level'
			))
		->condition('sal.acl_id', $acl_id)
		->condition('sal.users_id', $user_id)
		->execute()
		->fetch();

	if (!$level) {
		$res = $level;
	}
	else {
		$res = $level->level;
	}
	;
	return $res;
}

/**
 * Checks if a user owns the Access Control List
 *
 * @param	int		$acl_id		The ID of the Access Control List
 * @param	int		$user_id	The owner of the ACL and the one who can modify it
 * @access	public
 * @return	bool
 */
function check_user_owns_acl($acl_id, $user_id) {
	$res = false;
	$db_res = db_select('sgroup_acl', 'sa')->condition('id', $acl_id)
		->fields('sa', array(
			'users_id'
		))
		->execute()
		->fetch();
	if ($user_id == $db_res->users_id) {
		// user owns acl
		$res = true;
	}
	/*	elseif ($db_res->users_id == '0') {
	        // 
	        $res = check_admin_permission($user_id);
	    }*/
	return $res;
}

/**
 * Change ownership of an ACL
 *
 * @param	int		$acl_id		The ID of the Access Control List
 * @param	int		$user_id	The owner of the ACL and the one who can modify it
 * @access	public
 * @return	bool
 */
function chown_acl($acl_id, $user_id) {
	$res = update_acl_owner($acl_id, $user_id);

	if ($res) {
		$res = add_user_to_acl($acl_id, $user_id, 99);
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
	$num_deleted = db_delete('sgroup_acl_link')->condition('acl_id', $acl_id)
		->execute();
	if ($num_deleted > 0) {
		$res = TRUE;
		$acl = get_acl($acl_id);
		if ($acl) {
			// add owner to acl
			add_user_to_acl($acl_id, $acl->users_id, 99);
		}
		// add root group with add_group_to_acl
		// no: -> admin permission is checked in corresponding module
	}
	return $res;
}

/**
 * Copy the user and group permissions of one acl to another
 *
 * @param	int		$sAcl	The source ACL to copy
 * @param	int		$dAcl	The destination ACL to copy to
 * @param	int		$level
 * @access	public
 * @return	int				The (new) destination ACL
 */
function copy_acl($sAcl, $dAcl = 0, $level = 0) {
	global $GO_CONFIG;

	if ($dAcl > 0) {
		clear_acl($dAcl);
	}
	else {
		$dAcl = new_acl();
	}

	$query = db_select('sgroup_acl_link', 'sal')->fields('sal')
		->condition('sal.acl_id', $sAcl)
		->condition('sal.level', $level, '>=')
		->execute();

	while ($record = $query->fetch()) {
		if ($record->sgroup_id != 0) {
			// group_id != root group
			// no: -> admin permission is checked in corresponding module
			add_group_to_acl($dAcl, $record->sgroup_id, $record->level);
		}
		if ($record->users_id != 0) {
			add_user_to_acl($dAcl, $record->users_id, $record->level);
		}
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
	$num_deleted = db_delete('sgroup_acl_link')->condition('acl_id', $acl_id)
		->execute();
	if ($num_deleted > 0) {
		$num_deleted = db_delete('sgroup_acl')->condition('id', $acl_id)
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
function delete_group_from_acl($acl_id, $group_id, $force_group_root = false) {

	$res = false;
	// if ($force_group_root /* || $group_id != root group / get drupal naturwerk root group*/) {
	// do not delete root group except $froce_group_root is set
	// no: -> admin permission is checked in corresponding module
	$num_deleted = db_delete('sgroup_acl_link')->condition('acl_id', $acl_id)
		->condition('sgroup_id', $group_id)
		->execute();
	if ($num_deleted > 0) {
		$res = true;
	}
	//}
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
	$num_deleted = db_delete('sgroup_acl_link')->condition('acl_id', $acl_id)
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
	$acl = db_select('sgroup_acl', 'sa')->fields('sa')
		->condition('sa.id', $acl_id)
		->execute()
		->fetch();
	return $acl;
}

/**
 * Gets all groups from an ACL
 *
 * @param	int		$acl_id	The ACL ID
 * @param	int		$level 
 * @access	public
 * @return	Array			all sgroups from ACL (sg.id, sg.name, sal.level)
 */
function get_groups_in_acl($acl_id, $level = 0) {
	$query = db_select('sgroup', 'sg')->fields('sg',
			array(
				'id',
				'name'
			));
	$query->join('sgroup_acl_link', 'sal', 'sg.id = sal.sgroup_id');
	$db_groups = $query->fields('sal', array(
			'level'
		))
		->condition('sal.acl_id', $acl_id)
		->condition('sal.level', $level, '>=')
		->orderBy('sg.name')
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
 * @param	int		$acl_id	The ACL ID
 * @param	int		$level 
 * @access	public
 * @return	Array			all users from ACL (u.uid, u.name, sal.level)
 */
function get_users_in_acl($acl_id, $level = 0) {
	$query = db_select('users', 'u')->fields('u',
			array(
				'uid',
				'name',
			));
	$query->join('sgroup_acl_link', 'sal', 'sal.users_id=u.uid');
	$db_users = $query->fields('sal', array(
			'level',
		))
		->condition('sal.acl_id', $acl_id)
		->condition('sal.level', $level, '>=')
		->condition('u.uid', 0, '!=')
		->orderBy('u.name')
		->execute();

	$users = array();
	while ($record = $db_users->fetch()) {
		$users[] = $record;
	}
	return $users;
}

/**
 * Creates and returns a new Access Control List to secure an object
 * 
 * @param	string	$description	Description of the ACL
 * @param	int		$user_id		The owner of the ACL and the one who can modify it
 *									default is the current logged in user.
 * @access	public
 * @return	int						The ID of the new Access Control List
 */
function new_acl($description = '', $user_id = -1) {
	global $user;
	$res = false;
	if ($user_id == -1) {
		$user_id = $user->uid;
	}

	$id = db_insert('sgroup_acl')->fields(
			array(
				'description' => $description,
				'users_id' => $user_id,
			))
		->execute();
	//$this->add_group_to_acl($id, $GLOBALS['GO_CONFIG']->group_root, 99);
	// bypass manage permissions if admin
	// no: -> admin permission is checked in corresponding module
	if (add_user_to_acl($id, $user_id, 99)) {
		$res = $id;
	}
	return $res;
}

/**
 * Set's the owner of an access control list
 *
 * @param	int		$acl_id		The ID of the Access Control List
 * @param	int		$user_id	The user ID of the new owner
 * @access	public
 * @return	bool				True on success
 */
function update_acl_owner($acl_id, $user_id) {
	$ret = FALSE;
	$res = db_update('sgroup_acl')->fields(
			array(
				'users_id' => $user_id
			))
		->condition('id', $acl_id)
		->execute();
	if ($res != 0) {
		$ret = TRUE;
	}
	return $ret;
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
	$res = db_update('sgroup_acl_link')->fields(
			array(
				'level' => $level
			))
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
	$res = db_update('sgroup_acl_link')->fields(
			array(
				'level' => $level
			))
		->condition('acl_id', $acl_id)
		->condition('users_id', $user_id)
		->execute();
	$res = ($res != 0) ? TRUE : FALSE;
	return $res;
}

/*
 * @todo: do we need this?
function get_global_read_only_acl() {
    global $GO_CONFIG;

    $acl_id = $GLOBALS['GO_CONFIG']->get_setting('global_read_only_acl');
    if (!$acl_id) {
        $acl_id = $this->new_acl('global', 1);

        $this->set_read_only_acl_permissions($acl_id);

        $GLOBALS['GO_CONFIG']->save_setting('global_read_only_acl', $acl_id);
    }

    return $acl_id;
}
 */
/*
 * @todo: do we need this?
function set_read_only_acl_permissions($acl_id = false) {
    global $GO_CONFIG;

    if (!$acl_id)
        $acl_id = $GLOBALS['GO_CONFIG']->get_setting('global_read_only_acl');

    $this->delete_group_from_acl($acl_id, $GLOBALS['GO_CONFIG']->group_root, true);
    $this->add_group_to_acl($acl_id, $GLOBALS['GO_CONFIG']->group_everyone);
    $this->delete_user_from_acl($acl_id, 1);
}
 */

/**
 * Get all groups that are connected to a given acl.
 *
 * This function fetches all groups that have permissions for the given acl,
 * and returns an array of IDs.
 *
 * @access public
 *
 * @param Integer $acl_id is the ID whose groups should be fetched.
 *
 * @return Array of the group IDs.
 */
/*
 * @todo: use function get_groups_in_acl?
function get_group_ids_from_acl($acl_id) {
    trigger_error('get_group_ids_from_acl() is an abstract method.',
        E_USER_ERROR);
    return false;
}
 */

/**
 * Get's all authorized users from an ACL
 *
 * @param	int			$acl_id	The ACL ID
 * @param	int			$level The minimum permission level
 *
 * @access public
 * @return Array			The user id's
 */
/*
 * @todo: do we need this?
 * -> gets all users in acl incl. users from groups in acl.
function get_authorized_users_in_acl($acl_id, $level = 0) {
    $users = array();
    $sql = "SELECT user_id FROM go_acl WHERE acl_id='" . $this->escape($acl_id)
            . "' AND user_id!=0";

    if ($level > 0) {
        $sql .= " AND level>=" . $this->escape($level);
    }

    $this->query($sql);
    while ($this->next_record()) {
        $users[] = $this->f('user_id');
    }

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
    return $users;
}
 */

/**
 * Get's an ACL id based on the desciption. Use carefully.
 *
 * @param	string			$description	The description of an ACL
 * @access public
 * @return int			True if the group is in the ACL
 */
/*
 * @todo: any use for this?
function get_acl_id($description) {
    $sql = "SELECT id FROM go_acl_items WHERE description='"
            . $this->escape($description) . "'";
    $this->query($sql);
    if ($this->next_record()) {
        return $this->f('id');
    }
    return false;
}
 */

/**
 * Checks if an ACL exists in acl_items. Use carefully!
 *
 * Returns:
 *	false if the acl does not exist
 *	true if the acl does exist in acl_items
 *
 * @param int $acl_id
 * @access public
 * @return bool
 */
/*
 * @todo: any use for this?
function acl_exists($acl_id) {
    $sql = "SELECT * FROM go_acl_items WHERE id='" . $this->escape($acl_id)
            . "'";
    $this->query($sql);
    if ($this->num_rows() != 0) {
        return true;
    }
    #    $sql = "SELECT * FROM acl WHERE acl_id='$acl_id'";
    #    $this->query($sql);
    #    if ( $this->num_rows() != 0 ) {
    #      $retval += 2;
    #    }
    return false;
}
 */
?>