<?php

class Application_Form_Area extends Zend_Form{
  
  public function init(){

    $this->setMethod('post')
        ->setAttrib('id', 'Area');
    
    //Flurname
    $this->addElement('text', 'field_name', array(
          'label'      => 'Flurname',
          'required'   => true,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
      )));
      
     //Parzellennummer
     $this->addElement('text', 'parcel_nr', array(
          'label'      => 'Parzellen Nr.',
          'required'   => true,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
      )));
    
    //Ort
    $this->addElement('text', 'locality', array(
          'label'      => 'Ortschaft:',
          'required'   => true,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
      )));
      
     //Postleitzahl
     $this->addElement('text', 'zip', array(
          'label'      => 'Postleitzahl:',
          'required'   => true,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
    )));
    
    
    //Gemeinde
    $this->addElement('text', 'township', array(
          'label'      => 'Gemeinde:',
          'required'   => true,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
      )));
    
    //Kanton
    $this->addElement('text', 'canton', array(
          'label'      => 'Kanton:',
          'required'   => true,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
      )));
      
     //Fläche
     $this->addElement('text', 'surface_area', array(
          'label'      => 'Fläche (m2)',
          'required'   => false,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
      ))); 
          
     //Lebensraum ID 
     $this->addElement('hidden', 'habitat_id', array(
          'disableLoadDefaultDecorators' => true,
          'decorators' => array(new Zend_Form_Decorator_ViewHelper()),
          'required'   => false,
          //'isArray'    => true,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
      )));
      
     //Fläche
     $this->addElement('text', 'altitude', array(
          'label'      => 'Meter über Meer',
          'required'   => false,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
      )));
      
      //Lebensräume
      $this->addElement('text', 'habitat', array(
          'label'      => 'Lebensräume',
          'required'   => false,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
      )));
      
      
      //Beschreibung
      $this->addElement('textarea', 'comment', array(
          'label'      => 'Beschreibung',
          'cols'       => '60',
          'rows'       => '10',
          'required'   => false,
          'filters'    => array('StringTrim'),
          'validators' => array(
              array('validator' => new Zend_Validate_Alnum(array('allowWhiteSpace' => true))
              )
      )));
      
      
//      // Save & new area
//      $this->addElement('button', 'save_new_area', array(
//          'ignore'   => true,
//          'label' => 'Gebiet speichern'
//      ));
//
//      // Save & go on to inventory
//      $this->addElement('button', 'save_to_inventory', array(
//          'ignore'   => true,
//          'label' => 'Gebiet speichern'
//      ));
      
      // And finally add some CSRF protection
      $this->addElement('hash', 'csrf', array(
          'ignore' => true,
      ));
  }
}
