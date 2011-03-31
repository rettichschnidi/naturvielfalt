<?php

class Application_Form_Inventory extends Zend_Form{

	private $area;
	private $headInventory;

	public function init(){

	}

	public function setArea($area)
	{
		$this->area = $area;
	}

	public function setHeadInventory($headInventory)
	{
		$this->headInventory = $headInventory;
	}

	public function startForm(){
		$this->setMethod('post')
		->setAttrib('id', 'Inventory')
		->setAction('inventory/save');

		// Inventartypen-Dropdown
		$invTypeMpr = new Application_Model_InventoryTypeMapper();
		$this->addElement("select","inventory_types", array(
			'label' => "Inventartyp",
			'multiOptions' => Application_Model_Util::resulsToHash($invTypeMpr->fetchList(null, 'name asc'), 'id', 'name')
		));

		// Inventar hinzufügen
		$this->addElement('button', 'add_inventory', array(
          'ignore'   => true,
          'label' => 'Artgruppe hinzufügen'
          ));

          // Head Inventory Id
          $this->addElement('hidden', 'head_inventory_id', array(
          'id' => 'head_inventory_id',
          'ignore'   => true,
			'value'=>$this->headInventory->getId()
          ));

          // Inventar speichern
          $this->addElement('button', 'save_inventory', array(
          'id' => 'save_inventory',
          'label' => 'Speichern'
          ));
	}
}
