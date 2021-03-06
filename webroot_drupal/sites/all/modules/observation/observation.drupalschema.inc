<?php
/**
 * do not modify this file, it is generated!
 * To make modifications on the schema alter the file in /doc/module/<modulename> and regenerate it. 
 * See also description in wiki at 
 * http://www.naturwerk.info/wiki/naturvielfalt:entwicklungsumgebung?s[]=ermaster#drupal_schema_api_beschreibungen_generieren 
 */

function observation_get_schema() {
	return array (
  'observation_file_managed' => 
  array (
    'fields' => 
    array (
      'observation_id' => 
      array (
        'description' => 'No description for column observation_id available, please fix',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'file_managed_fid' => 
      array (
        'description' => 'File ID.',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'description' => 
      array (
        'description' => 'No description for column description available, please fix',
        'type' => 'text',
      ),
    ),
    'foreign keys' => 
    array (
      'observation_file_managed_2_observation_attribute' => 
      array (
        'table' => 'observation_attribute',
        'columns' => 
        array (
          'observation_id' => 'id',
        ),
      ),
    ),
  ),
  'observation' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'inventory_id' => 
      array (
        'description' => 'Id des Inventars',
        'type' => 'int',
      ),
      'users_uid' => 
      array (
        'description' => 'Primary Key: Unique user ID.',
        'type' => 'int',
        'not null' => 'TRUE',
        'default' => '0',
        'size' => 'big',
      ),
      'area_geometry_id' => 
      array (
        'description' => 'Primary Key',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'organism_artgroup_id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'int',
      ),
      'organism_artgroup_detmethod_id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'int',
      ),
      'observer' => 
      array (
        'description' => 'No description for column observer available, please fix',
        'type' => 'text',
      ),
      'count' => 
      array (
        'description' => 'No description for column count available, please fix',
        'type' => 'int',
      ),
      'found_as_lang' => 
      array (
        'description' => 'No description for column found_as_lang available, please fix',
        'type' => 'int',
        'size' => 'tiny',
        'pgsql_type' => 'boolean',
      ),
      'found_as_lat' => 
      array (
        'description' => 'No description for column found_as_lat available, please fix',
        'type' => 'int',
        'size' => 'tiny',
        'pgsql_type' => 'boolean',
      ),
      'date' => 
      array (
        'description' => 'No description for column date available, please fix',
        'type' => 'int',
      ),
      'c_time' => 
      array (
        'description' => 'No description for column c_time available, please fix',
        'type' => 'int',
      ),
      'm_time' => 
      array (
        'description' => 'No description for column m_time available, please fix',
        'type' => 'int',
      ),
      'acl_id' => 
      array (
        'description' => 'No description for column acl_id available, please fix',
        'type' => 'serial',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
  ),
  'observation_attribute' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'observation_id' => 
      array (
        'description' => 'der boebachtung',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'organism_artgroup_attr_id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'int',
      ),
      'organism_artgroup_attr_values_id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'int',
      ),
      'value' => 
      array (
        'description' => 'Der Inhalt des Attributes',
        'type' => 'text',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'observation_attribute_2_observation' => 
      array (
        'table' => 'observation',
        'columns' => 
        array (
          'observation_id' => 'id',
        ),
      ),
    ),
  ),
);
}
?>