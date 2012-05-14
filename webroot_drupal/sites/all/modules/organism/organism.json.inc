<?php
/**
 * @file organism.json.inc
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
								{organism_classification} oc
								LEFT JOIN {organism_classification_subscription} ocs ON oc.id = ocs.organism_classification_id
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
 */
function organism_show_classification_classificationjson($classificationid) {
	/* Original example query:
	    SELECT
	        ds.id,
	        ds.name,
	        SUM(sc.cnt)
	    FROM (
	        SELECT
	            id,
	            name,
	            left_value,
	            right_value,
	            prime_father_id
	        FROM
	            drupal_organism_classification oc
	        WHERE
	            oc.parent_id = 37 AND oc.id != oc.parent_id
	        ) ds
	        JOIN (
	            ------------------------------------------------------------
	            -- Count number of organism within each classification group
	            ------------------------------------------------------------
	            SELECT
	                oc.id AS oc_id,
	                oc.left_value AS oc_left_value,
	                oc.right_value AS oc_right_value,
	                oc.prime_father_id AS oc_prime_father_id,
	                COUNT(ocs.id) AS cnt
	            FROM
	                drupal_organism_classification oc
	                JOIN drupal_organism_classification_subscription ocs ON ocs.organism_classification_id = oc.id
	            GROUP BY
	                oc_id,
	                oc_left_value,
	                oc_right_value,
	                oc_prime_father_id
	            ORDER BY
	                oc_id
	            ) sc ON sc.oc_left_value >= ds.left_value AND sc.oc_right_value <= ds.right_value AND sc.oc_prime_father_id = ds.prime_father_id
	    GROUP BY
	        ds.id,
	        ds.name
	 */

	$sql['dbPreStatement'] = "CREATE TEMPORARY VIEW mytempclassifications AS
		SELECT ds.id AS ds_id, ds.ds_name, SUM(sc.cnt) AS cnt FROM (
			SELECT
				id,
				name AS ds_name,
				left_value,
				right_value,
				prime_father_id
			FROM
				{organism_classification} oc
			WHERE
				oc.parent_id = :classificationid AND oc.id != oc.parent_id
			) ds JOIN (
				SELECT
					oc.id AS oc_id,
					oc.left_value AS oc_left_value,
					oc.right_value AS oc_right_value,
					oc.prime_father_id AS oc_prime_father_id,
					COUNT(ocs.id) AS cnt
				FROM
					{organism_classification} oc
					JOIN {organism_classification_subscription} ocs ON ocs.organism_classification_id = oc.id
				GROUP BY
					oc_id,
					oc_left_value,
					oc_right_value,
					oc_prime_father_id
				ORDER BY
					oc_id
				) sc ON sc.oc_left_value >= ds.left_value AND sc.oc_right_value <= ds.right_value AND sc.oc_prime_father_id = ds.prime_father_id
			GROUP BY
				ds_id,
				ds_name
			ORDER BY
				ds_id;";
	/* Set the db fields to select */
	$sql['dbColumns'] = array(
			'ds_id',
			'ds_name',
			'cnt'
	);

	$sql['dbSearchColumns'] = array(
			'ds_name'
	);

	/* DB table to use */
	$sql['dbTable'] = 'mytempclassifications';
	/* Group */
	$sql['id'] = 'ds_id';

	$prearguments = array(
			':classificationid' => $classificationid
	);

	$output = datatable_dbRequest($sql, array(), $prearguments);
	return drupal_json_output($output);
}

/**
 * Menu callback. This will be called for visits to /organism/classification/%/organismjson.
 * @return array
 */
function organism_show_classification_organismjson($classificationid) {
	global $user;
	global $language;
	/*
	    CREATE TEMP VIEW mytempviewdata AS
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
	                    drupal_organism_classification oc
	                WHERE
	                   oc.id = 2
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
	                    drupal_organism_classification oc
	                    JOIN subclassification_constraints scs ON oc.left_value >= scs.left_value AND oc.right_value <= scs.right_value AND oc.prime_father_id = scs.prime_father_id
	                    JOIN drupal_organism_classification_subscription ocs ON ocs.organism_classification_id = oc.id
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
	                    JOIN drupal_organism_classification_subscription ocs ON ocs.organism_id = i_o.organism_id
	                    JOIN drupal_organism_classification oc ON oc.id = ocs.organism_classification_id
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
	                    JOIN drupal_organism_classification oc ON oc.left_value <= inc_org_lowest_class.left_value AND oc.right_value >= inc_org_lowest_class.right_value AND oc.prime_father_id = inc_org_lowest_class.prime_father_id
	            )
	            -- This select could be merged with the CTE above (included_organisms_with_all_classifications)
	            SELECT
	                inc_org_all_class.organism_id AS organism_id,
	                inc_org_all_class.oc_name AS classification_name,
	                ocl.name AS organism_classification_level_name
	            FROM
	                included_organisms_with_all_classifications inc_org_all_class
	                JOIN drupal_organism_classification_level ocl ON ocl.id = inc_org_all_class.organism_classification_level_id
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
	                        drupal_organism_classification oc
	                        JOIN drupal_organism_classification_level ocl ON ocl.id = oc.organism_classification_level_id
	                    WHERE
	                        oc.id = 2
	                )
	            SELECT
	                ocl.name
	            FROM
	                oc_level
	                JOIN drupal_organism_classification_level ocl ON ocl.prime_father_id = oc_level.prime_father_id;
	            -- And another one...
	            CREATE TEMP VIEW mytempfinaltable AS
	                SELECT
	                    osn.organism_id AS organism_id,
	                    osn.name AS osn_name,
	                    ol_name,
	                    oc_CSCF, oc_class, oc_order, oc_family, oc_genus, oc_subgenus
	                FROM crosstab
	                    (
	                       'SELECT organism_id AS rowid, organism_classification_level_name AS category, classification_name AS values FROM mytempviewdata',
	                       'SELECT name FROM mytempviewlevel'
	                    ) AS nn(
	                       organism_id integer, oc_CSCF text, oc_class text, oc_order text, oc_family text, oc_genus text, oc_subgenus text
	                    ) JOIN drupal_organism_scientific_name osn ON osn.organism_id = nn.organism_id
	                       LEFT JOIN (SELECT organism_id, name AS ol_name FROM drupal_organism_lang WHERE languages_language = 'de') AS mylang ON mylang.organism_id = nn.organism_id;
	    SELECT organism_id, osn_name, ol_name, oc_CSCF, oc_class, oc_order, oc_family, oc_genus, oc_subgenus FROM mytempfinaltable;
	    DROP VIEW mytempviewdata;
	    DROP VIEW mytempviewlevel;
	    DROP VIEW mytempfinaltable;
	 */
	$classificationleveldata = organism_get_available_classification_levels(
		$classificationid);

	if (empty($classificationleveldata)) {
		return drupal_json_output(array('error' => 'No classifications found'));
	}

	// build an array with all classification level names
	$levelnamearray = array();
	foreach ($classificationleveldata as $classificationlevel) {
		$levelname = 'oc_' . $classificationlevel->name;
		// make sure we do not have any illegal characters
		assert(preg_match('/^[a-zA-Z_]+$/', $levelname) == 1);
		$levelnamearray[] = $levelname;
	}
	$levelnamesdatastring = implode(' text, ', $levelnamearray) . ' text';
	$levelnamesstring = implode(', ', $levelnamearray);

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
		-- And another one...
		CREATE TEMP VIEW finaltable AS
			SELECT
				osn.organism_id AS organism_id,
				osn.name AS osn_name,
				ol_name,
				$levelnamesstring
			FROM crosstab
				(
				   'SELECT organism_id AS rowid, organism_classification_level_name AS category, classification_name AS values FROM mytempviewdata',
				   'SELECT name FROM mytempviewlevel'
				) AS nn(
				   organism_id integer, $levelnamesdatastring
				) JOIN {organism_scientific_name} osn ON osn.organism_id = nn.organism_id
				   LEFT JOIN (SELECT organism_id, name AS ol_name FROM {organism_lang} WHERE languages_language = :language) AS mylang ON mylang.organism_id = nn.organism_id
	";

	/* Set the db fields to select */
	$dbcolumns = array_merge(
		array('organism_id',
				'osn_name',
				'ol_name'
		),
		$levelnamearray);
	$sql['dbColumns'] = $dbcolumns;

	$sql['dbSearchColumns'] = array_merge(
		array('ol_name',
				'osn_name'
		),
		$levelnamearray);

	/* DB table to use */
	$sql['dbTable'] = "finaltable";
	/* Group */
	$sql['id'] = 'organism_id';

	$prearguments = array(
			':classificationid' => $classificationid,
			':language' => isset($user->language) ? $user->language
					: $language->language
	);

	$arguments = array();

	$output = datatable_dbRequest($sql, $arguments, $prearguments);
	return drupal_json_output($output);
}

/**
 * Menu callback. This will be called for visits to /organism/%organism/organismjson.
 * @param $organismdata
 * 	Array with all data about an organism.
 * @return array
 */
function organism_show_organism_json($organism) {
	if (empty($organismdata)) {
		drupal_set_message(t('No such organism available.'), 'error');
		return;
	}
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
			':oid' => $organism['id']
	);

	$output = datatable_dbRequest($sql, $arguments);
	return drupal_json_output($output);
}

/**
 * Menu callback; AJAX autocomplete for selecting organisms
 *
 * @return array Array of organisms matching the term having the following keys:
 *  - "id": The ID of the organism
 *  - "label": The name of the organism
 *  - "label_latin": The latin name of the organism
 *  - "old_id"[Optional]: The ID of the initially found organism
 *    (only available if the found organism has a reference to an official one)
 *  - "old_label"[Optional]: The name of the initially found organism
 *    (only available if the found organism has a reference to an official one)
 *  - "old_label_latin"[Optional]: The latin name of the initially found organism
 *    (only available if the found organism has a reference to an official one)
 */
function organism_search_organism_json($classification = false, $term = false,
		$plain = false) {
	global $user;

	module_load_include('inc', 'organism', 'organism.artgroup');

	$langcode = isset($user->language)  || $user->language == '' ? $user->language : 'de';

	if (!$classification)
		isset($_REQUEST['inv_id']) ? $classification = $_REQUEST['inv_id']
				: $classification = false;
	$type_id = $classification;

	$fauna = ($type_id == 16 ? false : true);

	if (!$term) {
		if (isset($_REQUEST['term'])) {
			$term = $_REQUEST['term'];
		} else {
			return drupal_json_output(array('label' => 'No search term'));
		}
	}

	$artgroup_join = "";

	if (isset($_REQUEST['artgroup'])) {
		$artgroup = $_REQUEST['artgroup'];
		if ($artgroup == '3')
			$artgroup = false;
		if ($artgroup > 0 && $artgroup < 1000000) {
			$artgroup_ids = organism_artgroup_get_children($artgroup);
			$artgroup_ids[] = $artgroup;
			$artgroup_join = '
								LEFT JOIN {organism_artgroup} AS oa ON oa.id=oas.organism_artgroup_id
								AND ('
					. organism_artgroup_get_sql_from_id($artgroup_ids, 'oa.id')
					. ')';
		}
	}

	$term = strtolower($term);
	$terms = explode(' ', trim($term));

	// why limit result with the length of the search term??
	// 	$limit = strlen(trim($term)) > 3 ? 100000 : 20;
	$limit = 30;

	$organisms = array();

	$params = array();
	if ($type_id)
		$params[':inventoryTypeId'] = $type_id;
	$params[':limit'] = $limit;
	$params[':lang'] = $langcode;
	$params[':term'] = '%' . trim($term) . '%';

	if (count($terms) > 2) {
		$params[':term1'] = $terms[0] . '% ' . $terms[1] . '% ' . $terms[2]
				. '%';
	} elseif (count($terms) > 1) {
		$params[':term1'] = $terms[0] . '% ' . $terms[1] . '%';
	} else {
		$params[':term1'] = $terms[0] . '%';
	}

	$i = 1;
	$query = 'SELECT DISTINCT ON (o.id) o.id as organism_id, ol.name as ol_name, osn.name as osn_name, oas.organism_artgroup_id artg_id
				FROM {organism} AS o
				LEFT JOIN {organism_lang} AS ol ON ol.organism_id = o.id AND ol.languages_language = :lang
				INNER JOIN {organism_scientific_name} AS osn ON osn.organism_id = o.id
				LEFT JOIN {organism_artgroup_subscription} AS oas ON oas.organism_id=o.id
				' . $artgroup_join
			. '
				WHERE ol.name ILIKE :term OR osn.name ILIKE :term1
				ORDER BY o.id
				LIMIT :limit;';

	$result = db_query($query, $params)->fetchAllAssoc('organism_id');
	foreach ($result as $record) {

		$artg_attr = '';
		$det_methods = '';
		if ($record->artg_id) {
			$artg_attr = organism_artgroup_get_attributes_ids($record->artg_id);
			$det_methods = organism_artgroup_get_detmethods_ids(
				$record->artg_id);
		}
		$organism = array(
				'id' => $record->organism_id,
				'label' => strval($record->ol_name),
				'label_latin' => $record->osn_name,
				'attributes' => $artg_attr,
				'det_methods' => $det_methods,
				'artgr' => $record->artg_id,
		);

		$organisms[] = $organism;
	}

	if (!$plain)
		$organisms = drupal_json_output($organisms);

	debug($organisms, 'JSON_OUTPUT', true);
	return $organisms;
}

?>
