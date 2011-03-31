<?php
require_once('AttributeFormatMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_AttributeFormat extends MainModel
{

	/**
	 * mysql var type int(11) unsigned
	 *
	 * @var int
	 */
	protected $_Id;

	/**
	 * mysql var type varchar(45)
	 *
	 * @var string
	 */
	protected $_Name;

	/**
	 * mysql var type varchar(45)
	 *
	 * @var string
	 */
	protected $_Format;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'name'=>'Name',
    'format'=>'Format',
		));
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
	 * sets column id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_AttributeFormat
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
	 * sets column name type varchar(45)
	 *
	 * @param string $data
	 * @return Application_Model_AttributeFormat
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
	 * sets column format type varchar(45)
	 *
	 * @param string $data
	 * @return Application_Model_AttributeFormat
	 *
	 **/

	public function setFormat($data)
	{
		$this->_Format=$data;
		return $this;
	}

	/**
	 * gets column format type varchar(45)
	 * @return string
	 */
	 
	public function getFormat()
	{
		return $this->_Format;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_AttributeFormatMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_AttributeFormatMapper());
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

