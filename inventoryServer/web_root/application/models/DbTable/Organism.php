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

class Application_Model_DbTable_Organism extends MainDbTable
{
	/**
	 * $_name - name of database table
	 *
	 * @var string
	 */
	protected $_name='organism';

	/**
	 * $_id - this is the primary key name

	 *
	 * @var string

	 */
	protected $_id='id';

	protected $_referenceMap    = array(

                   'Application_Model_DbTable_InventoryEntry' => array(
                       'columns' => 'id',
                       'refTableClass' => 'Application_Model_DbTable_InventoryEntry',
                       'refColumns' =>  'organism_id'
                       ),
                       );
}


