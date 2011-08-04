<?php

namespace Naturwerk\Find;

/**
 * Find organisms, modifies the geo query to match sighting child position.
 *
 * @author fabian.vogler
 */
class Organisms extends Finder {

    /**
     * @param \Elastica_Index $index
     * @param Parameters $parameter
     */
    public function __construct(\Elastica_Index $index, Parameters $parameters) {
        parent::__construct($index, 'organism', $parameters);
    }

    /**
     * @see Naturwerk\Find\Finder::geo()
     */
    protected function geo(\Elastica_Query_Abstract $query) {

        // geo
        if (count($this->geo) == 2) {
            $geo = $this->index->filter()->geo('sighting.position', $this->geo);
            $query = $this->index->query()->filtered($query, $geo);
            $query = $this->index->query()->hasChild($query, 'sighting');
        }

        return $query;
    }
}
