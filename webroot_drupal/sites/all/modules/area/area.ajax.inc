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
	
	// get the areas of the request
	$param = filter_xss($_REQUEST['areas']);
		
	$export_id = uniqid();
	$base_query = "SELECT geom FROM area_geometry a_g, area a WHERE a_g.id = a.area_geometry_id AND a.id in $param AND GeometryType(geom)=";
	
	export_shape( "POINT", $base_query, $export_id);
	export_shape( "LINESTRING", $base_query, $export_id);
	export_shape( "POLYGON", $base_query, $export_id);;

 	// zip
	$tempdir = file_directory_temp();

	$cmd = strtr('"C:\Program Files\7-Zip\7z.exe" a %archive %shapefile'
			, array(
					'%archive' => $tempdir .'/'.$export_id .'.zip' ,
					'%shapefile' => file_directory_temp() .'/'. $export_id.'*.*'
			)
	);
	
	$answer = shell_exec($cmd);
	
	// open for return
	$filename = $tempdir .'/'.$export_id .'.zip'; 
	$handle = fopen($filename, "r");
	$contents = fread($handle, filesize($filename));
	fclose($handle);
	
	// move to archive
	// TODO use drupal function
	$cmd = strtr('MOVE %file %archive '
			, array(
					'%file' => $tempdir ."\\".$export_id .'.zip' ,
					'%archive' => file_directory_temp() .'/export_archive'
			)
	);
	
	//echo $cmd.'<br>';
	$answer = shell_exec($cmd);

	// delete 
	// TODO use drupal function
	$cmd = strtr('DEL %files'
			, array(
					'%files' => file_directory_temp() ."\\". $export_id.'*.*'
			)
	);
	
	//echo $cmd.'<br>';	
	$answer = shell_exec($cmd);
	
	// log to db (necessary?)
	
	$date = "12.05.2013, 11:12:13";
	$split = explode(', ', $date);
	$split_date = explode('.', $split[0]);
	$split_time = explode(':', $split[1]);
	
	$unixtime = mktime($split_time[0],$split_time[1],$split_time[2],$split_date[1],$split_date[0],$split_date[2]).'<br>';	
	echo date("d.m.y G:i:s", $unixtime).'<br>';

	
	// set header information
	header('Content-type: application/zip');
	header('Content-Disposition: attachment; filename="export.zip"');
	echo $contents;

// 	header('Content-type: text/plain');
// 	header('Content-Disposition: attachment; filename="response.text"');
	

}

function export_shape($geometrytype, $base_query, $export_Id){
	
	$query = $base_query."'".$geometrytype."'";
	
	// check if there are any shapes at all for this type
	$result = db_query($query);
	
	if($result->rowCount() > 0)
	{
		global $databases;
		
		$cmd = strtr('%postgresql_bin%exe -f %shapefile -h %host -u %user -P %password %database %query'
				, array(
						'%postgresql_bin' => '"C:\Program Files (x86)\PostgreSQL\8.4\bin\pgsql2shp'
						, '%shapefile' => file_directory_temp() .'\\'. $export_Id.$geometrytype
						, '%query' => '"'.$query.'"'
						, '%host' =>  $databases['default']['default']['host']
						, '%user' => $databases['default']['default']['username']
						, '%password' => $databases['default']['default']['password']
						, '%database' =>  $databases['default']['default']['database']
						, '%exe' => '.exe"'
				)
		);
		
		$cmd = 'echo . | ' . $cmd.' 2>&1';
	 	//echo $cmd.'<br>';	
		shell_exec($cmd).'<br>';
	}
}
?>
