<?php

namespace Naturwerk\Find;
/**
 * Find areas.
 *
 * @author fabian.vogler
 */
class Areas extends Finder {

	/**
	 * @param \Elastica_Index $index
	 * @param Parameters $parameter
	 */
	public function __construct(\Elastica_Index $index, Parameters $parameters) {

		$this->addColumn('name', t('Area name'), true, 'link');
		$this->addColumn('town', t('Town name'));
		$this->addColumn('canton', t('Canton'));
		$this->addColumn('user', t('User'));

		parent::__construct($index, 'area', $parameters);
	}
}
