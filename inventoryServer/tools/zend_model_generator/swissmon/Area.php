<?php
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */
 
class Default_Model_Area extends MainModel
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
    protected $_TypeId;
    
    /**
     * mysql var type varchar(100)
     *
     * @var string     
     */
    protected $_FieldName;
    
    /**
     * mysql var type int(6)
     *
     * @var int     
     */
    protected $_Altitude;
    
    /**
     * mysql var type text
     *
     * @var text     
     */
    protected $_Comment;
    

    

function __construct() {
    $this->setColumnsList(array(
    'id'=>'Id',
    'type_id'=>'TypeId',
    'field_name'=>'FieldName',
    'altitude'=>'Altitude',
    'comment'=>'Comment',
    ));
}

	
    
    /**
     * sets column id type int(11)     
     *
     * @param int $data
     * @return Default_Model_Area     
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
     * sets column type_id type int(11)     
     *
     * @param int $data
     * @return Default_Model_Area     
     *
     **/

    public function setTypeId($data)
    {
        $this->_TypeId=$data;
        return $this;
    }

    /**
     * gets column type_id type int(11)
     * @return int     
     */
     
    public function getTypeId()
    {
        return $this->_TypeId;
    }
    
    /**
     * sets column field_name type varchar(100)     
     *
     * @param string $data
     * @return Default_Model_Area     
     *
     **/

    public function setFieldName($data)
    {
        $this->_FieldName=$data;
        return $this;
    }

    /**
     * gets column field_name type varchar(100)
     * @return string     
     */
     
    public function getFieldName()
    {
        return $this->_FieldName;
    }
    
    /**
     * sets column altitude type int(6)     
     *
     * @param int $data
     * @return Default_Model_Area     
     *
     **/

    public function setAltitude($data)
    {
        $this->_Altitude=$data;
        return $this;
    }

    /**
     * gets column altitude type int(6)
     * @return int     
     */
     
    public function getAltitude()
    {
        return $this->_Altitude;
    }
    
    /**
     * sets column comment type text     
     *
     * @param text $data
     * @return Default_Model_Area     
     *
     **/

    public function setComment($data)
    {
        $this->_Comment=$data;
        return $this;
    }

    /**
     * gets column comment type text
     * @return text     
     */
     
    public function getComment()
    {
        return $this->_Comment;
    }
    
    /**
     * returns the mapper class
     *
     * @return Default_Model_AreaMapper
     *
     */

    public function getMapper()
    {
        if (null === $this->_mapper) {
            $this->setMapper(new Default_Model_AreaMapper());
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

