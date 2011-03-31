<?php
require_once('DbTable/InventoryTypeAttributeDropdownValue.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_InventoryTypeAttributeDropdownValueMapper {

    /**
     * $_dbTable - instance of Application_Model_DbTable_InventoryTypeAttributeDropdownValue
     *
     * @var Application_Model_DbTable_InventoryTypeAttributeDropdownValue     
     */
    protected $_dbTable;

    /**
     * finds a row where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Application_Model_InventoryTypeAttributeDropdownValue $cls
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
		->setInventoryTypeAttributeId($row->inventory_type_attribute_id)
		->setValue($row->value);
	    return $cls;
    }


    /**
     * returns an array, keys are the field names.
     *
     * @param new Application_Model_InventoryTypeAttributeDropdownValue $cls
     * @return array
     *
     */
    public function toArray($cls) {
        $result = array(
        
            'id' => $cls->getId(),
            'inventory_type_attribute_id' => $cls->getInventoryTypeAttributeId(),
            'value' => $cls->getValue(),
                    
        );
        return $result;
    }

    /**
     * finds rows where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Application_Model_InventoryTypeAttributeDropdownValue $cls
     * @return array
     */
    public function findByField($field, $value, $cls)
    {
            $table = $this->getDbTable();
            $select = $table->select();
            $result = array();

            $rows = $table->fetchAll($select->where("{$field} = ?", $value));
            foreach ($rows as $row) {
                    $cls=new Application_Model_InventoryTypeAttributeDropdownValue();
                    $result[]=$cls;
                    $cls->setId($row->id)
		->setInventoryTypeAttributeId($row->inventory_type_attribute_id)
		->setValue($row->value);
            }
            return $result;
    }
    
    /**
     * sets the dbTable class
     *
     * @param Application_Model_DbTable_InventoryTypeAttributeDropdownValue $dbTable
     * @return Application_Model_InventoryTypeAttributeDropdownValueMapper
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
     * @return Application_Model_DbTable_InventoryTypeAttributeDropdownValue     
     */
    public function getDbTable()
    {
        if (null === $this->_dbTable) {
            $this->setDbTable('Application_Model_DbTable_InventoryTypeAttributeDropdownValue');
        }
        return $this->_dbTable;
    }

    /**
     * saves current row
     *
     * @param Application_Model_InventoryTypeAttributeDropdownValue $cls
     *
     */
     
    public function save(Application_Model_InventoryTypeAttributeDropdownValue $cls,$ignoreEmptyValuesOnUpdate=true)
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
     * @param Application_Model_InventoryTypeAttributeDropdownValue $cls
     */

    public function find($id, Application_Model_InventoryTypeAttributeDropdownValue $cls)
    {
        $result = $this->getDbTable()->find($id);
        if (0 == count($result)) {
            return;
        }

        $row = $result->current();

        $cls->setId($row->id)
		->setInventoryTypeAttributeId($row->inventory_type_attribute_id)
		->setValue($row->value);
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
            $entry = new Application_Model_InventoryTypeAttributeDropdownValue();
            $entry->setId($row->id)
                  ->setInventoryTypeAttributeId($row->inventory_type_attribute_id)
                  ->setValue($row->value)
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
                    $entry = new Application_Model_InventoryTypeAttributeDropdownValue();
                    $entry->setId($row->id)
                          ->setInventoryTypeAttributeId($row->inventory_type_attribute_id)
                          ->setValue($row->value)
                          ->setMapper($this);
                    $entries[] = $entry;
            }
            return $entries;
    }

}
