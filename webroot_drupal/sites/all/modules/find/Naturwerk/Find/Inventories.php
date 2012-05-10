<?php

namespace Naturwerk\Find;
/**
 * Find inventories.
 *
 * @author fabian.vogler
 */
class Inventories extends Finder {

	/**
	 * @param \Elastica_Index $index
	 * @param Parameters $parameter
	 */
	public function __construct(\Elastica_Index $index, Parameters $parameters) {

		$this->addColumn('name', t('Inventory name'), true, 'link', array(
						$this,
						'permission'
				));
		$this->addColumn('area', t('Area name'));
		$this->addColumn('town', t('Town name'));
		$this->addColumn('canton', t('Canton'));
		$this->addColumn('user', t('User'));

		parent::__construct($index, 'inventory', $parameters);
	}

	/**
	 * Check if user has permissions to access object.
	 *
	 * @param object $object
	 * @param Parameters $parameters
	 * @return boolean
	 */
	public function permission($object, $parameters) {

		if ($object->shared) {
			return true;
		}

		if (in_array($parameters->getUser(), $object->access)) {
			return true;
		}

		return false;
	}

}
