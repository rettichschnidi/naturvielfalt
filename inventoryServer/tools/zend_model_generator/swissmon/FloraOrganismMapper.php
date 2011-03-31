<?php

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Default_Model_FloraOrganismMapper {

    /**
     * $_dbTable - instance of Default_Model_DbTable_FloraOrganism
     *
     * @var Default_Model_DbTable_FloraOrganism     
     */
    protected $_dbTable;

    /**
     * finds a row where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Default_Model_FloraOrganism $cls
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
		->setSisfNr($row->sisf_nr)
		->setStatus($row->status)
		->setName($row->name)
		->setFamilie($row->Familie)
		->setGattung($row->Gattung)
		->setArt($row->Art)
		->setAutor($row->Autor)
		->setRang($row->Rang)
		->setNameUnterart($row->NameUnterart)
		->setAutorUnterart($row->AutorUnterart)
		->setGueltigeNamen($row->GueltigeNamen)
		->setOfficialFloraOrfganismId($row->official_flora_orfganism_id)
		->setNameDe($row->name_de)
		->setNameFr($row->name_fr)
		->setNameIt($row->name_it)
		->setNameReference($row->name_reference)
		->setIsNeophyte($row->is_neophyte);
	    return $cls;
    }


    /**
     * returns an array, keys are the field names.
     *
     * @param new Default_Model_FloraOrganism $cls
     * @return array
     *
     */
    public function toArray($cls) {
        $result = array(
        
            'id' => $cls->getId(),
            'sisf_nr' => $cls->getSisfNr(),
            'status' => $cls->getStatus(),
            'name' => $cls->getName(),
            'Familie' => $cls->getFamilie(),
            'Gattung' => $cls->getGattung(),
            'Art' => $cls->getArt(),
            'Autor' => $cls->getAutor(),
            'Rang' => $cls->getRang(),
            'NameUnterart' => $cls->getNameUnterart(),
            'AutorUnterart' => $cls->getAutorUnterart(),
            'GueltigeNamen' => $cls->getGueltigeNamen(),
            'official_flora_orfganism_id' => $cls->getOfficialFloraOrfganismId(),
            'name_de' => $cls->getNameDe(),
            'name_fr' => $cls->getNameFr(),
            'name_it' => $cls->getNameIt(),
            'name_reference' => $cls->getNameReference(),
            'is_neophyte' => $cls->getIsNeophyte(),
                    
        );
        return $result;
    }

    /**
     * finds rows where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Default_Model_FloraOrganism $cls
     * @return array
     */
    public function findByField($field, $value, $cls)
    {
            $table = $this->getDbTable();
            $select = $table->select();
            $result = array();

            $rows = $table->fetchAll($select->where("{$field} = ?", $value));
            foreach ($rows as $row) {
                    $cls=new Default_Model_FloraOrganism();
                    $result[]=$cls;
                    $cls->setId($row->id)
		->setSisfNr($row->sisf_nr)
		->setStatus($row->status)
		->setName($row->name)
		->setFamilie($row->Familie)
		->setGattung($row->Gattung)
		->setArt($row->Art)
		->setAutor($row->Autor)
		->setRang($row->Rang)
		->setNameUnterart($row->NameUnterart)
		->setAutorUnterart($row->AutorUnterart)
		->setGueltigeNamen($row->GueltigeNamen)
		->setOfficialFloraOrfganismId($row->official_flora_orfganism_id)
		->setNameDe($row->name_de)
		->setNameFr($row->name_fr)
		->setNameIt($row->name_it)
		->setNameReference($row->name_reference)
		->setIsNeophyte($row->is_neophyte);
            }
            return $result;
    }
    
    /**
     * sets the dbTable class
     *
     * @param Default_Model_DbTable_FloraOrganism $dbTable
     * @return Default_Model_FloraOrganismMapper
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
     * @return Default_Model_DbTable_FloraOrganism     
     */
    public function getDbTable()
    {
        if (null === $this->_dbTable) {
            $this->setDbTable('Default_Model_DbTable_FloraOrganism');
        }
        return $this->_dbTable;
    }

    /**
     * saves current row
     *
     * @param Default_Model_FloraOrganism $cls
     *
     */
     
    public function save(Default_Model_FloraOrganism $cls,$ignoreEmptyValuesOnUpdate=true)
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
     * @param Default_Model_FloraOrganism $cls
     */

    public function find($id, Default_Model_FloraOrganism $cls)
    {
        $result = $this->getDbTable()->find($id);
        if (0 == count($result)) {
            return;
        }

        $row = $result->current();

        $cls->setId($row->id)
		->setSisfNr($row->sisf_nr)
		->setStatus($row->status)
		->setName($row->name)
		->setFamilie($row->Familie)
		->setGattung($row->Gattung)
		->setArt($row->Art)
		->setAutor($row->Autor)
		->setRang($row->Rang)
		->setNameUnterart($row->NameUnterart)
		->setAutorUnterart($row->AutorUnterart)
		->setGueltigeNamen($row->GueltigeNamen)
		->setOfficialFloraOrfganismId($row->official_flora_orfganism_id)
		->setNameDe($row->name_de)
		->setNameFr($row->name_fr)
		->setNameIt($row->name_it)
		->setNameReference($row->name_reference)
		->setIsNeophyte($row->is_neophyte);
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
            $entry = new Default_Model_FloraOrganism();
            $entry->setId($row->id)
                  ->setSisfNr($row->sisf_nr)
                  ->setStatus($row->status)
                  ->setName($row->name)
                  ->setFamilie($row->Familie)
                  ->setGattung($row->Gattung)
                  ->setArt($row->Art)
                  ->setAutor($row->Autor)
                  ->setRang($row->Rang)
                  ->setNameUnterart($row->NameUnterart)
                  ->setAutorUnterart($row->AutorUnterart)
                  ->setGueltigeNamen($row->GueltigeNamen)
                  ->setOfficialFloraOrfganismId($row->official_flora_orfganism_id)
                  ->setNameDe($row->name_de)
                  ->setNameFr($row->name_fr)
                  ->setNameIt($row->name_it)
                  ->setNameReference($row->name_reference)
                  ->setIsNeophyte($row->is_neophyte)
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
                    $entry = new Default_Model_FloraOrganism();
                    $entry->setId($row->id)
                          ->setSisfNr($row->sisf_nr)
                          ->setStatus($row->status)
                          ->setName($row->name)
                          ->setFamilie($row->Familie)
                          ->setGattung($row->Gattung)
                          ->setArt($row->Art)
                          ->setAutor($row->Autor)
                          ->setRang($row->Rang)
                          ->setNameUnterart($row->NameUnterart)
                          ->setAutorUnterart($row->AutorUnterart)
                          ->setGueltigeNamen($row->GueltigeNamen)
                          ->setOfficialFloraOrfganismId($row->official_flora_orfganism_id)
                          ->setNameDe($row->name_de)
                          ->setNameFr($row->name_fr)
                          ->setNameIt($row->name_it)
                          ->setNameReference($row->name_reference)
                          ->setIsNeophyte($row->is_neophyte)
                          ->setMapper($this);
                    $entries[] = $entry;
            }
            return $entries;
    }

}
