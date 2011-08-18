<?php

namespace Naturwerk\Find;

/**
 * Find inventories.
 *
 * @author fabian.vogler
 */
class Inventories extends Finder {

    public function permission($object, $parameters) {

        if ($object->shared) {
            return true;
        }

        if (in_array($parameters->getUser(), $object->access)) {
            return true;
        }

        return false;
    }

    /**
     * @param \Elastica_Index $index
     * @param Parameters $parameter
     */
    public function __construct(\Elastica_Index $index, Parameters $parameters) {
        parent::__construct($index, 'inventory', $parameters);

        $this->addColumn('name', 'Inventarname', 'link', array($this, 'permission'));
        $this->addColumn('area', 'Gebiet / Flurname');
        $this->addColumn('user', 'Benutzer');
    }
}
