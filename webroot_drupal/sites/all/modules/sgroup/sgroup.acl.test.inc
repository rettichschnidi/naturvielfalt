<?php 
/**
 * 
 * Enter description here ...
 */
function run_acl_test() {
	global $user;
	
	$output['introduction'] = array(
			'#markup' => t("ACL tests are executed and the result is listed in the table below"),
	);
	$success = t('success');
	$fail = t('fail');
	$tableHeader[] = t('Name');
	$tableHeader[] = t('Description');
	$tableHeader[] = t('Result');
	
	$row = array();
	$row[] = t('New ACL by active User');
	$row[] = t('Create a new ACL by the active user');
	$id = new_acl("Description #1");
	$doNext = FALSE;
	if ($id != NULL) {
		$row[] = $success;
		$doNext = TRUE;
	}
	else {
		$row[] = $fail;
	}
	$rows[] = $row;
	
	if (doNext) {
		$row = array();
		$row[] = t('Active user in ACL');
		$row[] = t('Check if active uesr is in created ACL id='.$id);
		$lvl = check_user_in_acl($id);
		if ($lvl == 4) {
			$row[] = $success;
		}
		else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}
	
	$row = array();
	$row[] = t('Create new ACL');
	$row[] = t('Create a new ACL by user_id = 2');
	$id = new_acl("Description #2", 2);
	$doNext = FALSE;
	if ($id != NULL) {
		$row[] = $success;
		$doNext = TRUE;
	}
	else {
		$row[] = $fail;
	}
	$rows[] = $row;
	
	if (doNext) {
		$row = array();
		$row[] = t('User in ACL');
		$row[] = t('Check if user_id=2 is in created ACL id='.$id);
		$lvl = check_user_in_acl($id, 2);
		if ($lvl == 4) {
			$row[] = $success;
		}
		else {
			$row[] = $fail;
		}
		$rows[] = $row;
	}
	
	$table = array(
			'#theme' => 'table',
			'#header' => $tableHeader,
			'#rows' => $rows
	);
}

?>