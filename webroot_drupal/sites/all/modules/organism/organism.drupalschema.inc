<?php
/**
 * do not modify this file, it is generated!
 * To make modifications on the schema alter the file in /doc/module/<modulename> and regenerate it. 
 * See also description in wiki at 
 * http://www.naturwerk.info/wiki/naturvielfalt:entwicklungsumgebung?s[]=ermaster#drupal_schema_api_beschreibungen_generieren 
 */

function organism_get_schema() {
	return array (
  'organism_artgroup' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'name' => 
      array (
        'description' => 'No description for column name available, please fix',
        'type' => 'text',
        'not null' => 'TRUE',
      ),
      'parent' => 
      array (
        'description' => 'No description for column parent available, please fix',
        'type' => 'int',
        'default' => '1',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
  ),
  'organism_artgroup_attr' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_artgroup_attr_type_id' => 
      array (
        'description' => 'No description for column organism_artgroup_attr_type_id available, please fix',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'name' => 
      array (
        'description' => 'No description for column name available, please fix',
        'type' => 'text',
      ),
      'users_uid' => 
      array (
        'description' => 'No description for column users_uid available, please fix',
        'type' => 'int',
        'default' => '0',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_artgroup_attr_2_organism_artgroup_attr_type' => 
      array (
        'table' => 'organism_artgroup_attr_type',
        'columns' => 
        array (
          'organism_artgroup_attr_type_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_artgroup_attr_subscription' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_artgroup_id' => 
      array (
        'description' => 'No description for column organism_artgroup_id available, please fix',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'organism_artgroup_attr_id' => 
      array (
        'description' => 'No description for column organism_artgroup_attr_id available, please fix',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_artgroup_attr_subscription_2_organism_artgroup' => 
      array (
        'table' => 'organism_artgroup',
        'columns' => 
        array (
          'organism_artgroup_id' => 'id',
        ),
      ),
      'organism_artgroup_attr_subscription_2_organism_artgroup_attr' => 
      array (
        'table' => 'organism_artgroup_attr',
        'columns' => 
        array (
          'organism_artgroup_attr_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_artgroup_attr_type' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'name' => 
      array (
        'description' => 'No description for column name available, please fix',
        'type' => 'text',
      ),
      'format' => 
      array (
        'description' => 'No description for column format available, please fix',
        'type' => 'text',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
  ),
  'organism_artgroup_attr_values' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_artgroup_attr_id' => 
      array (
        'description' => 'No description for column organism_artgroup_attr_id available, please fix',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'value' => 
      array (
        'description' => 'No description for column value available, please fix',
        'type' => 'text',
      ),
    	'value_en' =>
    		array (
    			'description' => 'No description for column value available, please fix',
    			'type' => 'text',
    		),
    	'value_fr' =>
    		array (
    			'description' => 'No description for column value available, please fix',
    			'type' => 'text',
    		),
    	'value_it' =>
    		array (
    			'description' => 'No description for column value available, please fix',
    			'type' => 'text',
    		),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_artgroup_attr_values_2_organism_artgroup_attr' => 
      array (
        'table' => 'organism_artgroup_attr',
        'columns' => 
        array (
          'organism_artgroup_attr_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_artgroup_detmethod' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'name' => 
      array (
        'description' => 'No description for column name available, please fix',
        'type' => 'text',
      ),
      'cscf_id' => 
      array (
        'description' => 'No description for column cscf_id available, please fix',
        'type' => 'int',
      ),
    	'name_en' =>
    	array (
    				'description' => 'No description for column name available, please fix',
    				'type' => 'text',
    		),
    	'name_fr' =>
    	array (
    				'description' => 'No description for column name available, please fix',
    				'type' => 'text',
    	),
    	'name_it' =>
    	array (
    				'description' => 'No description for column name available, please fix',
    				'type' => 'text',
    	),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
  ),
  'organism_artgroup_detmethod_subscription' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_artgroup_id' => 
      array (
        'description' => 'No description for column organism_artgroup_id available, please fix',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'organism_artgroup_detmethod_id' => 
      array (
        'description' => 'No description for column organism_artgroup_detmethod_id available, please fix',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'organism_artgroup_id_AND_organism_artgroup_detmethod_id' => 
      array (
        0 => 'organism_artgroup_id',
        1 => ' organism_artgroup_detmethod_id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_artgroup_detmethod_subscription_2_organism_artgroup' => 
      array (
        'table' => 'organism_artgroup',
        'columns' => 
        array (
          'organism_artgroup_id' => 'id',
        ),
      ),
      'organism_artgroup_detmethod_subscription_2_organism_artgroup_detmethod' => 
      array (
        'table' => 'organism_artgroup_detmethod',
        'columns' => 
        array (
          'organism_artgroup_detmethod_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_artgroup_subscription' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'No description for column id available, please fix',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_artgroup_id' => 
      array (
        'description' => 'No description for column organism_artgroup_id available, please fix',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'organism_id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_artgroup_subscription_2_organism_artgroup' => 
      array (
        'table' => 'organism_artgroup',
        'columns' => 
        array (
          'organism_artgroup_id' => 'id',
        ),
      ),
      'organism_artgroup_subscription_2_organism' => 
      array (
        'table' => 'organism',
        'columns' => 
        array (
          'organism_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_attribute_value' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_attribute_id' => 
      array (
        'description' => 'Fremdschlüssel auf die Tabelle organism_attribute. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'text_value' => 
      array (
        'description' => 'Falls der valuetype = t, dann kann wird der Text hier gespeichert. ',
        'type' => 'text',
      ),
      'boolean_value' => 
      array (
        'description' => 'Falls der valuetype = b, dann kann wird der Boolean hier gespeichert. ',
        'type' => 'int',
        'size' => 'tiny',
        'pgsql_type' => 'boolean',
      ),
      'number_value' => 
      array (
        'description' => 'Falls der valuetype = n, dann kann wird die Nummer hier gespeichert. ',
        'type' => 'int',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'id_AND_organism_attribute_id' => 
      array (
        0 => 'id',
        1 => ' organism_attribute_id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_attribute_value_2_organism_attribute' => 
      array (
        'table' => 'organism_attribute',
        'columns' => 
        array (
          'organism_attribute_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_attribute_value_subscription' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_id' => 
      array (
        'description' => 'Fremdschlüssel auf die Tabelle organism. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'organism_attribute_value_id' => 
      array (
        'description' => 'Fremdschlüssel auf die Tabelle organism_attribute_value. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'organism_id_AND_organism_attribute_value_id' => 
      array (
        0 => 'organism_id',
        1 => ' organism_attribute_value_id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_attribute_value_subscription_2_organism_attribute_value' => 
      array (
        'table' => 'organism_attribute_value',
        'columns' => 
        array (
          'organism_attribute_value_id' => 'id',
        ),
      ),
      'organism_attribute_value_subscription_2_organism' => 
      array (
        'table' => 'organism',
        'columns' => 
        array (
          'organism_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_classification' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'parent_id' => 
      array (
        'description' => 'Eine Referenz auf das Vater-Element. Bei Top-Level-Einträgen ist parent_id = id. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'prime_father_id' => 
      array (
        'description' => 'Eine Referenz auf das oberste Elements dieses Baumes. Damit einhält jede Reihe genügend Informationen, um einfach den gesamten Baum abfragen zu können. Bei Top-Level-Einträgen ist prime_father_id = id. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'organism_classification_level_id' => 
      array (
        'description' => 'Fremdschlüssel auf die Tabelle organism_classification_level.',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'left_value' => 
      array (
        'description' => 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. ',
        'type' => 'int',
        'not null' => 'TRUE',
        'default' => '1',
      ),
      'right_value' => 
      array (
        'description' => 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. ',
        'type' => 'int',
        'not null' => 'TRUE',
        'default' => '2',
      ),
      'name' => 
      array (
        'description' => 'Der Name des Klassifizierungslevels in Latein.',
        'type' => 'text',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'parent_id_AND_prime_father_id_AND_name' => 
      array (
        0 => 'parent_id',
        1 => ' prime_father_id',
        2 => ' name',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_classification_2_organism_classification' => 
      array (
        'table' => 'organism_classification',
        'columns' => 
        array (
          'prime_father_id' => 'id',
        ),
      ),
      'organism_classification_2_organism_classification_level' => 
      array (
        'table' => 'organism_classification_level',
        'columns' => 
        array (
          'organism_classification_level_id' => 'id',
        ),
      ),
    ),
    'indexes' => 
    array (
      'primaryKey' => 
      array (
        0 => 'id',
      ),
      'index_pfather_right_oc' => 
      array (
        0 => 'prime_father_id',
        1 => 'right_value',
      ),
      'index_pfather_left_oc' => 
      array (
        0 => 'prime_father_id',
        1 => 'left_value',
      ),
      'classification_level' => 
      array (
        0 => 'organism_classification_level_id',
      ),
      'prime_father' => 
      array (
        0 => 'prime_father_id',
      ),
      'name' => 
      array (
        0 => 'name',
      ),
      'parent_id_index_oc' => 
      array (
        0 => 'parent_id',
      ),
      'index_left_value_oc' => 
      array (
        0 => 'left_value',
      ),
      'index_right_value_oc' => 
      array (
        0 => 'right_value',
      ),
    ),
  ),
  'organism_classification_lang' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_classification_lang_id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'languages_language' => 
      array (
        'description' => 'Language code, e.g. \'\'de\'\' or \'\'en-US\'\'.',
        'type' => 'varchar',
        'size' => 'normal',
        'not null' => 'TRUE',
      ),
      'name' => 
      array (
        'description' => 'Der Name des Klassifizierungslevels in Latein.',
        'type' => 'text',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'organism_classification_lang_id_AND_languages_language' => 
      array (
        0 => 'organism_classification_lang_id',
        1 => ' languages_language',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_classification_lang_2_organism_classification' => 
      array (
        'table' => 'organism_classification',
        'columns' => 
        array (
          'organism_classification_lang_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_classification_level' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'parent_id' => 
      array (
        'description' => 'Eine Referenz auf das Vater-Element. Bei Top-Level-Einträgen ist parent_id = id. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'prime_father_id' => 
      array (
        'description' => 'Eine Referenz auf das oberste Elements dieses Baumes. Damit einhält jede Reihe genügend Informationen, um einfach den gesamten Baum abfragen zu können. Bei Top-Level-Einträgen ist prime_father_id = id. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'left_value' => 
      array (
        'description' => 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis.',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'right_value' => 
      array (
        'description' => 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis.',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'name' => 
      array (
        'description' => 'Enthält den Klassifizierungsrang als englischer Text (family/order/class/etc.) ',
        'type' => 'text',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'prime_father_id_AND_name' => 
      array (
        0 => 'prime_father_id',
        1 => ' name',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_classification_level_2_organism_classification_level' => 
      array (
        'table' => 'organism_classification_level',
        'columns' => 
        array (
          'parent_id' => 'id',
        ),
      ),
    ),
    'indexes' => 
    array (
      'index_pfather_left_ocl' => 
      array (
        0 => 'prime_father_id',
        1 => 'left_value',
      ),
      'index_pfather_right_ocl' => 
      array (
        0 => 'prime_father_id',
        1 => 'right_value',
      ),
      'id' => 
      array (
        0 => 'id',
      ),
      'parent_id_index_ocl' => 
      array (
        0 => 'parent_id',
      ),
      'index_left_value_ocl' => 
      array (
        0 => 'left_value',
      ),
      'index_right_value_ocl' => 
      array (
        0 => 'right_value',
      ),
    ),
  ),
  'organism_classification_subscription' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_id' => 
      array (
        'description' => 'Fremdschlüssel auf die Tabelle organism. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'organism_classification_id' => 
      array (
        'description' => 'Fremdschlüssel auf die Tabelle organism_classification. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'organism_id' => 
      array (
        0 => 'organism_id',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_classification_subscription_2_organism_classification' => 
      array (
        'table' => 'organism_classification',
        'columns' => 
        array (
          'organism_classification_id' => 'id',
        ),
      ),
      'organism_classification_subscription_2_organism' => 
      array (
        'table' => 'organism',
        'columns' => 
        array (
          'organism_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_lang' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'languages_language' => 
      array (
        'description' => 'Language code, e.g. \'\'de\'\' or \'\'en-US\'\'.',
        'type' => 'varchar',
        'size' => 'normal',
        'not null' => 'TRUE',
      ),
      'organism_id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'name' => 
      array (
        'description' => 'Enthält den Klassifizierungsrang als englischer Text (family/order/class/etc.) ',
        'type' => 'text',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'languages_language_AND_organism_id_AND_name' => 
      array (
        0 => 'languages_language',
        1 => ' organism_id',
        2 => ' name',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_lang_2_organism' => 
      array (
        'table' => 'organism',
        'columns' => 
        array (
          'organism_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_synonym' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'organism_id' => 
      array (
        'description' => 'Fremdschlüssel auf die Tabelle organism. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'name' => 
      array (
        'description' => 'Der lateinische Name. ',
        'type' => 'text',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'name' => 
      array (
        0 => 'name',
      ),
      'organism_id_AND_name' => 
      array (
        0 => 'organism_id',
        1 => ' name',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_synonym_2_organism' => 
      array (
        'table' => 'organism',
        'columns' => 
        array (
          'organism_id' => 'id',
        ),
      ),
    ),
    'indexes' => 
    array (
      'index_organism_id_osn' => 
      array (
        0 => 'organism_id',
      ),
      'index_osn' => 
      array (
        0 => 'name',
      ),
    ),
  ),
  'organism' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'parent_id' => 
      array (
        'description' => 'Eine Referenz auf das Vater-Element. Bei Top-Level-Einträgen ist parent_id = id. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'prime_father_id' => 
      array (
        'description' => 'Eine Referenz auf das oberste Elements dieses Baumes. Damit einhält jede Reihe genügend Informationen, um einfach den gesamten Baum abfragen zu können. Bei Top-Level-Einträgen ist prime_father_id = id. ',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'left_value' => 
      array (
        'description' => 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. ',
        'type' => 'int',
        'not null' => 'TRUE',
        'default' => '1',
      ),
      'right_value' => 
      array (
        'description' => 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. ',
        'type' => 'int',
        'not null' => 'TRUE',
        'default' => '2',
      ),
      'scientific_name' => 
      array (
        'description' => 'No description for column scientific_name available, please fix',
        'type' => 'text',
        'not null' => 'TRUE',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'scientific_name' => 
      array (
        0 => 'scientific_name',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
    'foreign keys' => 
    array (
      'organism_2_organism' => 
      array (
        'table' => 'organism',
        'columns' => 
        array (
          'prime_father_id' => 'id',
        ),
      ),
    ),
    'indexes' => 
    array (
      'FK_organism_11' => 
      array (
        0 => 'id',
      ),
      'FK_organism_2' => 
      array (
        0 => 'left_value',
        1 => 'prime_father_id',
      ),
      'idx_name_de' => 
      array (
        0 => 'right_value',
        1 => 'prime_father_id',
      ),
      'parent_id_index_o' => 
      array (
        0 => 'parent_id',
      ),
      'index_left_value_o' => 
      array (
        0 => 'left_value',
      ),
      'index_right_value_o' => 
      array (
        0 => 'right_value',
      ),
    ),
  ),
  'organism_attribute' => 
  array (
    'fields' => 
    array (
      'id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'serial',
        'not null' => 'TRUE',
      ),
      'valuetype' => 
      array (
        'description' => 'Gibt den Typ an, welcher in den dazugehörigen organism_attribute_value gespeichert wird. Zuordnung: n=number, b=boolean, t=text.',
        'type' => 'char',
        'not null' => 'TRUE',
      ),
      'name' => 
      array (
        'description' => 'Der Name des Klassifizierungslevels in Latein.',
        'type' => 'text',
        'not null' => 'TRUE',
      ),
      'comment' => 
      array (
        'description' => 'An information text about the aim of this attribute.',
        'type' => 'text',
      ),
    ),
    'unique keys' => 
    array (
      'id' => 
      array (
        0 => 'id',
      ),
      'name' => 
      array (
        0 => 'name',
      ),
    ),
    'primary key' => 
    array (
      0 => 'id',
    ),
  ),
  'organism_file_managed' => 
  array (
    'fields' => 
    array (
      'organism_id' => 
      array (
        'description' => 'Die eigene Id, wird fortlaufend inkrementiert.',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'file_managed_fid' => 
      array (
        'description' => 'file_managed_fid',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'description' => 
      array (
        'description' => 'No description for column description available, please fix',
        'type' => 'text',
      ),
      'author' => 
      array (
        'description' => 'Stores information about the author of the document',
        'type' => 'text',
      ),
    ),
    'unique keys' => 
    array (
      'organism_id_AND_file_managed_fid' => 
      array (
        0 => 'organism_id',
        1 => ' file_managed_fid',
      ),
    ),
    'foreign keys' => 
    array (
      'organism2file' => 
      array (
        'table' => 'organism',
        'columns' => 
        array (
          'organism_id' => 'id',
        ),
      ),
    ),
  ),
  'organism_habitat_subscription' => 
  array (
    'fields' => 
    array (
      'organism_id' => 
      array (
        'description' => 'FK to organism.id',
        'type' => 'int',
        'not null' => 'TRUE',
      ),
      'habitat_id' => 
      array (
        'description' => 'FK to habitat.id',
        'type' => 'int',
        'not null' => 'TRUE',
        'size' => 'big',
      ),
      'characteristic_organism' => 
      array (
        'description' => 'True if this organism is a characteristic organism (Leitorganismus) for this habitat.',
        'type' => 'int',
        'not null' => 'TRUE',
        'size' => 'tiny',
        'pgsql_type' => 'boolean',
      ),
    ),
    'unique keys' => 
    array (
      'organism_id_AND_habitat_id' => 
      array (
        0 => 'organism_id',
        1 => ' habitat_id',
      ),
    ),
    'foreign keys' => 
    array (
      'organism_habitat_subscription_2_organism' => 
      array (
        'table' => 'organism',
        'columns' => 
        array (
          'organism_id' => 'id',
        ),
      ),
    ),
    'indexes' => 
    array (
      'organism_habitat_habitat_id_idx' => 
      array (
        0 => 'habitat_id',
      ),
      'organism_habitat_organism_id_idx' => 
      array (
        0 => 'organism_id',
      ),
    ),
  ),
);
}
?>
