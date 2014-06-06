<?php
/**
 * @file organism.json.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'organism', 'organism');
module_load_include('inc', 'datatable');

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
			'organismcount.cnt AS cnt'
	);

	$sql['dbSearchColumns'] = array(
			'oc.id',
			'oc.name',
			'organismcount.cnt'
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
	
	    Original example answer:
	     oc_id |   oc_name    | levelcount |  cnt  
	    -------+--------------+------------+-------
	     33400 | Swisslichens |          2 |  1354
	         1 | CSCF         |          7 | 21501
	     28643 | CRSF         |          4 |  4044
	     33386 | Fungus       |          1 |  5877
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
	            oc.parent_id = 1 AND oc.id != oc.parent_id
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
	        ds.name;
	        
	 Original example answer:
	      id   |      name      |  sum  
	    -------+----------------+-------
	     28599 | Polychaeta     |     1
	     28604 | Reptilia       |    20
	      2603 | Gastropoda     |   305
	     28246 | Malacostraca   |    70
	      2289 | Bivalvia       |    30
	         2 | Actinopterygii |    72
	      3228 | Insecta        | 19256
	      2347 | Diplopoda      |   166
	     28370 | Mammalia       |   128
	       189 | Arachnida      |  1028
	      1607 | Aves           |   401
	       150 | Amphibia       |    22
	      2341 | Cyclostomi     |     2
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
			'ds_name',
			'cnt'
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
		-- Get recursive all subclassification for a certain classification
		-- -------------------------------------------------
		CREATE TEMP VIEW tmp_classification_recursive AS
			WITH    RECURSIVE
			q AS
			(
			SELECT  oc.id,  oc.name as classification_name, ocl.name as organism_classification_level_name, oc.organism_classification_level_id
			FROM  {organism_classification} oc
			INNER JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.id
			WHERE oc.id = :classificationid			
			UNION ALL			
			SELECT  oc1.id,  oc1.name as classification_name, ocl.name as organism_classification_level_name, oc1.organism_classification_level_id
			FROM    {organism_classification} oc1
			INNER JOIN {organism_classification_level} ocl ON oc1.organism_classification_level_id = ocl.id
			INNER JOIN q ON oc1.parent_id = q.id
			where oc1.parent_id <> oc1.id			
			)
		SELECT  * 
		FROM  q;
		
		-- -------------------------------------------------
		-- Get all classification recursive for all organisms 
		-- -------------------------------------------------
		CREATE TEMP VIEW mytempviewdata AS
			WITH    RECURSIVE
			q AS
			(
			SELECT  o.id as organism_id, oc.id, oc.name as classification_name, tor.organism_classification_level_name, oc.organism_classification_level_id, oc.parent_id        
			FROM  organism o
			INNER JOIN {organism_classification_subscription} ocs ON o.id = ocs.organism_id
			INNER JOIN {organism_classification} oc ON oc.id = ocs.organism_classification_id
			INNER JOIN tmp_classification_recursive tor ON tor.id = oc.id
			UNION ALL			
			SELECT  q.organism_id, oc1.id, oc1.name as classification_name, ocl.name as organism_classification_level_name, oc1.organism_classification_level_id, oc1.parent_id
			FROM    q
			INNER JOIN    {organism_classification} oc1
			ON      oc1.id = q.parent_id
			INNER JOIN {organism_classification_level} ocl
			ON oc1.organism_classification_level_id = ocl.id			
			where oc1.parent_id <> q.id
			)
			SELECT  organism_id,
	                classification_name,
	                organism_classification_level_name
    	FROM q 
	        ORDER BY organism_id, organism_classification_level_id;		
	   
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
	                    osy.organism_id AS organism_id,
	                    osy.name AS osy_name,
	                    ol_name,
	                    oc_CSCF, oc_class, oc_order, oc_family, oc_genus, oc_subgenus
	                FROM crosstab
	                    (
	                       'SELECT organism_id AS rowid, organism_classification_level_name AS category, classification_name AS values FROM mytempviewdata',
	                       'SELECT name FROM mytempviewlevel'
	                    ) AS nn(
	                       organism_id integer, oc_CSCF text, oc_class text, oc_order text, oc_family text, oc_genus text, oc_subgenus text
	                    ) JOIN drupal_organism_synonym osy ON osy.organism_id = nn.organism_id
	                       LEFT JOIN (SELECT organism_id, name AS ol_name FROM drupal_organism_lang WHERE languages_language = 'de') AS mylang ON mylang.organism_id = nn.organism_id;
	    SELECT organism_id, osy_name, ol_name, oc_CSCF, oc_class, oc_order, oc_family, oc_genus, oc_subgenus FROM mytempfinaltable;
	    DROP VIEW mytempviewdata;
	    DROP VIEW mytempviewlevel;
	    DROP VIEW mytempfinaltable;
	    DROP VIEW tmp_classification_recursive;
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

	// get all attributes for a organismclass
	// 	$classificationattributedata = organism_get_available_attributes(
	// 			$classificationid);
	// 	if (empty($classificationattributedata)) {
	// 		die("NO ATTRIBUTES FOUND - HANDLE THIS CASE PLEASE");
	// 	}

	$imagetypes = join("','",_gallery_getMediaInfo(GALLERY_MEDIATYPE_IMAGE)->mimeTypes);
	$soundtypes = join("','",_gallery_getMediaInfo(GALLERY_MEDIATYPE_SOUND)->mimeTypes);
	$videotypes = join("','",_gallery_getMediaInfo(GALLERY_MEDIATYPE_VIDEO)->mimeTypes);
	
	$sql['dbPreStatement'] = "
		-- -------------------------------------------------
		-- Get recursive all subclassification for a certain classification
		-- -------------------------------------------------
		CREATE TEMP VIEW tmp_classification_recursive AS
			WITH    RECURSIVE
			q AS
			(
			SELECT  oc.id,  oc.name as classification_name, ocl.name as organism_classification_level_name, oc.organism_classification_level_id
			FROM  {organism_classification} oc
			INNER JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.id
			WHERE oc.id = :classificationid			
			UNION ALL			
			SELECT  oc1.id,  oc1.name as classification_name, ocl.name as organism_classification_level_name, oc1.organism_classification_level_id
			FROM    {organism_classification} oc1
			INNER JOIN {organism_classification_level} ocl ON oc1.organism_classification_level_id = ocl.id
			INNER JOIN q ON oc1.parent_id = q.id
			where oc1.parent_id <> oc1.id			
			)
		SELECT  * 
		FROM  q;
		
		-- -------------------------------------------------
		-- Get all classification recursive for all organisms 
		-- -------------------------------------------------
		CREATE TEMP VIEW mytempviewdata AS
			WITH    RECURSIVE
			q AS
			(
			SELECT  o.id as organism_id, oc.id, oc.name as classification_name, tor.organism_classification_level_name, oc.organism_classification_level_id, oc.parent_id        
			FROM  {organism} o
			INNER JOIN {organism_classification_subscription} ocs ON o.id = ocs.organism_id
			INNER JOIN {organism_classification} oc ON oc.id = ocs.organism_classification_id
			INNER JOIN tmp_classification_recursive tor ON tor.id = oc.id
			UNION ALL			
			SELECT  q.organism_id, oc1.id, oc1.name as classification_name, ocl.name as organism_classification_level_name, oc1.organism_classification_level_id, oc1.parent_id
			FROM    q
			INNER JOIN    {organism_classification} oc1
			ON      oc1.id = q.parent_id
			INNER JOIN {organism_classification_level} ocl
			ON oc1.organism_classification_level_id = ocl.id			
			where oc1.parent_id <> q.id
			)
			SELECT  organism_id,
	                classification_name,
	                organism_classification_level_name
    	FROM q 
	        ORDER BY organism_id, organism_classification_level_id;			

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
		CREATE TEMP VIEW final_classification_table AS
			SELECT
				nn.organism_id,
				o.scientific_name,
				mylang.ol_name,
				$levelnamesstring
			FROM crosstab
				(
				   'SELECT organism_id AS rowid, organism_classification_level_name AS category, classification_name AS values FROM mytempviewdata',
				   'SELECT name FROM mytempviewlevel'
				) AS nn(
				   organism_id integer, $levelnamesdatastring
				)  
				   JOIN {organism} as o on o.id = nn.organism_id
		   LEFT JOIN (
		   	SELECT
		   		organism_id,
		   		name AS ol_name
		   	FROM
		   		{organism_lang}
		   	WHERE
		   		languages_language = :language) AS mylang ON mylang.organism_id = nn.organism_id";
				
	/* Set the db fields to select */
	$dbcolumns = array_merge(
		array('organism_id',
				'scientific_name',
				'ol_name',
				'coalesce(imagescount, 0) AS imagescount',
				'coalesce(videoscount, 0) AS videoscount',
				'coalesce(soundscount, 0) AS soundscount',
				'coalesce(documentscount, 0) AS documentscount'
		),
		$levelnamearray);
	$sql['dbColumns'] = $dbcolumns;

	$sql['dbSearchColumns'] = array_merge(
		array('ol_name',
				'scientific_name'
		),
		$levelnamearray);
	
	/* DB table to use */
	$sql['dbTable'] = "final_classification_table";
	$sql['dbJoins'] = '
		LEFT JOIN
		(
		SELECT gi.item_id, COUNT(*) AS imagescount
		FROM {gallery_image} gi
		LEFT JOIN {file_managed} fm ON gi.file_managed_fid = fm.fid
			WHERE gi.item_type = \'organismreference\'
				AND fm.filemime IN (\''.$imagetypes.'\')
			GROUP BY gi.item_id
		) images ON images.item_id = organism_id
		LEFT JOIN
		(
		SELECT gi.item_id, COUNT(*) AS videoscount
		FROM {gallery_image} gi
		LEFT JOIN {file_managed} fm ON gi.file_managed_fid = fm.fid
			WHERE gi.item_type = \'organismreference\'
				AND fm.filemime IN (\''.$videotypes.'\')
			GROUP BY gi.item_id
		) videos ON videos.item_id = organism_id
		LEFT JOIN
		(
		SELECT gi.item_id, COUNT(*) AS soundscount
		FROM {gallery_image} gi
		LEFT JOIN {file_managed} fm ON gi.file_managed_fid = fm.fid
			WHERE gi.item_type = \'organismreference\'
				AND fm.filemime IN (\''.$soundtypes.'\')
			GROUP BY gi.item_id
		) sounds ON sounds.item_id = organism_id
		LEFT JOIN
		(
		SELECT ofm.organism_id AS ofm_oid, COUNT(*) AS documentscount
		FROM {organism_file_managed} ofm
		GROUP BY ofm_oid
		) documents ON ofm_oid = organism_id';
	
	/* Group */
	$sql['id'] = 'organism_id';

	$prearguments = array(
			':classificationid' => $classificationid,
			':language' => $language->language
	);

	$arguments = array();

	$output = datatable_dbRequest($sql, $arguments, $prearguments);
	
	foreach ($output['rows'] as &$row) {
		
		// get images
		$images = gallery_images(GALLERY_MEDIATYPE_IMAGE, 'organismreference', $row['id']);
		
		// thumbnail
		$row['cell']['thumbnail'] = '';
		if ($images['count'] > 0) {
			// display the thumbnail (for the table view)
			$row['cell']['thumbnail'] = '<img onclick="event.stopPropagation(); gallery_lightbox.openLightBoxEntry('.$row['id'].');" src="' . image_style_url('gallery_thumbnail', $images['images'][0]['uri']) . '" alt="' . $images['images'][0]['title'] . '" height="80" width="80" />';
		}
		
		// add the html-source for the table-gallery-view
		$row['cell']['gallery_image'] = __organism_get_gallery_image($row['cell']);
	}
	
	return drupal_json_output($output);
}

/**
 * the datatable gallery addon wants the html code for each image. 
 * this function returns the html for this. 
 * the gallery display either a proof image (Belegbild) or a portrait image (organism module).
 * this function only retuns one image and only proof images for which the user is authorized (read access) 
 * are displayed
 * @param array $organism_data and array with data for this organism (the "cell" part of the json)
 */
function __organism_get_gallery_image($organism_data){
	global $user;
	global $base_url;
	
	$organism_id = $organism_data['organism_id'];
	
	// let's find an observation for this organism
	// that meets the following criteria
	//	- same organism
	//  - has a picture
	//  - the user or it's groups have read access 
	$sql = "SELECT ob.id, ob.acl_id FROM {observation} ob
			 -- only observations with images
			 INNER JOIN {gallery_image} gi
				ON ob.id = gi.item_id and item_type = 'observation' and
				   ob.organism_id = :organism_id
			 ";
	
	$authrorization_sql = "-- make sure, that we have read rights on this observation
							 LEFT JOIN {acl_sgroup} ag 
								ON ag.acl_id = ob.acl_id
							 LEFT JOIN {sgroup} g 
								ON g.id = ag.sgroup_id
							 LEFT JOIN {acl_users} 
								au ON au.users_id =  :user_id
							 WHERE 
								-- the user, one of it's groups or the group 'ALL' need 
								-- acces to this observation. otherwise we are not allowed 
								-- to see it's pictures
								( 
									(ag.sgroup_id = :acl_group_all AND ag.level >= :acl_read)
									OR (au.acl_id = g.acl_id AND ag.level >= :acl_read )
									OR (au.acl_id = ob.acl_id AND au.level >= :acl_read)
								)";
	
	$sql_suffix = "LIMIT 1;";
	
	// admins can see all images. users must have the right premissions
	if(check_admin_permission($user->uid)){
		$query = $sql . ' ' . $sql_suffix; 
		$params =  array('organism_id' => $organism_id);
	} else {
		$query = $sql . ' ' . $authrorization_sql . ' ' . $sql_suffix;
		$params =  array('organism_id' => $organism_id,
					 'user_id' => $user->uid,
					 'acl_group_all' => variable_get('ACL_GROUP_ALL_ID'),
					 'acl_read' => variable_get('ACL_READ')
			);
	}
	
	$result = db_query($query, $params);
	
	$observation_id = NULL;
	$observation_aclId = NULL;
	if($result->rowCount() > 0){
		$row = $result->fetchObject();
		$observation_id = $row->id;
		$observation_aclId = $row->acl_id;
	}
			
	// link to the species detail page
	$detail = '<a href="' . $base_url . '/organism/' . $organism_id . '" style="color: black; text-decoration:underline">';
	
	return datatable_get_gallery_image($observation_id, $organism_id, 
			array(
				'icons' => array(
					// note needed here. (see the observation module for more infos)
				),
				'data' => array(
					$organism_data['oc_family'],
					$detail . '<i>' . $organism_data['scientific_name'] . '</i></a>',
					$organism_data['ol_name']
				)
			),
			$observation_aclId,
			DATATABLE_IMGSRC_PORTRAIT
			);
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
			'osy.id AS osy_id',
			'osy.name AS osy_name',
			'osy.organism_id AS o_id',
	);

	$sql['dbSearchColumns'] = array(
			'osy.id',
			'osy.name',
			'osy.organism_id'
	);

	/* DB table to use */
	$sql['dbTable'] = '{organism_synonym} osy';
	/* Group */
	$sql['dbWhere'] = 'osy.organism_id = :oid';
	$sql['id'] = 'osy_id';

	$arguments = array(
			':oid' => $organism['id']
	);

	$output = datatable_dbRequest($sql, $arguments);
	return drupal_json_output($output);
}

/**
 * Menu callback; AJAX autocomplete for selecting organisms
 * searches an organism by its german/french/englisch-name, lat-name or synonym-name
 *
 * @return array Array of organisms matching the term having the following keys:
 *  - "id": The ID of the organism
 *  - "label": The name of the organism
 *  - "label_latin": The latin name of the organism
 *  - "old_id"[Optional]: The ID of the initially found organism
 *  - "artgroup": The id of the speciesgroup to search in
 *    (only available if the found organism has a reference to an official one)
 *  - "old_label"[Optional]: The name of the initially found organism
 *    (only available if the found organism has a reference to an official one)
 *  - "old_label_latin"[Optional]: The latin name of the initially found organism
 *    (only available if the found organism has a reference to an official one)
 */
function organism_search_organism_json($artgroup, $string) {
	global $user;
	
	module_load_include('inc', 'organism', 'organism.artgroup');
	
	global $language;
	
	$langcode = $language->language;
	
	$term = $string;
	$term = strtolower($term);
	$terms = explode(' ', trim($term));
	
	// why limit result with the length of the search term??
	// $limit = strlen(trim($term)) > 3 ? 100000 : 20;
	$limit = 30;
	
	$organisms = array();
	
	$params = array();

	$params[':limit'] = $limit;
	$params[':lang'] = $langcode;
	$params[':artgroupid'] = $artgroup;
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
	$query = 'WITH RECURSIVE anchestors AS
	(
		SELECT oa.id, oa.name, oa.parent FROM {organism_artgroup} oa WHERE id = :artgroupid
		UNION ALL
		SELECT child.id, child.name, child.parent FROM {organism_artgroup} child INNER JOIN anchestors p ON p.id = child.parent
	)
	SELECT DISTINCT ON (o.id) o.id as organism_id, ol.name as ol_name, oas.organism_artgroup_id artg_id, osy.id as synonym_id,
				-- Standardmaessig wird der richtige latienische name angezeigt. organism.scientific_name
				-- falls aber nach dem synonym gesucht wird, so wird dieses als lateinischer Name verwendet
				CASE WHEN (osy.name IS NULL) then o.scientific_name ELSE osy.name END AS scientific_name
				FROM {organism} AS o
				LEFT JOIN {organism_lang} AS ol ON ol.organism_id = o.id AND ol.languages_language = :lang
				LEFT JOIN {organism_synonym} AS osy ON osy.organism_id = o.id
				LEFT JOIN {organism_artgroup_subscription} AS oas ON oas.organism_id=o.id
				JOIN anchestors AS a ON a.id=oas.organism_artgroup_id
				WHERE ol.name ILIKE :term OR osy.name ILIKE :term1 OR o.scientific_name ILIKE :term1
				ORDER BY o.id
				LIMIT :limit;';
	
	$result = db_query($query, $params)->fetchAllAssoc('organism_id');
	$matches = array();
		foreach ($result as $row) {
			if($row->ol_name != "") {
				$matches[$row->ol_name] = $row->ol_name.'<br><em>'.$row->scientific_name.'</em>';
			} else {
				$matches[$row->scientific_name] = $row->scientific_name.'<br><em>'.t('No translation available').'</em>';
			}
		}
	
	drupal_json_output($matches);
}

?>
