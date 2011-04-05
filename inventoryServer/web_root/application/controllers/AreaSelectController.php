<?php
class AreaSelectController extends Zend_Controller_Action {
  
  public function indexAction(){
    $this->view->headLink()->appendStylesheet($this->view->baseUrl() .'/css/area-select.css');
    
    $this->view->headScript()->appendFile("http://maps.google.com/maps/api/js?key=ABQIAAAABuqUv_uyCZ4WzUTgK5G-thR8vyPbVAPvWWUSjekUdI5ADbIJJRSNaY0WIlEy744RJmMGHB5KrWGGKw&sensor=false");
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/v3_epoly_sphericalArea.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/css/overlay-style.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery-1.4.2.min.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery-ui-1.8.6.custom.min.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery.dataTables.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery.dataTables.pluginAPI.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/ui.geo_autocomplete.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/util.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/Config.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/MapGeometryOverlays.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/MapCache.js');
    //$this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/MapAjaxProxy.js');
    //$this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/GeometryOverlayControl.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/GeometryOverlay.js');
    //$this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/PolygonControl.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/Polygon.js');
    //$this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/MarkerControl.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/Marker.js');
    $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/area-select.js');
  }


  public function getAreaAction(){
   
    $request = $this->getRequest();
    $where = '';
    
    if($request->isXmlHttpRequest() and $request->isPost()){
      //echo '<pre>'.print_r($areas_array,true).'</pre>';
      $aColumns = array( '', 'field_name', 'locality', 'township', 'canton', 'surface_area', 'altitude');
     /*
     * Names - this is specific to server-side column re-ordering
     */
     $aNames = explode( ',', $request->getParam('sColumns') );
      
      if ( $request->getParam('iDisplayStart') != null && $request->getParam('iDisplayLength') != '-1' ){
        $dispStart = $request->getParam('iDisplayStart');
        $dispLength = $request->getParam('iDisplayLength');
      }
      
      //ORDERING
      if ( $request->getParam('iSortCol_0') != null ){
        $order = "";
        for ( $i=0 ; $i<intval( $request->getParam('iSortingCols') ) ; $i++ )
        {
          if ( $request->getParam( 'bSortable_'.intval($request->getParam('iSortCol_'.$i)) ) == "true" )
          {
            //$iColumnIndex = array_search( $aNames[intval( $request->getParam('iSortCol_'.$i) )], $aColumns );
            
            $order .= $aColumns[ $request->getParam('iSortCol_'.$i) ]."
              ". $request->getParam('sSortDir_'.$i)  .", ";
          }
        }
        $order = substr_replace( $order, "", -2 );
      }
      
      //SEARCH
      if ( $request->getParam('sSearch') != "" ){
        $search = $request->getParam('sSearch');

        $where =    "field_name ILIKE '%" . $search . "%' OR " .
                    "locality ILIKE '%" . $search . "%' OR " .
                    "township ILIKE '%" . $search . "%' OR " .
                    "canton ILIKE '%" . $search . "%'";

        if(is_numeric($search)){
            $where .=   " OR ".
                        "CAST(surface_area AS TEXT) ILIKE '%" . $search . "%' OR " .
                        "CAST(altitude AS TEXT) ILIKE '%" . $search . "%'";
        }

//        $where =  "CAST(surface_area AS TEXT) ILIKE '%" . $search . "%' OR " .
//                        "CAST(altitude AS TEXT) ILIKE '%" . $search . "%'";

      }

      /* Individual column filtering */
      for ( $i=0 ; $i<count($aColumns) ; $i++ ){
        if ( $request->getParam('bSearchable_'.$i) == "true" && $request->getParam('sSearch_'.$i) != '' )
        {
        	/*
          if ( $sWhere != "" )
          {
            $where .= " AND ";
          }
          */
          $iColumnIndex = array_search( $aNames[$i], $aColumns );
          $where .= $aColumns[$iColumnIndex]." LIKE '%".$request->getParam('sSearch_'.$i)."%' ";
        }
      }
      if($where == ''){
        $where = null;
      }
      
      $area_mapper = new Application_Model_AreaMapper();
      //$area_entrys = $area_mapper->fetchList($where, $order, $dispLength, $dispStart);
      $area_entrys = $area_mapper->fetchList($where, $order, $dispLength, $dispStart);
      $areas_array = array();
      foreach($area_entrys as $area){
        array_push($areas_array, $area->toJsonArray());
      }
      $display = array(
        'sEcho' => $request->getParam('sEcho'),
        'iTotalRecords' => $area_mapper->getDbTable()->countAllRows(),
        'iTotalDisplayRecords' => $area_mapper->getDbTable()->countByQuery($where),
        //'debug' => "sWhere: ".$sWhere." iColumnIndex: ".$iColumnIndex,
        'areas' => $areas_array,
      );
      $this->_helper->json($display);
    }
  }
  
  public function getAreaTableAction(){
    $this->_helper->layout()->disableLayout();
    $this->_helper->viewRenderer->setNoRender(true);
    
    $request = $this->getRequest();
    if($request->isXmlHttpRequest() and $request->isGet()){

      echo '<pre>'.print_r($request->getParams(),true).'</pre>';
      //echo '<pre>'.print_r($areas_array,true).'</pre>';
    }
  }

  public function getAreasForMapAction(){
    $request = $this->getRequest();
    if($request->isXmlHttpRequest() and $request->isGet()){

      echo '<pre>'.print_r($request->getParams(),true).'</pre>';
      //echo '<pre>'.print_r($areas_array,true).'</pre>';
    }
  }
}