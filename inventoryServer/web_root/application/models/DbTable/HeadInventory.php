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

class Application_Model_DbTable_HeadInventory extends MainDbTable
{
        /**
         * $_name - name of database table
         *
         * @var string
         */
	protected $_name='head_inventory';

        /**
         * $_id - this is the primary key name

         *
         * @var string

         */
	protected $_id='id';

        protected $_referenceMap    = array(

                   'Application_Model_DbTable_Area' => array(
                       'columns' => 'area_id',
                       'refTableClass' => 'Application_Model_DbTable_Area',
                       'refColumns' =>  'id'
                           )          
                );
                
        /**
         * $_area_id - order id
         *
         * @var string
         */                
     // protected $_area_id='area_id';
}


