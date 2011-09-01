<?php

namespace Naturwerk\Find;

/**
 * Find images.
 *
 * @author fabian.vogler
 */
class Images extends Finder {

    /**
     * @param \Elastica_Index $index
     * @param Parameters $parameter
     */
    public function __construct(\Elastica_Index $index, Parameters $parameters) {

        $this->addColumn('id', 'Default');

        parent::__construct($index, 'image', $parameters);
    }
}
