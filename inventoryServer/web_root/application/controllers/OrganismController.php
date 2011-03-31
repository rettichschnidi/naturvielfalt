<?php

class OrganismController extends Zend_Controller_Action
{

    public function init()
    {
        /* Initialize action controller here */
    }

    public function indexAction()
    {
        // action body
        $organismMapper = new Application_Model_OrganismMapper();
        $organismMapper->fetchAll();
        
        
        $this->view->oneEntry = $organismMapper->findById(1);
        
    }


}

