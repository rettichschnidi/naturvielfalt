<?php

namespace Naturwerk\Find;
/**
 * Find sightings.
 *
 * @author fabian.vogler
 */
class Sightings extends Inventories {

	/**
	 * @param \Elastica_Index $index
	 * @param Parameters $parameter
	 */
	public function __construct(\Elastica_Index $index, Parameters $parameters) {

		$this->addColumn('name_la', t('Latin name'));
		$this->addColumn('name', t('German name'));
		$this->addColumn('user', t('User'));
		$this->addColumn('inventory', t('Inventory'), true, 'link', array(
						$this,
						'permission'
				));
		$this->addColumn('town', t('Town name'));
		$this->addColumn('canton', t('Canton'));
		$this->addColumn('date', t('Observation date'), true, 'date');
		$this->addColumn('redlist', t('Red list'), false, 'translate');

		Finder::__construct($index, 'sighting', $parameters);
	}
}
