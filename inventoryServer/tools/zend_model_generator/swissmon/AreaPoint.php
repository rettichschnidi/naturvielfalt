<?php
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */
 
class Default_Model_AreaPoint extends MainModel
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
     * mysql var type float(10,6)
     *
     * @var      
     */
    protected $_Lat;
    
    /**
     * mysql var type float(10,6)
     *
     * @var      
     */
    protected $_Lng;
    
    /**
     * mysql var type int(11)
     *
     * @var int     
     */
    protected $_Seq;
    

    

function __construct() {
    $this->setColumnsList(array(
    'id'=>'Id',
    'area_id'=>'AreaId',
    'lat'=>'Lat',
    'lng'=>'Lng',
    'seq'=>'Seq',
    ));
}

	
    
    /**
     * sets column id type int(11)     
     *
     * @param int $data
     * @return Default_Model_AreaPoint     
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
     * @return Default_Model_AreaPoint     
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
     * sets column lat type float(10,6)     
     *
     * @param  $data
     * @return Default_Model_AreaPoint     
     *
     **/

    public function setLat($data)
    {
        $this->_Lat=$data;
        return $this;
    }

    /**
     * gets column lat type float(10,6)
     * @return      
     */
     
    public function getLat()
    {
        return $this->_Lat;
    }
    
    /**
     * sets column lng type float(10,6)     
     *
     * @param  $data
     * @return Default_Model_AreaPoint     
     *
     **/

    public function setLng($data)
    {
        $this->_Lng=$data;
        return $this;
    }

    /**
     * gets column lng type float(10,6)
     * @return      
     */
     
    public function getLng()
    {
        return $this->_Lng;
    }
    
    /**
     * sets column seq type int(11)     
     *
     * @param int $data
     * @return Default_Model_AreaPoint     
     *
     **/

    public function setSeq($data)
    {
        $this->_Seq=$data;
        return $this;
    }

    /**
     * gets column seq type int(11)
     * @return int     
     */
     
    public function getSeq()
    {
        return $this->_Seq;
    }
    
    /**
     * returns the mapper class
     *
     * @return Default_Model_AreaPointMapper
     *
     */

    public function getMapper()
    {
        if (null === $this->_mapper) {
            $this->setMapper(new Default_Model_AreaPointMapper());
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

