<?php
require_once 'DrupalSession.php';

class InventoryController extends Zend_Controller_Action {

	public function indexAction(){
		$view = $this->view;
		$view->headScript()->appendFile($this->view->baseUrl() . '/js/inventory.js');
		$view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery-1.4.2.min.js');
		$view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery-ui-1.8.6.custom.min.js');
		$view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery-texotela-numeric.js');

		$form = new Application_Form_Inventory();

		$request = $this->getRequest();

		$areaId = $request->getParam('area_id');
		$area = new Application_Model_Area();
		$area->find($areaId);

		if($areaId == $area->getId())
		{
			$headInventoryId = $request->getParam('head_inventory_id');
			$headInventory = new Application_Model_HeadInventory();
			if(!isset($headInventoryId))
			{
				$headInventory->setAreaId($area->getId());
				$headInventory->save();
			}
			else
			{
				$headInventory->find($headInventoryId);
				// TODO: Not found, throw error
				if($headInventoryId != $headInventory->getId())
				{
				}
			}

			$this->view->area = $area;
			$form->setArea($area);
			$form->setHeadInventory($headInventory);
			$invArr = array();
			foreach($headInventory->getInventorys() as $inventory)
			{

				$invArr[$inventory->getId()] = array();
				// Save the inventory column description in the array
				$invArr[$inventory->getId()]["invDesc"] = $this->getInventoryTable($inventory->getInventoryType());

				// Save the values in the array
				foreach($inventory->getInventoryEntrys() as $inventoryEntry)
				{
					$invArr[$inventory->getId()][$inventoryEntry->getId()] = array();
					$invArr[$inventory->getId()][$inventoryEntry->getId()]["orgId"] = $inventoryEntry->getOrganismId();
					$invArr[$inventory->getId()][$inventoryEntry->getId()]["label"] = $inventoryEntry->getOrganism()->getLabel();
					foreach($inventoryEntry->getInventoryTypeAttributeInventoryEntrys() as $value)
					{
						// Dropdown
						$colVal = "";
						if($value->getInventoryTypeAttributeDropdownValueId() > 0)
						{
							$colVal = $value->getInventoryTypeAttributeDropdownValueId();
						}
						else
						{
							$colVal = $value->getValue();
						}
						$invArr[$inventory->getId()][$inventoryEntry->getId()]["col_".$value->getInventoryTypeAttributeId()] = isset($colVal) ? $colVal : "";
					}
				}
			}
			$this->view->inventorys = Zend_Json::encode($invArr);
		}

		$form->startForm();
		$this->view->form = $form;

	}
	public function saveAjaxAction(){
		// check if user has permission to access content
		/*
		$ds = new DrupalSession();
		if (!$ds->hasPermission('node', 'access content')){
			throw new Exception('You are not allowed to perform this action!');
		}
		*/
		
		$this->_helper->layout()->disableLayout();
		$this->_helper->viewRenderer->setNoRender(true);

		$request = $this->getRequest();

		if($request->isPost()){
			$return = array();
			$data = $request->getPost();

			// Remove row to delete
			if(isset($data["deleteRows"]))
			{
				foreach($data["deleteRows"] as $rowId)
				{
					$inventoryEntry = new Application_Model_InventoryEntry();
					$inventoryEntry->find($rowId);

					// TODO: inventory entry not found
					if($rowId != $inventoryEntry->getId())
					{
						throw new Exception("Inventory entry not found");
					}
					else
					{
						$inventoryEntry->deleteRowByPrimaryKey();
					}
				}
			}

			// Save the changed rows
			if(isset($data["addRows"]))
			{
				foreach($data["addRows"] as $invIdKey => $invIdValue)
				{
					$inventory = new Application_Model_Inventory();
					$inventoryType = new Application_Model_InventoryType();
					// New inventory
					if(substr($invIdKey, 0, 8) == "inv_new_")
					{
						$inventoryType = $inventoryType->find($invIdValue['invTypeId']);
						$inventory->setInventoryType($inventoryType);

						$headInventory = new Application_Model_HeadInventory();
						$headInventory->setId($data["headInventoryId"]);
						$inventory->setHeadInventory($headInventory);
						$inventory->save();
						$return[$invIdKey] = $inventory->getId();
					}
					// Edit already existing inventory
					else
					{
						$inventory->find($invIdKey);
						$inventoryType = $inventory->getInventoryType();
					}

					foreach($invIdValue as $rowIdKey => $rowIdValue)
					{
						// This is not an entry
						if($rowIdKey == "invTypeId")
						{
							continue;
						}

						$inventoryEntry = new Application_Model_InventoryEntry();
						// New entry
						if(substr($rowIdKey, 0, 8) == "row_new_")
						{
							$inventoryEntry->setInventory($inventory);
							$organism = new Application_Model_Organism();
							$organism = $organism->find($rowIdValue['orgId']);
							$inventoryEntry->setOrganism($organism);
							$inventoryEntry->save();
							$return[$rowIdKey] = $inventoryEntry->getId();
						}
						// Edit already existing entry
						else
						{
							$inventoryEntry->find($rowIdKey);
						}

						$invTypeAttrVals = array();
						foreach($inventoryType->getInventoryTypeAttributes() as $invTypeAttr)
						{
							$invTypeAttrVal = new Application_Model_InventoryTypeAttributeInventoryEntry();
							$invTypeAttrVal->setInventoryEntry($inventoryEntry);
							$invTypeAttrVal->setInventoryTypeAttribute($invTypeAttr);

							if($invTypeAttr->getAttributeFormat()->getFormat() == 'dropdown')
							{
								if($rowIdValue['col_'.$invTypeAttr->getId()] > 0)
								{
									$invTypeAttrVal->setInventoryTypeAttributeDropdownValueId($rowIdValue['col_'.$invTypeAttr->getId()]);
								}
								$invTypeAttrVals[] = $invTypeAttrVal;
							}
							else
							{
								$invTypeAttrVal->setValue($rowIdValue['col_'.$invTypeAttr->getId()]);
								$invTypeAttrVals[] = $invTypeAttrVal;
							}
						}
						$inventoryEntry->setInventoryTypeAttributeInventoryEntrys($invTypeAttrVals);
					}
				}
			}
			print Zend_Json::encode($return);
		}
	}

	public function newInventoryAction(){
		$this->_helper->layout()->disableLayout();
		$this->_helper->viewRenderer->setNoRender(true);

		$request = $this->getRequest();
		$invId = $request->getParam('inv_id');

		$invMpr = new Application_Model_InventoryTypeMapper();
		$invType = new Application_Model_InventoryType();
		$invMpr->find($invId, $invType);
		if($invType === null){
			echo "Inventory not found.";
		}

		echo Zend_Json::encode($this->getInventoryTable($invType));
	}

	private function getInventoryTable(Application_Model_InventoryType $invType){
		$invJson = array();

		$invJson["name"] = $invType->getName();
		$invJson["id"] = $invType->getId();
		$i = 0;
		foreach($invType->getInventoryTypeAttributes() as $invTypeAttr){
			$i++;
			$invJson["cols"][$i] =
			array(
					"name" => $invTypeAttr->getName(),
					"id" => $invTypeAttr->getId(),
					"format" => $invTypeAttr->getAttributeFormat()->getFormat()
			);

			// If it is a dropdown list
			if($invTypeAttr->getAttributeFormat()->getFormat() == "dropdown")
			{
				$invJson["cols"][$i]["dropdown_values"] = array();
				foreach($invTypeAttr->getInventoryTypeAttributeDropdownValue() as $dropdownValue)
				{
					array_push($invJson["cols"][$i]["dropdown_values"], array("id" =>$dropdownValue->getId(), "value" => $dropdownValue->getValue()));
				}
			}

		}

		return $invJson;
	}

	public function getOrganismsAction(){
		$this->_helper->layout()->disableLayout();
		$this->_helper->viewRenderer->setNoRender(true);

		$request = $this->getRequest();
		$invId = $request->getParam('inv_id');
		$term = $request->getParam('term');

		$fauna = ($invId == 16 ? false : true);

		$termBSpace = substr($term, 0, strpos($term, " "));
		if($termBSpace == "") {
			$termBSpace = "99999999999";	
		}
		$termASpace = substr($term, strpos($term, " ") +1);

		$org = new Application_Model_InventoryType();
		$select = $org->getMapper()->getDbTable()->getAdapter()->select()
		->from(array('t' => 'inventory_type_organism'))
		->join(array('o' => 'organism'),
                    'o.id = t.organism_id', array("ref_organism_id" => "organism_id", "o_id" => "id"));

		// TODO: Fauna und Flora sollen in der Vorschlagsliste gleichzeitig angezeigt werden können.
		// TODO: rewrite, use prepared statements
		if($fauna) {
			$select->join(array('f' => 'fauna_organism'), "f.id = o.organism_id AND o.organism_type = 1")
			->where("t.inventory_type_id = '".$invId."' AND (f.name_de ILIKE '%".$term."%' OR (f.genus ILIKE '".$termBSpace."%' AND f.species ILIKE '".$termASpace."%'))");
		} else {
			$select->join(array('f' => 'flora_organism'), "f.id = o.organism_id AND o.organism_type = 2")
			->where("t.inventory_type_id = '".$invId."' AND (f.name_de ILIKE '%".$term."%' OR (\"Gattung\" ILIKE '".$termBSpace."%' AND \"Art\" ILIKE '".$termASpace."%'))");
		}


		$results = $org->getMapper()->getDbTable()->getAdapter()->query($select);
		$rows = $results->fetchAll();
		print $this->organismToJson($rows, $fauna);
	}

	private function organismToJson($organisms, $fauna){
		$organisms_json = array();
		$organism_view = array();
		foreach($organisms as $organism){
			if(!$fauna)
			{
				// Is it not the official name?
				while($organism["ref_organism_id"] != $organism["official_flora_orfganism_id"])
				{
					$floraOrganism = new Application_Model_FloraOrganism();
					$floraOrganism->getMapper()->find($organism["official_flora_orfganism_id"], $floraOrganism);

					$org = new Application_Model_Organism();
					$org = $org->fetchList("organism_type=2 AND organism_id=".$floraOrganism->getId());

					$organism_view["old_name_de"] = ($organism["name_de"] == null ? "" : $organism["name_de"]);
					$organism_view["old_genus"] = $organism["Gattung"];
					$organism_view["old_species"] = $organism["Art"];

					$organism["official_flora_orfganism_id"] = $floraOrganism->getOfficialFloraOrfganismId();
					$organism["ref_organism_id"] = $floraOrganism->getId();
					$organism["name_de"] = $floraOrganism->getNameDe();
					$organism["Art"] = $floraOrganism->getArt();
					$organism["Gattung"] = $floraOrganism->getGattung();
					$organism["o_id"] = $org[0]->getId();
				}
			}

			$organism_view['id'] = $organism["o_id"];
			$organism_view['genus'] = ($fauna ? $organism["genus"] : $organism["Gattung"]);
			$organism_view['genus'] = ($organism_view['genus'] == null ? "" : $organism_view['genus']);

			$organism_view['species'] = ($fauna ? $organism["species"] : $organism["Art"]);
			$organism_view['species'] = ($organism_view['species'] == null ? "" : $organism_view['species']);

			$organism_view['label'] = $organism["name_de"]." [".$organism_view['genus']." ".$organism_view['species']."]";
			$organism_view['name_de'] = ($organism["name_de"] == null ? "" : $organism["name_de"]);
			//$organism_view['position'] = $organism["position"];
			$organism_view['position'] =1000;
			$organisms_json[] = $organism_view;
		}
		return Zend_Json::encode($organisms_json);
	}
}
?>