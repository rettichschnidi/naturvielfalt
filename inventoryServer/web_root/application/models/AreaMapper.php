<?php
require_once('DbTable/Area.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_AreaMapper {

    /**
     * $_dbTable - instance of Application_Model_DbTable_Area
     *
     * @var Application_Model_DbTable_Area     
     */
    protected $_dbTable;

    /**
     * finds a row where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Application_Model_Area $cls
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
              ->setSurfaceArea($row->surface_area)
              ->setLocality($row->locality)
              ->setZip($row->zip)
              ->setTownship($row->township)
              ->setCanton($row->canton)
              ->setCountry($row->country)
              ->setCentroidLat($row->centroid_lat)
              ->setCentroidLng($row->centroid_lng)
              ->setComment($row->comment);
	    return $cls;
    }


    /**
     * returns an array, keys are the field names.
     *
     * @param new Application_Model_Area $cls
     * @return array
     *
     */
    public function toArray($cls) {
        $result = array(
        
            'id' => $cls->getId(),
            'type_id' => $cls->getTypeId(),
            'field_name' => $cls->getFieldName(),
            'altitude' => $cls->getAltitude(),
            'surface_area' => $cls->getSurfaceArea(),
            'locality' => $cls->getLocality(),
            'zip' => $cls->getZip(),
            'township' => $cls->getTownship(),
            'canton' => $cls->getCanton(),
            'country' => $cls->getCountry(),
            'centroid_lat' => $cls->getCentroidLat(),
            'centroid_lng' => $cls->getCentroidLng(),
            'comment' => $cls->getComment(),
                    
        );
        return $result;
    }

    public function toArrayDeep($cls){
      $row = $this->find($cls->getId(), new Application_Model_Area());
      $area_point_rows = $row->findDependentRowset('Application_Model_DbTable_AreaPoint');
      $habitat_rows = $row->findDependentRowset('Application_Model_DbTable_AreaHabitat');
      echo '<pre>'.print_r($row->toArray(), true).'</pre>';
      echo '<pre>'.print_r($area_point_rows->toArray(), true).'</pre>';
      echo '<pre>'.print_r($habitat_rows->toArray(), true).'</pre>';
      foreach($habitat_rows as $hab_row){
        echo '<pre>'.print_r($hab_row->findParentRow('Application_Model_DbTable_Habitat')->toArray(), true).'</pre>';
      }
    }
    /**
     * finds rows where $field equals $value
     *
     * @param string $field
     * @param mixed $value
     * @param Application_Model_Area $cls
     * @return array
     */
    public function findByField($field, $value, $cls)
    {
            $table = $this->getDbTable();
            $select = $table->select();
            $result = array();

            $rows = $table->fetchAll($select->where("{$field} = ?", $value));
            foreach ($rows as $row) {
                    $cls=new Application_Model_Area();
                    $result[]=$cls;
                    $cls->setId($row->id)
                          ->setTypeId($row->type_id)
                          ->setFieldName($row->field_name)
                          ->setAltitude($row->altitude)
                          ->setSurfaceArea($row->surface_area)
                          ->setLocality($row->locality)
                          ->setZip($row->zip)
                          ->setTownship($row->township)
                          ->setCanton($row->canton)
                          ->setCountry($row->country)
                          ->setCentroidLat($row->centroid_lat)
                          ->setCentroidLng($row->centroid_lng)
                          ->setComment($row->comment);
            }
            return $result;
    }
    
    /**
     * sets the dbTable class
     *
     * @param Application_Model_DbTable_Area $dbTable
     * @return Application_Model_AreaMapper
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
     * @return Application_Model_DbTable_Area     
     */
    public function getDbTable()
    {
        if (null === $this->_dbTable) {
            $this->setDbTable('Application_Model_DbTable_Area');
        }
        return $this->_dbTable;
    }

    /**
     * saves current row
     *
     * @param Application_Model_Area $cls
     *
     */
     
    public function save(Application_Model_Area $cls,$ignoreEmptyValuesOnUpdate=true)
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
     * @param Application_Model_Area $cls
     */

    public function find($id, Application_Model_Area $cls)
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
              ->setSurfaceArea($row->surface_area)
              ->setLocality($row->locality)
              ->setZip($row->zip)
              ->setTownship($row->township)
              ->setCanton($row->canton)
              ->setCountry($row->country)
              ->setCentroidLat($row->centroid_lat)
              ->setCentroidLng($row->centroid_lng)
              ->setComment($row->comment);
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
            $entry = new Application_Model_Area();
            $entry->setId($row->id)
                  ->setTypeId($row->type_id)
                  ->setFieldName($row->field_name)
                  ->setAltitude($row->altitude)
                  ->setSurfaceArea($row->surface_area)
                  ->setLocality($row->locality)
                  ->setZip($row->zip)
                  ->setTownship($row->township)
                  ->setCanton($row->canton)
                  ->setCountry($row->country)
                  ->setCentroidLat($row->centroid_lat)
                  ->setCentroidLng($row->centroid_lng)
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
                    $entry = new Application_Model_Area();
                    $entry->setId($row->id)
                          ->setTypeId($row->type_id)
                          ->setFieldName($row->field_name)
                          ->setAltitude($row->altitude)
                          ->setSurfaceArea($row->surface_area)
                          ->setLocality($row->locality)
                          ->setZip($row->zip)
                          ->setTownship($row->township)
                          ->setCanton($row->canton)
                          ->setCountry($row->country)
                          ->setCentroidLat($row->centroid_lat)
                          ->setCentroidLng($row->centroid_lng)
                          ->setComment($row->comment)
                          ->setMapper($this);
                    $entries[] = $entry;
            }
            return $entries;
    }

}
