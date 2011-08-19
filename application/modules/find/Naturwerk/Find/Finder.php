<?php

namespace Naturwerk\Find;

/**
 * Find Naturwerk objects.
 *
 * @author fabian.vogler
 */
class Finder {

    /**
     * @var array
     */
    protected $columns = array();

    /**
     * @var \Elastica_Index
     */
    protected $index;

    /**
     * @var string
     */
    protected $type;

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

        // resort and active columns
        if (count($parameters->getColumns()) > 0) {

            $columns = array();
            foreach ($this->columns as $i => $column) {
                $index = array_search($column->getName(), $parameters->getColumns());

                if ($index === false) {
                    $column->setActive(false);
                    $columns[999 + $i] = $column;
                } else {
                    $column->setActive(true);
                    $columns[$index] = $column;
                }
            }
            
            ksort($columns);
            $this->columns = $columns;
        }
    }

    public function addColumn($name, $title, $active = true, $template = 'plain', $condition = null) {
        $this->columns[] = new Column($name, $title, $active, $template, $condition);
    }

    /**
     * @param \Elastica_Query_Abstract $query
     * @return \Elastica_Query_Abstract query with applied geo filter
     */
    protected function geo(\Elastica_Query_Abstract $query) {

        // geo
        $geo = $this->parameters->getGeo();
        if (count($geo) > 1) {
            $geo = new \Elastica_Filter_GeoPolygon('position', $this->parameters->getGeo());
            $query = new \Elastica_Query_Filtered($query, $geo);
        }

        return $query;
    }

    /**
     * @param \Elastica_Query_Abstract $query
     * @return \Elastica_Query_Abstract query with applied access filter
     */
    protected function access(\Elastica_Query_Abstract $query) {

        return $query;
    }

    /**
     * @return \Elastica_Filter_Abstract date filter
     */
    protected function getDateFilter() {
        $range = new \Elastica_Filter_Range('date', $this->parameters->getDate());
        return $range;
    }

    /**
     * @return \Elastica_Filter_Abstract|boolean access filter or false
     */
    protected function getAccessFilter() {
        return false;
    }

    /**
     * @return array
     */
    public function getColumns() {
        return $this->columns;
    }

    /**
     * @return array
     */
    public function getActiveColumns() {

        $columns = array();
        foreach ($this->columns as $column) {
            if ($column->isActive()) {
                $columns[] = $column;
            }
        }

        return $columns;
    }

    public function getFilter() {

        // facets filter
        $filter = new \Elastica_Filter_And();

        // add class filter
        $class = $this->parameters->getClass();
        if (count($class) > 0) {
            $term = new \Elastica_Filter_Terms('class', $class);
            $filter->addFilter($term);
        }

        // add town filter
        $town = $this->parameters->getTown();
        if (count($town) > 0) {
            $term = new \Elastica_Filter_Terms('town', $town);
            $filter->addFilter($term);
        }

        // add user filter
        $user = $this->parameters->getUser();
        if (count($user) > 0) {
            $term = new \Elastica_Filter_Terms('user', $user);
            $filter->addFilter($term);
        }

        // filter date
        $date = $this->parameters->getDate();
        if (count($date) > 0) {
            $range = $this->getDateFilter();
            $filter->addFilter($range);
        }

        return $filter;
    }

    /**
     * @return \Elastica_Query_Abstract
     */
    public function getQuery() {

        // main query
        $search = $this->parameters->getSearch();
        if ($search) {
            $main = new \Elastica_Query_FuzzyLikeThis();
            $main->setLikeText($search);
        } else {
            $main = new \Elastica_Query_MatchAll();
        }
        $main = $this->geo($main);
        $main = $this->access($main);

        return $main;
    }

    /**
     * @return Parameters
     */
    public function getParameters() {
        return $this->parameters;
    }

    /**
     * @return \Elastica_Type
     */
    public function getType() {
        return $this->index->getType($this->type);
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

        $facetTown = new \Elastica_Facet_Terms('town');
        $facetTown->setField('town');

        $facetUser = new \Elastica_Facet_Terms('user');
        $facetUser->setField('user');

        // add class filter
        $class = $this->parameters->getClass();
        if (count($class) > 0) {
            $term = new \Elastica_Filter_Terms('class', $class);
            $facetUser->setFilter($term);
        }

        // build search query
        $query = new \Elastica_Query();
        $query->setQuery($this->getQuery());
        $query->addFacet($facetClass);
        $query->addFacet($facetTown);
        $query->addFacet($facetUser);
        $query->setSize(100);

        // sorting
        $sort = $this->parameters->getSort();
        if (count($sort) > 0) {
            $query->setSort($sort);
        }

        // add filter to query
        $filter = $this->getFilter();
        if (count($filter->getParams()) > 0) {
            $query->setFilter($filter);
        }

        $result = $this->getType()->search($query);

        return $result;
    }

    /**
     * Count method.
     *
     * @return int
     */
    public function count() {

        $query = $this->getQuery();

        $filter = $this->getFilter();
        if (count($filter->getParams()) > 0) {
            $query = new \Elastica_Query_Filtered($query, $filter);
        }

        return $this->getType()->count($query);
    }
}
