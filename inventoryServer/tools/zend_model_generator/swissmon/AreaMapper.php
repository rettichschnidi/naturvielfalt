<?php

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Default_Model_AreaMapper {

    /**
     * $_dbTable - instance of Default_Model_DbTable_Area
     *
     * @var Default_Model_DbTable_Area     
     */
    protected $_dbTable;

    /**
     * finds a row where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Default_Model_Area $cls
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
		->setTypeId($row->type_id)
		->setFieldName($row->field_name)
		->setAltitude($row->altitude)
		->setComment($row->comment);
	    return $cls;
    }


    /**
     * returns an array, keys are the field names.
     *
     * @param new Default_Model_Area $cls
     * @return array
     *
     */
    public function toArray($cls) {
        $result = array(
        
            'id' => $cls->getId(),
            'type_id' => $cls->getTypeId(),
            'field_name' => $cls->getFieldName(),
            'altitude' => $cls->getAltitude(),
            'comment' => $cls->getComment(),
                    
        );
        return $result;
    }

    /**
     * finds rows where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Default_Model_Area $cls
     * @return array
     */
    public function findByField($field, $value, $cls)
    {
            $table = $this->getDbTable();
            $select = $table->select();
            $result = array();

            $rows = $table->fetchAll($select->where("{$field} = ?", $value));
            foreach ($rows as $row) {
                    $cls=new Default_Model_Area();
                    $result[]=$cls;
                    $cls->setId($row->id)
		->setTypeId($row->type_id)
		->setFieldName($row->field_name)
		->setAltitude($row->altitude)
		->setComment($row->comment);
            }
            return $result;
    }
    
    /**
     * sets the dbTable class
     *
     * @param Default_Model_DbTable_Area $dbTable
     * @return Default_Model_AreaMapper
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
     * @return Default_Model_DbTable_Area     
     */
    public function getDbTable()
    {
        if (null === $this->_dbTable) {
            $this->setDbTable('Default_Model_DbTable_Area');
        }
        return $this->_dbTable;
    }

    /**
     * saves current row
     *
     * @param Default_Model_Area $cls
     *
     */
     
    public function save(Default_Model_Area $cls,$ignoreEmptyValuesOnUpdate=true)
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
     * @param Default_Model_Area $cls
     */

    public function find($id, Default_Model_Area $cls)
    {
        $result = $this->getDbTable()->find($id);
        if (0 == count($result)) {
            return;
        }

        $row = $result->current();

        $cls->setId($row->id)
		->setTypeId($row->type_id)
		->setFieldName($row->field_name)
		->setAltitude($row->altitude)
		->setComment($row->comment);
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
            $entry = new Default_Model_Area();
            $entry->setId($row->id)
                  ->setTypeId($row->type_id)
                  ->setFieldName($row->field_name)
                  ->setAltitude($row->altitude)
                  ->setComment($row->comment)
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
                    $entry = new Default_Model_Area();
                    $entry->setId($row->id)
                          ->setTypeId($row->type_id)
                          ->setFieldName($row->field_name)
                          ->setAltitude($row->altitude)
                          ->setComment($row->comment)
                          ->setMapper($this);
                    $entries[] = $entry;
            }
            return $entries;
    }

}
