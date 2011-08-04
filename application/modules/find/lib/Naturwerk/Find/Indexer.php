<?php

namespace Naturwerk\Find;

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
        $this->inventories();
    }

    /**
     * Index all organsims.
     */
    public function organisms() {
        
        $mapping = \Elastica_Type_Mapping::create(array(
        	'class' => array('type' => 'string', 'index' => 'not_analyzed'),
        	'family' => array('type' => 'string', 'index' => 'not_analyzed'),
        	'genus' => array('type' => 'string', 'index' => 'not_analyzed'),
        ));

        // Flora, Fauna
        $sql = '
            SELECT
                organism.id,
                organism.organism_type,
                \'Pflanzen\' AS class,
                flora_organism.name_de AS name,
                flora_organism."Gattung" || \' \' || flora_organism."Art" AS name_la,
                flora_organism."Familie" AS family,
                flora_organism."Gattung" AS genus,
                \'organism/\' || organism.id AS url
            FROM organism
            LEFT JOIN flora_organism ON organism.organism_id = flora_organism.id
            WHERE organism.organism_type = 2

			UNION

            SELECT
                organism.id,
                organism.organism_type,
                fauna_class.name_de AS class,
                fauna_organism.name_de AS name,
                fauna_organism.genus || \' \' || fauna_organism.species AS name_la,
                fauna_organism.family AS family,
                fauna_organism.genus AS genus,
                \'organism/\' || organism.id AS url
            FROM organism
            LEFT JOIN fauna_organism ON fauna_organism.id = organism.organism_id
            LEFT JOIN fauna_class ON fauna_class.id = fauna_organism.fauna_class_id
            WHERE organism.organism_type = 1';

        $this->sql('organism', $sql, $mapping);
    }

    /**
     * Index all sightings.
     */
    public function sightings() {
        
        $mapping = \Elastica_Type_Mapping::create(array(
        	'position' => array('type' => 'geo_point'),
        	'class' => array('type' => 'string', 'index' => 'not_analyzed'),
        	'family' => array('type' => 'string', 'index' => 'not_analyzed'),
        	'genus' => array('type' => 'string', 'index' => 'not_analyzed'),
        	'user' => array('type' => 'string', 'index' => 'not_analyzed'),
        ));
        $mapping->setParam('_parent', array('type' => 'organism'));

        // Flora, Fauna
        $sql = '
            SELECT
                inventory_entry.id AS id,
                ST_AsGeoJSON(area.geom) AS geom,
                ST_AsGeoJSON(ST_Centroid(area.geom)) AS centroid,
                inventory_entry.organism_id AS parent,
                organism.organism_type,
                \'Pflanzen\' AS class,
                flora_organism.name_de AS name,
                flora_organism."Gattung" || \' \' || flora_organism."Art" AS name_la,
                flora_organism."Familie" AS family,
                flora_organism."Gattung" AS genus,
                head_inventory.name AS inventory,
                ua.field_address_first_name || \' \' || ua.field_address_last_name AS user,
                \'inventory/\' || inventory.head_inventory_id AS url
            FROM area
            INNER JOIN head_inventory ON head_inventory.area_id = area.id
            INNER JOIN inventory ON inventory.head_inventory_id = head_inventory.id
            INNER JOIN inventory_entry ON inventory_entry.inventory_id = inventory.id
            INNER JOIN organism ON organism.id = inventory_entry.organism_id
            INNER JOIN flora_organism ON flora_organism.id = organism.organism_id
            INNER JOIN users ON users.uid = head_inventory.owner_id
    		LEFT JOIN field_data_field_address ua ON ua.entity_id = users.uid
        	WHERE organism.organism_type = 2
    
            UNION

            SELECT
                inventory_entry.id AS id,
                ST_AsGeoJSON(area.geom) AS geom,
                ST_AsGeoJSON(ST_Centroid(area.geom)) AS centroid,
                inventory_entry.organism_id AS parent,
                organism.organism_type,
                fauna_class.name_de AS class,
                fauna_organism.name_de AS name,
                fauna_organism.genus || \' \' || fauna_organism.species AS name_la,
                fauna_organism.family AS family,
                fauna_organism.genus AS genus,
                head_inventory.name AS inventory,
                ua.field_address_first_name || \' \' || ua.field_address_last_name AS user,
                \'inventory/\' || inventory.head_inventory_id AS url
            FROM area
            INNER JOIN head_inventory ON head_inventory.area_id = area.id
            INNER JOIN inventory ON inventory.head_inventory_id = head_inventory.id
            INNER JOIN inventory_entry ON inventory_entry.inventory_id = inventory.id
            INNER JOIN organism ON organism.id = inventory_entry.organism_id
            INNER JOIN fauna_organism ON fauna_organism.id = organism.organism_id
            INNER JOIN fauna_class ON fauna_class.id = fauna_organism.fauna_class_id
            INNER JOIN users ON users.uid = head_inventory.owner_id
    		LEFT JOIN field_data_field_address ua ON ua.entity_id = users.uid
        	WHERE organism.organism_type = 1';

        $this->sql('sighting', $sql, $mapping);
    }

    /**
     * Index all inventories.
     */
    public function inventories() {

        $sql = '
            SELECT
                head_inventory.id AS id,
                head_inventory.name AS name,
                ST_AsGeoJSON(area.geom) AS geom,
                ST_AsGeoJSON(ST_Centroid(area.geom)) AS centroid,
                ua.field_address_first_name || \' \' || ua.field_address_last_name AS user,
                \'inventory/\' || head_inventory.id AS url
            FROM head_inventory
            INNER JOIN area ON area.id = head_inventory.area_id
            INNER JOIN users ON users.uid = head_inventory.owner_id
    		LEFT JOIN field_data_field_address ua ON ua.entity_id = users.uid';

        $mapping = \Elastica_Type_Mapping::create(array(
        	'position' => array('type' => 'geo_point'),
        	'class' => array('type' => 'string', 'index' => 'not_analyzed'),
        	'user' => array('type' => 'string', 'index' => 'not_analyzed'),
        	'family' => array('type' => 'string', 'index' => 'not_analyzed'),
        	'genus' => array('type' => 'string', 'index' => 'not_analyzed'),
        ));

        $this->sql('inventory', $sql, $mapping, array(
            'class' => '
                SELECT
                    DISTINCT fauna_class.name_de AS class
                FROM inventory_entry
                INNER JOIN inventory ON inventory_entry.inventory_id = inventory.id
                INNER JOIN organism ON inventory_entry.organism_id = organism.id
                INNER JOIN fauna_organism ON fauna_organism.id = organism.organism_id
                INNER JOIN fauna_class ON fauna_class.id = fauna_organism.fauna_class_id
                WHERE organism.organism_type = 1 AND inventory.head_inventory_id = :id

                UNION

                SELECT
                    DISTINCT \'Pflanzen\' AS class
                FROM inventory_entry
                INNER JOIN inventory ON inventory_entry.inventory_id = inventory.id
                INNER JOIN organism ON inventory_entry.organism_id = organism.id
                WHERE organism.organism_type = 2 AND inventory.head_inventory_id = :id',

            'family' => '
                SELECT
                    DISTINCT fauna_organism.family AS family
                FROM inventory_entry
                INNER JOIN inventory ON inventory_entry.inventory_id = inventory.id
                INNER JOIN organism ON inventory_entry.organism_id = organism.id
                INNER JOIN fauna_organism ON fauna_organism.id = organism.organism_id
                WHERE organism.organism_type = 1 AND inventory.head_inventory_id = :id

                UNION

                SELECT
                    DISTINCT flora_organism."Familie" AS family
                FROM inventory_entry
                INNER JOIN inventory ON inventory_entry.inventory_id = inventory.id
                INNER JOIN organism ON inventory_entry.organism_id = organism.id
                INNER JOIN flora_organism ON flora_organism.id = organism.organism_id
                WHERE organism.organism_type = 2 AND inventory.head_inventory_id = :id',

            'genus' => '
                SELECT
                    DISTINCT fauna_organism.genus AS genus
                FROM inventory_entry
                INNER JOIN inventory ON inventory_entry.inventory_id = inventory.id
                INNER JOIN organism ON inventory_entry.organism_id = organism.id
                INNER JOIN fauna_organism ON fauna_organism.id = organism.organism_id
                WHERE organism.organism_type = 1 AND inventory.head_inventory_id = :id

                UNION

                SELECT
                    DISTINCT flora_organism."Gattung" AS genus
                FROM inventory_entry
                INNER JOIN inventory ON inventory_entry.inventory_id = inventory.id
                INNER JOIN organism ON inventory_entry.organism_id = organism.id
                INNER JOIN flora_organism ON flora_organism.id = organism.organism_id
                WHERE organism.organism_type = 2 AND inventory.head_inventory_id = :id',
        ));
    }

    /**
     * Index a SQL query result.
     *
     * @param string $type Object type for the index
     * @param string $sql SQL query
     * @param function|array $callback
     */
    protected function sql($type, $sql, $mapping = false, $callback = false) {

        $statement = db_query($sql);

        // get type object from index
        $type = $this->index->getType($type);
        if ($mapping) {
            $type->setMapping($mapping);
        }

        // fetch as associative array
        $result = $statement->fetchAll(\PDO::FETCH_ASSOC);

        $i = 0;
        $total = count($result);
        foreach ($result as $data) {

            // convert geo
            if (isset($data['geom'])) {

                $json = json_decode($data['geom']);
                if ($json) {

                    $data['position'] = array();
                    if ('Polygon' == $json->type) {
                        foreach ($json->coordinates[0] as $coordinate) {
                            $data['position'][] = $coordinate[1] . ', ' . $coordinate[0];
                        }
                    } else if ('Point' == $json->type) {
                        $data['position'][] = $json->coordinates[1] . ', ' . $json->coordinates[0];
                    }
                }

                unset($data['geom']);
            }

            // convert centroid
            if (isset($data['centroid'])) {

                $json = json_decode($data['centroid']);
                if ($json) {

                    if (!isset($data['position'])) {
                        $data['position'] = array();
                    }
                    $data['position'][] = $json->coordinates[1] . ', ' . $json->coordinates[0];
                }

                unset($data['centroid']);
            }

            // array or function callback
            if (is_array($callback)) {

                foreach ($callback as $key => $sql) {

                    $data[$key] = array();

                    // fetch a single column for additional data
                    $result = db_query($sql, array('id' => $data['id']));
                    $result->setFetchMode(\PDO::FETCH_COLUMN, 0);
                    foreach ($result as $object) {
                        $data[$key][] = $object;
                    }
                }

            } else if ($callback) {
                $data = $callback($data);
            }

            $this->index($type, $data['id'], $data, $data['parent'] ?: false);

            echo "  Indexing {$type->getName()}... " . ceil(++$i / $total * 100) . "%\r";
        }
        
        echo "\n";
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
