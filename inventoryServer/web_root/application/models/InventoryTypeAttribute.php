<?php
require_once('InventoryTypeAttributeMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_InventoryTypeAttribute extends MainModel
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
	 * mysql var type varchar(45)
	 *
	 * @var string
	 */
	protected $_Name;

	/**
	 * mysql var type int(11) unsigned
	 *
	 * @var int
	 */
	protected $_AttributeFormatId;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'inventory_type_id'=>'InventoryTypeId',
    'name'=>'Name',
    'attribute_format_id'=>'AttributeFormatId',
		));
	}


	/**
	 * gets the reference of Application_Model_InventoryType
	 * @return Application_Model_InventoryType
	 */

	public function getInventoryType()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_InventoryType'), Application_Model_InventoryType);
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
		$this->setManyToOne($InventoryType, Application_Model_InventoryType);
	}

	/**
	 * gets the reference of Application_Model_AttributeFormat
	 * @return Application_Model_AttributeFormat
	 */

	public function getAttributeFormat()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_AttributeFormat'), 'Application_Model_AttributeFormat');
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_AttributeFormat model
	 *
	 * @param Application_Model_AttributeFormat
	 *
	 **/

	public function setAttributeFormat($AttributeFormat)
	{
		$this->setManyToOne($AttributeFormat, Application_Model_AttributeFormat);
	}

	/**
	 * gets the references of Application_Model_InventoryTypeAttributeDropdownValue
	 * @return array
	 */

	public function getInventoryTypeAttributeDropdownValue()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_InventoryTypeAttributeDropdownValue'),
		new Application_Model_InventoryTypeAttributeDropdownValue());
	}

	/**
	 * sets the Application_Model_InventoryTypeAttributeDropdownValue models
	 *
	 * @param array  Array of Application_Model_InventoryTypeAttributeDropdownValue
	 *
	 **/

	public function setInventoryTypeAttributeDropdownValue($InventoryTypeAttributeDropdownValue)
	{
		$this->setOneToMany($InventoryTypeAttributeDropdownValue, Application_Model_InventoryTypeAttributeDropdownValue);
	}

	/**
	 * sets column id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryTypeAttribute
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
	 * @return Application_Model_InventoryTypeAttribute
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
	 * sets column name type varchar(45)
	 *
	 * @param string $data
	 * @return Application_Model_InventoryTypeAttribute
	 *
	 **/

	public function setName($data)
	{
		$this->_Name=$data;
		return $this;
	}

	/**
	 * gets column name type varchar(45)
	 * @return string
	 */

	public function getName()
	{
		return $this->_Name;
	}

	/**
	 * sets column attribute_format_id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_InventoryTypeAttribute
	 *
	 **/

	public function setAttributeFormatId($data)
	{
		$this->_AttributeFormatId=$data;
		return $this;
	}

	/**
	 * gets column attribute_format_id type int(11) unsigned
	 * @return int
	 */

	public function getAttributeFormatId()
	{
		return $this->_AttributeFormatId;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_InventoryTypeAttributeMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_InventoryTypeAttributeMapper());
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

