<?php
// noteresource.inc

/**
 * Callback for retrieving note resources.
 *
 * @param int $id
 * @return object
 */
function _organismresource_retrieve($id) {
	global $user;

	$notes = array();

	if ($id == 16) {
		$res = db_query(
				"SELECT 
					o.id,
					f.name,
					\"Familie\",
					\"Gattung\",
					\"Art\",
					f.name_de,
					f.is_neophyte,
					f.status
				FROM
					flora_organism AS f
					INNER JOIN organism as o ON o.organism_type = 2 AND o.organism_id = f.id
				ORDER BY
					f.name_de;");
	} else {
		$res = db_query(
				"SELECT
					fa.id,
					i.name,
					family,
					protection_ch,
					cscf_nr,
					fa.name_de,
					fa.genus,
					fa.species,
					fa.fauna_class_id
				FROM 
					organism AS o
					INNER JOIN fauna_organism as fa ON fa.fauna_class_id = o.organism_id
					INNER JOIN inventory_type as i ON i.id = fa.fauna_class_id
			WHERE
				o.organism_type = 1 AND i.id = " . $id . " ORDER BY fa.name_de"); // FIXME: SQL injection?!
	}

	foreach ($res as $note) {
		$notes[] = $note;
	}

	return $notes;
}

//FIXME: document me...
/**
 * 
 * Enter description here ...
 * @param unknown_type $page
 * @param unknown_type $parameters
 * @return
 */
function _organismresource_index($page, $parameters) {
	global $user;

	$notes = array();

	$res = db_query(
			'SELECT
				name,
				inventory_type_id,
				count(inventory_type_id)
			FROM
				organism
				LEFT JOIN inventory_type it ON it.id = organism.inventory_type_id
			GROUP BY
				inventory_type_id, name
			ORDER BY
				name');

	foreach ($res as $note) {
		$notes[] = $note;
	}

	return $notes;
}

/**
 * Access callback for the note resource.
 *
 * @param string $op
 *  The operation that's going to be performed.
 * @param array $args
 *  The arguments that will be passed to the callback.
 * @return bool
 *  Whether access is given or not.
 */
//FIXME: document me...
//FIXME: SECURITY???!
function _organismresource_access($op, $args) {
	global $user;
	/*
	$access = FALSE;

	switch ($op) {
	case 'view':
	   $note = noteresource_get_note($args[0]);
	   $access = user_access('note resource view any note');
	   $access = $access || $note->uid == $user->uid && user_access('note resource view own notes');
	   break;
	case 'update':
	   $note = noteresource_get_note($args[0]->id);
	   $access = user_access('note resource edit any note');
	   $access = $access || $note->uid == $user->uid && user_access('note resource edit own notes');
	   break;
	case 'delete':
	   $note = noteresource_get_note($args[0]);
	   $access = user_access('note resource delete any note');
	   $access = $access || $note->uid == $user->uid && user_access('note resource delete own notes');
	   break;
	}
	 */

	$access = TRUE;

	return $access;
}

?>