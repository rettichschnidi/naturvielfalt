<?php
/**
 * @file organism.inc
 * @author Reto Schneider, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

/**
 * Return an organism, just lat&lang name
 * maybe later we need more fields
 *
 * @param int $organismid
 * @return array
 */
function organism_get($organismid) {
	global $user;
	global $language;
	
	$langcode = (isset($user->language) && !empty($user->language))
	? $user->language
	: $language->language;

	$organismid = intval(filter_xss($organismid));
	if ($organismid < 1 || $organismid > 1000000)
		return false;
	$resultarray = array();
	return db_query(
		'SELECT
			o.id, o.scientific_name name_lat,
			ol.name name_lang
		FROM
			{organism} o
			LEFT JOIN {organism_lang} ol ON ol.organism_id=:oid AND ol.languages_language=:lang_code
		WHERE
			o.id = :oid',
		array(':oid' => $organismid,
				'lang_code' => $langcode,
// 				':lang_code' => $user->language
		))->fetchAssoc();
}

function organism_get_byname($organism) {
	global $user;
	
// 	$result = db_query('SELECT o.id, o.scientific_name name_lat, ol.name name_lang
// 			 FROM {organism} o, {organism_lang} ol 
// 			 WHERE o.id = ol.organism_id AND ol.languages_language = :lang_code AND ol.name = :o_name;',
// 			array(		':o_name' =>   $organism,
// 						':lang_code' => 'de',
// 			));
	
// 	foreach ($result as $record) {
// 		return $record;
// 	}

	// TODO take language of the user (if exists)
// 	$language = $user->language;
	//$language = 'de';
	
	global $language;
	
	$langcode = (isset($user->language) && !empty($user->language))
	? $user->language
	: $language->language;
	
	// FIXME: if an organism is picked from vote this doesn't work
	$result = db_query(
			'SELECT DISTINCT o.id
			 FROM {organism} o, {organism_lang} ol 
			 WHERE (o.id = ol.organism_id AND ol.languages_language = :lang_code AND ol.name = :o_name) OR (o.scientific_name = :o_name);',
			array( ':lang_code' => $langcode,
					':o_name' => $organism,
			))->fetchAssoc();
	//debug($result);
	return $result;
}

function organism_artgroup_byname($organism) {
	global $user;

	// TODO take language of the user (if exists)
	// 	$language = $user->language;
	//$language = 'de';
	global $language;
	
	$langcode = (isset($user->language) && !empty($user->language))
	? $user->language
	: $language->language;

	$result = db_query(
			'SELECT oas.organism_artgroup_id, ol.organism_id 
			 FROM {organism_artgroup_subscription} oas, {organism_lang} ol
			 WHERE oas.organism_id = ol.organism_id 
			AND ol.languages_language = :lang_code AND ol.name ILIKE :o_name;',
			array( ':lang_code' => $langcode,
					':o_name' => $organism,
			))->fetchAssoc();

	return $result;
}

/**
 * Return all classification levels of a given organism.
 *
 * @param $classificationid
 * 	Id of the organism in question.
 */
function organism_get_available_classification_levels($classificationid) {
	$classificationdata = array();
	$attributevalueresult = db_query(
		'SELECT
			ocl2.id,
			ocl2.left_value,
			ocl2.name
		FROM
			{organism_classification} oc
			JOIN {organism_classification_level} ocl ON oc.organism_classification_level_id = ocl.id
			JOIN {organism_classification_level} ocl2 ON ocl2.prime_father_id = ocl.prime_father_id
		WHERE
			oc.id = :organismid
		ORDER BY
			left_value;',
		array(':organismid' => $classificationid));
	return $attributevalueresult->fetchAll();
}

function organism_get_available_attributes($classificationid) {
	$sqlGetAttributes = 'WITH subclassification_constraints AS (
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
	)
	SELECT
		DISTINCT ON (oa.id)
		oa.id,
		oa.name
	FROM
		included_organisms io
		JOIN drupal_organism_attribute_value_subscription oavs ON oavs.organism_id = io.organism_id
		JOIN drupal_organism_attribute_value oav ON oav.id = oavs.organism_attribute_value_id
		JOIN drupal_organism_attribute oa ON oa.id = oav.organism_attribute_id
	ORDER BY oa.id';

	$rows = db_query(
		$sqlGetAttributes,
		array(':classificationid' => $classificationid),
		array('fetch' => PDO::FETCH_ASSOC))->fetchAll();

	$levelnamesstring = '';
	foreach ($rows as $row) {}
}

/**
 * Check if wikipedia has an article for an organism in the current language.
 * If found, return the URL to the site.
 * @param  $organismdata organismdata
 * @return String with link to article or false, if none existing.
 */
function organism_check_if_wikipedia_article_exists($organismdata) {
	global $user;
	global $language;
	$langcode = isset($user->language) && !empty($user->language) ? $user->language : $language->language;
	$organism_name = $organismdata['scientific_name'];
	// Reduce to first two words, as this delivers the best results.
	if (preg_match('/([a-zA-Z0-9]+\ [a-zA-Z0-9]+).*/', $organism_name, $matches)) {
		$organism_name = $matches[1];
	}

	$host = $langcode . '.wikipedia.org';
	$url = '/w/api.php?';
	$postdata = 'action=query&prop=revisions&titles='
			. urlencode($organism_name)
			. '&rvprop=content&rvparse&format=json&redirects';
	$fp = pfsockopen($host, 80, $errno, $errstr, 60);
	if (!$fp) {
		return false;
	}
	fputs($fp, "POST $url$postdata HTTP/1.1\n");
	fputs($fp, "Host: $host\n");
	fputs($fp, "User-Agent: Post\n");
	fputs($fp, "Accept: */*\n");
	fputs($fp, "Content-type: application/x-www-form-urlencoded\n");
	fputs($fp, "Content-length: " . strlen($postdata) . "\n\n");
	fputs($fp, "$postdata\n\n");
	$o = '';
	while (!feof($fp))
		$o .= fgets($fp, 1024);
	fclose($fp);

	$pos = strpos($o, '{"query":');
	$o = substr($o, $pos);
	// Check lenght of article. If article has more than 300 chars, then assume that article exists.
	// Value choosen by accident, will work. Mostly.
	if (strlen($o) > 300) {
		return "//$host/wiki/$organism_name";
	}
	return false;
}
?>
