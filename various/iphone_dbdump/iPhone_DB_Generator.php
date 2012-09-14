<?php
require_once(dirname(__FILE__) . '/config/database.php');
global $drupalprefix;
global $config;

$table_o = $drupalprefix . 'organism';
$table_osy = $drupalprefix . 'organism_synonym';
$table_ol = $drupalprefix . 'organism_lang';
$table_oa = $drupalprefix . 'organism_artgroup';
$table_oas = $drupalprefix . 'organism_artgroup_subscription';

$web_db = pg_connect(
	'host=' . $config['naturvielfalt_dev']['host'] . ' port='
			. $config['naturvielfalt_dev']['port'] . ' dbname='
			. $config['naturvielfalt_dev']['name'] . ' user='
			. $config['naturvielfalt_dev']['user'] . ' password='
			. $config['naturvielfalt_dev']['password']);
$file = 'iPhone_DB.sql';

$sql_create_iphone_db_structur = '
	DROP TABLE IF EXISTS classification;
	CREATE TABLE classification (
	-- Artgroup id
		classification_id integer PRIMARY KEY,
	-- something classified? :P (NOT NULL removed, standart set to 1)
		class_level integer,
	-- parent artgroup id
		parent integer NOT NULL,
		name_de character varying(60),
		name_fr character varying(50),
		name_it character varying(50),
		name_en character varying(50),
		classification_type_id integer,
		"position" integer
	);

	DROP TABLE IF EXISTS observation;
	CREATE TABLE observation (
		ID INTEGER PRIMARY KEY AUTOINCREMENT,
		ORGANISM_ID INTEGER,
		ORGANISMGROUP_ID INTEGER,
		ORGANISM_NAME TEXT,
		ORGANISM_NAME_LAT TEXT,
		ORGANISM_FAMILY TEXT,
		AUTHOR TEXT,
		DATE TEXT,
		AMOUNT INTEGER,
		LOCATION_LAT REAL,
		LOCATION_LON REAL,
		ACCURACY INTEGER,
		COMMENT TEXT,
		IMAGE BLOB
	);

	DROP TABLE IF EXISTS organism;
	CREATE TABLE organism (
	-- entry id
	--	id bigint PRIMARY KEY,
	-- the original organsim id
		organism_id bigint,
	-- flora or group or something other, not used on the new system (NOT NULL removed)
		organism_type bigint,
	-- organism id
	--	organism_id bigint NOT NULL,
	-- artgroup?
		inventory_type_id bigint,
	-- translations
		name_de character varying(80),
		name_fr character varying(100),
		name_it character varying(80),
	-- original name
		name_sc character varying(100),
		name_rm character varying(80),
		name_en character varying(80)
	);

	DROP TABLE IF EXISTS classification_taxon;
	CREATE TABLE classification_taxon (
		classification_taxon_id INTEGER PRIMARY KEY,
	-- artgroup id
		classification_id INTEGER REFERENCES classification,
	-- organismus id
		taxon_id bigint INTEGER REFERENCES organism,
		display_level integer
	);

	CREATE INDEX idx_classification_idx1 ON classification(parent);
	CREATE INDEX idx_classification_idx2 ON classification(class_level);
	CREATE INDEX idx_classification_taxon_idx1 ON classification_taxon(classification_id);
	CREATE INDEX idx_classification_taxon_idx2 ON classification_taxon(taxon_id);';

$query_all_organism = "
	-- Alle Organismen, der richtige wissenschaftliche Namen und die Uebersetzung selektieren
	SELECT
		o.id organism_id,
		o.scientific_name name_sc,
		ol_de.name name_de,
		ol_fr.name name_fr,
		ol_it.name name_it,
		ol_rm.name name_rm,
		ol_en.name name_en

	FROM $table_o o
		LEFT JOIN $table_ol ol_de ON ol_de.languages_language='de' AND ol_de.organism_id=o.id
		LEFT JOIN $table_ol ol_fr ON ol_fr.languages_language='fe' AND ol_fr.organism_id=o.id
		LEFT JOIN $table_ol ol_it ON ol_it.languages_language='it' AND ol_it.organism_id=o.id
		LEFT JOIN $table_ol ol_rm ON ol_rm.languages_language='rm' AND ol_rm.organism_id=o.id
		LEFT JOIN $table_ol ol_en ON ol_en.languages_language='en' AND ol_en.organism_id=o.id

	-- Alle Synonyme hinzuholen
	UNION SELECT
		osy.organism_id organism_id,
		osy.name name_sc,
		ol_de.name name_de,
		ol_fr.name name_fr,
		ol_it.name name_it,
		ol_rm.name name_rm,
		ol_en.name name_en

	FROM $table_osy osy
		LEFT JOIN $table_ol ol_de ON ol_de.languages_language='de' AND ol_de.organism_id=osy.organism_id
		LEFT JOIN $table_ol ol_fr ON ol_fr.languages_language='fe' AND ol_fr.organism_id=osy.organism_id
		LEFT JOIN $table_ol ol_it ON ol_it.languages_language='it' AND ol_it.organism_id=osy.organism_id
		LEFT JOIN $table_ol ol_rm ON ol_rm.languages_language='rm' AND ol_rm.organism_id=osy.organism_id
		LEFT JOIN $table_ol ol_en ON ol_en.languages_language='en' AND ol_en.organism_id=osy.organism_id
";

$query_all_artgroups = "
	SELECT
		id classification_id,
		name name_de,
		parent parent
	FROM $table_oa";

$query_organism_artgroup_subscription = "
	SELECT
		oas.organism_id taxon_id,
		oas.organism_artgroup_id classification_id
	FROM $table_oas oas";

echo "get all organism\n";
$results_all_organism = pg_fetch_all(pg_query($web_db, $query_all_organism));
echo "get all artgroups\n";
$results_all_artgroups = pg_fetch_all(pg_query($web_db, $query_all_artgroups));
echo "get all subscriptions\n";
$results_organism_artgroup_subscription = pg_fetch_all(
	pg_query($web_db, $query_organism_artgroup_subscription));

echo "build artgroups sql inserts\n";
$sql_all_artgroups = '

';
foreach ($results_all_artgroups as $artgroup) {
	$sql_all_artgroups .= "
INSERT INTO classification (classification_id, name_de, parent, class_level) values
	('" . pg_escape_string($artgroup['classification_id']) . "','"
			. pg_escape_string($artgroup['name_de']) . "','"
			. pg_escape_string($artgroup['parent']) . "', 1);";
}

echo "build organism sql inserts\n";
$sql_all_organism = '

';
foreach ($results_all_organism as $organism) {
	$sql_all_artgroups .= "
INSERT INTO organism (organism_id, name_sc, name_de, name_fr, name_it, name_rm, name_en) values ('"
			. pg_escape_string($organism['organism_id']) . "','"
			. pg_escape_string($organism['name_sc']) . "','"
			. pg_escape_string($organism['name_de']) . "','"
			. pg_escape_string($organism['name_fr']) . "','"
			. pg_escape_string($organism['name_it']) . "','"
			. pg_escape_string($organism['name_rm']) . "','"
			. pg_escape_string($organism['name_en']) . "');";
}

echo "build all subscription sql inserts\n";

foreach ($results_organism_artgroup_subscription as $subs) {
	$sql_all_artgroups .= "
INSERT INTO classification_taxon (taxon_id, classification_id) values ('"
			. pg_escape_string($subs['taxon_id']) . "','"
			. pg_escape_string($subs['classification_id']) . "');";
}

echo "save all to the file '$file'\n";
$save = $sql_create_iphone_db_structur . $sql_all_artgroups . $sql_all_organism;

if (file_exists($file)) {
	unlink($file);
}
$fhandler = fopen($file, 'w');
fwrite($fhandler, $save);
fclose($fhandler);

