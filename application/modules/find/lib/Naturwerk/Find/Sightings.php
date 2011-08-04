<?php

namespace Naturwerk\Find;

/**
 * Find sightings.
 *
 * @author fabian.vogler
 */
class Sightings extends Finder {

    /**
     * @param \Elastica_Index $index
     * @param Parameters $parameter
     */
    public function __construct(\Elastica_Index $index, Parameters $parameters) {
        parent::__construct($index, 'sighting', $parameters);
    }
}
