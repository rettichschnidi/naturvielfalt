<?php
/**
 * @file area.admin.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

/**
 * Edit the permissions for an area
 * @return render array
 */
function area_edit_permission($areadata) {
	$output = array();
	$defaults = array(
			array(
					'type' => variable_get("ACL_READ"),
					'name' => t('Read'),
					'value' => FALSE,
			),
			array(
					'type' => variable_get("ACL_WRITE"),
					'name' => t('Write'),
					'value' => FALSE,
			),
			array(
					'type' => variable_get("ACL_ADMIN"),
					'name' => t('Administration'),
					'value' => FALSE,
			)
	);

	$output['acl'] = array(
			'#theme' => 'c_acl',
			'#defaults' => $defaults,
			'#acl_id' => $areadata['acl_id'],
			'#add_element' => true,
	);
	return $output;
}
?>
