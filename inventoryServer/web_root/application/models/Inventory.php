<?php
require_once('InventoryMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_Inventory extends MainModel
{

	/**
	 * mysql var type int(11) unsigned
	 *
	 * @var int
	 */
	protected $_Id;

	/**
	 * mysql var type int(11) unsigned
	 *
	 * @var int
	 */
	protected $_InventoryTypeId;

	/**
	 * mysql var type text
	 *
	 * @var text
	 */
	protected $_Comment;

	/**
	 * mysql var type int(11)
	 *
	 * @var int
	 */
	protected $_HeadInventoryId;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'inventory_type_id'=>'InventoryTypeId',
    'comment'=>'Comment',
    'head_inventory_id'=>'HeadInventoryId',
		));
	}
	/**
	 * gets the references of Application_Model_InventoryEntry
	 * @return array
	 */

	public function getInventoryEntrys()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_InventoryEntry'),
		new Application_Model_InventoryEntry());
	}

	/**
	 * sets the Application_Model_InventoryEntry models
	 *
	 * @param array  Array of Application_Model_InventoryEntry
	 *
	 **/

	public function setInventoryEntrys($InventoryEntrys)
	{
		$this->setOneToMany($InventoryEntrys, new Application_Model_InventoryEntry());
	}

	/**
	 * gets the reference of Application_Model_InventoryType
	 * @return Application_Model_InventoryType
	 */

	public function getInventoryType()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_InventoryType'), new Application_Model_InventoryType());
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_InventoryType model
	 *
	 * @param Application_Model_InventoryType
	 *
	 **/

	public function setInventoryType($InventoryType)
	{
		$this->setManyToOne($InventoryType, new Application_Model_InventoryType());
	}
	
	/**
	 * gets the reference of Application_Model_HeadInventory
	 * @return Application_Model_HeadInventory
	 */

	public function getHeadInventory()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_HeadInventory'), new Application_Model_HeadInventory());
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_HeadInventory model
	 *
	 * @param Application_Model_HeadInventory
	 *
	 **/

	public function setHeadInventory($HeadInventory)
	{
		$this->setManyToOne($HeadInventory, new Application_Model_HeadInventory());
	}


	/**
	 * sets column id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_Inventory
	 *
	 **/

	public function setId($data)
	{
		$this->_Id=$data;
		return $this;
	}

	/**
	 * gets column id type int(11) unsigned
	 * @return int
	 */
	 
	public function getId()
	{
		return $this->_Id;
	}

	/**
	 * sets column inventory_type_id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_Inventory
	 *
	 **/

	public function setInventoryTypeId($data)
	{
		$this->_InventoryTypeId=$data;
		return $this;
	}

	/**
	 * gets column inventory_type_id type int(11) unsigned
	 * @return int
	 */
	 
	public function getInventoryTypeId()
	{
		return $this->_InventoryTypeId;
	}

	/**
	 * sets column comment type text
	 *
	 * @param text $data
	 * @return Application_Model_Inventory
	 *
	 **/

	public function setComment($data)
	{
		$this->_Comment=$data;
		return $this;
	}

	/**
	 * gets column comment type text
	 * @return text
	 */
	 
	public function getComment()
	{
		return $this->_Comment;
	}

	/**
	 * sets column head_inventory_id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_Inventory
	 *
	 **/

	public function setHeadInventoryId($data)
	{
		$this->_HeadInventoryId=$data;
		return $this;
	}

	/**
	 * gets column head_inventory_id type int(11)
	 * @return int
	 */
	 
	public function getHeadInventoryId()
	{
		return $this->_HeadInventoryId;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_InventoryMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_InventoryMapper());
		}
		return $this->_mapper;
	}


	/**
	 * deletes current row by deleting a row that matches the primary key
	 *
	 * @return int
	 */

	public function deleteRowByPrimaryKey()
	{
		if (!$this->getId())
		throw new Exception('Primary Key does not contain a value');
		return $this->getMapper()->getDbTable()->delete('id = '.$this->getId());
	}

}

