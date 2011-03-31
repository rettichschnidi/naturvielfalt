<?php

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Default_Model_FloraRedListMapper {

    /**
     * $_dbTable - instance of Default_Model_DbTable_FloraRedList
     *
     * @var Default_Model_DbTable_FloraRedList     
     */
    protected $_dbTable;

    /**
     * finds a row where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Default_Model_FloraRedList $cls
     */     
    public function findOneByField($field, $value, $cls)
    {
            $table = $this->getDbTable();
            $select = $table->select();

            $row = $table->fetchRow($select->where("{$field} = ?", $value));
            if (0 == count($row)) {
                    return;
            }

            $cls->setId($row->id)
		->setFloraOrganismId($row->flora_organism_id)
		->setRedListNr($row->red_list_nr)
		->setIsBerneConvention($row->is_berne_convention)
		->setIsIucn($row->is_iucn)
		->setProtection($row->protection)
		->setNeophyteType($row->neophyte_type)
		->setIsInvasive($row->is_invasive)
		->setDiffIsfs($row->diff_isfs)
		->setRedListCh($row->red_list_ch)
		->setRedListAdvJu($row->red_list_adv_ju)
		->setRedListJuTot($row->red_list_ju_tot)
		->setRedListJu1($row->red_list_ju1)
		->setRedListJu2($row->red_list_ju2)
		->setRedListAdvMp($row->red_list_adv_mp)
		->setRedListMpTot($row->red_list_mp_tot)
		->setRedListMp1($row->red_list_mp1)
		->setRedListMp2($row->red_list_mp2)
		->setRedListAdvNa($row->red_list_adv_na)
		->setRedListNaTot($row->red_list_na_tot)
		->setRedListNa1($row->red_list_na1)
		->setRedListNa2($row->red_list_na2)
		->setRedListAdvWa($row->red_list_adv_wa)
		->setRedListWaTot($row->red_list_wa_tot)
		->setRedListAdvEa($row->red_list_adv_ea)
		->setRedListEaTot($row->red_list_ea_tot)
		->setRedListAdvSa($row->red_list_adv_sa)
		->setRedListSaTot($row->red_list_sa_tot)
		->setRedListSa1($row->red_list_sa1)
		->setRedListSa2($row->red_list_sa2)
		->setRedListSa3($row->red_list_sa3)
		->setEcologicalGrp($row->ecological_grp);
	    return $cls;
    }


    /**
     * returns an array, keys are the field names.
     *
     * @param new Default_Model_FloraRedList $cls
     * @return array
     *
     */
    public function toArray($cls) {
        $result = array(
        
            'id' => $cls->getId(),
            'flora_organism_id' => $cls->getFloraOrganismId(),
            'red_list_nr' => $cls->getRedListNr(),
            'is_berne_convention' => $cls->getIsBerneConvention(),
            'is_iucn' => $cls->getIsIucn(),
            'protection' => $cls->getProtection(),
            'neophyte_type' => $cls->getNeophyteType(),
            'is_invasive' => $cls->getIsInvasive(),
            'diff_isfs' => $cls->getDiffIsfs(),
            'red_list_ch' => $cls->getRedListCh(),
            'red_list_adv_ju' => $cls->getRedListAdvJu(),
            'red_list_ju_tot' => $cls->getRedListJuTot(),
            'red_list_ju1' => $cls->getRedListJu1(),
            'red_list_ju2' => $cls->getRedListJu2(),
            'red_list_adv_mp' => $cls->getRedListAdvMp(),
            'red_list_mp_tot' => $cls->getRedListMpTot(),
            'red_list_mp1' => $cls->getRedListMp1(),
            'red_list_mp2' => $cls->getRedListMp2(),
            'red_list_adv_na' => $cls->getRedListAdvNa(),
            'red_list_na_tot' => $cls->getRedListNaTot(),
            'red_list_na1' => $cls->getRedListNa1(),
            'red_list_na2' => $cls->getRedListNa2(),
            'red_list_adv_wa' => $cls->getRedListAdvWa(),
            'red_list_wa_tot' => $cls->getRedListWaTot(),
            'red_list_adv_ea' => $cls->getRedListAdvEa(),
            'red_list_ea_tot' => $cls->getRedListEaTot(),
            'red_list_adv_sa' => $cls->getRedListAdvSa(),
            'red_list_sa_tot' => $cls->getRedListSaTot(),
            'red_list_sa1' => $cls->getRedListSa1(),
            'red_list_sa2' => $cls->getRedListSa2(),
            'red_list_sa3' => $cls->getRedListSa3(),
            'ecological_grp' => $cls->getEcologicalGrp(),
                    
        );
        return $result;
    }

    /**
     * finds rows where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Default_Model_FloraRedList $cls
     * @return array
     */
    public function findByField($field, $value, $cls)
    {
            $table = $this->getDbTable();
            $select = $table->select();
            $result = array();

            $rows = $table->fetchAll($select->where("{$field} = ?", $value));
            foreach ($rows as $row) {
                    $cls=new Default_Model_FloraRedList();
                    $result[]=$cls;
                    $cls->setId($row->id)
		->setFloraOrganismId($row->flora_organism_id)
		->setRedListNr($row->red_list_nr)
		->setIsBerneConvention($row->is_berne_convention)
		->setIsIucn($row->is_iucn)
		->setProtection($row->protection)
		->setNeophyteType($row->neophyte_type)
		->setIsInvasive($row->is_invasive)
		->setDiffIsfs($row->diff_isfs)
		->setRedListCh($row->red_list_ch)
		->setRedListAdvJu($row->red_list_adv_ju)
		->setRedListJuTot($row->red_list_ju_tot)
		->setRedListJu1($row->red_list_ju1)
		->setRedListJu2($row->red_list_ju2)
		->setRedListAdvMp($row->red_list_adv_mp)
		->setRedListMpTot($row->red_list_mp_tot)
		->setRedListMp1($row->red_list_mp1)
		->setRedListMp2($row->red_list_mp2)
		->setRedListAdvNa($row->red_list_adv_na)
		->setRedListNaTot($row->red_list_na_tot)
		->setRedListNa1($row->red_list_na1)
		->setRedListNa2($row->red_list_na2)
		->setRedListAdvWa($row->red_list_adv_wa)
		->setRedListWaTot($row->red_list_wa_tot)
		->setRedListAdvEa($row->red_list_adv_ea)
		->setRedListEaTot($row->red_list_ea_tot)
		->setRedListAdvSa($row->red_list_adv_sa)
		->setRedListSaTot($row->red_list_sa_tot)
		->setRedListSa1($row->red_list_sa1)
		->setRedListSa2($row->red_list_sa2)
		->setRedListSa3($row->red_list_sa3)
		->setEcologicalGrp($row->ecological_grp);
            }
            return $result;
    }
    
    /**
     * sets the dbTable class
     *
     * @param Default_Model_DbTable_FloraRedList $dbTable
     * @return Default_Model_FloraRedListMapper
     * 
     */
    public function setDbTable($dbTable)
    {
        if (is_string($dbTable)) {
            $dbTable = new $dbTable();
        }
        if (!$dbTable instanceof Zend_Db_Table_Abstract) {
            throw new Exception('Invalid table data gateway provided');
        }
        $this->_dbTable = $dbTable;
        return $this;
    }

    /**
     * returns the dbTable class
     * 
     * @return Default_Model_DbTable_FloraRedList     
     */
    public function getDbTable()
    {
        if (null === $this->_dbTable) {
            $this->setDbTable('Default_Model_DbTable_FloraRedList');
        }
        return $this->_dbTable;
    }

    /**
     * saves current row
     *
     * @param Default_Model_FloraRedList $cls
     *
     */
     
    public function save(Default_Model_FloraRedList $cls,$ignoreEmptyValuesOnUpdate=true)
    {
        if ($ignoreEmptyValuesOnUpdate) {
            $data = $cls->toArray();
            foreach ($data as $key=>$value) {
                if (is_null($value) or $value == '')
                    unset($data[$key]);
            }
        }

        if (null === ($id = $cls->getId())) {
            unset($data['id']);
            $id=$this->getDbTable()->insert($data);
            $cls->setId($id);
        } else {
            if ($ignoreEmptyValuesOnUpdate) {
             $data = $cls->toArray();
             foreach ($data as $key=>$value) {
                if (is_null($value) or $value == '')
                    unset($data[$key]);
                }
            }

            $this->getDbTable()->update($data, array('id = ?' => $id));
        }
    }

    /**
     * finds row by primary key
     *
     * @param int $id
     * @param Default_Model_FloraRedList $cls
     */

    public function find($id, Default_Model_FloraRedList $cls)
    {
        $result = $this->getDbTable()->find($id);
        if (0 == count($result)) {
            return;
        }

        $row = $result->current();

        $cls->setId($row->id)
		->setFloraOrganismId($row->flora_organism_id)
		->setRedListNr($row->red_list_nr)
		->setIsBerneConvention($row->is_berne_convention)
		->setIsIucn($row->is_iucn)
		->setProtection($row->protection)
		->setNeophyteType($row->neophyte_type)
		->setIsInvasive($row->is_invasive)
		->setDiffIsfs($row->diff_isfs)
		->setRedListCh($row->red_list_ch)
		->setRedListAdvJu($row->red_list_adv_ju)
		->setRedListJuTot($row->red_list_ju_tot)
		->setRedListJu1($row->red_list_ju1)
		->setRedListJu2($row->red_list_ju2)
		->setRedListAdvMp($row->red_list_adv_mp)
		->setRedListMpTot($row->red_list_mp_tot)
		->setRedListMp1($row->red_list_mp1)
		->setRedListMp2($row->red_list_mp2)
		->setRedListAdvNa($row->red_list_adv_na)
		->setRedListNaTot($row->red_list_na_tot)
		->setRedListNa1($row->red_list_na1)
		->setRedListNa2($row->red_list_na2)
		->setRedListAdvWa($row->red_list_adv_wa)
		->setRedListWaTot($row->red_list_wa_tot)
		->setRedListAdvEa($row->red_list_adv_ea)
		->setRedListEaTot($row->red_list_ea_tot)
		->setRedListAdvSa($row->red_list_adv_sa)
		->setRedListSaTot($row->red_list_sa_tot)
		->setRedListSa1($row->red_list_sa1)
		->setRedListSa2($row->red_list_sa2)
		->setRedListSa3($row->red_list_sa3)
		->setEcologicalGrp($row->ecological_grp);
    }

    /**
     * fetches all rows 
     *
     * @return array
     */
    public function fetchAll()
    {
        $resultSet = $this->getDbTable()->fetchAll();
        $entries   = array();
        foreach ($resultSet as $row) {
            $entry = new Default_Model_FloraRedList();
            $entry->setId($row->id)
                  ->setFloraOrganismId($row->flora_organism_id)
                  ->setRedListNr($row->red_list_nr)
                  ->setIsBerneConvention($row->is_berne_convention)
                  ->setIsIucn($row->is_iucn)
                  ->setProtection($row->protection)
                  ->setNeophyteType($row->neophyte_type)
                  ->setIsInvasive($row->is_invasive)
                  ->setDiffIsfs($row->diff_isfs)
                  ->setRedListCh($row->red_list_ch)
                  ->setRedListAdvJu($row->red_list_adv_ju)
                  ->setRedListJuTot($row->red_list_ju_tot)
                  ->setRedListJu1($row->red_list_ju1)
                  ->setRedListJu2($row->red_list_ju2)
                  ->setRedListAdvMp($row->red_list_adv_mp)
                  ->setRedListMpTot($row->red_list_mp_tot)
                  ->setRedListMp1($row->red_list_mp1)
                  ->setRedListMp2($row->red_list_mp2)
                  ->setRedListAdvNa($row->red_list_adv_na)
                  ->setRedListNaTot($row->red_list_na_tot)
                  ->setRedListNa1($row->red_list_na1)
                  ->setRedListNa2($row->red_list_na2)
                  ->setRedListAdvWa($row->red_list_adv_wa)
                  ->setRedListWaTot($row->red_list_wa_tot)
                  ->setRedListAdvEa($row->red_list_adv_ea)
                  ->setRedListEaTot($row->red_list_ea_tot)
                  ->setRedListAdvSa($row->red_list_adv_sa)
                  ->setRedListSaTot($row->red_list_sa_tot)
                  ->setRedListSa1($row->red_list_sa1)
                  ->setRedListSa2($row->red_list_sa2)
                  ->setRedListSa3($row->red_list_sa3)
                  ->setEcologicalGrp($row->ecological_grp)
                              ->setMapper($this);
            $entries[] = $entry;
        }
        return $entries;
    }

    /**
     * fetches all rows optionally filtered by where,order,count and offset
     * 
     * @param string $where
     * @param string $order
     * @param int $count
     * @param int $offset 
     *
     */
    public function fetchList($where=null, $order=null, $count=null, $offset=null)
    {
            $resultSet = $this->getDbTable()->fetchAll($where, $order, $count, $offset);
            $entries   = array();
            foreach ($resultSet as $row)
            {
                    $entry = new Default_Model_FloraRedList();
                    $entry->setId($row->id)
                          ->setFloraOrganismId($row->flora_organism_id)
                          ->setRedListNr($row->red_list_nr)
                          ->setIsBerneConvention($row->is_berne_convention)
                          ->setIsIucn($row->is_iucn)
                          ->setProtection($row->protection)
                          ->setNeophyteType($row->neophyte_type)
                          ->setIsInvasive($row->is_invasive)
                          ->setDiffIsfs($row->diff_isfs)
                          ->setRedListCh($row->red_list_ch)
                          ->setRedListAdvJu($row->red_list_adv_ju)
                          ->setRedListJuTot($row->red_list_ju_tot)
                          ->setRedListJu1($row->red_list_ju1)
                          ->setRedListJu2($row->red_list_ju2)
                          ->setRedListAdvMp($row->red_list_adv_mp)
                          ->setRedListMpTot($row->red_list_mp_tot)
                          ->setRedListMp1($row->red_list_mp1)
                          ->setRedListMp2($row->red_list_mp2)
                          ->setRedListAdvNa($row->red_list_adv_na)
                          ->setRedListNaTot($row->red_list_na_tot)
                          ->setRedListNa1($row->red_list_na1)
                          ->setRedListNa2($row->red_list_na2)
                          ->setRedListAdvWa($row->red_list_adv_wa)
                          ->setRedListWaTot($row->red_list_wa_tot)
                          ->setRedListAdvEa($row->red_list_adv_ea)
                          ->setRedListEaTot($row->red_list_ea_tot)
                          ->setRedListAdvSa($row->red_list_adv_sa)
                          ->setRedListSaTot($row->red_list_sa_tot)
                          ->setRedListSa1($row->red_list_sa1)
                          ->setRedListSa2($row->red_list_sa2)
                          ->setRedListSa3($row->red_list_sa3)
                          ->setEcologicalGrp($row->ecological_grp)
                          ->setMapper($this);
                    $entries[] = $entry;
            }
            return $entries;
    }

}
