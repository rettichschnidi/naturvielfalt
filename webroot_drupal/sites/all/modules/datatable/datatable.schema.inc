<?php
/**
 * do not modify this file, it is generated!
 * To make modifications on the schema alter the file in /doc/module/<modulename> and regenerate it. 
 * See also description in wiki at 
 * http://www.naturwerk.info/wiki/naturvielfalt:entwicklungsumgebung?s[]=ermaster#drupal_schema_api_beschreibungen_generieren 
 */

function datatable_get_schema() {
		return array (
	  'datatable_columns' =>
	  array (
	    'fields' =>
	    array (
	      'id' =>
	      array (
	        'type' => 'serial',
			'not null' => 'TRUE',
	      ),
	      'datatable_name' =>
	      array (
	        'type' => 'varchar',
	        'size' => 'normal',
	        'not null' => 'TRUE',
	      ),
	      'name' =>
	      array (
	        'type' => 'text',
	        'not null' => 'TRUE',
	      ),
	      'hide' =>
	      array (
	        'type' => 'int',
	        'not null' => 'TRUE',
	        'size' => 'tiny',
	        'pgsql_type' => 'boolean',
	      ),
	      'width' =>
	      array (
	        'type' => 'int',
	        'not null' => 'TRUE',
	      ),
	      'column_order' =>
	      array (
	        'type' => 'int',
	        'not null' => 'TRUE',
	      ),
	    ),
	    'primary key' =>
	    array (
	      0 => 'id',
	    ),
	  ),
	  'datatable_columns_user' =>
	  array (
	    'fields' =>
	    array (
	      'id' =>
	      array (
	        'type' => 'serial',
	        'not null' => 'TRUE',
	      ),
	      'column_id' =>
	      array (
	        'type' => 'int',
	        'not null' => 'TRUE',
	      ),
    	  'user_uid' =>
    		array (
    				'type' => 'int',
    				'not null' => 'TRUE',
    		),
	    ),
	    'primary key' =>
	    array (
	      0 => 'id',
	    ),
	    'foreign keys' =>
	    array (
	      'datatable_columns_user_2_datatable_columns' =>
	      array (
	        'table' => 'datatable_columns',
	        'columns' =>
	        array (
	          'column_id' => 'id',
	        ),
	      ),
	    ),
	  ),
	);
}
?>