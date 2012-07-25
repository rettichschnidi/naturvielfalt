<?php
/**
 * @file dump_swissmon_organism_pictures.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Extract all organism referencepictures and dump them to $OUTPUTDIR/$FILE_ID/$FILENAME
 */

require_once(dirname(__FILE__) . '/lib/bootstrap.php');
require_once($configdir . '/databases.php');
require_once($libdir . 'Db.php');

$dbevab = new Db(
		$config['swissmon']['driver'],
		$config['swissmon']['name'],
		$config['swissmon']['user'],
		$config['swissmon']['password'],
		$config['swissmon']['host'],
		$config['swissmon']['port']);

$arguments = getopt("i:o:");
if(empty($arguments['i'])) {
	die("Please provide an input directory!\n");
} else {
	$inputdir = $arguments['i'];
}

if(empty($arguments['o'])) {
	die("Please provide an output directory!\n");
} else {
	$inputdir = $arguments['o'];
}

$sql = "
	SELECT
		fm.filename,
		fm.uri,
		o.name_sc,
		gi.title,
		gi.description,
		gi.author,
		gi.location
	FROM
		gallery_image gi
		JOIN organism o ON o.id = gi.item_id
		JOIN file_managed fm ON fm.fid = gi.fid;
";

$rows = $dbevab->query($sql, array(), array());
assert(!empty($rows));
print_r($rows);
?>
