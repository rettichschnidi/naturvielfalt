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
     * @var array
     */
    protected $sort;

    /**
     * @param \Elastica_Index $index
     * @param string $type
     * @param Naturwerk\Find\Parameters $parameter
     */
    public function __construct($index, $type, Parameters $parameters) {

        $this->index = $index;
        $this->type = $type;
        $this->parameters = $parameters;

        // resort and activate columns
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

        // set default sort
        $this->sort = $this->parameters->getSort();
        $active = array();
        foreach ($this->columns as $column) {
            if ($column->isActive()) {
                $active[] = $column->getName();
            }
        }
        if (count($this->sort) == 0 || !in_array(key($this->sort), $active)) {
            $column = reset($this->columns);
            $this->sort = array($column->getName() => 'asc');
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

    public function getFilter($exclude = false) {

        // facets filter
        $filter = new \Elastica_Filter_And();

        // add class filter
        $class = $this->parameters->getClass();
        if (count($class) > 0 && $exclude != 'class') {
            $term = new \Elastica_Filter_Terms('class', $class);
            $filter->addFilter($term);
        }

        // add family filter
        $family = $this->parameters->getFamily();
        if (count($family) > 0 && $exclude != 'family') {
            $term = new \Elastica_Filter_Terms('family', $family);
            $filter->addFilter($term);
        }

        // add town filter
        $town = $this->parameters->getTown();
        if (count($town) > 0 && $exclude != 'town') {
            $term = new \Elastica_Filter_Terms('town', $town);
            $filter->addFilter($term);
        }

        // add canton filter
        $canton = $this->parameters->getCanton();
        if (count($canton) > 0 && $exclude != 'canton') {
            $term = new \Elastica_Filter_Terms('canton', $canton);
            $filter->addFilter($term);
        }

        // add user filter
        $user = $this->parameters->getUser();
        if (count($user) > 0 && $exclude != 'user') {
            $term = new \Elastica_Filter_Terms('user', $user);
            $filter->addFilter($term);
        }

        // add habitat filter
        $habitat = $this->parameters->getHabitat();
        if (count($habitat) > 0 && $exclude != 'habitat') {
            $term = new \Elastica_Filter_Terms('habitat', $habitat);
            $filter->addFilter($term);
        }

        // add redlist filter
        $redlist = $this->parameters->getRedlist();
        if (count($redlist) > 0 && $exclude != 'redlist') {
            $term = new \Elastica_Filter_Terms('redlist', $redlist);
            $filter->addFilter($term);
        }

        // add image_type filter
        $image_type = $this->parameters->getImage_type();
        if (count($image_type) > 0 && $exclude != 'image_type') {
            $term = new \Elastica_Filter_Terms('image_type', $image_type);
            $filter->addFilter($term);
        }

        // filter date
        $date = $this->parameters->getDate();
        if (count($date) > 0 && $exclude != 'date') {
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

            $main = new \Elastica_Query_QueryString();
            $main->setQueryString($search);
            $main->setParam('analyze_wildcard', true);
            
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

    public function getFacet($field) {

        $facet = new \Elastica_Facet_Terms($field);
        $facet->setField($field);
        $facet->setSize(500);

        $filter = $this->getFilter($field);
        if (count($filter->getParams()) > 0) {
            $facet->setFilter($filter);
        }

        return $facet;
    }

    /**
     * Search method.
     *
     * @return Elastica_ResultSet
     */
    public function search() {

        $index = $this->index;

        // build search query
        $query = new \Elastica_Query();
        $query->setQuery($this->getQuery());
        $query->addFacet($this->getFacet('class'));
        $query->addFacet($this->getFacet('family'));
        $query->addFacet($this->getFacet('town'));
        $query->addFacet($this->getFacet('canton'));
        $query->addFacet($this->getFacet('user'));
        $query->addFacet($this->getFacet('habitat'));
        $query->addFacet($this->getFacet('redlist'));
        $query->addFacet($this->getFacet('image_type'));
        $query->setSize(100);

        // sorting
        if (count($this->sort) > 0) {
            $query->setSort($this->sort);
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

    /**
     * @return array $sort
     */
    public function getSort() {
        return $this->sort;
    }
}
