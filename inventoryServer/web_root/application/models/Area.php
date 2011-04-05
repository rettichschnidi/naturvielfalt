<?php
require_once('AreaMapper.php');
require_once('MainModel.php');

/**
 * Add your description here
 *
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

 //TODO: filter & validators for all values!
 
class Application_Model_Area extends MainModel
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
	protected $_TypeId;

	/**
	 * mysql var type varchar(100)
	 *
	 * @var string
	 */
	protected $_FieldName;
  
  /**
   * mysql var type text
   *
   * @var text
   */
  protected $_ParcelNr;  
  
	/**
	 * mysql var type int(6)
	 *
	 * @var int
	 */
	protected $_Altitude;

  /**
   * mysql var type varchar(30)
   *
   * @var string
   */
  protected $_Locality;
  
  /**
   * mysql var type varchar(6)
   *
   * @var string
   */
  protected $_Zip;
  
  /**
   * mysql var type varchar(30)
   *
   * @var string
   */
  protected $_Township;
  
  /**
   * mysql var type varchar(30)
   *
   * @var string
   */
  protected $_Canton;

 /**
   * mysql var type varchar(30)
   *
   * @var string
   */
  protected $_Country;


  /**
   * mysql var type int(10)
   *
   * @var int
   */
  protected $_SurfaceArea;
  
    /**
   * mysql var type float(17,13)
   *
   * @var float
   */
  protected $_CentroidLat;
  
    /**
   * mysql var type float(17,13)
   *
   * @var float
   */
  protected $_CentroidLng;


	/**
	 * mysql var type text
	 *
	 * @var text
	 */
	protected $_Comment;




	function __construct() {
		$this->setColumnsList(array(
    'id'=>'Id',
    'type_id'=>'TypeId',
    'field_name'=>'FieldName',
    'parcel_nr'     =>'ParcelNr',
    'altitude'      =>'Altitude',
    'surface_area'  =>'SurfaceArea',
    'locality'      =>'Locality',
    'zip'           =>'Zip',
    'township'      =>'Township',
    'canton'        =>'Canton',
    'country'       =>'Country',
    'centroid_lad'  =>'CentroidLat',
    'centroid_lng'  =>'CentroidLng',
    'comment'       =>'Comment',
		));
	}

	/**
	 * gets the references of Application_Model_Habitat
	 * @return array
	 */

	public function getHabitats()
	{
		return $this->getRowsAsModel($this->getMapper()->find($this->getId(), $this)->findManyToManyRowset("Application_Model_DbTable_Habitat",
                "Application_Model_DbTable_AreaHabitat", "Application_Model_DbTable_Area"), new Application_Model_Habitat());
	}

	/**
	 * sets the Application_Model_Habitat models
	 *
	 * @param array  Array of Application_Model_Habitat
	 *
	 **/

	public function setHabitats($Habitats)
	{
		$this->setManyToMany($Habitats, Application_Model_AreaHabitat, Application_Model_Habitat);
	}


	/**
	 * gets the references of Application_Model_AreaPoint
	 * @return array
	 */

	public function getAreaPoints()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_AreaPoint'),
		new Application_Model_AreaPoint());
	}

	/**
	 * sets the Application_Model_AreaPoint models
	 *
	 * @param array  Array of Application_Model_AreaPoint
	 *
	 **/

	public function setAreaPoints($AreaPoints)
	{
		$this->setOneToMany($AreaPoints, Application_Model_AreaPoint);
	}


	public function setRawPoints($raw_points){
		$area_points[] = array();
		foreach($raw_points as $key=>$raw_point){
			$area_points[$key] = new Application_Model_AreaPoint();
			$area_points[$key]->setLat((float) $raw_point['lat']);
			$area_points[$key]->setLng((float) $raw_point['lng']);
			$area_points[$key]->setSeq((int) $key);
		}
		$this->setAreaPoints($area_points);
	}
	/**
	 * gets the reference of Application_Model_AreaType
	 * @return Application_Model_AreaType
	 */

	public function getAreaType()
	{
		$rowset = $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_AreaType'), Application_Model_AreaType);
		return $rowset[0];
	}

	/**
	 * sets the Application_Model_AreaType model
	 *
	 * @param Application_Model_AreaType
	 *
	 **/

	public function setAreaType($AreaType)
	{
		$this->setManyToOne($AreaType, Application_Model_AreaType);
	}


	/**
	 * gets the references of Application_Model_HeadInventory
	 * @return array
	 */

	public function getHeadInventorys()
	{
		return $this->getRowsAsModel($this->getCurrentRow()->findDependentRowset('Application_Model_DbTable_HeadInventory'),
		Application_Model_HeadInventory);
	}

	/**
	 * sets the Application_Model_HeadInventory models
	 *
	 * @param array  Array of Application_Model_HeadInventory
	 *
	 **/

	public function setHeadInventorys($HeadInventorys)
	{
		$this->setOneToMany($HeadInventorys, Application_Model_HeadInventory);
	}

	/**
	 * sets column id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_Area
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
	 * sets column type_id type int(11)
	 *
	 * @param int $data
	 * @return Application_Model_Area
	 *
	 **/

	public function setTypeId($data)
	{
		$this->_TypeId=$data;
		return $this;
	}

	/**
	 * gets column type_id type int(11)
	 * @return int
	 */

	public function getTypeId()
	{
		return $this->_TypeId;
	}

  /**
   * gets desc for type_id
   * @return string
   */
  public function getType(){
     $type_mapper = new Application_Model_AreaTypeMapper();
     $area_type = new Application_Model_AreaType();
     $type_mapper->find($this->getTypeId(), $area_type);
     return $area_type->getDesc();
  }
	/**
	 * sets column field_name type varchar(100)
	 *
	 * @param string $data
	 * @return Application_Model_Area
	 *
	 **/

	public function setFieldName($data)
	{
		$this->_FieldName=$data;
		return $this;
	}

	/**
	 * gets column field_name type varchar(100)
	 * @return string
	 */

	public function getFieldName()
	{
		return $this->_FieldName;
	}

	/**
	 * sets column altitude type int(6)
	 *
	 * @param int $data
	 * @return Application_Model_Area
	 *
	 **/

	public function setAltitude($data)
	{
		$this->_Altitude=$data;
		return $this;
	}


  public function getAltitude()
  {
    return $this->_Altitude;
  }


  public function setParcelNr($data)
  {
    $this->_ParcelNr=$data;
    return $this;
  }


	public function getParcelNr()
	{
		return $this->_ParcelNr;
	}

  public function setLocality($data)
  {
    $this->_Locality=$data;
    return $this;
  }

  public function getLocality()
  {
    return $this->_Locality;
  }

  public function setZip($data)
  {
    $this->_Zip=$data;
    return $this;
  }

  public function getZip()
  {
    return $this->_Zip;
  }

  public function setTownship($data)
  {
    $this->_Township=$data;
    return $this;
  }

  public function getTownship()
  {
    return $this->_Township;
  }

  public function setCanton($data)
  {
    $this->_Canton=$data;
    return $this;
  }
  
  public function getCanton()
  {
    return $this->_Canton;
  }
  
  public function setCountry($data)
  {
    $this->_Country=$data;
    return $this;
  }
  
  public function getCountry()
  {
    return $this->_Country;
  }
  
  public function setSurfaceArea($data)
  {
    $this->_SurfaceArea=$data;
    return $this;
  }
  
  public function getCentroidLat()
  {
    return $this->_CentroidLat;
  }

  public function setCentroidLat($data)
  {
    $this->_CentroidLat=$data;
    return $this;
  }
  
  public function getCentroidLng()
  {
    return $this->_CentroidLng;
  }
  
  public function setCentroidLng($data)
  {
    $this->_CentroidLng=$data;
    return $this;
  }
  
  public function getSurfaceArea()
  {
    return $this->_SurfaceArea;
  }

  public function setCentroid($data)
  {
    $this->_CentroidLat=$data['lat'];
    $this->_CentroidLng=$data['lng'];
    return $this;
  }


  public function getCentroid()
  {
    return array(
      'lat' => $this->_CentroidLat,
      'lng' => $this->_CentroidLng
      );
  }

	/**
	 * sets column comment type text
	 *
	 * @param text $data
	 * @return Application_Model_Area
	 *
	 **/

	public function setComment($data)
	{
		$this->_Comment=$data;
		return $this;
	}

	/**
	 * gets column comment type text
	 * @return text
	 */

	public function getComment()
	{
		return $this->_Comment;
	}

	/**
	 * returns the mapper class
	 *
	 * @return Application_Model_AreaMapper
	 *
	 */

	public function getMapper()
	{
		if (null === $this->_mapper) {
			$this->setMapper(new Application_Model_AreaMapper());
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

  public function toJsonArray(){
    $habitats_array = array();
    foreach($this->getHabitats() as $habitat){
      array_push($habitats_array, $habitat->toJsonArray());
    }
    
    $area_points_array = array();
    foreach($this->getAreaPoints() as $area_point){
      $area_points_array[$area_point->getSeq()]  = $area_point->toJsonArray();
    }
    
    $area_array = array(
      'id'         => $this->getId(),
      'field_name' => $this->getFieldName(),
      'parcel_nr'  => $this->getParcelNr(),
      'type'       => $this->getType(),
      'altitude'   => $this->getAltitude(),
      'surface_area' => $this->getSurfaceArea(),
      'locality'   => $this->getLocality(),
      'township'   => $this->getTownship(),
      'canton'     => $this->getCanton(),
      'centroid'   => $this->getCentroid(),
      'comment'    => $this->getComment(),
      'habitats'   => $habitats_array,
      'area_points'=> $area_points_array
    );
    return $area_array;
  }
}

