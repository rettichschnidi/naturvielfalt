<?php
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */
 
class Default_Model_FloraOrganismStatus extends MainModel
{

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
    protected $_Desc;
    

    

function __construct() {
    $this->setColumnsList(array(
    'status'=>'Status',
    'desc'=>'Desc',
    ));
}

	
    
    /**
     * sets column status type varchar(1)     
     *
     * @param string $data
     * @return Default_Model_FloraOrganismStatus     
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
     * sets column desc type varchar(255)     
     *
     * @param string $data
     * @return Default_Model_FloraOrganismStatus     
     *
     **/

    public function setDesc($data)
    {
        $this->_Desc=$data;
        return $this;
    }

    /**
     * gets column desc type varchar(255)
     * @return string     
     */
     
    public function getDesc()
    {
        return $this->_Desc;
    }
    
    /**
     * returns the mapper class
     *
     * @return Default_Model_FloraOrganismStatusMapper
     *
     */

    public function getMapper()
    {
        if (null === $this->_mapper) {
            $this->setMapper(new Default_Model_FloraOrganismStatusMapper());
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
        if (!$this->getStatus())
            throw new Exception('Primary Key does not contain a value');
        return $this->getMapper()->getDbTable()->delete('status = '.$this->getStatus());
    }

}

