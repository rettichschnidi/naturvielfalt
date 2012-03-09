<?php
/**
 * @file organism.overview.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'organism', 'organism');

/**
 * Menu callback. This will be called for visits to /organism/json.
 * Per default it returns all classifier as a json.
 * Post-Requests from datatable are supportet as well (search, partial response, offset, etc).
 * 
 * Example JSON answer:
 * {
 *  "page":null,
 *  "total":4,
 *  "rows":[
 *      {
 *          "id":null,
 *          "cell":{
 *              "oc_id":"1",
 *              "oc_name":"SwissLichens",
 *              "levelcount":"1",
 *              "cnt":"666"
 *          }
 *      },
 *      {
 *          "id":null,
 *          "cell":{
 *              "oc_id":"7382",
 *              "oc_name":"CSCF",
 *              "levelcount":"5",
 *              "cnt":"1337"
 *          }
 *      }
 *  ]
 * }
 * @param none, except the fileds via post
 * @return json string
 */
function organism_show_classifiers_json() {
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'oc.id AS oc_id',
			'oc.name AS oc_name',
			'COUNT(ocl.id) - 1 AS levelcount',
			'organismcount.cnt'
	);

	$sql['dbSearchColumns'] = array(
			'oc.id',
			'oc.name'
	);

	/* Original query:
	    SELECT
	        oc.id AS oc_id,
	        oc.name AS oc_name,
	        COUNT(ocl.id) - 1 AS levelcount,
	        organismcount.cnt
	    FROM
	        drupal_organism_classification oc 
	        JOIN drupal_organism_classification_level ocl ON oc.organism_classification_level_id = ocl.prime_father_id
	        JOIN (
	            SELECT
	                oc.prime_father_id AS prime_father_id,
	                COUNT(ocs.id) AS cnt
	            FROM
	                drupal_organism_classification_subscription ocs
	                JOIN drupal_organism_classification oc ON oc.id = ocs.organism_classification_id 
	            GROUP BY
	                prime_father_id
	        )
	        AS organismcount ON organismcount.prime_father_id = oc.prime_father_id
	    WHERE
	        oc.prime_father_id = oc.id
	    GROUP BY
	        oc_id,
	        oc_name,
	        organismcount.cnt;
	 */
	/* DB table to use */
	$sql['dbTable'] = "{organism_classification} oc";
	/* Joins to use */
	$sql['dbJoins'] = "JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.prime_father_id
						JOIN (
							SELECT
								oc.prime_father_id AS prime_father_id,
								COUNT(ocs.id) AS cnt
							FROM
								{organism_classification_subscription} ocs
								JOIN {organism_classification} oc ON oc.id = ocs.organism_classification_id 
							GROUP BY
								prime_father_id
						) AS organismcount ON organismcount.prime_father_id = oc.prime_father_id";
	/* Group */
	$sql['dbGroupBy'] = "oc_id, oc_name, organismcount.cnt";
	$sql['dbWhere'] = 'oc.prime_father_id = oc.id';
	$sql['id'] = 'oc_id';

	$output = datatable_dbRequest($sql);
	return drupal_json_output($output);
}

/**
 * Menu callback. This will be called for visits to /organism/classification/%/classificationjson.
 * @return array
 * @todo Find a better title
 */
function organism_show_classification_classificationjson($classificationid) {
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'oc.id AS oc_id',
			'oc.name AS oc_name',
			'oc.organism_classification_level_id AS classification_level_id',
			'ocl.name AS levelname'
	);

	$sql['dbSearchColumns'] = array(
			'oc.id',
			'oc.name',
			'ocl.name'
	);

	/* DB table to use */
	$sql['dbTable'] = '{organism_classification} oc';
	/* Joins to use */
	$sql['dbJoins'] = 'JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.id';
	/* Group */
	$sql['dbGroupBy'] = 'oc_id, oc_name, classification_level_id, levelname';
	$sql['dbWhere'] = 'oc.parent_id != oc.id AND oc.parent_id = :ocid';
	$sql['id'] = 'oc_id';

	$arguments = array(
			':ocid' => $classificationid
	);

	$output = datatable_dbRequest($sql, $arguments);
	return drupal_json_output($output);
}

/**
 * Menu callback. This will be called for visits to /organism/classification/%/organismjson.
 * @return array
 */
function organism_show_classification_organismjson($classificationid) {
	global $user;
	/*
	    CREATE TEMP VIEW mytempviewdata AS 
	        WITH subclassification_constraints AS (
	            -- Get the left_value and right_value
	            -- Those values will allow us to recieve the subtree
	            SELECT
	                --id,
	                --name,
	                prime_father_id,
	                left_value,
	                right_value
	            FROM
	                drupal_organism_classification oc
	            WHERE
	               oc.id = 777
	        ), all_organisms AS (
	            -- Get all organisms within those subclassifications
	            SELECT
	                ocs.organism_id AS organism_id
	            FROM
	                drupal_organism_classification oc
	                JOIN subclassification_constraints scs ON oc.left_value >= scs.left_value AND oc.right_value <= scs.right_value AND oc.prime_father_id = scs.prime_father_id
	                JOIN drupal_organism_classification_subscription ocs ON ocs.organism_classification_id = oc.id
	            ORDER BY
	                oc.left_value ASC
	        ), all_organisms_and_lowest_classification AS (
	            SELECT
	                a_o.organism_id AS organism_id,
	                --ocs.id AS ocs_id,
	                oc.name AS oc_name,
	                oc.left_value,
	                oc.right_value,
	                oc.prime_father_id
	            FROM
	                all_organisms a_o
	                JOIN drupal_organism_classification_subscription ocs ON ocs.organism_id = a_o.organism_id
	                JOIN drupal_organism_classification oc ON oc.id = ocs.organism_classification_id
	        ), all_organisms_with_levels AS (
	            SELECT
	 *
	            FROM
	                all_organisms_and_lowest_classification a_olc
	                JOIN drupal_organism_classification oc ON oc.left_value <= a_olc.left_value AND oc.right_value >= a_olc.right_value AND oc.prime_father_id = a_olc.prime_father_id
	        )
	        SELECT
	            a_olc.organism_id AS organism_id,
	            a_olc.name AS classification_name,
	            ocl.name AS organism_classification_level_name
	        FROM
	            all_organisms_with_levels a_olc
	            JOIN drupal_organism_classification_level ocl ON ocl.id = a_olc.organism_classification_level_id
	        ORDER BY
	            organism_id,
	            ocl.left_value;
	    -- create temporary level view
	    CREATE TEMP VIEW mytempviewlevel AS 
	       WITH oc_level AS (SELECT
	               DISTINCT ocl.prime_father_id
	           FROM
	               drupal_organism_classification oc
	               JOIN drupal_organism_classification_level ocl ON ocl.id = oc.organism_classification_level_id
	           WHERE
	               oc.id = 777
	       )
	    SELECT
	       ocl.name
	    FROM
	       oc_level
	       JOIN drupal_organism_classification_level ocl ON ocl.prime_father_id = oc_level.prime_father_id;
	    -- create final datatable      
	    SELECT osn.organism_id, osn.name, mylang.languages_language AS lang, oc_CSCF, oc_class, oc_order, oc_family, oc_genus, oc_subgenus FROM crosstab
	            (
	                'SELECT organism_id AS rowid, organism_classification_level_name AS category, classification_name AS values FROM mytempviewdata',
	                'SELECT name FROM mytempviewlevel'
	            ) AS nn(
	                organism_id integer, oc_CSCF text, oc_class text, oc_order text, oc_family text, oc_genus text, oc_subgenus text
	            ) JOIN drupal_organism_scientific_name osn ON osn.organism_id = nn.organism_id
	                 LEFT JOIN (SELECT * FROM drupal_organism_lang WHERE languages_language = 'de') AS mylang ON mylang.organism_id = nn.organism_id;
	    DROP VIEW mytempviewdata;
	    DROP VIEW mytempviewlevel;
	 */
	$classificationleveldata = organism_get_available_classification_levels(
		$classificationid);

	// build an array with all classification level names
	$levelnamearray = array();
	foreach ($classificationleveldata as $classificationlevel) {
		$levelnamearray[] = pg_escape_string('oc_' . $classificationlevel->name);
	}
	$levelnamesdatastring = implode(' text, ', $levelnamearray) . ' text';

	$sql['dbPreStatement'] = "CREATE TEMP VIEW mytempviewdata AS
		WITH subclassification_constraints AS (
	        -----------------------------------------------
	        -- Get the left_value and right_value
	        -- Those values will allow us to recieve the subtree
	        -- Example result:
	        --  prime_father_id | left_value | right_value 
	        -- -----------------+------------+-------------
			--                1 |          4 |           7
	        -----------------------------------------------
	        SELECT
	            --id,
	            --name,
	            prime_father_id,
	            left_value,
	            right_value
	        FROM
	            {organism_classification} oc
	        WHERE
	           oc.id = :classificationid
	    ), included_organisms AS (
	        -----------------------------------------------------
	        -- Get all organisms within those subclassifications
	        -- Example result:
	        --  organism_id 
	        -- -------------
	        --        20706
	        --        20707
	        --        20708
	        --        20709
	        --        20710
	        --        20757
	        --        20755
	        -----------------------------------------------------
	        SELECT
	            ocs.organism_id AS organism_id
	        FROM
	            {organism_classification} oc
	            JOIN subclassification_constraints scs ON oc.left_value >= scs.left_value AND oc.right_value <= scs.right_value AND oc.prime_father_id = scs.prime_father_id
	            JOIN {organism_classification_subscription} ocs ON ocs.organism_classification_id = oc.id
	        ORDER BY
	            oc.left_value ASC
	    ), included_organisms_with_lowest_classification AS (
	        ----------------------------------------------------------------------------------
	        -- For each organism, get the classification it is subscribed to
	        -- Examble result:
	        --  organism_id |      oc_name       | left_value | right_value | prime_father_id 
	        -- -------------+--------------------+------------+-------------+-----------------
	        --        20706 | Acipenser          |          5 |           6 |               1
	        --        20707 | Anguilla           |         11 |          12 |               1
	        --        20708 | Alosa              |         17 |          18 |               1
	        --        20709 | Alosa              |         17 |          18 |               1
	        --        20710 | Alosa              |         17 |          18 |               1
	        --        20757 | Barbatula          |         21 |          22 |               1
	        --        20755 | Cobitis            |         23 |          24 |               1
	        --        20756 | Misgurnus          |         25 |          26 |               1
	        --        20726 | Abramis            |         29 |          30 |               1
	        ----------------------------------------------------------------------------------
	        SELECT
	            i_o.organism_id AS organism_id,
	            --ocs.id AS ocs_id,
	            oc.name AS oc_name,
	            oc.left_value,
	            oc.right_value,
	            oc.prime_father_id
	        FROM
	            included_organisms i_o
	            JOIN {organism_classification_subscription} ocs ON ocs.organism_id = i_o.organism_id
	            JOIN {organism_classification} oc ON oc.id = ocs.organism_classification_id
	    ), included_organisms_with_all_classifications AS (
	        ------------------------------------------------------------------------
	        -- Join table above (included_organisms_with_lowest_classification) to
	        -- include all levels
	        --  organism_id |      oc_name       | organism_classification_level_id 
	        -- -------------+--------------------+----------------------------------
	        --        20706 | CSCF               |                                1
	        --        20706 | Actinopterygii     |                                2
	        --        20706 | Acipenseriformes   |                                3
	        --        20706 | Acipenseridae      |                                4
	        --        20706 | Acipenser          |                                5
	        --        20707 | CSCF               |                                1
	        --        20707 | Actinopterygii     |                                2
	        --        20707 | Anguilliformes     |                                3
	        --        20707 | Anguillidae        |                                4
	        --        20707 | Anguilla           |                                5
	        ------------------------------------------------------------------------
	        SELECT
	            inc_org_lowest_class.organism_id, oc.name AS oc_name, oc.organism_classification_level_id
	        FROM
	            included_organisms_with_lowest_classification inc_org_lowest_class
	            JOIN {organism_classification} oc ON oc.left_value <= inc_org_lowest_class.left_value AND oc.right_value >= inc_org_lowest_class.right_value AND oc.prime_father_id = inc_org_lowest_class.prime_father_id
	    )
	    -- This select could be merged with the CTE above (included_organisms_with_all_classifications)
	    SELECT
	        inc_org_all_class.organism_id AS organism_id,
	        inc_org_all_class.oc_name AS classification_name,
	        ocl.name AS organism_classification_level_name
	    FROM
	        included_organisms_with_all_classifications inc_org_all_class
	        JOIN {organism_classification_level} ocl ON ocl.id = inc_org_all_class.organism_classification_level_id
	    ORDER BY
	        organism_id,
	        ocl.left_value;
	    -- -------------------------------------------------
	    -- Create a view with all classification level names
	    -- Example result:
	    --    name   
	    -- ----------
	    --  CSCF
	    --  class
	    --  order
	    --  family
	    --  genus
	    --  subgenus
	    -- --------------------------------------------------
	    CREATE TEMP VIEW mytempviewlevel AS 
	        WITH oc_level AS (SELECT
	                DISTINCT ocl.prime_father_id
	            FROM
	                {organism_classification} oc
	                JOIN {organism_classification_level} ocl ON ocl.id = oc.organism_classification_level_id
	            WHERE
	                oc.id = :classificationid
	        )
	    SELECT
	        ocl.name
	    FROM
	        oc_level
	        JOIN {organism_classification_level} ocl ON ocl.prime_father_id = oc_level.prime_father_id;
		";

	/* Set the db fields to select */
	$sql['dbColumns'] = array_merge(
		array('osn.organism_id AS organism_id',
				'osn.name AS osn_name',
				'mylang.name AS ol_name'
		),
		$levelnamearray);

	$sql['dbSearchColumns'] = array_merge(array('ol_name'), $levelnamearray);

	/* DB table to use */
	$sql['dbTable'] = "crosstab
	    (
	        'SELECT organism_id AS rowid, organism_classification_level_name AS category, classification_name AS values FROM mytempviewdata',
	        'SELECT name FROM mytempviewlevel'
	    ) AS nn(
	        organism_id integer, $levelnamesdatastring
	    )";
	$sql['dbJoins'] = 'JOIN {organism_scientific_name} osn ON osn.organism_id = nn.organism_id
	        LEFT JOIN (SELECT * FROM {organism_lang} WHERE languages_language = :language) AS mylang ON mylang.organism_id = nn.organism_id';

	/* Group */
	$sql['id'] = 'organism_id';

	$prearguments = array(
			':classificationid' => $classificationid
	);

	$arguments = array(
			':language' => $user->language
	);

	$output = datatable_dbRequest($sql, $arguments, $prearguments);
	return drupal_json_output($output);
}
/**
 * Menu callback. This will be called for visits to /organism/%/organismjson.
 * @return array
 */
function organism_show_organism_json($organismid) {
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'osn.id AS osn_id',
			'osn.name AS osn_name',
			'osn.organism_id AS o_id',
	);

	$sql['dbSearchColumns'] = array(
			'osn.id',
			'osn.name',
			'osn.organism_id'
	);

	/* DB table to use */
	$sql['dbTable'] = '{organism_scientific_name} osn';
	/* Group */
	$sql['dbWhere'] = 'osn.organism_id = :oid';
	$sql['id'] = 'osn_id';

	$arguments = array(
			':oid' => $organismid
	);

	$output = datatable_dbRequest($sql, $arguments);
	return drupal_json_output($output);
}
?>
