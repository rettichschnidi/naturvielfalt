<?php
/**
 * Returns the edit group page
 * 
 * @param	string	$sgroup_id	id of group to edit
 * @return	Array				Output array
 */
function sgroup_admin($sgroup_id) {
	// display short instruction text
	$render_array['introduction'] = array(
			'#markup' => t(
				'Administrate the group permissions. When finished click "Save".'),
			'#weight' => 1,
	);
	// get group infos
	$db_group = db_select('sgroup', 'g')->fields('g', array('acl_id', 'name',))
		->condition('g.id', $sgroup_id)
		->execute();
	if ($record = $db_group->fetch()) {
		// set group admin title
		drupal_set_title(t('Administrate ').check_plain($record->name));
		$acl_id = $record->acl_id;
		// define permission defaults to pass to acl theme
		$formDefaults = array(
				array(
						'type' => variable_get("ACL_READ"),
						'name' => t('Read'),
						'value' => TRUE,
				),
				array(
						'type' => variable_get("ACL_WRITE"),
						'name' => t('Write'),
						'value' => FALSE,
				),
				array(
						'type' => variable_get("ACL_ADMIN"),
						'name' => t('Admin'),
						'value' => FALSE,
				),
		);
		$render_array['acl'] = array(
				'#theme' => 'c_acl',
				'#defaults' => $formDefaults,
				'#acl_id' => $acl_id,
				'#users_only' => TRUE, // no subgroups possible, ACL modul does not support it (yet?)
				'#weight' => 2,
		);
	} else {
		drupal_set_message(
			t(
				'Error occured permission settings load (group_id = "'
						. $sgroup_id . '" does not exist).'),
			'error');
	}

	return $render_array;
}
?>