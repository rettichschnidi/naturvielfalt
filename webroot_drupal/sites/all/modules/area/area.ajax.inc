<?php
/**
 * @file area.ajax.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */
module_load_include('inc', 'area', 'area.create');
module_load_include('inc', 'area', 'area.show');
module_load_include('inc', 'area', 'area.delete');

/**
 * Create a new area or update it (only from webservice).
 *
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 *  @param boolean $from_api
 *  true if method was invoked from the webservice api
 * @todo Maybe use the ajax version?
 */
function area_save($form = false, &$form_state, $from_api = false) {
	global $user;
	if (false) {
		$debugmessage = var_export($form_state['values'], true);
		drupal_set_message("Creating area: <pre>$debugmessage</pre>");
	}
	$areaid = 0;
	
	/*
	 * Get submitted values and escape values before submitting
	* them to the database.
	*/
	if($from_api) {
		$name = filter_xss($form_state['fieldname']);
		$zip = filter_xss($form_state['zip']);
		$canton = filter_xss($form_state['canton']);
		$country = filter_xss($form_state['country']);
		$township = filter_xss($form_state['township']);
		$areaType = filter_xss($form_state['area_type']);
		$altitude = !empty($form_state['altitude'])
		? filter_xss($form_state['altitude']) : 0;
		$jsonCoordinates = filter_xss($form_state['area-coords']);
		$comment = isset($form_state['comment']) ? filter_xss($form_state['comment']) : '';
		//put NULL value in db, if no parent_id or 0 has been passed
		$parent_id = (isset($form_state['parent_area']) && (int) $form_state['parent_area'] > 0) ? filter_xss($form_state['parent_area']) : NULL;
		$date_parts = explode('.', filter_xss($form_state['date']));
		$timesplit = array(null, null);
		if(isset($form_state['time'])) $timesplit = explode(':', filter_xss($form_state['time']));

		$timestamp = mktime($timesplit[0], $timesplit[1], null, $date_parts[1], $date_parts[0], $date_parts[2]);
		$createdate = date('Y-m-d H:i:s', $timestamp);

		$areaid = intval($form_state['area_id']);
	} else {
		//put NULL value in db, if no parent_id or 0 has been passed
		$parent_id = (isset($form_state['values']['parent_area']) && (int) $form_state['values']['parent_area'] > 0) ? filter_xss($form_state['values']['parent_area']) : NULL;

		$name = filter_xss(
				!empty($form_state['values']['fieldname'])
				? $form_state['values']['fieldname'] : date("r"));
		$zip = filter_xss($form_state['values']['zip']);
		$canton = filter_xss($form_state['values']['canton']);
		$country = filter_xss($form_state['values']['country']);
		$township = filter_xss($form_state['values']['township']);
		$areaType = filter_xss($form_state['values']['area_type']);
		$altitude = !empty($form_state['values']['altitude'])
		? filter_xss($form_state['values']['altitude']) : 0;
		$jsonCoordinates = filter_xss($form_state['values']['area-coords']);
		$comment = '';
		$createdate = 'NOW()';
	}

	$values = $_POST;
	if($from_api) $values = $custom_data;

	$userid = isset($form_state['set_user_uid']) && $form_state['set_user_uid'] ? $form_state['set_user_uid']
	: $user->uid;

	if (empty($jsonCoordinates) || empty($areaType)) {
		debug($areaType, 'areaType');
		drupal_set_message(t('New area could not be created.'), 'error');
		$form_state['redirect'] = 'area/new';
		return;
	}
	$transaction = db_transaction();
	try {
		//update area
		if($areaid > 0) {
			$area = area_get_full_dataset_of_specific_area($areaid);
			$area_geometry_id = $area['area_geometry_id'];
			$acl_id = $area['acl_id'];
				
			area_update_geometry($area_geometry_id, array('area_points' => $jsonCoordinates,
			'type' => $areaType,
			'zip' => $zip,
			'canton' => $canton,
			'country' => $country,
			'township' => $township,
			'altitude' => $altitude,
			), $from_api ? true : false);
			
			db_update('area')->fields(
			array(
			'name' => $name,
			'comment' => $comment,
			'create_time' => $createdate,
			'modify_time' => 'NOW()',
			'acl_id' => $acl_id,
			'area_geometry_id' => $area_geometry_id,
			'parent_id' => $parent_id
			))
			->condition('id', $areaid)
			->execute();
			$output['update'] = true;
		}
		//create new area
		else {
			$geometry_id = area_create_geometry(
					array('area_points' => $jsonCoordinates,
							'type' => $areaType,
							'zip' => $zip,
							'canton' => $canton,
							'country' => $country,
							'township' => $township,
							'altitude' => $altitude,
					), $from_api ? true : false);
			assert($geometry_id);
			$acl_id = new_acl('area', $userid);
			$areaid = db_insert('area')->fields(
					array(
							'name' => $name,
							'comment' => $comment,
							'create_time' => $createdate,
							'modify_time' => 'NOW()',
							'acl_id' => $acl_id,
							'area_geometry_id' => $geometry_id,
							'parent_id' => $parent_id
					))
					->execute();
				
			// Read access for ALL group
			add_group_to_acl($acl_id, variable_get('ACL_GROUP_ALL_ID'), variable_get('ACL_RED_WRITE'));
		}

		/**
		 * Store incoming medias from webservice
		 */
		if($from_api) {
	
			$image = array(
					'item_id' => $areaid,
					'author' => $user->name,
					'location' => $township . ' ' . $canton,
			);
	
			gallery_implementation_save_manual('area', $image, 'area_id');
		}

		$output['success'] = true;
		$output['guid'] = $areaid;

	} catch (Exception $e) {
		$transaction->rollback();
		watchdog_exception('area', $e);
		$output['success'] = false;
		$output['message'] = array(
				t('Could not save the area because of an unknown error.'),
				$e->getMessage()
		);
		drupal_set_message(t('New area could not be created.'), 'error');
		$form_state['redirect'] = 'area/new';
	}

	if ($from_api) {
		return $output;
	}
	else {
		drupal_set_message(
		t('Area "@areaname" got created.', array('@areaname' => $name)));
		$form_state['redirect'] = 'area/' . $areaid . '/show';
	}
}

/**
 * Renders a form which allows the user to submit an area name.
 * For usage with ajax.
 */
function area_get_new_areaname_form_callback($area_id = NULL) {
	$area_data = NULL;
	if($area_id) {
		$area_data = area_load($area_id);
	}
	print drupal_render(drupal_get_form('area_new_areaname_form', $area_data));
}

/**
 * Renders a form which allows the user to submit an area name.
 * For usage with ajax.
 */
function area_overview_ajax_callback($areadata = NULL) {
	print drupal_render(drupal_get_form('area_overview_form', $areadata));
}

function area_show_ajax_callback($areadata = NULL) {
	$output['map'] = array(
			'#search' => true,
			'#create' => true,
			'#showall' => true,
			'#theme' => 'area'
	);

	$output['button'] = array(
			'#type' => 'submit',
			'#value' => t('Close')
	);

	return $output;
}

/**
 * Deletes a set of areas. The parameter "areas" is a
 * list of all areas to delete separated with a coma and brackets:
 * {1,23,443,4343)
 */
function area_delete() {
	global $user;

	if ($user->uid == 0) {
		$output['success'] = false;
		$output['type'] = 'error';
		$output['message'] = t('Please log in first');
		return drupal_json_output($output);
	}

	if (!isset($_REQUEST['areas'])) {
		$output['success'] = false;
		$output['type'] = 'error';
		$output['message'] = t('Parameter \'areas\' missing');
		return drupal_json_output($output);
	}

	$param = filter_xss($_REQUEST['areas']);
	$areas = explode(',', substr($param, 1, strlen($param) - 2));

	$deleteCount = 0;
	foreach ($areas as $area_id) {
		$area = area_get_full_dataset_of_specific_area($area_id);
		if (check_admin_acl($area['acl_id'])) {
			$inventories = inventory_get_area_inventories($area['id']);
			$children = area_get_children($area, false, false);
			if(!count($inventories) && !$children['count']) {
				area_delete_by_id($area_id);
				$deleteCount++;
			}
		}
	}

	$output['success'] = true;
	$output['count'] = $deleteCount;
	if ($deleteCount == count($areas)) {
		$output['type'] = 'status';
		$output['message'] = ($deleteCount == 1)
		? t('Area deleted')
		: t('Areas deleted');
	}
	else {
		$output['type'] = 'warning';
		$output['message'] = t('Some areas could not be deleted (no permission, or the area may still contain inventories or subareas.)');
	}

	return drupal_json_output($output);
}


/**
 * Exports a set of areas.
 * The parameter "columns" is an array of the column indexes to export.
 * The parameter "areas" is an array of all observation ids to export.
 */
function area_export() {
	
	// check if user is logged in
	global $user;
	if ($user->uid == 0) {
		$output['success'] = false;
		$output['type'] = 'error';
		$output['message'] = t('Please log in first');
		return drupal_json_output($output);
	}
	
	$failed = true;
	
	// get the areas of the request
	$param = filter_xss($_REQUEST['areas']);
	
	$export_id = uniqid();
	$base_query = "SELECT geom FROM naturvielfalt_area_geometry a_g, naturvielfalt_area a WHERE a_g.id = a.area_geometry_id AND a.id in $param AND GeometryType(geom)=";

	export_shape_ogr2ogr( "POINT", $base_query, $export_id);
	export_shape_ogr2ogr( "LINESTRING", $base_query, $export_id);
	export_shape_ogr2ogr( "POLYGON", $base_query, $export_id);

 	// zip
	$tempdir = file_directory_temp();

	// FOR WINDOWS
	// 	$cmd = strtr('"%path7z.exe" a %archive %shapefile'
	// 			, array(
	// 					'%path' => variable_get('area_path_zip', AREA_PATH_ZIP),
	// 					'%archive' => $tempdir .'\\'.$export_id .'.zip' ,
	// 					'%shapefile' => file_directory_temp() .'\\'. $export_id.'*.*'
	// 			)
	// 	);

	$cmd = strtr('tar -zcvf %archive %shapefile'
			, array(
					'%path' => variable_get('area_path_zip', AREA_PATH_ZIP),
					'%archive' => $tempdir .'/'.$export_id .'.tar' ,
					'%shapefile' => file_directory_temp() .'/'. $export_id.'*.*'
			)
	);
	
	shell_exec($cmd);
	//echo($cmd.'<br>');
	
	// open for return
	$filename = $tempdir .'/'.$export_id .'.tar'; 
	$handle = fopen($filename, "r");
	$contents = fread($handle, filesize($filename));
	fclose($handle);
	
	// move to archive
	// TODO use drupal function
	$cmd = strtr('mv %file %archive '
			, array(
					'%file' => $tempdir ."/".$export_id .'.tar' ,
					'%archive' => file_directory_temp() .'/export_archive/'
			)
	);
	
	shell_exec($cmd);
	//echo($cmd.'<br>');

	// delete 
	// TODO use drupal function
	$cmd = strtr('rm %files'
			, array(
					'%files' => file_directory_temp() ."/". $export_id.'*.*'
			)
	);
	shell_exec($cmd);
	//echo($cmd.'<br>');
	
	// set header information
	header('Content-Type: application/octet-stream');
	header('Content-Disposition: attachment; filename="export.tar"');
	echo $contents;
}

function export_shape_ogr2ogr($geometrytype, $base_query, $export_Id){
	$query = $base_query."'".$geometrytype."'";
	
	// check if there are any shapes at all for this type
	$result = db_query($query);
	
	if($result->rowCount() > 0)
	{
		global $databases;
		
		// FOR WINDOWS
// 		$cmd = strtr('"%ogr2ogr_pathogr2ogr.exe" -f "ESRI Shapefile" %shapefile "PG:dbname=%database host=%host port=%port user=%user password=%password tables=%tables" -sql  %query'
// 				, array(
// 						'%ogr2ogr_path' => variable_get('area_path_ogr2ogr', AREA_PATH_OGR2OGR)
// 						, '%shapefile' => '"'.file_directory_temp() .'\\'. $export_Id.$geometrytype.'.shp"'
// 						, '%query' => '"'.$query.'"'
// 						, '%port' =>  5432 //TODO
// 						, '%host' =>  $databases['default']['default']['host']
// 						, '%user' => $databases['default']['default']['username']
// 						, '%password' => $databases['default']['default']['password']
// 						, '%database' =>  $databases['default']['default']['database']
// 						, '%tables' => 'area_geometry'
// 				)
// 		);
		
		$cmd = strtr('ogr2ogr -f "ESRI Shapefile" %shapefile "PG:dbname=%database host=%host port=%port user=%user password=%password tables=%tables" -sql  %query'
				, array(
						'%ogr2ogr_path' => variable_get('area_path_ogr2ogr', AREA_PATH_OGR2OGR)
						, '%shapefile' => '"'.file_directory_temp() .'\\'. $export_Id.$geometrytype.'.shp"'
						, '%query' => '"'.$query.'"'
						, '%port' =>  5432 //TODO
						, '%host' =>  $databases['default']['default']['host']
						, '%user' => $databases['default']['default']['username']
						, '%password' => $databases['default']['default']['password']
						, '%database' =>  $databases['default']['default']['database']
						, '%tables' => 'naturvielfalt_area_geometry'
				)
		);
		
		echo  $databases['default']['default']['port'];
		
		//$cmd = 'echo . | ' . $cmd.' 2>&1';
		shell_exec($cmd);
		//echo($cmd.'<br>');
	}
}

/**
 * Returns the ids of all child areas of the given area, or an empty array
 * 
 * @param string $area
 */
function area_get_children($area = NULL, $json = true, $permission_check = true) {
	global $user;
	
	$children = array();
	if(!$area) return $children;
	
	/*
	 *  Check for permissions
	*/
	$joins = '';
	$whereClause = '';
	if ($permission_check && !check_admin_permission($user->uid)) {
		$whereClause = ' AND ';
		$minLevel = variable_get('ACL_READ');
	
		$joins = ' LEFT JOIN {acl_sgroup} ag ON ag.acl_id = a.acl_id'
				.' LEFT JOIN {sgroup} g ON g.id = ag.sgroup_id'
				.' LEFT JOIN {acl_users} au ON (au.acl_id = a.acl_id OR au.acl_id = g.acl_id) AND au.users_id = ' . $user->uid;

		// level of ALL group
		$whereClause .= ' ( (ag.sgroup_id = ' . variable_get('ACL_GROUP_ALL_ID')
		.' AND ag.level >= ' . $minLevel . ')';
		// level of user group
		$whereClause .= ' OR (au.acl_id = g.acl_id'
				.' AND ag.level >= ' . $minLevel . ')';
		$whereClause .= ' OR (au.acl_id = a.acl_id'
				.' AND au.level >= ' . $minLevel . ') )';
	
	}
	
	$query = 'SELECT a.id AS id 
	 FROM {area} a
	 '.$joins.'
	 WHERE parent_id = '.$area['id'].'
	 '.$whereClause;
	
	$children = db_query($query)->fetchAll();
	
	$output['count'] = count($children);
	$output['children'] = (array) $children;
	
	return $json ? drupal_json_output($output) : $output;
	
}

/**
 * Returns the areas, which a user can add a subarea to. (must have red_write_access, and must not be a subarea)
 *
 */
function area_add_to_area_data() {
	$areas = array();
	if(isset($_POST['areas']) && is_array($_POST['areas'])) {
		$areas = $_POST['areas'];
	}
	return area_get_red_writeable_areas($areas);
}

/**
 * Returns the inventories and areas, which a user can add an observation to.
 *
 */
function area_add_to_area($area = null) {
	$output['success'] = false;
	$output['successfull'] = 0;
	
	//check if the area is already a child. Then it is not possible to append children to it.
	if(isset($area['parent_id']) && $area['parent_id'] > 0) {
		return drupal_json_output($output);
	}
 
	if($area && isset($_POST['areas']) && is_array($_POST['areas'])) {
		$areas = $_POST['areas'];
		$output['total'] = count($areas);
	} else return drupal_json_output($output);

	if(!area_check_access($area, 'ACL_RED_WRITE'))
		return drupal_json_output($output);

	try {
		foreach($areas as $area_id) {
			//user must have write access to the area, which he will add as subarea
			//and this area must not contain children
			$children = area_get_children(area_load($area_id), false, false);
			if(area_write_access_by_id($area_id) && $children['count'] == 0) {
				db_update('area')->fields(array('parent_id' => $area['id']))
				->condition('id', $area_id)
				->execute();
				$output['successfull']++;
			}
		}
		$output['success'] = true;
	} catch(Exception $e) {
		$output['success'] = false;
	}

	return drupal_json_output($output);
}
?>
