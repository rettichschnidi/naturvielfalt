<?php
require_once('InventoryTypeAttributeDropdownValueMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_InventoryTypeAttributeDropdownValue extends MainModel
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
	protected $_InventoryTypeAttributeId;

	/**
	 * mysql var type varchar(45)
	 *
	 * @var string
	 */
	protected $_Value;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'inventory_type_attribute_id'=>'InventoryTypeAttributeId',
    'value'=>'Value',
		));
	}
	/**
	 * gets the reference of Application_Model_InventoryTypeAttribute
	 * @return Application_Model_InventoryTypeAttribute
	 */

	public function getInventoryTypeAttribute()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_InventoryTypeAttribute'), new Application_Model_InventoryTypeAttribute());
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_InventoryTypeAttribute model
	 *
	 * @param Application_Model_InventoryTypeAttribute
	 *
	 **/

	public function setInventoryTypeAttribute($InventoryTypeAttribute)
	{
		$this->setManyToOne($InventoryTypeAttribute, Application_Model_InventoryTypeAttribute);
	}



	/**
	 * sets column id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryTypeAttributeDropdownValue
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
	 * sets column inventory_type_attribute_id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryTypeAttributeDropdownValue
	 *
	 **/

	public function setInventoryTypeAttributeId($data)
	{
		$this->_InventoryTypeAttributeId=$data;
		return $this;
	}

	/**
	 * gets column inventory_type_attribute_id type int(11) unsigned
	 * @return int
	 */
	 
	public function getInventoryTypeAttributeId()
	{
		return $this->_InventoryTypeAttributeId;
	}

	/**
	 * sets column value type varchar(45)
	 *
	 * @param string $data
	 * @return Application_Model_InventoryTypeAttributeDropdownValue
	 *
	 **/

	public function setValue($data)
	{
		$this->_Value=$data;
		return $this;
	}

	/**
	 * gets column value type varchar(45)
	 * @return string
	 */
	 
	public function getValue()
	{
		return $this->_Value;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_InventoryTypeAttributeDropdownValueMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_InventoryTypeAttributeDropdownValueMapper());
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

