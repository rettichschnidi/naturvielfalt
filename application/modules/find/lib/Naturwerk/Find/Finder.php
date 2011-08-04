<?php

namespace Naturwerk\Find;

/**
 * Find Naturwerk objects.
 *
 * @author fabian.vogler
 */
class Finder {

    /**
     * @var \Elastica_Index
     */
    protected $index;

    /**
     * @var string
     */
    protected $type;

    /**
     * @var string
     */
    protected $search;

    /**
     * @var array
     */
    protected $geo;

    /**
     * @var array
     */
    protected $class;

    /**
     * @var array
     */
    protected $family;

    /**
     * @var array
     */
    protected $genus;

    /**
     * @param \Elastica_Index $index
     * @param string $type
     * @param Naturwerk\Find\Parameters $parameter
     */
    public function __construct($index, $type, Parameters $parameters) {

        $this->index = $index;
        $this->type = $type;

        $this->search = $parameters->getSearch();
        $this->geo = $parameters->getGeo();
        $this->class = $parameters->getClass();
        $this->family = $parameters->getFamily();
        $this->genus = $parameters->getGenus();
    }

    /**
     * @return \Elastica_Query_Abstract main query with search parameter
     */
    protected function getQuery() {

        // main query
        if ($this->search) {
            $query = $this->index->query()->flt($this->search);
        } else {
            $query = $this->index->query()->all();
        }

        // apply geo
        $query = $this->geo($query);

        return $query;
    }

    /**
     * @param \Elastica_Query_Abstract $query
     * @return \Elastica_Query_Abstract query with applied geo filter
     */
    protected function geo(\Elastica_Query_Abstract $query) {

        // geo
        if (count($this->geo) == 2) {
            $geo = $this->index->filter()->geo('position', $this->geo);
            $query = $this->index->query()->filtered($query, $geo);
        }

        return $query;
    }

    /**
     * Search method.
     *
     * @return Elastica_ResultSet
     */
    public function search() {

        $index = $this->index;

        $facetClass = $index->facet()->terms('class');
        $facetClass->setField('class');

        $facetFamily = $index->facet()->terms('family');
        $facetFamily->setField('family');
        $facetFamily->setSize(900);

        $facetGenus = $index->facet()->terms('genus');
        $facetGenus->setField('genus');
        $facetGenus->setSize(900);

        $facetUser = $index->facet()->terms('user');
        $facetUser->setField('user');

        // facets filter
        $filter = $index->filter()->and_();
        $f = false;

        // add class filter
        if (count($this->class) > 0) {
            $term = $index->filter()->terms('class', $this->class);
            $filter->addFilter($term);
            $facetGenus->setFilter($term);
            $facetFamily->setFilter($term);
            $f = true;
        }

        // add family filter
        if (count($this->family) > 0) {
            $term = $index->filter()->terms('family', $this->family);
            $filter->addFilter($term);
            $facetClass->setFilter($term);
            $facetGenus->setFilter($term);
            $f = true;
        }

        // add genus filter
        if (count($this->genus) > 0) {
            $term = $index->filter()->terms('genus', $this->genus);
            $filter->addFilter($term);
            $facetClass->setFilter($term);
            $facetFamily->setFilter($term);
            $f = true;
        }

        // build search query
        $query = new \Elastica_Query();
        $query->setQuery($this->getQuery());
        $query->addFacet($facetClass);
        $query->addFacet($facetFamily);
        $query->addFacet($facetGenus);
        $query->addFacet($facetUser);
        $query->setSize(100);

        // add filter to query
        if ($f) {
            $query->setFilter($filter);
        }

        $type = $index->getType($this->type);
        $result = $type->search($query);

        return $result;
    }
}
