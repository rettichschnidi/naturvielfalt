<?php
require_once('InventoryTypeOrganismMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */
 
class Application_Model_InventoryTypeOrganism extends MainModel
{

    /**
     * mysql var type int(10) unsigned
     *
     * @var int     
     */
    protected $_Id;
    
    /**
     * mysql var type int(10) unsigned
     *
     * @var int     
     */
    protected $_OrganismId;
    
    /**
     * mysql var type int(10) unsigned
     *
     * @var int     
     */
    protected $_InventoryTypeId;
    

    

function __construct() {
    $this->setColumnsList(array(
    'id'=>'Id',
    'organism_id'=>'OrganismId',
    'inventory_type_id'=>'InventoryTypeId',
    ));
}

	
    
    /**
     * sets column id type int(10) unsigned     
     *
     * @param int $data
     * @return Application_Model_InventoryTypeOrganism     
     *
     **/

    public function setId($data)
    {
        $this->_Id=$data;
        return $this;
    }

    /**
     * gets column id type int(10) unsigned
     * @return int     
     */
     
    public function getId()
    {
        return $this->_Id;
    }
    
    /**
     * sets column organism_id type int(10) unsigned     
     *
     * @param int $data
     * @return Application_Model_InventoryTypeOrganism     
     *
     **/

    public function setOrganismId($data)
    {
        $this->_OrganismId=$data;
        return $this;
    }

    /**
     * gets column organism_id type int(10) unsigned
     * @return int     
     */
     
    public function getOrganismId()
    {
        return $this->_OrganismId;
    }
    
    /**
     * sets column inventory_type_id type int(10) unsigned     
     *
     * @param int $data
     * @return Application_Model_InventoryTypeOrganism     
     *
     **/

    public function setInventoryTypeId($data)
    {
        $this->_InventoryTypeId=$data;
        return $this;
    }

    /**
     * gets column inventory_type_id type int(10) unsigned
     * @return int     
     */
     
    public function getInventoryTypeId()
    {
        return $this->_InventoryTypeId;
    }
    
    /**
     * returns the mapper class
     *
     * @return Application_Model_InventoryTypeOrganismMapper
     *
     */

    public function getMapper()
    {
        if (null === $this->_mapper) {
            $this->setMapper(new Application_Model_InventoryTypeOrganismMapper());
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

