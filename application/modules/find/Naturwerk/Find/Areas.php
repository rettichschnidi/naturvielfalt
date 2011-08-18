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

        $this->addColumn('name', 'Gebiet / Flurname', true, 'link');
        $this->addColumn('town', 'Ortschaft');
        $this->addColumn('user', 'Benutzer');

        parent::__construct($index, 'area', $parameters);
    }
}
