<?php

class AreaCreateController extends Zend_Controller_Action {

    public function indexAction() {

        $this->view->headLink()->appendStylesheet($this->view->baseUrl() . '/css/area.css');

        $this->view->headScript()->appendFile("http://maps.google.com/maps/api/js?key=ABQIAAAABuqUv_uyCZ4WzUTgK5G-thR8vyPbVAPvWWUSjekUdI5ADbIJJRSNaY0WIlEy744RJmMGHB5KrWGGKw&sensor=false");
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/v3_epoly_sphericalArea.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/css/overlay-style.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery-1.4.2.min.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery-ui-1.8.6.custom.min.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/ui.geo_autocomplete.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/util.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/Config.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/MapCache.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/MapAjaxProxy.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/GeometryOverlayControl.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/GeometryOverlay.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/PolygonControl.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/Polygon.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/MarkerControl.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/rwo_gmaps/Marker.js');
        $this->view->headScript()->appendFile($this->view->baseUrl() . '/js/area.js');

        $form = new Application_Form_Area();
        $this->view->form = $form;
    }

    /**
     * Saves an Area in the Database
     */
    public function saveAction() {

        $this->_helper->layout()->disableLayout();
        $this->_helper->viewRenderer->setNoRender(true);

        $request = $this->getRequest();

        if ($request->isXmlHttpRequest() and $request->isPost()) {
            $data = $request->getPost();
            $type_mapper = new Application_Model_AreaTypeMapper();
            $habitat_mapper = new Application_Model_HabitatMapper();
            $habitats = array();
            /*
            foreach ($data['habitats'] as $habitat_id) {
                $model = new Application_Model_Habitat();
                $habitat_mapper->find($habitat_id, $model);
                $habitats[] = $model;
            }
            */
            //$this->_helper->json($data);
            $area = new Application_Model_Area();
            $area->setFieldName($data['field_name']);
            $area->setComment($data['comment']);
            $area->setAltitude($data['altitude']);
            $area->setLocality($data['locality']);
            $area->setZip($data['zip']);
            $area->setTownship($data['township']);
            $area->setCanton($data['canton']);
            $area->setCountry('Schweiz');
            $area->setSurfaceArea($data['surface_area']);
            $area->setCentroid($data['centroid']);
            $area->setTypeId($type_mapper->findOneByField('"desc"', $data['area']['type'], new Application_Model_AreaType())->getId());
            $area->getMapper()->save($area);
            $area->setRawPoints($data['area']['coords']);
            $area->setHabitats($habitats);
            //send id to client
            $this->_helper->json(array(
                'id' => $area->getId()
            ));
        }
    }

    public function getAreasAction() {
        if ($request->isXmlHttpRequest() and $request->isPost()) {
            
        }
    }

    /**
     * Return habitats for autocomplete.
     */
    public function getHabitatsAction() {
        $request = $this->getRequest();

        if ($request->isXmlHttpRequest() and $request->isGet()) {
            //TODO: validate & filter 'term'
            $term_raw = preg_quote($request->getParam('term'));
            $term = preg_replace_callback('/[aou]/i', 'AreaCreateController::catchUmlaut', $term_raw);

            $habitat_mapper = new Application_Model_HabitatMapper();
            $habitats = $habitat_mapper->fetchList("name_de ~* '" . $term . "' OR label ~* '^" . $term . "%'");
            $this->_helper->json($this->habitatsToArray($habitats));
        }
    }

    private static function habitatsToArray($habitats) {
        foreach ($habitats as $habitat) {
            $habitats_array[] = array(
                'id' => $habitat->getId(),
                'label' => $habitat->getLabel(),
                'name' => $habitat->getNameDe()
            );
        }
        return $habitats_array;
    }

    // to find with term 'wasser' -> 'wasser' and 'gewässer'
    private static function catchUmlaut($m) {
        $hash = array(
            'a' => '(ä|a)',
            'o' => '(ö|o)',
            'u' => '(ü|u)');
        return $hash[$m[0]];
    }

}

?>