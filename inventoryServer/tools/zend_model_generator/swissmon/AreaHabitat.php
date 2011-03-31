<?php
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */
 
class Default_Model_AreaHabitat extends MainModel
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
    protected $_AreaId;
    
    /**
     * mysql var type int(11)
     *
     * @var int     
     */
    protected $_HabitatId;
    

    

function __construct() {
    $this->setColumnsList(array(
    'id'=>'Id',
    'area_id'=>'AreaId',
    'habitat_id'=>'HabitatId',
    ));
}

	
    
    /**
     * sets column id type int(11)     
     *
     * @param int $data
     * @return Default_Model_AreaHabitat     
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
     * sets column area_id type int(11)     
     *
     * @param int $data
     * @return Default_Model_AreaHabitat     
     *
     **/

    public function setAreaId($data)
    {
        $this->_AreaId=$data;
        return $this;
    }

    /**
     * gets column area_id type int(11)
     * @return int     
     */
     
    public function getAreaId()
    {
        return $this->_AreaId;
    }
    
    /**
     * sets column habitat_id type int(11)     
     *
     * @param int $data
     * @return Default_Model_AreaHabitat     
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
     * returns the mapper class
     *
     * @return Default_Model_AreaHabitatMapper
     *
     */

    public function getMapper()
    {
        if (null === $this->_mapper) {
            $this->setMapper(new Default_Model_AreaHabitatMapper());
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

