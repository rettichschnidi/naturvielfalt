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
    protected $date;

    /**
     * @var array
     */
    protected $class;

    /**
     * @var array
     */
    protected $user;

    /**
     * @var array
     */
    protected $family;

    /**
     * @var array
     */
    protected $genus;

    /**
     * @var Naturwerk\Find\Parameters
     */
    protected $parameters;

    /**
     * @param \Elastica_Index $index
     * @param string $type
     * @param Naturwerk\Find\Parameters $parameter
     */
    public function __construct($index, $type, Parameters $parameters) {

        $this->index = $index;
        $this->type = $type;
        $this->parameters = $parameters;

        $this->search = $parameters->getSearch();
        $this->geo = $parameters->getGeo();
        $this->date = $parameters->getDate();
        $this->class = $parameters->getClass();
        $this->user = $parameters->getUser();
        $this->family = $parameters->getFamily();
        $this->genus = $parameters->getGenus();
    }

    /**
     * @param \Elastica_Query_Abstract $query
     * @return \Elastica_Query_Abstract query with applied geo filter
     */
    protected function geo(\Elastica_Query_Abstract $query) {

        // geo
        if (count($this->geo) == 2) {
            $geo = new \Elastica_Filter_GeoBoundingBox('position', $this->geo);
            $query = new \Elastica_Query_Filtered($query, $geo);
        }

        return $query;
    }

    /**
     * @return \Elastica_Filter_Abstract date filter
     */
    protected function getDateFilter() {
        $range = new \Elastica_Filter_Range('date', $this->date);
        return $range;
    }

    /**
     * Search method.
     *
     * @return Elastica_ResultSet
     */
    public function search() {

        $index = $this->index;

        $facetClass = new \Elastica_Facet_Terms('class');
        $facetClass->setField('class');

        $facetUser = new \Elastica_Facet_Terms('user');
        $facetUser->setField('user');

        $facetFamily = new \Elastica_Facet_Terms('family');
        $facetFamily->setField('family');
        $facetFamily->setSize(900);

        $facetGenus = new \Elastica_Facet_Terms('genus');
        $facetGenus->setField('genus');
        $facetGenus->setSize(900);

        // facets filter
        $filter = new \Elastica_Filter_And();
        $f = false;

        // add class filter
        if (count($this->class) > 0) {
            $term = new \Elastica_Filter_Terms('class', $this->class);
            $filter->addFilter($term);
            $facetUser->setFilter($term);
            $facetGenus->setFilter($term);
            $facetFamily->setFilter($term);
            $f = true;
        }

        // add user filter
        if (count($this->user) > 0) {
            $term = new \Elastica_Filter_Terms('user', $this->user);
            $filter->addFilter($term);
            $facetClass->setFilter($term);
            $facetGenus->setFilter($term);
            $facetFamily->setFilter($term);
            $f = true;
        }

        // add family filter
        if (count($this->family) > 0) {
            $term = new \Elastica_Filter_Terms('family', $this->family);
            $filter->addFilter($term);
            $facetClass->setFilter($term);
            $facetUser->setFilter($term);
            $facetGenus->setFilter($term);
            $f = true;
        }

        // add genus filter
        if (count($this->genus) > 0) {
            $term = new \Elastica_Filter_Terms('genus', $this->genus);
            $filter->addFilter($term);
            $facetClass->setFilter($term);
            $facetUser->setFilter($term);
            $facetFamily->setFilter($term);
            $f = true;
        }

        // filter date
        if (count($this->date) > 0) {
            $range = $this->getDateFilter();
            $filter->addFilter($range);
            $f = true;
        }

        // main query
        if ($this->search) {
            $main = new \Elastica_Query_FuzzyLikeThis($this->search);
        } else {
            $main = new \Elastica_Query_MatchAll();
        }
        $this->geo($main);

        // build search query
        $query = new \Elastica_Query();
        $query->setQuery($main);
        $query->addFacet($facetClass);
        $query->addFacet($facetFamily);
        $query->addFacet($facetGenus);
        $query->addFacet($facetUser);
        $query->setSize(100);

        // sorting
        $sort = $this->parameters->getSort();
        if (count($sort) > 0) {
            $query->setSort($sort);
        }

        // add filter to query
        if ($f) {
            $query->setFilter($filter);
        }

        $type = $index->getType($this->type);
        $result = $type->search($query);

        return $result;
    }
}
