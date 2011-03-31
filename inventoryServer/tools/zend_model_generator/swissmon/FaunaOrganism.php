<?php
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */
 
class Default_Model_FaunaOrganism extends MainModel
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
     * mysql var type varchar(50)
     *
     * @var string     
     */
    protected $_Class;
    
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
    'class'=>'Class',
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
     * sets column id type int(11)     
     *
     * @param int $data
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * sets column class type varchar(50)     
     *
     * @param string $data
     * @return Default_Model_FaunaOrganism     
     *
     **/

    public function setClass($data)
    {
        $this->_Class=$data;
        return $this;
    }

    /**
     * gets column class type varchar(50)
     * @return string     
     */
     
    public function getClass()
    {
        return $this->_Class;
    }
    
    /**
     * sets column order type varchar(50)     
     *
     * @param string $data
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganism     
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
     * @return Default_Model_FaunaOrganismMapper
     *
     */

    public function getMapper()
    {
        if (null === $this->_mapper) {
            $this->setMapper(new Default_Model_FaunaOrganismMapper());
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

