<?php
require_once('HabitatMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_Habitat extends MainModel
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
	protected $_LrMethodId;

	/**
	 * mysql var type int(11)
	 *
	 * @var int
	 */
	protected $_ENr;

	/**
	 * mysql var type varchar(10)
	 *
	 * @var string
	 */
	protected $_Label;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_NameDe;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_NameLt;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_NameEn;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_NameFr;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_NameIt;

	/**
	 * mysql var type text
	 *
	 * @var text
	 */
	protected $_Notes;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'LrMethodId'=>'LrMethodId',
    'ENr'=>'ENr',
    'label'=>'Label',
    'name_de'=>'NameDe',
    'name_lt'=>'NameLt',
    'name_en'=>'NameEn',
    'name_fr'=>'NameFr',
    'name_it'=>'NameIt',
    'notes'=>'Notes',
		));
	}

	/**
	 * gets the references of Application_Model_Organism
	 * @return array
	 */

	public function getOrganisms()
	{
		return $this->getRowsAsModel($this->getMapper()->find($this->getId(), $this)->findManyToManyRowset("Application_Model_DbTable_Organism",
                "Application_Model_DbTable_OrganismHabitat", "Application_Model_DbTable_Habitat"), new Application_Model_DbTable_Organism());
	}

	/**
	 * sets the Application_Model_Organism models
	 *
	 * @param array  Array of Application_Model_Organism
	 *
	 **/

	public function setOrganisms($Organisms)
	{
		$this->setManyToMany($Organisms, Application_Model_OrganismHabitat, Application_Model_Organism);
	}



	/**
	 * gets the references of Application_Model_Area
	 * @return array
	 */

	public function getAreas()
	{
		return $this->getRowsAsModel($this->getMapper()->find($this->getId(), $this)->findManyToManyRowset("Application_Model_DbTable_Area",
                "Application_Model_DbTable_AreaHabitat", "Application_Model_DbTable_Habitat"), Application_Model_DbTable_Area);
	}

	/**
	 * sets the Application_Model_Area models
	 *
	 * @param array  Array of Application_Model_Area
	 *
	 **/

	public function setAreas($Areas)
	{
		$this->setManyToMany($Areas, Application_Model_AreaHabitat, Application_Model_Area);
	}



	/**
	 * sets column id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_Habitat
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
	 * sets column LrMethodId type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_Habitat
	 *
	 **/

	public function setLrMethodId($data)
	{
		$this->_LrMethodId=$data;
		return $this;
	}

	/**
	 * gets column LrMethodId type int(11)
	 * @return int
	 */

	public function getLrMethodId()
	{
		return $this->_LrMethodId;
	}

	/**
	 * sets column ENr type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_Habitat
	 *
	 **/

	public function setENr($data)
	{
		$this->_ENr=$data;
		return $this;
	}

	/**
	 * gets column ENr type int(11)
	 * @return int
	 */

	public function getENr()
	{
		return $this->_ENr;
	}

	/**
	 * sets column label type varchar(10)
	 *
	 * @param string $data
	 * @return Application_Model_Habitat
	 *
	 **/

	public function setLabel($data)
	{
		$this->_Label=$data;
		return $this;
	}

	/**
	 * gets column label type varchar(10)
	 * @return string
	 */

	public function getLabel()
	{
		return $this->_Label;
	}

	/**
	 * sets column name_de type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_Habitat
	 *
	 **/

	public function setNameDe($data)
	{
		$this->_NameDe=$data;
		return $this;
	}

	/**
	 * gets column name_de type varchar(100)
	 * @return string
	 */

	public function getNameDe()
	{
		return $this->_NameDe;
	}

	/**
	 * sets column name_lt type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_Habitat
	 *
	 **/

	public function setNameLt($data)
	{
		$this->_NameLt=$data;
		return $this;
	}

	/**
	 * gets column name_lt type varchar(100)
	 * @return string
	 */

	public function getNameLt()
	{
		return $this->_NameLt;
	}

	/**
	 * sets column name_en type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_Habitat
	 *
	 **/

	public function setNameEn($data)
	{
		$this->_NameEn=$data;
		return $this;
	}

	/**
	 * gets column name_en type varchar(100)
	 * @return string
	 */

	public function getNameEn()
	{
		return $this->_NameEn;
	}

	/**
	 * sets column name_fr type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_Habitat
	 *
	 **/

	public function setNameFr($data)
	{
		$this->_NameFr=$data;
		return $this;
	}

	/**
	 * gets column name_fr type varchar(100)
	 * @return string
	 */

	public function getNameFr()
	{
		return $this->_NameFr;
	}

	/**
	 * sets column name_it type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_Habitat
	 *
	 **/

	public function setNameIt($data)
	{
		$this->_NameIt=$data;
		return $this;
	}

	/**
	 * gets column name_it type varchar(100)
	 * @return string
	 */

	public function getNameIt()
	{
		return $this->_NameIt;
	}

	/**
	 * sets column notes type text
	 *
	 * @param text $data
	 * @return Application_Model_Habitat
	 *
	 **/

	public function setNotes($data)
	{
		$this->_Notes=$data;
		return $this;
	}

	/**
	 * gets column notes type text
	 * @return text
	 */

	public function getNotes()
	{
		return $this->_Notes;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_HabitatMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_HabitatMapper());
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

  public function toJsonArray(){
    $result = array(
      'id'      => $this->getId(),
      'label'   => $this->getLabel(),
      'name_de' => $this->getNameDe()
    );
    return $result;
  }
}

