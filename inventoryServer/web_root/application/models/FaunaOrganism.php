<?php
require_once('FaunaOrganismMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_FaunaOrganism extends MainModel
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
	protected $_CscfNr;

	/**
	 * mysql var type int(11)
	 *
	 * @var int
	 */
	protected $_FaunaClassId;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_Order;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_Family;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_Genus;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_SubGenus;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_Species;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_SubSpecies;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_AuthorYear;

	/**
	 * mysql var type varchar(255)
	 *
	 * @var string
	 */
	protected $_NameDe;

	/**
	 * mysql var type varchar(255)
	 *
	 * @var string
	 */
	protected $_NameFr;

	/**
	 * mysql var type varchar(255)
	 *
	 * @var string
	 */
	protected $_NameIt;

	/**
	 * mysql var type varchar(255)
	 *
	 * @var string
	 */
	protected $_NameRo;

	/**
	 * mysql var type varchar(255)
	 *
	 * @var string
	 */
	protected $_NameEn;

	/**
	 * mysql var type int(11)
	 *
	 * @var int
	 */
	protected $_ProtectionCh;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'cscf_nr'=>'CscfNr',
    'fauna_class_id'=>'FaunaClassId',
    'order'=>'Order',
    'family'=>'Family',
    'genus'=>'Genus',
    'sub_genus'=>'SubGenus',
    'species'=>'Species',
    'sub_species'=>'SubSpecies',
    'author_year'=>'AuthorYear',
    'name_de'=>'NameDe',
    'name_fr'=>'NameFr',
    'name_it'=>'NameIt',
    'name_ro'=>'NameRo',
    'name_en'=>'NameEn',
    'protection_ch'=>'ProtectionCh',
		));
	}
	/**
	 * gets the reference of Application_Model_FaunaClass
	 * @return Application_Model_FaunaClass
	 */

	public function getFaunaClass()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_FaunaClass'), Application_Model_FaunaClass);
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_FaunaClass model
	 *
	 * @param Application_Model_FaunaClass
	 *
	 **/

	public function setFaunaClass($FaunaClass)
	{
		$this->setManyToOne($FaunaClass, Application_Model_FaunaClass);
	}



	/**
	 * sets column id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_FaunaOrganism
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
	 * sets column cscf_nr type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setCscfNr($data)
	{
		$this->_CscfNr=$data;
		return $this;
	}

	/**
	 * gets column cscf_nr type int(11)
	 * @return int
	 */
	 
	public function getCscfNr()
	{
		return $this->_CscfNr;
	}

	/**
	 * sets column fauna_class_id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setFaunaClassId($data)
	{
		$this->_FaunaClassId=$data;
		return $this;
	}

	/**
	 * gets column fauna_class_id type int(11)
	 * @return int
	 */
	 
	public function getFaunaClassId()
	{
		return $this->_FaunaClassId;
	}

	/**
	 * sets column order type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setOrder($data)
	{
		$this->_Order=$data;
		return $this;
	}

	/**
	 * gets column order type varchar(50)
	 * @return string
	 */
	 
	public function getOrder()
	{
		return $this->_Order;
	}

	/**
	 * sets column family type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setFamily($data)
	{
		$this->_Family=$data;
		return $this;
	}

	/**
	 * gets column family type varchar(50)
	 * @return string
	 */
	 
	public function getFamily()
	{
		return $this->_Family;
	}

	/**
	 * sets column genus type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setGenus($data)
	{
		$this->_Genus=$data;
		return $this;
	}

	/**
	 * gets column genus type varchar(50)
	 * @return string
	 */
	 
	public function getGenus()
	{
		return $this->_Genus;
	}

	/**
	 * sets column sub_genus type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setSubGenus($data)
	{
		$this->_SubGenus=$data;
		return $this;
	}

	/**
	 * gets column sub_genus type varchar(50)
	 * @return string
	 */
	 
	public function getSubGenus()
	{
		return $this->_SubGenus;
	}

	/**
	 * sets column species type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setSpecies($data)
	{
		$this->_Species=$data;
		return $this;
	}

	/**
	 * gets column species type varchar(50)
	 * @return string
	 */
	 
	public function getSpecies()
	{
		return $this->_Species;
	}

	/**
	 * sets column sub_species type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setSubSpecies($data)
	{
		$this->_SubSpecies=$data;
		return $this;
	}

	/**
	 * gets column sub_species type varchar(50)
	 * @return string
	 */
	 
	public function getSubSpecies()
	{
		return $this->_SubSpecies;
	}

	/**
	 * sets column author_year type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setAuthorYear($data)
	{
		$this->_AuthorYear=$data;
		return $this;
	}

	/**
	 * gets column author_year type varchar(100)
	 * @return string
	 */
	 
	public function getAuthorYear()
	{
		return $this->_AuthorYear;
	}

	/**
	 * sets column name_de type varchar(255)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setNameDe($data)
	{
		$this->_NameDe=$data;
		return $this;
	}

	/**
	 * gets column name_de type varchar(255)
	 * @return string
	 */
	 
	public function getNameDe()
	{
		return $this->_NameDe;
	}

	/**
	 * sets column name_fr type varchar(255)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setNameFr($data)
	{
		$this->_NameFr=$data;
		return $this;
	}

	/**
	 * gets column name_fr type varchar(255)
	 * @return string
	 */
	 
	public function getNameFr()
	{
		return $this->_NameFr;
	}

	/**
	 * sets column name_it type varchar(255)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setNameIt($data)
	{
		$this->_NameIt=$data;
		return $this;
	}

	/**
	 * gets column name_it type varchar(255)
	 * @return string
	 */
	 
	public function getNameIt()
	{
		return $this->_NameIt;
	}

	/**
	 * sets column name_ro type varchar(255)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setNameRo($data)
	{
		$this->_NameRo=$data;
		return $this;
	}

	/**
	 * gets column name_ro type varchar(255)
	 * @return string
	 */
	 
	public function getNameRo()
	{
		return $this->_NameRo;
	}

	/**
	 * sets column name_en type varchar(255)
	 *
	 * @param string $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setNameEn($data)
	{
		$this->_NameEn=$data;
		return $this;
	}

	/**
	 * gets column name_en type varchar(255)
	 * @return string
	 */
	 
	public function getNameEn()
	{
		return $this->_NameEn;
	}

	/**
	 * sets column protection_ch type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_FaunaOrganism
	 *
	 **/

	public function setProtectionCh($data)
	{
		$this->_ProtectionCh=$data;
		return $this;
	}

	/**
	 * gets column protection_ch type int(11)
	 * @return int
	 */
	 
	public function getProtectionCh()
	{
		return $this->_ProtectionCh;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_FaunaOrganismMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_FaunaOrganismMapper());
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

