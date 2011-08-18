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

        $this->addColumn('name_la', 'Fachbezeichnung', true, 'link', array($this, 'permission'));
        $this->addColumn('name', 'Name', true, 'link', array($this, 'permission'));
        $this->addColumn('user', 'Benutzer');
        $this->addColumn('inventory', 'Inventar');
        $this->addColumn('date', 'Datum', true, 'date');

        Finder::__construct($index, 'sighting', $parameters);
    }
}
