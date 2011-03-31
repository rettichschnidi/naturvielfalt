<?php
require_once('HeadInventoryMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_HeadInventory extends MainModel
{

	/**
	 * mysql var type int(11)
	 *
	 * @var int
	 */
	protected $_Id;

	/**
	 * mysql var type int(11)
	 *
	 * @var int
	 */
	protected $_AreaId;

	/**
	 * mysql var type varchar(512)
	 *
	 * @var string
	 */
	protected $_Name;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'area_id'=>'AreaId',
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
		new Application_Model_Inventory());
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
	 * sets column id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_HeadInventory
	 *
	 **/

	public function setId($data)
	{
		$this->_Id=$data;
		return $this;
	}

	/**
	 * gets column id type int(11)
	 * @return int
	 */

	public function getId()
	{
		return $this->_Id;
	}

	/**
	 * sets column area_id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_HeadInventory
	 *
	 **/

	public function setAreaId($data)
	{
		$this->_AreaId=$data;
		return $this;
	}

	/**
	 * gets column area_id type int(11)
	 * @return int
	 */

	public function getAreaId()
	{
		return $this->_AreaId;
	}

	/**
	 * sets column name type varchar(512)
	 *
	 * @param string $data
	 * @return Application_Model_HeadInventory
	 *
	 **/

	public function setName($data)
	{
		$this->_Name=$data;
		return $this;
	}

	/**
	 * gets column name type varchar(512)
	 * @return string
	 */

	public function getName()
	{
		return $this->_Name;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_HeadInventoryMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_HeadInventoryMapper());
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

