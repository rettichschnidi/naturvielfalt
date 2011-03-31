<?php
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */
 
class Default_Model_FaunaOrganismHabitat extends MainModel
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
    protected $_FaunaOrganismId;
    
    /**
     * mysql var type int(11)
     *
     * @var int     
     */
    protected $_HabitatId;
    
    /**
     * mysql var type int(11)
     *
     * @var int     
     */
    protected $_DependencyType;
    
    /**
     * mysql var type int(11)
     *
     * @var int     
     */
    protected $_DependencyPhase;
    
    /**
     * mysql var type int(11)
     *
     * @var int     
     */
    protected $_DependencyFunction;
    
    /**
     * mysql var type varchar(3)
     *
     * @var string     
     */
    protected $_DependencyValue;
    
    /**
     * mysql var type varchar(255)
     *
     * @var string     
     */
    protected $_Note;
    

    

function __construct() {
    $this->setColumnsList(array(
    'id'=>'Id',
    'fauna_organism_id'=>'FaunaOrganismId',
    'habitat_id'=>'HabitatId',
    'dependency_type'=>'DependencyType',
    'dependency_phase'=>'DependencyPhase',
    'dependency_function'=>'DependencyFunction',
    'dependency_value'=>'DependencyValue',
    'note'=>'Note',
    ));
}

	
    
    /**
     * sets column id type int(11)     
     *
     * @param int $data
     * @return Default_Model_FaunaOrganismHabitat     
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
     * sets column fauna_organism_id type int(11)     
     *
     * @param int $data
     * @return Default_Model_FaunaOrganismHabitat     
     *
     **/

    public function setFaunaOrganismId($data)
    {
        $this->_FaunaOrganismId=$data;
        return $this;
    }

    /**
     * gets column fauna_organism_id type int(11)
     * @return int     
     */
     
    public function getFaunaOrganismId()
    {
        return $this->_FaunaOrganismId;
    }
    
    /**
     * sets column habitat_id type int(11)     
     *
     * @param int $data
     * @return Default_Model_FaunaOrganismHabitat     
     *
     **/

    public function setHabitatId($data)
    {
        $this->_HabitatId=$data;
        return $this;
    }

    /**
     * gets column habitat_id type int(11)
     * @return int     
     */
     
    public function getHabitatId()
    {
        return $this->_HabitatId;
    }
    
    /**
     * sets column dependency_type type int(11)     
     *
     * @param int $data
     * @return Default_Model_FaunaOrganismHabitat     
     *
     **/

    public function setDependencyType($data)
    {
        $this->_DependencyType=$data;
        return $this;
    }

    /**
     * gets column dependency_type type int(11)
     * @return int     
     */
     
    public function getDependencyType()
    {
        return $this->_DependencyType;
    }
    
    /**
     * sets column dependency_phase type int(11)     
     *
     * @param int $data
     * @return Default_Model_FaunaOrganismHabitat     
     *
     **/

    public function setDependencyPhase($data)
    {
        $this->_DependencyPhase=$data;
        return $this;
    }

    /**
     * gets column dependency_phase type int(11)
     * @return int     
     */
     
    public function getDependencyPhase()
    {
        return $this->_DependencyPhase;
    }
    
    /**
     * sets column dependency_function type int(11)     
     *
     * @param int $data
     * @return Default_Model_FaunaOrganismHabitat     
     *
     **/

    public function setDependencyFunction($data)
    {
        $this->_DependencyFunction=$data;
        return $this;
    }

    /**
     * gets column dependency_function type int(11)
     * @return int     
     */
     
    public function getDependencyFunction()
    {
        return $this->_DependencyFunction;
    }
    
    /**
     * sets column dependency_value type varchar(3)     
     *
     * @param string $data
     * @return Default_Model_FaunaOrganismHabitat     
     *
     **/

    public function setDependencyValue($data)
    {
        $this->_DependencyValue=$data;
        return $this;
    }

    /**
     * gets column dependency_value type varchar(3)
     * @return string     
     */
     
    public function getDependencyValue()
    {
        return $this->_DependencyValue;
    }
    
    /**
     * sets column note type varchar(255)     
     *
     * @param string $data
     * @return Default_Model_FaunaOrganismHabitat     
     *
     **/

    public function setNote($data)
    {
        $this->_Note=$data;
        return $this;
    }

    /**
     * gets column note type varchar(255)
     * @return string     
     */
     
    public function getNote()
    {
        return $this->_Note;
    }
    
    /**
     * returns the mapper class
     *
     * @return Default_Model_FaunaOrganismHabitatMapper
     *
     */

    public function getMapper()
    {
        if (null === $this->_mapper) {
            $this->setMapper(new Default_Model_FaunaOrganismHabitatMapper());
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

