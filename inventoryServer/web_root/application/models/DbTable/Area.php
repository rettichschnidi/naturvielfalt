<?php
require_once('Zend/Db/Table/Abstract.php');
require_once('MainDbTable.php');
/**
 * Add your description here
 * 
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_DbTable_Area extends MainDbTable
{
        /**
         * $_name - name of database table
         *
         * @var string
         */
	protected $_name='area';

        /**
         * $_id - this is the primary key name

         *
         * @var string

         */
	protected $_id='id';

/*  protected $_dependentTables = array(
      'Application_Model_DbTable_AreaPoint',
      'Application_Model_DbTable_AreaHabitat'
      );
 
  protected $_referenceMap = array(
      'AreaType' => array(
        'columsn'       => 'type_id',
        'refTableClass' => 'Application_Model_DbTable_AreaType',
        'refColumns'    => 'id') 
  );*/
   
}


