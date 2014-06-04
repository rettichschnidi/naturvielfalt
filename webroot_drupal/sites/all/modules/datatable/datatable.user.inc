<?php 
/**
 * @file datatable.user.inc
 * @author André Kälin, 2013
 * @copyright 2013 Naturwerk, Brugg
 */

/**
 * Constants which define the different tables, which the user con store its settings
 * Only these tables can get sorted. If later other tables should get stored, add the table name here
 */
define('DATATABLE_OBSERVATION', 'observations');
define('DATATABLE_AREA','areas');
define('DATATABLE_INVENTORY','inventories');
define('DATATABLE_CLASSIFIERS', 'classificators');
define('DATATABLE_CLASSIFIERS_SPECIES_CSCF', 'organisms_cscf');
define('DATATABLE_CLASSIFIERS_SPECIES_CRSF', 'organisms_crsf');
define('DATATABLE_CLASSIFIERS_SPECIES_FUNGUS', 'organisms_fungus');
define('DATATABLE_CLASSIFIERS_SPECIES_SWISSLICHENS', 'organisms_swisslichens');
define('DATATABLE_CLASSIFIERS_SPECIES_NISM', 'organisms_nism');
define('DATATABLE_FILES', 'files');
define('DATATABLE_HABITATS', 'habitats');

//serialize array to make array a scalar type (unserialize later)
define('DATATABLE_TYPES', serialize(array(
							DATATABLE_OBSERVATION, 
							DATATABLE_AREA, DATATABLE_INVENTORY, 
							DATATABLE_CLASSIFIERS, 
							DATATABLE_CLASSIFIERS_SPECIES_CSCF, 
							DATATABLE_CLASSIFIERS_SPECIES_CRSF,
							DATATABLE_CLASSIFIERS_SPECIES_FUNGUS,
							DATATABLE_CLASSIFIERS_SPECIES_SWISSLICHENS,
							DATATABLE_CLASSIFIERS_SPECIES_NISM,
							DATATABLE_FILES, 
							DATATABLE_HABITATS))
		);

/**
 * Query for the datatable user settings
 * @param unknown $table_name
 * @param unknown $table_id
 * @return multitype:
 */
function datatable_get_user_settings($table_name) {
	if(!in_array($table_name, unserialize(DATATABLE_TYPES))) 
		return array();
	
	global $user;
	$sql = 'SELECT dc.*, dcu.column_id, dcu.user_uid 
			FROM {datatable_columns} dc
			LEFT JOIN {datatable_columns_user} dcu ON dcu.column_id = dc.id
			WHERE dcu.user_uid = '.$user->uid.'
			AND dc.datatable_name LIKE \''.$table_name.'\'
			ORDER BY column_order';
	
	$result = db_query($sql)->fetchAllAssoc('name');

	return $result;
}

/**
 * Alters the tableHeader according to the current settings of the user
 * 
 * @param unknown $table_name
 * @param array $tableHeader
 */
function datatable_apply_user_settings($table_name, array &$tableHeader) {
	if(!in_array($table_name, unserialize(DATATABLE_TYPES))) {
		return;
	}

	
	$user_settings = datatable_get_user_settings($table_name);
	//no settings to apply
	if(empty($user_settings)) return;
	
	$i = 0;
	$tableHeaderSettingsApplied = $tableHeader;
    $table_is_visible=false; // if at least one column is visible
	foreach($tableHeader as $head) {
		if(isset($head['dbfield'])) {
			$name = $head['dbfield'];
			//search the head in the user settings
			$searchIndex = array_search($name, array_keys($user_settings));
			
			//if we found this head in the user settings, apply the settings
			if($searchIndex !== false) {
				//apply user settings
				$head['hide'] = filter_var($user_settings[$name]->hide, FILTER_VALIDATE_BOOLEAN);
				$head['width'] = $user_settings[$name]->width;
			
				$order = $user_settings[$name]->column_order;
				
				//apply the current order settings 
				if($order != $i) {
					$tableHeaderSettingsApplied[$order] = $head;
				} else {
					$tableHeaderSettingsApplied[$i] = $head;
				}
			}
		}
		$i++;
        if(!$table_is_visible && !$head['hide']){
            $table_is_visible = true;
        }
	}
    // make sure there is at least one element visible
    // if not displaying the user settings would lead to a missing table
    if($table_is_visible){
        $tableHeader = $tableHeaderSettingsApplied;
    } // else use the default column setting. as soon as the user changes his columns, the faulty setting is fixed
}








/**
 * Stores the table settings the user wants to apply (AJAX Call, data passed as JSON)
 */
function datatable_save_user_settings() {  
	global $user; 
	/**
	 * Save information provided by an ajax call in the following format:
	 * {"table_name" : observations (Must be in DATATABLE_TYPES),
	 *    "columns" : [
	 *    		{"name" : column_name (use db_field, its UNIQUE),
	 * 	     		"data" : { 
	 *          		"width" : int (pixel),
	 *           		"hide"  : boolean,
	 *           		"order" : int
	 *        		}
	 *     		},
	 *     		{ //next column }
	 * 		]
	 * }
	 * 
	 * 1. foreach column in array:
	 * 	a. select the column by name with the current user id
	 * 	b. if we there exists a column: update it accordingly, else insert a new one.
	 */
	
	if(!in_array($_POST['table_name'], unserialize(DATATABLE_TYPES)))
		return;
	
	if(isset($_POST['user']) && $_POST['user'] =='userlogged' && isset($_POST['table_name'])) {
		$table_name = $_POST['table_name'];

 		$result = datatable_get_user_settings($table_name);
 		if(!empty($result)){
 			return print "ok";
 		}
	}
	
	
	//RESET --> RESET THE DATATABLE-SETTINGS
	if(isset($_POST['status']) && isset($_POST['table_name']) && $_POST['status']==='reset'){
		$table = $_POST['table_name'];
		datatable_reset_settings($table);
		return;
	}
	//END RESET
	
	if(!isset($_POST['table_name']) || !isset($_POST['columns']) || !is_array($_POST['columns'])) return;
	
	$table = $_POST['table_name'];
	$columns = $_POST['columns'];
	
	$settings = datatable_get_user_settings($table);
	
	foreach($columns as $column) {
		if(isset($column['name']) && isset($column['data'])) {
			$name = $column['name'];
			
			$setting_exists = array_key_exists($name, $settings);
			if($setting_exists) { 
				
				$settings_column = $settings[$name];
				$id = $settings_column->id;
				db_update('datatable_columns')
				->fields(array(
						'datatable_name' => $table,
						'hide' => $column['data']['hide'],
						'width' => $column['data']['width'],
						'column_order' => $column['data']['order'],
				))
				->condition('id', $id)
				->execute();
				
			} else {
					$id = db_insert('datatable_columns')->fields(array(
							'datatable_name' => $table,
							'name' => $name,
							'hide' => $column['data']['hide'],
							'width' => $column['data']['width'],
							'column_order' => $column['data']['order'],
					))
					->execute();

					db_insert('datatable_columns_user')->fields(array(
							'column_id' => $id,
							'user_uid' => $user->uid,
					))
					->execute();
										
						
					
		     }
	}
  }
}




//RESET SETTINGS IN DATATABLE_COLUMNS AND DATATABLE_COLUMNS_USER
 
function datatable_reset_settings($table_name) {
	if(!in_array($table_name, unserialize(DATATABLE_TYPES)))
		return;
	
	global $user;

	 	$sql1 = "SELECT id FROM {datatable_columns} dc
	 			WHERE datatable_name='".$table_name."'
	 			AND id IN (SELECT id FROM {datatable_columns_user} dcu
	 			WHERE user_uid=".$user->uid.")";
	 	
	 	$result1 = db_query($sql1)->fetchCol("id");
	 	
 	 	if((count($result1))>0){ 		 	
 		 	$array=implode(",",$result1);
		 	
 		 	$sql2 = "DELETE FROM {datatable_columns_user}
 		 			WHERE id in($array);
 		 			DELETE FROM {datatable_columns}
 		 			WHERE id in($array)";
	 			
 		 	$result2 = db_query($sql2);
 		 	return;
 		 	//return print_r("geht nur wenn die Felder in db existieren");
 	 	}
	//delete all entries in datatable_columns and datatable_columns_user of this table of the current user.
}

//END RESET

