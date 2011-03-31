<?php
require_once('OrganismMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_Organism extends MainModel
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
	protected $_OrganismType;

	/**
	 * mysql var type int(11) unsigned
	 *
	 * @var int
	 */
	protected $_OrganismId;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'organism_type'=>'OrganismType',
    'organism_id'=>'OrganismId',
		));
	}

	public function isFauna()
	{
		return $this->getOrganismType() == 1 ? true : false;
	}

	public function getLabel()
	{
		$label = "";
		$organism = "";
		if($this->isFauna())
		{
			$organism = new Application_Model_FaunaOrganism();
			$organism->find($this->getOrganismId());
		}
		else
		{
			$organism = new Application_Model_FloraOrganism();
			$organism->find($this->getOrganismId());
		}


		$genus = ($this->isFauna() ? $organism->getGenus() : $organism->getGattung());
		$genus = ($genus == null ? "" : $genus);
		$species = ($this->isFauna() ? $organism->getSpecies() : $organism->getArt());
		$species = ($species == null ? "" : $species);
		$label = $organism->getNameDe()." [".$genus." ".$species."]";

		return $label;
	}

	/**
	 * gets the reference of Application_Model_FloraOrganism
	 * @return Application_Model_FloraOrganism
	 */

	public function getFloraOrganism()
	{
		$mpr = new Application_Model_FloraOrganismMapper();
		return $mpr->find($this->getOrganismId());
	}

	/**
	 * sets the Application_Model_FloraOrganism model
	 *
	 * @param Application_Model_FloraOrganism
	 *
	 **/

	public function setFloraOrganism($FloraOrganism)
	{
		$this->setId($FloraOrganism->getId());
	}

	/**
	 * gets the reference of Application_Model_FaunaOrganism
	 * @return Application_Model_FaunaOrganism
	 */

	public function getFaunaOrganism()
	{
		$mpr = new Application_Model_FaunaOrganismMapper();
		return $mpr->find($this->getOrganismId());
	}

	/**
	 * sets the Application_Model_FaunaOrganism model
	 *
	 * @param Application_Model_FaunaOrganism
	 *
	 **/

	public function setFaunaOrganism($FaunaOrganism)
	{
		$this->setId($FaunaOrganism->getId());
	}

	/**
	 * gets the references of Application_Model_InventoryEntry
	 * @return array
	 */

	public function getInventoryEntrys()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_InventoryEntry'),
		Application_Model_InventoryEntry);
	}

	/**
	 * sets the Application_Model_InventoryEntry models
	 *
	 * @param array  Array of Application_Model_InventoryEntry
	 *
	 **/

	public function setInventoryEntrys($InventoryEntrys)
	{
		$this->setOneToMany($InventoryEntrys, Application_Model_InventoryEntry);
	}


	/**
	 * gets the references of Application_Model_Habitat
	 * @return array
	 */

	public function getHabitats()
	{
		return $this->getRowsAsModel($this->getMapper()->find($this->getId(), $this)->findManyToManyRowset("Application_Model_DbTable_Habitat",
                "Application_Model_DbTable_OrganismHabitat", "Application_Model_DbTable_Organism"), Application_Model_DbTable_Habitat);
	}

	/**
	 * sets the Application_Model_Habitat models
	 *
	 * @param array  Array of Application_Model_Habitat
	 *
	 **/

	public function setHabitats($Habitats)
	{
		$this->setManyToMany($Habitats, Application_Model_OrganismHabitat, Application_Model_Habitat);
	}


	/**
	 * gets the references of Application_Model_InventoryType
	 * @return array
	 */

	public function getInventoryTypes()
	{
		return $this->getRowsAsModel($this->getMapper()->find($this->getId(), $this)->findManyToManyRowset("Application_Model_DbTable_InventoryType",
                "Application_Model_DbTable_InventoryTypeOrganism", "Application_Model_DbTable_Organism"), Application_Model_DbTable_InventoryType);
	}

	/**
	 * sets the Application_Model_InventoryType models
	 *
	 * @param array  Array of Application_Model_InventoryType
	 *
	 **/

	public function setInventoryTypes($InventoryTypes)
	{
		$this->setManyToMany($InventoryTypes, Application_Model_InventoryTypeOrganism, Application_Model_InventoryType);
	}



	/**
	 * sets column id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_Organism
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
	 * sets column organism_type type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_Organism
	 *
	 **/

	public function setOrganismType($data)
	{
		$this->_OrganismType=$data;
		return $this;
	}

	/**
	 * gets column organism_type type int(11) unsigned
	 * @return int
	 */

	public function getOrganismType()
	{
		return $this->_OrganismType;
	}

	/**
	 * sets column organism_id type int(11) unsigned
	 *
	 * @param int $data
	 * @return Application_Model_Organism
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
	 * returns the mapper class
	 *
	 * @return Application_Model_OrganismMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_OrganismMapper());
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

