<?php

namespace Naturwerk\Find;

use \Elastica_Filter_GeoPolygon as GeoPolygon;
use \Elastica_Filter_Range as Range;
use \Elastica_Filter_HasChild as HasChildFilter;
use \Elastica_Query_Filtered as Filtered;
use \Elastica_Query_HasChild as HasChild;
use \Elastica_Query_MatchAll as All;

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
        if (count($this->geo) > 1) {
            $geo = new GeoPolygon('sighting.position', $this->geo);
            $query = new Filtered($query, $geo);
            $query = new HasChild($query, 'sighting');
        }

        return $query;
    }

    /**
     * @return \Elastica_Filter_Abstract date filter
     */
    protected function getDateFilter() {
        $range = new Range('sighting.date', $this->date);
        $query = new Filtered(new All(), $range);
        $hasChild = new HasChildFilter($query, 'sighting');
        return $hasChild;
    }

}
