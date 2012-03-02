<?php
/**
 * Returns the edit group page
 * 
 * @param	string	$sgroup_id	id of group to edit
 * @return	Array				Output array
 */
function sgroup_admin($sgroup_id) {
	// display short instruction text
	$save_txt = t('Save');
	$render_array['introduction'] = array(
			'#markup' => t(
				'Administrate the group permissions. When finished click "@save".',
				array('@save' => $save_txt,)),
			'#weight' => 1,
	);
	// get group infos
	$db_group = db_select('sgroup', 'g')->fields(
			'g',
			array('acl_id',
					'name',
			))
		->condition('g.id', $sgroup_id)
		->execute();
	if ($record = $db_group->fetch()) {
		// set group admin title
		drupal_set_title(t('Administrate ') . check_plain($record->name));
		$acl_id = $record->acl_id;
		// define permission defaults to pass to acl theme
		$formDefaults = array(
				array(
						'type' => variable_get("ACL_PERMISSION_NOT_SET"),
						'name' => t('Hidden'),
						'value' => TRUE,
				),
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
		// get group infos of system group ALL
		$db_group = db_select('sgroup', 'g')->fields(
				'g',
				array('id',
						'name',
				))
			->condition('g.id', variable_get("ACL_GROUP_ALL_ID"))
			->execute();
		if ($record = $db_group->fetch()) {
			$groupDefaults = array(
					(object) array(
							'id' => $record->id,
							'name' => $record->name,
							'level' => variable_get("ACL_RED_READ"),
					),
			);
		}
		$render_array['acl'] = array(
				'#theme' => 'c_acl',
				'#defaults' => $formDefaults,
				'#defaults_group' => $groupDefaults,
				'#acl_id' => $acl_id,
				'#add_element' => FALSE,
				'#weight' => 2,
		);
	} else {
		drupal_set_message(
			t(
				'Error occured permission settings load (group_id = "@group_id" does not exist).',
				array('@group_id' => $sgroup_id,)),
			'error');
	}

	return $render_array;
}
?>