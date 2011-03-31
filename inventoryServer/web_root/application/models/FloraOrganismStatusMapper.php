<?php
require_once('DbTable/FloraOrganismStatus.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_FloraOrganismStatusMapper {

    /**
     * $_dbTable - instance of Application_Model_DbTable_FloraOrganismStatus
     *
     * @var Application_Model_DbTable_FloraOrganismStatus     
     */
    protected $_dbTable;

    /**
     * finds a row where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Application_Model_FloraOrganismStatus $cls
     */     
    public function findOneByField($field, $value, $cls)
    {
            $table = $this->getDbTable();
            $select = $table->select();

            $row = $table->fetchRow($select->where("{$field} = ?", $value));
            if (0 == count($row)) {
                    return;
            }

            $cls->setStatus($row->status)
		->setDesc($row->desc);
	    return $cls;
    }


    /**
     * returns an array, keys are the field names.
     *
     * @param new Application_Model_FloraOrganismStatus $cls
     * @return array
     *
     */
    public function toArray($cls) {
        $result = array(
        
            'status' => $cls->getStatus(),
            'desc' => $cls->getDesc(),
                    
        );
        return $result;
    }

    /**
     * finds rows where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Application_Model_FloraOrganismStatus $cls
     * @return array
     */
    public function findByField($field, $value, $cls)
    {
            $table = $this->getDbTable();
            $select = $table->select();
            $result = array();

            $rows = $table->fetchAll($select->where("{$field} = ?", $value));
            foreach ($rows as $row) {
                    $cls=new Application_Model_FloraOrganismStatus();
                    $result[]=$cls;
                    $cls->setStatus($row->status)
		->setDesc($row->desc);
            }
            return $result;
    }
    
    /**
     * sets the dbTable class
     *
     * @param Application_Model_DbTable_FloraOrganismStatus $dbTable
     * @return Application_Model_FloraOrganismStatusMapper
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
     * @return Application_Model_DbTable_FloraOrganismStatus     
     */
    public function getDbTable()
    {
        if (null === $this->_dbTable) {
            $this->setDbTable('Application_Model_DbTable_FloraOrganismStatus');
        }
        return $this->_dbTable;
    }

    /**
     * saves current row
     *
     * @param Application_Model_FloraOrganismStatus $cls
     *
     */
     
    public function save(Application_Model_FloraOrganismStatus $cls,$ignoreEmptyValuesOnUpdate=true)
    {
        if ($ignoreEmptyValuesOnUpdate) {
            $data = $cls->toArray();
            foreach ($data as $key=>$value) {
                if (is_null($value) or $value == '')
                    unset($data[$key]);
            }
        }

        if (null === ($id = $cls->getStatus())) {
            unset($data['status']);
            $id=$this->getDbTable()->insert($data);
            $cls->setStatus($id);
        } else {
            if ($ignoreEmptyValuesOnUpdate) {
             $data = $cls->toArray();
             foreach ($data as $key=>$value) {
                if (is_null($value) or $value == '')
                    unset($data[$key]);
                }
            }

            $this->getDbTable()->update($data, array('status = ?' => $id));
        }
    }

    /**
     * finds row by primary key
     *
     * @param string $id
     * @param Application_Model_FloraOrganismStatus $cls
     */

    public function find($id, Application_Model_FloraOrganismStatus $cls)
    {
        $result = $this->getDbTable()->find($id);
        if (0 == count($result)) {
            return;
        }

        $row = $result->current();

        $cls->setStatus($row->status)
		->setDesc($row->desc);
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
            $entry = new Application_Model_FloraOrganismStatus();
            $entry->setStatus($row->status)
                  ->setDesc($row->desc)
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
                    $entry = new Application_Model_FloraOrganismStatus();
                    $entry->setStatus($row->status)
                          ->setDesc($row->desc)
                          ->setMapper($this);
                    $entries[] = $entry;
            }
            return $entries;
    }

}
