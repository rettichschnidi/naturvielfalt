<?php
/**
 * @file import_swisslichens_basedata.php
 * @author Reto Schneider, 2012, github@reto-schneider.ch
 *
 * Import lichens to the new naturvielfalt DB
 */

require_once(dirname(__FILE__) . '/lib/bootstrap.php');
require_once($libdir . '/NaturvielfaltDb.php');
global $drupalprefix;
global $errors;

$db = new NaturvielfaltDb(
	$config['naturvielfalt_dev']['driver'],
	$config['naturvielfalt_dev']['name'],
	$config['naturvielfalt_dev']['user'],
	$config['naturvielfalt_dev']['password'],
	$config['naturvielfalt_dev']['host']);

$importTable = 'import_swisslichens_basedata_import';
$exportTable = 'import_swisslichens_basedata';

//-----------------------------

$finallichens = array();

$columns = array(
		'genus',
		'genusid',
		'species',
		'speciesid'
);
$sql = "FROM $importTable";

$rows = $db->select_query($columns, $sql, array(), array());

print "Got " . count($rows) . " to fetch.\n";
$counter = 0;
foreach ($rows as $row) {
	$counter++;
	if($counter % 10 == 0) {
		if(false)
			print "Last lichen fetched: " . $finallichens[count($finallichens) - 1]['genus'] . ", " . $finallichens[count($finallichens) - 1]['species'] . "\n";
		print "Fetched $counter lichens so far.\n";
	}
	$genus = $row['genus'];
	$genusid = $row['genusid'];
	$speciesid = $row['speciesid'];
	$species = $row['species'];
	$url = "http://merkur.wsl.ch/didado/swisslichens.map?fname=$genusid&fartnr=$speciesid";
	$fh = fopen($url, "r");
	$sData = '';
	while (!feof($fh))
		$sData .= fread($fh, 8192);
	fclose($fh);
	$m = preg_match(
		'/<div id="Titel">\n<div class="subtitle">(.+)(<\/div>|\n<\/div>)/',
		$sData,
		$match);
	if ($m != 1) {
		print "Could not find a lichensname at url $url\n";
		print "Got:\n" + $sData . "\n";
		continue;
	}
	$found = $match[1];
	$foundClean = str_replace('</div>', '', $found);
	$finallichen = array(
			'genus' => $genus,
			'species' => $species,
			'scientific_name' => $foundClean
	);
	$finallichens[] = $finallichen;
}
print "Fetched " . count($finallichens) . "\n";

foreach ($finallichens as $finallichen) {
	$columns = array(
			'genus',
			'species',
			'scientific_name'
	);
	$query = "INSERT INTO $exportTable (genus, species, scientific_name) VALUES(?, ?, ?)";
	$typesArray = array(
			'text',
			'text',
			'text'
	);
	$valuesArray = array(
			$finallichen['genus'],
			$finallichen['species'],
			$finallichen['scientific_name']
	);

	$rows = $db->query($query, $typesArray, $valuesArray);
	if($rows == 0) {
			print "Failed, probably already existing: " . var_export($finallichen, true) . "\n";
			continue;
	}
	assert($rows == 1);
}
//-----------------------------

?>
