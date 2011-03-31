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

class Application_Model_DbTable_Inventory extends MainDbTable
{
	/**
	 * $_name - name of database table
	 *
	 * @var string
	 */
	protected $_name='inventory';

	/**
	 * $_id - this is the primary key name

	 *
	 * @var string

	 */
	protected $_id='id';

	protected $_referenceMap    = array(

                   'Application_Model_DbTable_HeadInventory' => array(
                       'columns' => 'head_inventory_id',
                       'refTableClass' => 'Application_Model_DbTable_HeadInventory',
                       'refColumns' =>  'id'
                       ),
                              'Application_Model_DbTable_InventoryType' => array(
                       'columns' => 'inventory_type_id',
                       'refTableClass' => 'Application_Model_DbTable_InventoryType',
                       'refColumns' =>  'id'
                       )
                       );
}


