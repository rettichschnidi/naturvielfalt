<?php
require_once('FaunaClassMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_FaunaClass extends MainModel
{

	/**
	 * mysql var type int(11)
	 *
	 * @var int
	 */
	protected $_Id;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_NameLt;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_NameDe;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'name_lt'=>'NameLt',
    'name_de'=>'NameDe',
		));
	}
	/**
	 * gets the references of Application_Model_FaunaOrganism
	 * @return array
	 */

	public function getFaunaOrganisms()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_FaunaOrganism'),
		Application_Model_FaunaOrganism);
	}

	/**
	 * sets the Application_Model_FaunaOrganism models
	 *
	 * @param array  Array of Application_Model_FaunaOrganism
	 *
	 **/

	public function setFaunaOrganisms($FaunaOrganisms)
	{
		$this->setOneToMany($FaunaOrganisms, Application_Model_FaunaOrganism);
	}



	/**
	 * sets column id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_FaunaClass
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
	 * sets column name_lt type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaClass
	 *
	 **/

	public function setNameLt($data)
	{
		$this->_NameLt=$data;
		return $this;
	}

	/**
	 * gets column name_lt type varchar(50)
	 * @return string
	 */
	 
	public function getNameLt()
	{
		return $this->_NameLt;
	}

	/**
	 * sets column name_de type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaClass
	 *
	 **/

	public function setNameDe($data)
	{
		$this->_NameDe=$data;
		return $this;
	}

	/**
	 * gets column name_de type varchar(50)
	 * @return string
	 */
	 
	public function getNameDe()
	{
		return $this->_NameDe;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_FaunaClassMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_FaunaClassMapper());
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

