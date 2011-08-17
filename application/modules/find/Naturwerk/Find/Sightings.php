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
        Finder::__construct($index, 'sighting', $parameters);

        $this->addColumn('name_la', 'Fachbezeichnung', 'link');
        $this->addColumn('name', 'Name', 'link');
        $this->addColumn('user', 'Benutzer');
        $this->addColumn('inventory', 'Inventar');
        $this->addColumn('date', 'Datum', 'date');
    }
}
