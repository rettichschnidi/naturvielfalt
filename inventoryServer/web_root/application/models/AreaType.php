<?php
require_once('AreaTypeMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_AreaType extends MainModel
{

	/**
	 * mysql var type int(11)
	 *
	 * @var int
	 */
	protected $_Id;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_Desc;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'desc'=>'Desc',
		));
	}

	/**
	 * gets the references of Application_Model_Area
	 * @return array
	 */

	public function getAreas()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_Area'),
		new Application_Model_Area());
	}

	/**
	 * sets the Application_Model_Area models
	 *
	 * @param array  Array of Application_Model_Area
	 *
	 **/

	public function setAreas($Areas)
	{
		$this->setOneToMany($Areas, Application_Model_Area);
	}


	/**
	 * sets column id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_AreaType
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
	 * sets column desc type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_AreaType
	 *
	 **/

	public function setDesc($data)
	{
		$this->_Desc=$data;
		return $this;
	}

	/**
	 * gets column desc type varchar(100)
	 * @return string
	 */
	 
	public function getDesc()
	{
		return $this->_Desc;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_AreaTypeMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_AreaTypeMapper());
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

