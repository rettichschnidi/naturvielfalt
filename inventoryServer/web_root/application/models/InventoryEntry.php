<?php
require_once('InventoryEntryMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_InventoryEntry extends MainModel
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
	protected $_OrganismId;

	/**
	 * mysql var type int(11) unsigned
	 *
	 * @var int
	 */
	protected $_InventoryId;

	/**
	 * mysql var type double(11) unsigned
	 *
	 * @var int
	 */
	protected $_Position;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'organism_id'=>'OrganismId',
    'inventory_id'=>'InventoryId',
    'position'=>'Position',
		));
	}
	/**
	 * gets the reference of Application_Model_Organism
	 * @return Application_Model_Organism
	 */

	public function getOrganism()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_Organism'), new Application_Model_Organism());
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_Organism model
	 *
	 * @param Application_Model_Organism
	 *
	 **/

	public function setOrganism($Organism)
	{
		$this->setManyToOne($Organism, new Application_Model_Organism());
	}

	/**
	 * gets the references of Application_Model_InventoryTypeAttribute
	 * @return array
	 */

	public function getInventoryTypeAttributes()
	{	
		return $this->getRowsAsModel($this->getMapper()->find($this->getId(), $this)->findManyToManyRowset("Application_Model_DbTable_InventoryTypeAttribute",
                "Application_Model_DbTable_InventoryTypeAttributeInventoryEntry", "Application_Model_DbTable_InventoryEntry"), new Application_Model_DbTable_InventoryTypeAttribute());
	}

	/**
	 * sets the Application_Model_InventoryTypeAttribute models
	 *
	 * @param array  Array of Application_Model_InventoryTypeAttribute
	 *
	 **/

	public function setInventoryTypeAttributes($InventoryTypeAttributes)
	{
		$this->setManyToMany($InventoryTypeAttributes, Application_Model_InventoryTypeAttributeInventoryEntry, Application_Model_InventoryTypeAttribute);
	}
	/**
	 * gets the references of Application_Model_InventoryTypeAttributeInventoryEntry
	 * @return array
	 */

	public function getInventoryTypeAttributeInventoryEntrys()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_InventoryTypeAttributeInventoryEntry'),
		new Application_Model_InventoryTypeAttributeInventoryEntry());
	}
	/**
	 * gets the reference of Application_Model_Inventory
	 * @return Application_Model_Inventory
	 */

	public function getInventory()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_Inventory'), new Application_Model_Inventory());
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_Inventory model
	 *
	 * @param Application_Model_Inventory
	 *
	 **/

	public function setInventory($Inventory)
	{
		$this->setManyToOne($Inventory, new Application_Model_Inventory());
	}


	/**
	 * sets the Application_Model_InventoryTypeAttributeInventoryEntry models
	 *
	 * @param array  Array of Application_Model_InventoryTypeAttributeInventoryEntry
	 *
	 **/

	public function setInventoryTypeAttributeInventoryEntrys($InventoryTypeAttributeInventoryEntrys)
	{
		$this->setOneToMany($InventoryTypeAttributeInventoryEntrys, new Application_Model_InventoryTypeAttributeInventoryEntry());
	}

	/**
	 * Add an inventory attribute
	 * @param $InventoryTypeAttribute The InventoryTypeAttribute of type Application_Model_InventoryTypeAttribute
	 * @param $value Entered value of the attribute
	 * @param $InventoryTypeAttributeDropdownValue If the attribute is a dropdown, then value must be "" and this parameter must be a Application_Model_InventoryTypeAttributeDropdownValue
	 */

	public function addInventoryTypeAttributeValue($InventoryTypeAttribute, $value = "", $InventoryTypeAttributeDropdownValue = "")
	{
		$attrValue = new Application_Model_InventoryTypeAttributeInventoryEntry();
		$attrValue->setInventoryEntry($this);
		$attrValue->setInventoryTypeAttribute($InventoryTypeAttribute);
		
		// If it is not a dropdown, save the value as a string, otherwise save the dropdown id
		if($InventoryTypeAttributeDropdownValue === "")
		{
			$attrValue->setInventoryTypeAttributeDropdownValue($InventoryTypeAttributeDropdownValue);
		}
		else
		{
			$attrValue->setValue($value);
		}
		$attrValue->getMapper()->save($attrValue);
	}

	/**
	 * sets column id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryEntry
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
	 * sets column organism_id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryEntry
	 *
	 **/

	public function setOrganismId($data)
	{
		$this->_OrganismId=$data;
		return $this;
	}

	/**
	 * gets column organism_id type int(11) unsigned
	 * @return int
	 */

	public function getOrganismId()
	{
		return $this->_OrganismId;
	}

	/**
	 * sets column inventory_id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryEntry
	 *
	 **/

	public function setInventoryId($data)
	{
		$this->_InventoryId=$data;
		return $this;
	}

	/**
	 * gets column inventory_id type int(11) unsigned
	 * @return int
	 */

	public function getInventoryId()
	{
		return $this->_InventoryId;
	}
	
	/**
	 * sets column position type double(11) unsigned
	 *
	 * @param double $data
	 * @return Application_Model_InventoryEntry
	 *
	 **/

	public function setPosition($data)
	{
		$this->_Position=$data;
		return $this;
	}

	/**
	 * gets column position type double(11) unsigned
	 * @return double
	 */

	public function getPosition()
	{
		return $this->_Position;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_InventoryEntryMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_InventoryEntryMapper());
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

