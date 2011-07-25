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

    public function __construct($index) {
        $this->index = $index;
    }

    public function search($type, $search = '', $class = array(), $family = array(), $genus = array()) {

        $index = $this->index;
        
        $facetClass = $index->facet()->terms('class');
        $facetClass->setField('class');

        $facetFamily = $index->facet()->terms('family');
        $facetFamily->setField('family');
        $facetFamily->setSize(900);

        $facetGenus = $index->facet()->terms('genus');
        $facetGenus->setField('genus');
        $facetGenus->setSize(900);

        // main query
        if ($search) {
            $all = $index->query()->string($search);
            $all->setDefaultField('name');
        } else {
            $all = $index->query()->all();
        }
        $geo = $index->filter()->geo('sighting.position', 47.567261, 7.764587, 47.236355, 8.506165);
        $filtered = $index->query()->filtered($all, $geo);
        $hasChild = $index->query()->hasChild($filtered, 'sighting');

        $filter = $index->filter()->and_();
        $f = false;

        // add class filter
        if (count($class) > 0) {
            $term = $index->filter()->terms('class', $class);
            $filter->addFilter($term);
            $facetGenus->setFilter($term);
            $facetFamily->setFilter($term);
            $f = true;
        }

        // add family filter
        if (count($family) > 0) {
            $term = $index->filter()->terms('family', $family);
            $filter->addFilter($term);
            $facetClass->setFilter($term);
            $facetGenus->setFilter($term);
            $f = true;
        }

        // add genus filter
        if (count($genus) > 0) {
            $term = $index->filter()->terms('genus', $genus);
            $filter->addFilter($term);
            $facetClass->setFilter($term);
            $facetFamily->setFilter($term);
            $f = true;
        }

        $query = new \Elastica_Query();
        $query->setQuery($hasChild);
        $query->addFacet($facetClass);
        $query->addFacet($facetFamily);
        $query->addFacet($facetGenus);
        $query->setSize(100);

        // add filter to query
        if ($f) {
            $query->setFilter($filter);
        }

        $type = $index->getType($type);
        $result = $type->search($query);

        return $result;
    }
}

/**
 * Naturwerk indexer to index organisms, sightings, etc.
 *
 * @author fabian.vogler
 */
class Indexer {

    /**
     * @var \Elastica_Index
     */
    protected $index;

    public function __construct($index) {
        $this->index = $index;
    }

    /**
     * Delete complete index.
     */
    public function clear() {
        $this->index->create(array(), true);
    }

    /**
     * Index all types.
     */
    public function all() {
        $this->clear();
        $this->organisms();
        $this->sightings();
    }

    /**
     * Index all organsims.
     */
    public function organisms() {

        // Flora
        $sql = '
            SELECT
                organism.id,
                organism.organism_type,
                \'Planzen\' AS class,
                flora_organism."Gattung" || \' \' || flora_organism."Art" AS name,
                flora_organism.name_de AS name_de,
                flora_organism."Familie" AS family,
                flora_organism."Gattung" AS genus
            FROM organism
            LEFT JOIN flora_organism ON organism.organism_id = flora_organism.id
            WHERE organism.organism_type = 2';

        $mapping = \Elastica_Type_Mapping::create(array('class' => array('type' => 'string', 'index' => 'not_analyzed'), 'family' => array('type' => 'string', 'index' => 'not_analyzed'), 'genus' => array('type' => 'string', 'index' => 'not_analyzed')));
        $this->sql('organism', $sql, $mapping);

        // Fauna
        $sql = '
            SELECT
                organism.id,
                organism.organism_type,
                fauna_class.name_de AS class,
                fauna_organism.genus || \' \' || fauna_organism.species AS name,
                fauna_organism.name_de AS name_de,
                fauna_organism.family AS family,
                fauna_organism.genus AS genus
            FROM organism
            LEFT JOIN fauna_organism ON organism.organism_id = fauna_organism.id
            LEFT JOIN fauna_class ON fauna_class.id = fauna_organism.fauna_class_id
            WHERE organism.organism_type = 1';

        $mapping = \Elastica_Type_Mapping::create(array('family' => array('class' => array('type' => 'string', 'index' => 'not_analyzed'), 'type' => 'string', 'index' => 'not_analyzed'), 'genus' => array('type' => 'string', 'index' => 'not_analyzed')));
        $this->sql('organism', $sql, $mapping);
    }

    /**
     * Index all sightings.
     */
    public function sightings() {

        $sql = '
            SELECT
            	inventory_entry.id AS id,
                lat || \', \' || lng AS position,
                inventory_entry.organism_id AS parent
            FROM area_point
            INNER JOIN area ON area.id = area_point.area_id
            INNER JOIN head_inventory ON head_inventory.area_id = area.id
            INNER JOIN inventory ON inventory.head_inventory_id = head_inventory.id
            INNER JOIN inventory_entry ON inventory_entry.inventory_id = inventory.id';

        $mapping = \Elastica_Type_Mapping::create(array('position' => array('type' => 'geo_point')));
        $mapping->setParam('_parent', array('type' => 'organism'));
        $this->sql('sighting', $sql, $mapping);
    }

    /**
     * Index all inventories.
     */
    public function inventories() {

        $sql = '
            SELECT
            	inventory_entry.id AS id,
                lat || \', \' || lng AS position,
                inventory_entry.organism_id AS parent
            FROM area_point
            INNER JOIN area ON area.id = area_point.area_id
            INNER JOIN head_inventory ON head_inventory.area_id = area.id
            INNER JOIN inventory ON inventory.head_inventory_id = head_inventory.id
            INNER JOIN inventory_entry ON inventory_entry.inventory_id = inventory.id';

        $mapping = \Elastica_Type_Mapping::create(array('position' => array('type' => 'geo_point')));
        $mapping->setParam('_parent', array('type' => 'organism'));
        $this->sql('sighting', $sql, $mapping);
    }

    /**
     * Index a SQL query result.
     *
     * @param string $type Object type for the index
     * @param string $sql SQL query
     * @param function $callback
     */
    protected function sql($type, $sql, $mapping = false, $callback = false) {

        $result = db_query($sql);

        // get type object from index
        $type = $this->index->getType($type);
        if ($mapping) {
            $type->setMapping($mapping);
        }

        // fetch as associative array
        $result->setFetchMode(\PDO::FETCH_ASSOC);
        foreach ($result as $data) {

            if ($callback) {
                $data = $callback($data);
            }

            $this->index($type, $data['id'], $data, $data['parent'] ?: false);
        }
    }

    /**
     * Index a associative array.
     *
     * @param \Elastica_Type $type
     * @param int $id ID of document
     * @param array $data Array with data of document
     */
    protected function index(\Elastica_Type $type, $id, $data, $parent = false) {

        // create document
        $document = new \Elastica_Document($id, $data);
        if ($parent) {
            $document->setParent($parent);
        }

        // save to index
        $type->addDocument($document);
    }
}
