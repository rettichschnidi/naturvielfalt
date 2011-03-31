<?php
require_once('InventoryTypeMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_InventoryType extends MainModel
{

	/**
	 * mysql var type int(11) unsigned
	 *
	 * @var int
	 */
	protected $_Id;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_Name;

	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'name'=>'Name',
		));
	}
	/**
	 * gets the references of Application_Model_Inventory
	 * @return array
	 */

	public function getInventorys()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_Inventory'),
		Application_Model_Inventory);
	}

	/**
	 * sets the Application_Model_Inventory models
	 *
	 * @param array  Array of Application_Model_Inventory
	 *
	 **/

	public function setInventorys($Inventorys)
	{
		$this->setOneToMany($Inventorys, Application_Model_Inventory);
	}
	/**
	 * gets the references of Application_Model_InventoryTypeAttribute
	 * @return array
	 */

	public function getInventoryTypeAttributes()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_InventoryTypeAttribute'),
		new Application_Model_InventoryTypeAttribute());
	}

	/**
	 * sets the Application_Model_InventoryTypeAttribute models
	 *
	 * @param array  Array of Application_Model_InventoryTypeAttribute
	 *
	 **/

	public function setInventoryTypeAttributes($InventoryTypeAttributes)
	{
		$this->setOneToMany($InventoryTypeAttributes, Application_Model_InventoryTypeAttribute);
	}


	/**
	 * gets the references of Application_Model_Organism
	 * @return array
	 */

	public function getOrganisms()
	{
		return $this->getRowsAsModel($this->getMapper()->find($this->getId(), $this)->findManyToManyRowset("Application_Model_DbTable_Organism",
                "Application_Model_DbTable_InventoryTypeOrganism", "Application_Model_DbTable_InventoryType"), Application_Model_DbTable_Organism);
	}


	/**
	 * gets the references of Application_Model_Organism
	 * @return array
	 */

	public function findOrganisms()
	{
		$org = new Application_Model_Organism();
		$org->getMapper()->getDbTable()->select()->where('s');
		return $this->getRowsAsModel($this->getMapper()->find($this->getId(), $this)->findManyToManyRowset("Application_Model_DbTable_Organism",
                "Application_Model_DbTable_InventoryTypeOrganism", "Application_Model_DbTable_InventoryType"), Application_Model_DbTable_Organism);
	}
	
	/**
	 * sets the Application_Model_Organism models
	 *
	 * @param array  Array of Application_Model_Organism
	 *
	 **/

	public function setOrganisms($Organisms)
	{
		$this->setManyToMany($Organisms, Application_Model_InventoryTypeOrganism, Application_Model_Organism);
	}

	/**
	 * gets the reference of Application_Model_Area
	 * @return Application_Model_Area
	 */

	public function getArea()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_Area'), Application_Model_Area);
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_Area model
	 *
	 * @param Application_Model_Area
	 *
	 **/

	public function setArea($Area)
	{
		$this->setManyToOne($Area, Application_Model_Area);
	}

	/**
	 * sets column id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryType
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
	 * sets column name type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_InventoryType
	 *
	 **/

	public function setName($data)
	{
		$this->_Name=$data;
		return $this;
	}

	/**
	 * gets column name type varchar(50)
	 * @return string
	 */

	public function getName()
	{
		return $this->_Name;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_InventoryTypeMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_InventoryTypeMapper());
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

