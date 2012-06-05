<?php
// $olddb = pg_connect("host=localhost port=5432 dbname=iphone_db user=postgres password=postgres");
$web_db = pg_connect("host=localhost port=5432 dbname=naturvielfalt_dev user=postgres password=postgres");
$file = 'IPhone_DB_.sql';


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
	CREATE INDEX idx_classification_taxon_idx2 ON classification_taxon(taxon_id);

';

// removed DISTINCT ON (osn.organism_id)
$query_all_organism = "
	SELECT
	osn.organism_id organism_id,
	osn.name name_sc,
	ol_de.name name_de,
	ol_fr.name name_fr,
	ol_it.name name_it,
	ol_rm.name name_rm,
	ol_en.name name_en


	FROM organism_scientific_name osn
	LEFT JOIN organism_lang ol_de ON ol_de.languages_language='de' AND ol_de.organism_id=osn.organism_id
	LEFT JOIN organism_lang ol_fr ON ol_fr.languages_language='fe' AND ol_fr.organism_id=osn.organism_id
	LEFT JOIN organism_lang ol_it ON ol_it.languages_language='it' AND ol_it.organism_id=osn.organism_id
	LEFT JOIN organism_lang ol_rm ON ol_rm.languages_language='rm' AND ol_rm.organism_id=osn.organism_id
	LEFT JOIN organism_lang ol_en ON ol_en.languages_language='en' AND ol_en.organism_id=osn.organism_id

";

$query_all_artgroups = '
	SELECT
	id classification_id,
	name name_de,
	parent parent,
	pos "position"

	FROM organism_artgroup
';

$query_organism_artgroup_subscription = "
	SELECT
	oas.organism_id taxon_id,
	oas.organism_artgroup_id classification_id

	FROM organism_artgroup_subscription oas


";

ECHO 'get all organism<br>';
$results_all_organism = pg_fetch_all(pg_query($web_db, $query_all_organism));
echo 'get all artgroups<br>';
$results_all_artgroups = pg_fetch_all(pg_query($web_db, $query_all_artgroups));
echo 'get all subscriptions';
$results_organism_artgroup_subscription = pg_fetch_all(pg_query($web_db, $query_organism_artgroup_subscription));

echo 'build artgroups sql inserts<br>';
$sql_all_artgroups = '

';
foreach ($results_all_artgroups as $artgroup){
	$sql_all_artgroups .= "
INSERT INTO classification (classification_id, name_de, parent, \"position\", class_level) values
	('".pg_escape_string($artgroup['classification_id'])."','".pg_escape_string($artgroup['name_de'])."','".pg_escape_string($artgroup['parent'])."','".pg_escape_string($artgroup['position'])."', 1);";
}

echo 'build organism sql inserts<br>';
$sql_all_organism = '

';
foreach ($results_all_organism as $organism){
	$sql_all_artgroups .= "
INSERT INTO organism (organism_id, name_sc, name_de, name_fr, name_it, name_rm, name_en) values ('".pg_escape_string($organism['organism_id'])."','".pg_escape_string($organism['name_sc'])."','".pg_escape_string($organism['name_de'])."','".pg_escape_string($organism['name_fr'])."','".pg_escape_string($organism['name_it'])."','".pg_escape_string($organism['name_rm'])."','".pg_escape_string($organism['name_en'])."');";
}

echo 'build all subscription sql inserts<br>';
$sql_organism_artgroup_subscription = '

';
foreach ($results_organism_artgroup_subscription as $subs){
	$sql_all_artgroups .= "
INSERT INTO classification_taxon (taxon_id, classification_id) values ('".pg_escape_string($subs['taxon_id'])."','".pg_escape_string($subs['classification_id'])."');";
}

echo 'save all to the file';
$save = $sql_create_iphone_db_structur.$sql_all_artgroups.$sql_all_organism.$sql_organism_artgroup_subscription;
if(file_exists($file)) unlink($file);
$fhandler = fopen($file,'w');
fwrite($fhandler,$save);
fclose($fhandler);

