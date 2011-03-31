<?php
require_once('FloraOrganismMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_FloraOrganism extends MainModel
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
	protected $_SisfNr;

	/**
	 * mysql var type varchar(1)
	 *
	 * @var string
	 */
	protected $_Status;

	/**
	 * mysql var type varchar(255)
	 *
	 * @var string
	 */
	protected $_Name;

	/**
	 * mysql var type varchar(255)
	 *
	 * @var string
	 */
	protected $_Familie;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_Gattung;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_Art;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_Autor;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_Rang;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_NameUnterart;

	/**
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_AutorUnterart;

	/**
	 * mysql var type varchar(255)
	 *
	 * @var string
	 */
	protected $_GueltigeNamen;

	/**
	 * mysql var type int(11)
	 *
	 * @var int
	 */
	protected $_OfficialFloraOrfganismId;

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
	 * mysql var type varchar(50)
	 *
	 * @var string
	 */
	protected $_NameReference;

	/**
	 * mysql var type bit(1)
	 *
	 * @var bit
	 */
	protected $_IsNeophyte;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'sisf_nr'=>'SisfNr',
    'status'=>'Status',
    'name'=>'Name',
    'Familie'=>'Familie',
    'Gattung'=>'Gattung',
    'Art'=>'Art',
    'Autor'=>'Autor',
    'Rang'=>'Rang',
    'NameUnterart'=>'NameUnterart',
    'AutorUnterart'=>'AutorUnterart',
    'GueltigeNamen'=>'GueltigeNamen',
    'official_flora_orfganism_id'=>'OfficialFloraOrfganismId',
    'name_de'=>'NameDe',
    'name_fr'=>'NameFr',
    'name_it'=>'NameIt',
    'name_reference'=>'NameReference',
    'is_neophyte'=>'IsNeophyte',
		));
	}
	/**
	 * gets the reference of Application_Model_FloraOrganismStatus
	 * @return Application_Model_FloraOrganismStatus
	 */

	public function getFloraOrganismStatus()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_FloraOrganismStatus'), Application_Model_FloraOrganismStatus);
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_FloraOrganismStatus model
	 *
	 * @param Application_Model_FloraOrganismStatus
	 *
	 **/

	public function setFloraOrganismStatus($FloraOrganismStatus)
	{
		$this->setManyToOne($FloraOrganismStatus, Application_Model_FloraOrganismStatus);
	}
	/**
	 * gets the reference of Application_Model_FloraOrganism (officals)
	 * @return Application_Model_FloraOrganism
	 */

	public function getFloraOrganismOffical()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_FloraOrganism'), Application_Model_FloraOrganism);
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_FloraOrganism model (officals)
	 *
	 * @param Application_Model_FloraOrganism
	 *
	 **/

	public function setFloraOrganismOffical($FloraOrganismOffical)
	{
		$this->setManyToOne($FloraOrganismOffical, Application_Model_FloraOrganism);
	}

	/**
	 * gets the references of Application_Model_FloraOrganism (unofficals)
	 * @return array
	 */

	public function getFloraOrganismUnofficials()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_FloraOrganism'),
		Application_Model_FloraOrganism);
	}

	/**
	 * sets the Application_Model_FloraOrganism models (unofficials)
	 *
	 * @param array  Array of Application_Model_FloraOrganism
	 *
	 **/

	public function setFloraOrganismUnofficials($FloraOrganismUnofficals)
	{
		$this->setOneToMany($FloraOrganismUnofficals, Application_Model_FloraOrganism);
	}

	/**
	 * gets the reference of Application_Model_FloraRedList
	 * @return array
	 */

	public function getFloraRedList()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_FloraRedList'),
		Application_Model_FloraRedList);
		if(sizeof($rowset) >= 1)
		{
			return $rowset[0];
		}
		return;
	}

	/**
	 * sets the Application_Model_FloraRedList model
	 *
	 * @param of Application_Model_FloraRedList
	 *
	 **/

	public function setFloraRedLists($FloraRedList)
	{
		$arr = array();
		$arr[] = $FloraRedList;
		$this->setOneToMany($arr, Application_Model_FloraRedList);
	}

	/**
	 * sets column id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_FloraOrganism
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
	 * sets column sisf_nr type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setSisfNr($data)
	{
		$this->_SisfNr=$data;
		return $this;
	}

	/**
	 * gets column sisf_nr type int(11)
	 * @return int
	 */

	public function getSisfNr()
	{
		return $this->_SisfNr;
	}

	/**
	 * sets column status type varchar(1)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setStatus($data)
	{
		$this->_Status=$data;
		return $this;
	}

	/**
	 * gets column status type varchar(1)
	 * @return string
	 */

	public function getStatus()
	{
		return $this->_Status;
	}

	/**
	 * sets column name type varchar(255)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setName($data)
	{
		$this->_Name=$data;
		return $this;
	}

	/**
	 * gets column name type varchar(255)
	 * @return string
	 */

	public function getName()
	{
		return $this->_Name;
	}

	/**
	 * sets column Familie type varchar(255)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setFamilie($data)
	{
		$this->_Familie=$data;
		return $this;
	}

	/**
	 * gets column Familie type varchar(255)
	 * @return string
	 */

	public function getFamilie()
	{
		return $this->_Familie;
	}

	/**
	 * sets column Gattung type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setGattung($data)
	{
		$this->_Gattung=$data;
		return $this;
	}

	/**
	 * gets column Gattung type varchar(100)
	 * @return string
	 */

	public function getGattung()
	{
		return $this->_Gattung;
	}

	/**
	 * sets column Art type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setArt($data)
	{
		$this->_Art=$data;
		return $this;
	}

	/**
	 * gets column Art type varchar(100)
	 * @return string
	 */

	public function getArt()
	{
		return $this->_Art;
	}

	/**
	 * sets column Autor type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setAutor($data)
	{
		$this->_Autor=$data;
		return $this;
	}

	/**
	 * gets column Autor type varchar(100)
	 * @return string
	 */

	public function getAutor()
	{
		return $this->_Autor;
	}

	/**
	 * sets column Rang type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setRang($data)
	{
		$this->_Rang=$data;
		return $this;
	}

	/**
	 * gets column Rang type varchar(50)
	 * @return string
	 */

	public function getRang()
	{
		return $this->_Rang;
	}

	/**
	 * sets column NameUnterart type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setNameUnterart($data)
	{
		$this->_NameUnterart=$data;
		return $this;
	}

	/**
	 * gets column NameUnterart type varchar(100)
	 * @return string
	 */

	public function getNameUnterart()
	{
		return $this->_NameUnterart;
	}

	/**
	 * sets column AutorUnterart type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setAutorUnterart($data)
	{
		$this->_AutorUnterart=$data;
		return $this;
	}

	/**
	 * gets column AutorUnterart type varchar(50)
	 * @return string
	 */

	public function getAutorUnterart()
	{
		return $this->_AutorUnterart;
	}

	/**
	 * sets column GueltigeNamen type varchar(255)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setGueltigeNamen($data)
	{
		$this->_GueltigeNamen=$data;
		return $this;
	}

	/**
	 * gets column GueltigeNamen type varchar(255)
	 * @return string
	 */

	public function getGueltigeNamen()
	{
		return $this->_GueltigeNamen;
	}

	/**
	 * sets column official_flora_orfganism_id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setOfficialFloraOrfganismId($data)
	{
		$this->_OfficialFloraOrfganismId=$data;
		return $this;
	}

	/**
	 * gets column official_flora_orfganism_id type int(11)
	 * @return int
	 */

	public function getOfficialFloraOrfganismId()
	{
		return $this->_OfficialFloraOrfganismId;
	}

	/**
	 * sets column name_de type varchar(255)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
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
	 * @return Application_Model_FloraOrganism
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
	 * @return Application_Model_FloraOrganism
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
	 * sets column name_reference type varchar(50)
	 *
	 * @param string $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setNameReference($data)
	{
		$this->_NameReference=$data;
		return $this;
	}

	/**
	 * gets column name_reference type varchar(50)
	 * @return string
	 */

	public function getNameReference()
	{
		return $this->_NameReference;
	}

	/**
	 * sets column is_neophyte type bit(1)
	 *
	 * @param bit $data
	 * @return Application_Model_FloraOrganism
	 *
	 **/

	public function setIsNeophyte($data)
	{
		$this->_IsNeophyte=$data;
		return $this;
	}

	/**
	 * gets column is_neophyte type bit(1)
	 * @return bit
	 */

	public function getIsNeophyte()
	{
		return $this->_IsNeophyte;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_FloraOrganismMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_FloraOrganismMapper());
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

