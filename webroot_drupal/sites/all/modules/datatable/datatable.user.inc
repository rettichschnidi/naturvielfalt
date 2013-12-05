<?php 
/**
 * @file datatable.user.inc
 * @author André Kälin, 2013
 * @copyright 2013 Naturwerk, Brugg
 */

/**
 * Constants which define the different tables, which the user con store its settings
 * Only these tables can get sorted
 */
define('DATATABLE_OBSERVATION', 'observations');
define('DATATABLE_AREA','areas');
define('DATATABLE_INVENTORY','inventories');
define('DATATABLE_CLASSIFIERS', 'classificators');
define('DATATABLE_CLASSIFIERS_SPECIES', 'organisms');
define('DATATABLE_FILES', 'files');
define('DATATABLE_HABITATS', 'habitats');

//serialize array to make array a scalar type (unserialize later)
define('DATATABLE_TYPES', serialize(array(
							DATATABLE_OBSERVATION, 
							DATATABLE_AREA, DATATABLE_INVENTORY, 
							DATATABLE_CLASSIFIERS, 
							DATATABLE_CLASSIFIERS_SPECIES, 
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
	$sql = 'SELECT * 
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
	
	$user_settings = datatable_get_user_settings($table_name);
	//no settings to apply
	if(empty($user_settings)) return;
	
	$i = 0;
	foreach($tableHeader as &$head) {
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
				if($order != $i) {
					//the element is not yet in the right order, swap it.
					$tmp = $tableHeader[$i];
					$tableHeader[$i] = $tableHeader[$order];
					$tableHeader[$order] = $tmp;
				} 
			}
		}
		$i++;
	}
}

/**
 * Stores the table settings the user wants to apply (AJAX Call, data passed as JSON)
 */



function datatable_save_user_settings() {  
	global $user; 
	//debug($_REQUEST);
	debug($_POST);
	//debug("user".$user->name);	
	
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
	
	if(!isset($_POST['table_name']) || !isset($_POST['columns']) || !is_array($_POST['columns'])) return;
	
	$table = $_POST['table_name'];
	$columns = $_POST['columns'];
	
	$settings = datatable_get_user_settings($table);
	debug($settings);
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
						'name' => $name,
						'hide' => $column['data']['hide'],
						'width' => $column['data']['width'],
						'column_order' => $column['data']['order'],
				))
				->condition('id', $id)
				->execute();
				
			} else {
				//insert
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
// 	$jason_array(json_decode($jason_row);
// 	while (current($jason_array)!==false) {
// 		{
				
// 			$jason_single(json_decode(current($jason_row));
// 			$width = $jason_single[0];
// 			$hide = $jason_single[1];
// 			$order = $jason_single[2];
// 			$entry = $width . $hide . $order;
		
				
// 			// check if there is already an entrance in the DB
	
// 			//  if yes update
				
// 			//  if no insert

// 			}
			
// 			/**
// 			 * add a observation attribute
// 			 * @param array $entry
// 			 */

// 			}