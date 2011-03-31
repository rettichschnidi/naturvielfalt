<?php
class HeadInventoryController extends Zend_Controller_Action {

	public function indexAction(){
		$view = $this->view;
		$view->headScript()->appendFile($this->view->baseUrl() . '/js/inventory.js');
		$view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery-1.4.2.min.js');
		$view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery-ui-1.8.6.custom.min.js');
		$view->headScript()->appendFile($this->view->baseUrl() . '/js/lib/jquery/jquery-texotela-numeric.js');

		$form = new Application_Form_HeadInventory();
		$this->view->form = $form;

		$request = $this->getRequest();
		$areaId = $request->getParam('area_id');
		$area = new Application_Model_Area();
		$area->getMapper()->find($areaId, $area);
		if($areaId == $area->getId())
		{
			$this->view->area = $area;
		}
		//TODO Area does not exist: Throw error
		else
		{
				
		}
	}
	public function saveAction(){

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
			$organisms_json[] = $organism_view;
		}
		return Zend_Json::encode($organisms_json);
	}
}
?>