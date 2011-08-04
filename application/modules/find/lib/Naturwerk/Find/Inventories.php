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
        parent::__construct($index, 'inventory', $parameters);
    }
}
