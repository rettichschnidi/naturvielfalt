<?php
require_once('FloraRedListMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */
 
class Application_Model_FloraRedList extends MainModel
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
    protected $_FloraOrganismId;
    
    /**
     * mysql var type int(11)
     *
     * @var int     
     */
    protected $_RedListNr;
    
    /**
     * mysql var type bit(1)
     *
     * @var bit     
     */
    protected $_IsBerneConvention;
    
    /**
     * mysql var type bit(1)
     *
     * @var bit     
     */
    protected $_IsIucn;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_Protection;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_NeophyteType;
    
    /**
     * mysql var type bit(1)
     *
     * @var bit     
     */
    protected $_IsInvasive;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_DiffIsfs;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListCh;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListAdvJu;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListJuTot;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListJu1;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListJu2;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListAdvMp;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListMpTot;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListMp1;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListMp2;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListAdvNa;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListNaTot;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListNa1;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListNa2;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListAdvWa;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListWaTot;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListAdvEa;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListEaTot;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListAdvSa;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListSaTot;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListSa1;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListSa2;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_RedListSa3;
    
    /**
     * mysql var type int(1)
     *
     * @var int     
     */
    protected $_EcologicalGrp;
    

    

function __construct() {
    $this->setColumnsList(array(
    'id'=>'Id',
    'flora_organism_id'=>'FloraOrganismId',
    'red_list_nr'=>'RedListNr',
    'is_berne_convention'=>'IsBerneConvention',
    'is_iucn'=>'IsIucn',
    'protection'=>'Protection',
    'neophyte_type'=>'NeophyteType',
    'is_invasive'=>'IsInvasive',
    'diff_isfs'=>'DiffIsfs',
    'red_list_ch'=>'RedListCh',
    'red_list_adv_ju'=>'RedListAdvJu',
    'red_list_ju_tot'=>'RedListJuTot',
    'red_list_ju1'=>'RedListJu1',
    'red_list_ju2'=>'RedListJu2',
    'red_list_adv_mp'=>'RedListAdvMp',
    'red_list_mp_tot'=>'RedListMpTot',
    'red_list_mp1'=>'RedListMp1',
    'red_list_mp2'=>'RedListMp2',
    'red_list_adv_na'=>'RedListAdvNa',
    'red_list_na_tot'=>'RedListNaTot',
    'red_list_na1'=>'RedListNa1',
    'red_list_na2'=>'RedListNa2',
    'red_list_adv_wa'=>'RedListAdvWa',
    'red_list_wa_tot'=>'RedListWaTot',
    'red_list_adv_ea'=>'RedListAdvEa',
    'red_list_ea_tot'=>'RedListEaTot',
    'red_list_adv_sa'=>'RedListAdvSa',
    'red_list_sa_tot'=>'RedListSaTot',
    'red_list_sa1'=>'RedListSa1',
    'red_list_sa2'=>'RedListSa2',
    'red_list_sa3'=>'RedListSa3',
    'ecological_grp'=>'EcologicalGrp',
    ));
}

	
    
    /**
     * sets column id type int(11)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
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
     * sets column flora_organism_id type int(11)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setFloraOrganismId($data)
    {
        $this->_FloraOrganismId=$data;
        return $this;
    }

    /**
     * gets column flora_organism_id type int(11)
     * @return int     
     */
     
    public function getFloraOrganismId()
    {
        return $this->_FloraOrganismId;
    }
    
    /**
     * sets column red_list_nr type int(11)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListNr($data)
    {
        $this->_RedListNr=$data;
        return $this;
    }

    /**
     * gets column red_list_nr type int(11)
     * @return int     
     */
     
    public function getRedListNr()
    {
        return $this->_RedListNr;
    }
    
    /**
     * sets column is_berne_convention type bit(1)     
     *
     * @param bit $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setIsBerneConvention($data)
    {
        $this->_IsBerneConvention=$data;
        return $this;
    }

    /**
     * gets column is_berne_convention type bit(1)
     * @return bit     
     */
     
    public function getIsBerneConvention()
    {
        return $this->_IsBerneConvention;
    }
    
    /**
     * sets column is_iucn type bit(1)     
     *
     * @param bit $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setIsIucn($data)
    {
        $this->_IsIucn=$data;
        return $this;
    }

    /**
     * gets column is_iucn type bit(1)
     * @return bit     
     */
     
    public function getIsIucn()
    {
        return $this->_IsIucn;
    }
    
    /**
     * sets column protection type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setProtection($data)
    {
        $this->_Protection=$data;
        return $this;
    }

    /**
     * gets column protection type int(1)
     * @return int     
     */
     
    public function getProtection()
    {
        return $this->_Protection;
    }
    
    /**
     * sets column neophyte_type type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setNeophyteType($data)
    {
        $this->_NeophyteType=$data;
        return $this;
    }

    /**
     * gets column neophyte_type type int(1)
     * @return int     
     */
     
    public function getNeophyteType()
    {
        return $this->_NeophyteType;
    }
    
    /**
     * sets column is_invasive type bit(1)     
     *
     * @param bit $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setIsInvasive($data)
    {
        $this->_IsInvasive=$data;
        return $this;
    }

    /**
     * gets column is_invasive type bit(1)
     * @return bit     
     */
     
    public function getIsInvasive()
    {
        return $this->_IsInvasive;
    }
    
    /**
     * sets column diff_isfs type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setDiffIsfs($data)
    {
        $this->_DiffIsfs=$data;
        return $this;
    }

    /**
     * gets column diff_isfs type int(1)
     * @return int     
     */
     
    public function getDiffIsfs()
    {
        return $this->_DiffIsfs;
    }
    
    /**
     * sets column red_list_ch type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListCh($data)
    {
        $this->_RedListCh=$data;
        return $this;
    }

    /**
     * gets column red_list_ch type int(1)
     * @return int     
     */
     
    public function getRedListCh()
    {
        return $this->_RedListCh;
    }
    
    /**
     * sets column red_list_adv_ju type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListAdvJu($data)
    {
        $this->_RedListAdvJu=$data;
        return $this;
    }

    /**
     * gets column red_list_adv_ju type int(1)
     * @return int     
     */
     
    public function getRedListAdvJu()
    {
        return $this->_RedListAdvJu;
    }
    
    /**
     * sets column red_list_ju_tot type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListJuTot($data)
    {
        $this->_RedListJuTot=$data;
        return $this;
    }

    /**
     * gets column red_list_ju_tot type int(1)
     * @return int     
     */
     
    public function getRedListJuTot()
    {
        return $this->_RedListJuTot;
    }
    
    /**
     * sets column red_list_ju1 type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListJu1($data)
    {
        $this->_RedListJu1=$data;
        return $this;
    }

    /**
     * gets column red_list_ju1 type int(1)
     * @return int     
     */
     
    public function getRedListJu1()
    {
        return $this->_RedListJu1;
    }
    
    /**
     * sets column red_list_ju2 type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListJu2($data)
    {
        $this->_RedListJu2=$data;
        return $this;
    }

    /**
     * gets column red_list_ju2 type int(1)
     * @return int     
     */
     
    public function getRedListJu2()
    {
        return $this->_RedListJu2;
    }
    
    /**
     * sets column red_list_adv_mp type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListAdvMp($data)
    {
        $this->_RedListAdvMp=$data;
        return $this;
    }

    /**
     * gets column red_list_adv_mp type int(1)
     * @return int     
     */
     
    public function getRedListAdvMp()
    {
        return $this->_RedListAdvMp;
    }
    
    /**
     * sets column red_list_mp_tot type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListMpTot($data)
    {
        $this->_RedListMpTot=$data;
        return $this;
    }

    /**
     * gets column red_list_mp_tot type int(1)
     * @return int     
     */
     
    public function getRedListMpTot()
    {
        return $this->_RedListMpTot;
    }
    
    /**
     * sets column red_list_mp1 type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListMp1($data)
    {
        $this->_RedListMp1=$data;
        return $this;
    }

    /**
     * gets column red_list_mp1 type int(1)
     * @return int     
     */
     
    public function getRedListMp1()
    {
        return $this->_RedListMp1;
    }
    
    /**
     * sets column red_list_mp2 type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListMp2($data)
    {
        $this->_RedListMp2=$data;
        return $this;
    }

    /**
     * gets column red_list_mp2 type int(1)
     * @return int     
     */
     
    public function getRedListMp2()
    {
        return $this->_RedListMp2;
    }
    
    /**
     * sets column red_list_adv_na type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListAdvNa($data)
    {
        $this->_RedListAdvNa=$data;
        return $this;
    }

    /**
     * gets column red_list_adv_na type int(1)
     * @return int     
     */
     
    public function getRedListAdvNa()
    {
        return $this->_RedListAdvNa;
    }
    
    /**
     * sets column red_list_na_tot type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListNaTot($data)
    {
        $this->_RedListNaTot=$data;
        return $this;
    }

    /**
     * gets column red_list_na_tot type int(1)
     * @return int     
     */
     
    public function getRedListNaTot()
    {
        return $this->_RedListNaTot;
    }
    
    /**
     * sets column red_list_na1 type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListNa1($data)
    {
        $this->_RedListNa1=$data;
        return $this;
    }

    /**
     * gets column red_list_na1 type int(1)
     * @return int     
     */
     
    public function getRedListNa1()
    {
        return $this->_RedListNa1;
    }
    
    /**
     * sets column red_list_na2 type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListNa2($data)
    {
        $this->_RedListNa2=$data;
        return $this;
    }

    /**
     * gets column red_list_na2 type int(1)
     * @return int     
     */
     
    public function getRedListNa2()
    {
        return $this->_RedListNa2;
    }
    
    /**
     * sets column red_list_adv_wa type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListAdvWa($data)
    {
        $this->_RedListAdvWa=$data;
        return $this;
    }

    /**
     * gets column red_list_adv_wa type int(1)
     * @return int     
     */
     
    public function getRedListAdvWa()
    {
        return $this->_RedListAdvWa;
    }
    
    /**
     * sets column red_list_wa_tot type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListWaTot($data)
    {
        $this->_RedListWaTot=$data;
        return $this;
    }

    /**
     * gets column red_list_wa_tot type int(1)
     * @return int     
     */
     
    public function getRedListWaTot()
    {
        return $this->_RedListWaTot;
    }
    
    /**
     * sets column red_list_adv_ea type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListAdvEa($data)
    {
        $this->_RedListAdvEa=$data;
        return $this;
    }

    /**
     * gets column red_list_adv_ea type int(1)
     * @return int     
     */
     
    public function getRedListAdvEa()
    {
        return $this->_RedListAdvEa;
    }
    
    /**
     * sets column red_list_ea_tot type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListEaTot($data)
    {
        $this->_RedListEaTot=$data;
        return $this;
    }

    /**
     * gets column red_list_ea_tot type int(1)
     * @return int     
     */
     
    public function getRedListEaTot()
    {
        return $this->_RedListEaTot;
    }
    
    /**
     * sets column red_list_adv_sa type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListAdvSa($data)
    {
        $this->_RedListAdvSa=$data;
        return $this;
    }

    /**
     * gets column red_list_adv_sa type int(1)
     * @return int     
     */
     
    public function getRedListAdvSa()
    {
        return $this->_RedListAdvSa;
    }
    
    /**
     * sets column red_list_sa_tot type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListSaTot($data)
    {
        $this->_RedListSaTot=$data;
        return $this;
    }

    /**
     * gets column red_list_sa_tot type int(1)
     * @return int     
     */
     
    public function getRedListSaTot()
    {
        return $this->_RedListSaTot;
    }
    
    /**
     * sets column red_list_sa1 type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListSa1($data)
    {
        $this->_RedListSa1=$data;
        return $this;
    }

    /**
     * gets column red_list_sa1 type int(1)
     * @return int     
     */
     
    public function getRedListSa1()
    {
        return $this->_RedListSa1;
    }
    
    /**
     * sets column red_list_sa2 type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListSa2($data)
    {
        $this->_RedListSa2=$data;
        return $this;
    }

    /**
     * gets column red_list_sa2 type int(1)
     * @return int     
     */
     
    public function getRedListSa2()
    {
        return $this->_RedListSa2;
    }
    
    /**
     * sets column red_list_sa3 type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setRedListSa3($data)
    {
        $this->_RedListSa3=$data;
        return $this;
    }

    /**
     * gets column red_list_sa3 type int(1)
     * @return int     
     */
     
    public function getRedListSa3()
    {
        return $this->_RedListSa3;
    }
    
    /**
     * sets column ecological_grp type int(1)     
     *
     * @param int $data
     * @return Application_Model_FloraRedList     
     *
     **/

    public function setEcologicalGrp($data)
    {
        $this->_EcologicalGrp=$data;
        return $this;
    }

    /**
     * gets column ecological_grp type int(1)
     * @return int     
     */
     
    public function getEcologicalGrp()
    {
        return $this->_EcologicalGrp;
    }
    
    /**
     * returns the mapper class
     *
     * @return Application_Model_FloraRedListMapper
     *
     */

    public function getMapper()
    {
        if (null === $this->_mapper) {
            $this->setMapper(new Application_Model_FloraRedListMapper());
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

