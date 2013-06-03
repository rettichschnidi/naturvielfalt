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
 * Renders a form which allows the user to submit an area name.
 * For usage with ajax.
 */
function area_get_new_areaname_form_callback() {
	print drupal_render(drupal_get_form('area_new_areaname_form'));
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
		if (area_write_access_by_id($area_id)) {
			area_delete_by_id($area_id);
			$deleteCount++;
		}
	}

	$output['success'] = true;
	if ($deleteCount == count($areas)) {
		$output['type'] = 'status';
		$output['message'] = ($deleteCount == 1)
		? t('Areas deleted')
		: t('Areas deleted');
	}
	else {
		$output['type'] = 'warning';
		$output['message'] = t('Some areas could not be deleted (unauthorized access)');
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
	//$param = '(1)';	
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

// Old function with pgsql2shp which files were not importable to ArcGIS
// function export_shape($geometrytype, $base_query, $export_Id){
	
// 	$query = $base_query."'".$geometrytype."'";
	
// 	// check if there are any shapes at all for this type
// 	$result = db_query($query);
	
// 	if($result->rowCount() > 0)
// 	{
// 		global $databases;
		
// 		$cmd = strtr('%postgresql_bin%exe -f %shapefile -h %host -u %user -P %password %query'
// 				, array(
// 						'%postgresql_bin' => '"C:\Program Files (x86)\PostgreSQL\8.4\bin\pgsql2shp'
// 						, '%shapefile' => file_directory_temp() .'\\'. $export_Id.$geometrytype
// 						, '%query' => '"'.$query.'"'
// 						, '%host' =>  $databases['default']['default']['host']
// 						, '%user' => $databases['default']['default']['username']
// 						, '%password' => $databases['default']['default']['password']
// 						, '%database' =>  $databases['default']['default']['database']
// 						, '%exe' => '.exe"'
// 				)
// 		);
		
// 		// $cmd = 'echo . | ' . $cmd.' 2>&1';
// 	 	echo $cmd.'<br>';	
// 		shell_exec($cmd);
// 	}
// }
?>
