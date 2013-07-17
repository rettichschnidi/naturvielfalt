<?php
/**
 * do not modify this file, it is generated!
 * To make modifications on the schema alter the file in /doc/module/<modulename> and regenerate it. 
 * See also description in wiki at 
 * http://www.naturwerk.info/wiki/naturvielfalt:entwicklungsumgebung?s[]=ermaster#drupal_schema_api_beschreibungen_generieren 
 */

function inventory_get_schema() {
		return array (
			  'inventory' =>
			  array (
			    'fields' =>
			    array (
			      'id' =>
			      array (
			        'description' => 'Veraltet, PK wird ersetzt durch inventory_id',
			        'type' => 'serial',
			        'not null' => 'TRUE',
			      ),
			      'area_id' =>
			      array (
			        'description' => 'veraltet: PK, wird erstert durch area_id',
			        'type' => 'int',
			        'not null' => 'TRUE',
			        'size' => 'big',
			      ),
			      'name' =>
			      array (
			        'description' => 'Name des Inventars',
			        'type' => 'text',
			        'not null' => 'TRUE',
			      ),
			      'description' =>
			      array (
			        'description' => 'Kommentar',
			        'type' => 'text',
			      ),
			      'users_uid' =>
			      array (
			        'description' => 'No description for column users_uid available, pleasefix',
			        'type' => 'int',
			        'not null' => 'TRUE',
			        'size' => 'big',
			      ),
			      'observer' =>
			      array (
			        'description' => 'No description for column observer available, please fix',
			        'type' => 'text',
			        'not null' => 'TRUE',
			      ),
			      'create_time' =>
			      array (
			        'description' => 'No description for column create_time available, please fix',
			        'type' => 'datetime',
			        'not null' => 'TRUE',
			        'pgsql_type' => 'timestamp',
			      ),
			      'modify_time' =>
			      array (
			        'description' => 'No description for column modify_time available, please fix',
			        'type' => 'datetime',
			        'pgsql_type' => 'timestamp',
			      ),
			      'acl_id' =>
			      array (
			        'description' => 'No description for column acl_id available, please fix',
			        'type' => 'int',
			        'not null' => 'TRUE',
			        'size' => 'big',
			      ),
			    ),
			    'primary key' =>
			    array (
			      0 => 'id',
			    ),
			    'foreign keys' =>
			    array (
			      'inventory_2_acl' =>
			      array (
			        'table' => 'acl',
			        'columns' =>
			        array (
			          'acl_id' => 'id',
			        ),
			      ),
			    ),
			  ),
			  'inventory_file_managed' =>
			  array (
			    'fields' =>
			    array (
			      'description' =>
			      array (
			        'description' => 'No description for column description available, please fix',
			        'type' => 'text',
			      ),
			      'inventory_id' =>
			      array (
			        'description' => 'Veraltet, PK wird ersetzt durch inventory_id',
			        'type' => 'int',
			        'not null' => 'TRUE',
			      ),
			      'file_managed_id' =>
			      array (
			        'description' => 'File ID.',
			        'type' => 'int',
			        'not null' => 'TRUE',
			      ),
			    ),
			    'foreign keys' =>
			    array (
			      'inventory_file_managed_2_inventory' =>
			      array (
			        'table' => 'inventory',
			        'columns' =>
			        array (
			          'inventory_id' => 'id',
			        ),
			      ),
			    ),
			  ),
			);
}
?>