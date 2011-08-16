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
        $this->index->create(array('settings' => array(
            'analysis' => array(
                'analyzer' => array(
                    'sortable' => array(
                        'tokenizer' => 'keyword',
                        'filter' => array(
                            'standard',
                            'lowercase',
                            'asciifolding',
        )
        )
        )
        )
        )), true);
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
            'name' => array('type' => 'string', 'analyzer' => 'sortable'),
            'name_la' => array('type' => 'string', 'analyzer' => 'sortable'),
            'class' => array('type' => 'string', 'index' => 'not_analyzed'),
            'family' => array('type' => 'string', 'index' => 'not_analyzed'),
            'genus' => array('type' => 'string', 'index' => 'not_analyzed'),
            'user' => array('type' => 'string', 'index' => 'not_analyzed'),
        ));

        // Flora, Fauna
        $sql = '
            SELECT
                organism.id,
                organism.organism_type,
                \'Pflanzen\' AS class,
                flora_organism.name_de AS name,
                ARRAY_TO_STRING(ARRAY[flora_organism."Gattung", flora_organism."Art"], \' \') AS name_la,
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
                ARRAY_TO_STRING(ARRAY[fauna_organism.genus, fauna_organism.species], \' \') AS name_la,
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
            'name' => array('type' => 'string', 'analyzer' => 'sortable'),
            'name_la' => array('type' => 'string', 'analyzer' => 'sortable'),
            'position' => array('type' => 'geo_point'),
            'class' => array('type' => 'string', 'index' => 'not_analyzed'),
            'family' => array('type' => 'string', 'index' => 'not_analyzed'),
            'genus' => array('type' => 'string', 'index' => 'not_analyzed'),
            'inventory' => array('type' => 'string', 'index' => 'not_analyzed'),
            'user' => array('type' => 'string', 'index' => 'not_analyzed'),
            'date' => array('type' => 'date', 'format' => 'yyyy-MM-dd'),
        ));
        $mapping->setParam('_parent', array('type' => 'organism'));

        // Flora, Fauna
        $sql = '
            SELECT
                inventory_entry.id AS id,
                head_inventory.shared AS shared,
                ST_AsGeoJSON(area.geom) AS geom,
                ST_AsGeoJSON(ST_Centroid(area.geom)) AS centroid,
                inventory_entry.organism_id AS parent,
                organism.organism_type,
                \'Pflanzen\' AS class,
                flora_organism.name_de AS name,
                ARRAY_TO_STRING(ARRAY[flora_organism."Gattung", flora_organism."Art"], \' \') AS name_la,
                flora_organism."Familie" AS family,
                flora_organism."Gattung" AS genus,
                head_inventory.name AS inventory,
                ua.field_address_first_name || \' \' || ua.field_address_last_name AS user,
                \'inventory/\' || inventory.head_inventory_id AS url,
                (SELECT e.value FROM inventory_type_attribute_inventory_entry e INNER JOIN inventory_type_attribute a ON a.id = e.inventory_type_attribute_id WHERE a.name = \'Funddatum\' AND e.inventory_entry_id = inventory_entry.id) as date
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
                head_inventory.shared AS shared,
                ST_AsGeoJSON(area.geom) AS geom,
                ST_AsGeoJSON(ST_Centroid(area.geom)) AS centroid,
                inventory_entry.organism_id AS parent,
                organism.organism_type,
                fauna_class.name_de AS class,
                fauna_organism.name_de AS name,
                ARRAY_TO_STRING(ARRAY[fauna_organism.genus, fauna_organism.species], \' \') AS name_la,
                fauna_organism.family AS family,
                fauna_organism.genus AS genus,
                head_inventory.name AS inventory,
                ua.field_address_first_name || \' \' || ua.field_address_last_name AS user,
                \'inventory/\' || inventory.head_inventory_id AS url,
                (SELECT e.value FROM inventory_type_attribute_inventory_entry e INNER JOIN inventory_type_attribute a ON a.id = e.inventory_type_attribute_id WHERE a.name = \'Funddatum\' AND e.inventory_entry_id = inventory_entry.id) as date
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

        $this->sql('sighting', $sql, $mapping, array(
            'access' => '
            	SELECT
            		h.owner_id AS access
    			FROM inventory_entry e
            	INNER JOIN inventory i ON i.id = e.inventory_id
            	INNER JOIN head_inventory h ON h.id = i.head_inventory_id
    			WHERE e.id = :id
            	
    			UNION
            	
            	SELECT
            		DISTINCT u.uid AS access
    			FROM inventory_entry e
            	INNER JOIN inventory i ON i.id = e.inventory_id
            	INNER JOIN head_inventory h ON h.id = i.head_inventory_id
    			INNER JOIN sgroup_inventory g ON g.hiid = h.id
    			INNER JOIN sgroup_users u ON u.sgid = g.sgid
    			WHERE (read = 1 OR admin = 1) AND e.id = :id',
        ));
    }

    /**
     * Index all inventories.
     */
    public function inventories() {

        $mapping = \Elastica_Type_Mapping::create(array(
            'name' => array('type' => 'string', 'analyzer' => 'sortable'),
            'user' => array('type' => 'string', 'index' => 'not_analyzed'),
            'position' => array('type' => 'geo_point'),
            'class' => array('type' => 'string', 'index' => 'not_analyzed'),
            'user' => array('type' => 'string', 'index' => 'not_analyzed'),
            'family' => array('type' => 'string', 'index' => 'not_analyzed'),
            'genus' => array('type' => 'string', 'index' => 'not_analyzed'),
            'date' => array('type' => 'date', 'format' => 'yyyy-MM-dd'),
        ));

        $sql = '
            SELECT
                head_inventory.id AS id,
                head_inventory.shared AS shared,
                head_inventory.name AS name,
                ST_AsGeoJSON(area.geom) AS geom,
                ST_AsGeoJSON(ST_Centroid(area.geom)) AS centroid,
                ARRAY_TO_STRING(ARRAY[ua.field_address_first_name, ua.field_address_last_name], \' \') AS user,
                \'inventory/\' || head_inventory.id AS url
            FROM head_inventory
            INNER JOIN area ON area.id = head_inventory.area_id
            INNER JOIN users ON users.uid = head_inventory.owner_id
            LEFT JOIN field_data_field_address ua ON ua.entity_id = users.uid';

        $this->sql('inventory', $sql, $mapping, array(
            'access' => '
            	SELECT
            		owner_id AS access
    			FROM head_inventory
    			WHERE id = :id
            	
    			UNION
            	
            	SELECT
            		DISTINCT u.uid AS access
    			FROM head_inventory h
    			INNER JOIN sgroup_inventory g ON g.hiid = h.id
    			INNER JOIN sgroup_users u ON u.sgid = g.sgid
    			WHERE (read = 1 OR admin = 1) AND h.id = :id',

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

            'date' => '
                SELECT
                    DISTINCT e.value AS date
                FROM inventory_type_attribute_inventory_entry e
                INNER JOIN inventory_type_attribute a ON a.id = e.inventory_type_attribute_id
                INNER JOIN inventory_entry ie ON ie.id = e.inventory_entry_id
                INNER JOIN inventory i ON ie.inventory_id = i.id
                WHERE a.name = \'Funddatum\' AND i.head_inventory_id = :id',
        ));
    }

    protected static function convert($data) {


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

        // convert date
        if (isset($data['date'])) {

            $data['date'] = date('Y-m-d', strtotime($data['date']));
        }

        return $data;
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

            $data = self::convert($data);

            // array or function callback
            if (is_array($callback)) {

                foreach ($callback as $key => $sql) {

                    $data[$key] = array();

                    // fetch a single column for additional data
                    $result = db_query($sql, array('id' => $data['id']));
                    $result->setFetchMode(\PDO::FETCH_ASSOC);
                    foreach ($result as $object) {

                        $object = self::convert($object);

                        $data[$key][] = $object[$key];
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
