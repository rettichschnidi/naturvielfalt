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
	global $language;
	/*
	    CREATE TEMP VIEW mytempviewdata AS WITH subclassification_constraints AS (
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
	           oc.id = 28197
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
	    
	    CREATE TEMP VIEW mytempviewlevel AS 
	        WITH oc_level AS (SELECT
	                DISTINCT ocl.prime_father_id
	            FROM
	                drupal_organism_classification oc
	                JOIN drupal_organism_classification_level ocl ON ocl.id = oc.organism_classification_level_id
	            WHERE
	                oc.id = 28197
	        )
	    SELECT
	        ocl.name
	    FROM
	        oc_level
	        JOIN drupal_organism_classification_level ocl ON ocl.prime_father_id = oc_level.prime_father_id;
	    
	    SELECT * FROM crosstab
	    (
	        'SELECT organism_id AS rowid, organism_classification_level_name AS category, classification_name AS values FROM mytempviewdata',
	        'SELECT name FROM mytempviewlevel'
	    ) AS(
	        organism_id text,
	        cat1 text,
	        cat2 text
	    );
	    DROP VIEW mytempviewdata;
	    DROP VIEW mytempviewlevel;
	 */
	$classificationleveldata = organism_get_available_classification_levels(
		$classificationid);

	// build an array with all classification level names
	$levelnamearray = array();
	foreach ($classificationleveldata as $classificationlevel) {
		$levelnamearray[] = 'oc_' . $classificationlevel->name;
	}
	$levelnamesdatastring = implode(' text, ', $levelnamearray) . ' text';

	$sql['dbPreStatament'] = "CREATE TEMP VIEW mytempviewdata AS WITH subclassification_constraints AS (
	        -- Get the left_value and right_value
	        -- Those values will allow us to recieve the subtree
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
	    ), all_organisms AS (
	        -- Get all organisms within those subclassifications
	        SELECT
	            ocs.organism_id AS organism_id
	        FROM
	            {organism_classification} oc
	            JOIN subclassification_constraints scs ON oc.left_value >= scs.left_value AND oc.right_value <= scs.right_value AND oc.prime_father_id = scs.prime_father_id
	            JOIN {organism_classification_subscription} ocs ON ocs.organism_classification_id = oc.id
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
	            JOIN {organism_classification_subscription} ocs ON ocs.organism_id = a_o.organism_id
	            JOIN {organism_classification} oc ON oc.id = ocs.organism_classification_id
	    ), all_organisms_with_levels AS (
	        SELECT
	            *
	        FROM
	            all_organisms_and_lowest_classification a_olc
	            JOIN {organism_classification} oc ON oc.left_value <= a_olc.left_value AND oc.right_value >= a_olc.right_value AND oc.prime_father_id = a_olc.prime_father_id
	    )
	    SELECT
	        a_olc.organism_id AS organism_id,
	        a_olc.name AS classification_name,
	        ocl.name AS organism_classification_level_name
	    FROM
	        all_organisms_with_levels a_olc
	        JOIN {organism_classification_level} ocl ON ocl.id = a_olc.organism_classification_level_id
	    ORDER BY
	        organism_id,
	        ocl.left_value;
	    
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
	$sql['dbColumns'] = array_merge(array('organism_id'), $levelnamearray);
	
	// 
	$sql['dbSearchColumns'] = $levelnamearray;

	/* DB table to use */
	$sql['dbTable'] = "crosstab
	    (
	        'SELECT organism_id AS rowid, organism_classification_level_name AS category, classification_name AS values FROM mytempviewdata',
	        'SELECT name FROM mytempviewlevel'
	    ) AS(
	        organism_id text, $levelnamesdatastring
	    )";
	/* Group */
	$sql['id'] = 'organism_id';

	$arguments = array(
			':classificationid' => $classificationid
	);

	$output = datatable_dbRequest($sql, $arguments);
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
