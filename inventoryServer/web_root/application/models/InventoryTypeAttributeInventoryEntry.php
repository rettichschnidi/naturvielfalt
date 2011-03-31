<?php
require_once('InventoryTypeAttributeInventoryEntryMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_InventoryTypeAttributeInventoryEntry extends MainModel
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
	protected $_InventoryEntryId;

	/**
	 * mysql var type int(11) unsigned
	 *
	 * @var int
	 */
	protected $_InventoryTypeAttributeDropdownValueId;

	/**
	 * mysql var type text
	 *
	 * @var text
	 */
	protected $_Value;

	/**
	 * mysql var type int(11) unsigned
	 *
	 * @var int
	 */
	protected $_InventoryTypeAttributeId;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'inventory_entry_id'=>'InventoryEntryId',
    'inventory_type_attribute_dropdown_value_id'=>'InventoryTypeAttributeDropdownValueId',
    'value'=>'Value',
    'inventory_type_attribute_id'=>'InventoryTypeAttributeId',
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
		$this->setManyToOne($InventoryTypeAttribute, new Application_Model_InventoryTypeAttribute());
	}
	/**
	 * gets the reference of Application_Model_InventoryTypeAttributeDropdownValue
	 * @return Application_Model_InventoryTypeAttributeDropdownValue
	 */

	public function getInventoryTypeAttributeDropdownValue()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_InventoryTypeAttributeDropdownValue'), new Application_Model_InventoryTypeAttributeDropdownValue());
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_InventoryTypeAttributeDropdownValue model
	 *
	 * @param Application_Model_InventoryTypeAttributeDropdownValue
	 *
	 **/

	public function setInventoryTypeAttributeDropdownValue($InventoryTypeAttributeDropdownValue)
	{
		$this->setManyToOne($InventoryTypeAttributeDropdownValue, Application_Model_InventoryTypeAttributeDropdownValue);
	}
	/**
	 * gets the reference of Application_Model_InventoryEntry
	 * @return Application_Model_InventoryEntry
	 */

	public function getInventoryEntry()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_InventoryEntry'), Application_Model_InventoryEntry);
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_InventoryEntry model
	 *
	 * @param Application_Model_InventoryEntry
	 *
	 **/

	public function setInventoryEntry($InventoryEntry)
	{
		$this->setManyToOne($InventoryEntry, new Application_Model_InventoryEntry());
	}


	/**
	 * sets column id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryTypeAttributeInventoryEntry
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
	 * sets column inventory_entry_id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryTypeAttributeInventoryEntry
	 *
	 **/

	public function setInventoryEntryId($data)
	{
		$this->_InventoryEntryId=$data;
		return $this;
	}

	/**
	 * gets column inventory_entry_id type int(11) unsigned
	 * @return int
	 */
	 
	public function getInventoryEntryId()
	{
		return $this->_InventoryEntryId;
	}

	/**
	 * sets column inventory_type_attribute_dropdown_values_id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryTypeAttributeInventoryEntry
	 *
	 **/

	public function setInventoryTypeAttributeDropdownValueId($data)
	{
		$this->_InventoryTypeAttributeDropdownValueId=$data;
		return $this;
	}

	/**
	 * gets column inventory_type_attribute_dropdown_values_id type int(11) unsigned
	 * @return int
	 */
	 
	public function getInventoryTypeAttributeDropdownValueId()
	{
		return $this->_InventoryTypeAttributeDropdownValueId;
	}

	/**
	 * sets column value type text
	 *
	 * @param text $data
	 * @return Application_Model_InventoryTypeAttributeInventoryEntry
	 *
	 **/

	public function setValue($data)
	{
		$this->_Value=$data;
		return $this;
	}

	/**
	 * gets column value type text
	 * @return text
	 */
	 
	public function getValue()
	{
		return $this->_Value;
	}

	/**
	 * sets column inventory_type_attribute_id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryTypeAttributeInventoryEntry
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
	 * returns the mapper class
	 *
	 * @return Application_Model_InventoryTypeAttributeInventoryEntryMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_InventoryTypeAttributeInventoryEntryMapper());
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

