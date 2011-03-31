<?php
require_once('DbTable/FaunaOrganism.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_FaunaOrganismMapper {

    /**
     * $_dbTable - instance of Application_Model_DbTable_FaunaOrganism
     *
     * @var Application_Model_DbTable_FaunaOrganism     
     */
    protected $_dbTable;

    /**
     * finds a row where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Application_Model_FaunaOrganism $cls
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
		->setCscfNr($row->cscf_nr)
		->setFaunaClassId($row->fauna_class_id)
		->setOrder($row->order)
		->setFamily($row->family)
		->setGenus($row->genus)
		->setSubGenus($row->sub_genus)
		->setSpecies($row->species)
		->setSubSpecies($row->sub_species)
		->setAuthorYear($row->author_year)
		->setNameDe($row->name_de)
		->setNameFr($row->name_fr)
		->setNameIt($row->name_it)
		->setNameRo($row->name_ro)
		->setNameEn($row->name_en)
		->setProtectionCh($row->protection_ch);
	    return $cls;
    }


    /**
     * returns an array, keys are the field names.
     *
     * @param new Application_Model_FaunaOrganism $cls
     * @return array
     *
     */
    public function toArray($cls) {
        $result = array(
        
            'id' => $cls->getId(),
            'cscf_nr' => $cls->getCscfNr(),
            'fauna_class_id' => $cls->getFaunaClassId(),
            'order' => $cls->getOrder(),
            'family' => $cls->getFamily(),
            'genus' => $cls->getGenus(),
            'sub_genus' => $cls->getSubGenus(),
            'species' => $cls->getSpecies(),
            'sub_species' => $cls->getSubSpecies(),
            'author_year' => $cls->getAuthorYear(),
            'name_de' => $cls->getNameDe(),
            'name_fr' => $cls->getNameFr(),
            'name_it' => $cls->getNameIt(),
            'name_ro' => $cls->getNameRo(),
            'name_en' => $cls->getNameEn(),
            'protection_ch' => $cls->getProtectionCh(),
                    
        );
        return $result;
    }

    /**
     * finds rows where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Application_Model_FaunaOrganism $cls
     * @return array
     */
    public function findByField($field, $value, $cls)
    {
            $table = $this->getDbTable();
            $select = $table->select();
            $result = array();

            $rows = $table->fetchAll($select->where("{$field} = ?", $value));
            foreach ($rows as $row) {
                    $cls=new Application_Model_FaunaOrganism();
                    $result[]=$cls;
                    $cls->setId($row->id)
		->setCscfNr($row->cscf_nr)
		->setFaunaClassId($row->fauna_class_id)
		->setOrder($row->order)
		->setFamily($row->family)
		->setGenus($row->genus)
		->setSubGenus($row->sub_genus)
		->setSpecies($row->species)
		->setSubSpecies($row->sub_species)
		->setAuthorYear($row->author_year)
		->setNameDe($row->name_de)
		->setNameFr($row->name_fr)
		->setNameIt($row->name_it)
		->setNameRo($row->name_ro)
		->setNameEn($row->name_en)
		->setProtectionCh($row->protection_ch);
            }
            return $result;
    }
    
    /**
     * sets the dbTable class
     *
     * @param Application_Model_DbTable_FaunaOrganism $dbTable
     * @return Application_Model_FaunaOrganismMapper
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
     * @return Application_Model_DbTable_FaunaOrganism     
     */
    public function getDbTable()
    {
        if (null === $this->_dbTable) {
            $this->setDbTable('Application_Model_DbTable_FaunaOrganism');
        }
        return $this->_dbTable;
    }

    /**
     * saves current row
     *
     * @param Application_Model_FaunaOrganism $cls
     *
     */
     
    public function save(Application_Model_FaunaOrganism $cls,$ignoreEmptyValuesOnUpdate=true)
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
     * @param Application_Model_FaunaOrganism $cls
     */

    public function find($id, Application_Model_FaunaOrganism $cls)
    {
        $result = $this->getDbTable()->find($id);
        if (0 == count($result)) {
            return;
        }

        $row = $result->current();

        $cls->setId($row->id)
		->setCscfNr($row->cscf_nr)
		->setFaunaClassId($row->fauna_class_id)
		->setOrder($row->order)
		->setFamily($row->family)
		->setGenus($row->genus)
		->setSubGenus($row->sub_genus)
		->setSpecies($row->species)
		->setSubSpecies($row->sub_species)
		->setAuthorYear($row->author_year)
		->setNameDe($row->name_de)
		->setNameFr($row->name_fr)
		->setNameIt($row->name_it)
		->setNameRo($row->name_ro)
		->setNameEn($row->name_en)
		->setProtectionCh($row->protection_ch);
        return $row;
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
            $entry = new Application_Model_FaunaOrganism();
            $entry->setId($row->id)
                  ->setCscfNr($row->cscf_nr)
                  ->setFaunaClassId($row->fauna_class_id)
                  ->setOrder($row->order)
                  ->setFamily($row->family)
                  ->setGenus($row->genus)
                  ->setSubGenus($row->sub_genus)
                  ->setSpecies($row->species)
                  ->setSubSpecies($row->sub_species)
                  ->setAuthorYear($row->author_year)
                  ->setNameDe($row->name_de)
                  ->setNameFr($row->name_fr)
                  ->setNameIt($row->name_it)
                  ->setNameRo($row->name_ro)
                  ->setNameEn($row->name_en)
                  ->setProtectionCh($row->protection_ch)
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
                    $entry = new Application_Model_FaunaOrganism();
                    $entry->setId($row->id)
                          ->setCscfNr($row->cscf_nr)
                          ->setFaunaClassId($row->fauna_class_id)
                          ->setOrder($row->order)
                          ->setFamily($row->family)
                          ->setGenus($row->genus)
                          ->setSubGenus($row->sub_genus)
                          ->setSpecies($row->species)
                          ->setSubSpecies($row->sub_species)
                          ->setAuthorYear($row->author_year)
                          ->setNameDe($row->name_de)
                          ->setNameFr($row->name_fr)
                          ->setNameIt($row->name_it)
                          ->setNameRo($row->name_ro)
                          ->setNameEn($row->name_en)
                          ->setProtectionCh($row->protection_ch)
                          ->setMapper($this);
                    $entries[] = $entry;
            }
            return $entries;
    }

}
