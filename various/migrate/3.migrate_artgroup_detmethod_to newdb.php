<?php


$newdb = pg_connect("host=localhost port=5432 dbname=naturvielfalt_dev user=postgres password=postgres");


$sql_create = "
DROP TABLE IF EXISTS organism_artgroup_detmethod_subscription;
DROP TABLE IF EXISTS organism_artgroup_detmethod;


CREATE TABLE organism_artgroup_detmethod
(
	id serial NOT NULL UNIQUE,
	name text,
	cscf_id int,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_detmethod_subscription
(
	id serial NOT NULL UNIQUE,
	organism_artgroup_id int NOT NULL,
	organism_artgroup_detmethod_id int NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;



ALTER TABLE organism_artgroup_detmethod_subscription
	ADD FOREIGN KEY (organism_artgroup_id)
	REFERENCES organism_artgroup (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_artgroup_detmethod_subscription
	ADD FOREIGN KEY (organism_artgroup_detmethod_id)
	REFERENCES organism_artgroup_detmethod (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


";

// pg_query($sql_create);

/**
 * Data to import
 */

$methods_arr[] = array("100", "Sichtbeobachtung");
$methods_arr[] = array("101", "Fang");
$methods_arr[] = array("102", "Mit Fernglas bestimmt");
$methods_arr[] = array("103", "Ruf, Schrei, Gesang");
$methods_arr[] = array("104", "Gesang und Sichtbeobachtung");
$methods_arr[] = array("105", "Ultraschall");
$methods_arr[] = array("106", "Ruf (Ohr) und Foto");
$methods_arr[] = array("107", "Video");
$methods_arr[] = array("108", "Radiotelemetriedaten");
$methods_arr[] = array("109", "Foto");
$methods_arr[] = array("110", "Unter Binokular bestimmt");
$methods_arr[] = array("111", "genetisch bestimmt");
$methods_arr[] = array("112", "Genitalbestimmung");
$methods_arr[] = array("113", "Morphologische Bestimmung");
$methods_arr[] = array("201", "Kunstbau Biber");
$methods_arr[] = array("202", "Nest, Biberburg");
$methods_arr[] = array("203", "Puppenkammer");
$methods_arr[] = array("204", "Erdbau, Höhle, Auswurfhügel");
$methods_arr[] = array("205", "Suhle");
$methods_arr[] = array("206", "Damm");
$methods_arr[] = array("207", "Burg eingestürzt");
$methods_arr[] = array("208", "Schwimmende Burg");
$methods_arr[] = array("209", "Mittelbau Biber");
$methods_arr[] = array("210", "Schlupfloch, Fluchtröhre");
$methods_arr[] = array("300", "Totfund");
$methods_arr[] = array("301", "Verunfallt (Strasse)");
$methods_arr[] = array("302", "Abschuss");
$methods_arr[] = array("303", "Schädel");
$methods_arr[] = array("304", "Knochenreste");
$methods_arr[] = array("305", "Haare");
$methods_arr[] = array("306", "Gewölle");
$methods_arr[] = array("307", "Reste Körpergewebe, Muskel, Blut");
$methods_arr[] = array("308", "Lebendiges Tier, frisches Gehäuse");
$methods_arr[] = array("309", "Wahrscheinliches Vorkommen");
$methods_arr[] = array("310", "Status unklar");
$methods_arr[] = array("311", "Vorkommen vermutlich ehemalig");
$methods_arr[] = array("312", "Subfossile Gehäuse");
$methods_arr[] = array("313", "Kot, Losung");
$methods_arr[] = array("314", "Köcher");
$methods_arr[] = array("400", "Ausstieg, Wechsel");
$methods_arr[] = array("401", "Kanal");
$methods_arr[] = array("402", "Fällplatz");
$methods_arr[] = array("403", "Trittsiegel, Spurenfolge");
$methods_arr[] = array("404", "Schäden");
$methods_arr[] = array("405", "Nahrungsreste, Einzelfrassspuren");
$methods_arr[] = array("406", "Wintervorrat");
$methods_arr[] = array("407", "Frassplatz");
$methods_arr[] = array("408", "Raubtierriss");
$methods_arr[] = array("409", "Markierung, Bibergeil");

$insert_methods = "";
foreach ($methods_arr as $method){
	$insert_methods .= "insert into organism_artgroup_detmethod (name, cscf_id) values ('".pg_escape_string($method[1])."', '".$method[0]."');
";
}
// echo $insert_methods;
pg_query($insert_methods);

$methods_artgroup_arr[] = array("Amphibien", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Amphibien", "101", "Fang");
$methods_artgroup_arr[] = array("Amphibien", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Amphibien", "103", "Ruf, Schrei, Gesang");
$methods_artgroup_arr[] = array("Amphibien", "104", "Gesang und Sichtbeobachtung");
$methods_artgroup_arr[] = array("Amphibien", "106", "Ruf (Ohr) und Foto");
$methods_artgroup_arr[] = array("Amphibien", "107", "Video");
$methods_artgroup_arr[] = array("Amphibien", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Amphibien", "300", "Totfund");
$methods_artgroup_arr[] = array("Amphibien", "301", "Verunfallt (Strasse)");
$methods_artgroup_arr[] = array("Eintagsfliegen", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Eintagsfliegen", "101", "Fang");
$methods_artgroup_arr[] = array("Eintagsfliegen", "109", "Foto");
$methods_artgroup_arr[] = array("Eintagsfliegen", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Eintagsfliegen", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Eintagsfliegen", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Strahlenflosser (Fische)", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Strahlenflosser (Fische)", "101", "Fang");
$methods_artgroup_arr[] = array("Strahlenflosser (Fische)", "109", "Foto");
$methods_artgroup_arr[] = array("Strahlenflosser (Fische)", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Strahlenflosser (Fische)", "300", "Totfund");
$methods_artgroup_arr[] = array("Gleichflügler, Blattläuse", "101", "Fang");
$methods_artgroup_arr[] = array("Gleichflügler, Blattläuse", "109", "Foto");
$methods_artgroup_arr[] = array("Gleichflügler, Blattläuse", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Gleichflügler, Blattläuse", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Gleichflügler, Zikaden", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Gleichflügler, Zikaden", "101", "Fang");
$methods_artgroup_arr[] = array("Gleichflügler, Zikaden", "103", "Ruf, Schrei, Gesang");
$methods_artgroup_arr[] = array("Gleichflügler, Zikaden", "104", "Gesang und Sichtbeobachtung");
$methods_artgroup_arr[] = array("Gleichflügler, Zikaden", "105", "Ultraschall");
$methods_artgroup_arr[] = array("Gleichflügler, Zikaden", "106", "Ruf (Ohr) und Foto");
$methods_artgroup_arr[] = array("Gleichflügler, Zikaden", "107", "Video");
$methods_artgroup_arr[] = array("Gleichflügler, Zikaden", "109", "Foto");
$methods_artgroup_arr[] = array("Gleichflügler, Zikaden", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Gleichflügler, Zikaden", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Gottesanbeterin, Schaben, Termiten", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Gottesanbeterin, Schaben, Termiten", "101", "Fang");
$methods_artgroup_arr[] = array("Gottesanbeterin, Schaben, Termiten", "109", "Foto");
$methods_artgroup_arr[] = array("Gottesanbeterin, Schaben, Termiten", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Gottesanbeterin, Schaben, Termiten", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Gottesanbeterin, Schaben, Termiten", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Gottesanbeterin, Schaben, Termiten", "113", "Morphologische Bestimmung");
$methods_artgroup_arr[] = array("Gottesanbeterin, Schaben, Termiten", "304", "Knochenreste");
$methods_artgroup_arr[] = array("Gottesanbeterin, Schaben, Termiten", "306", "Gewölle");
$methods_artgroup_arr[] = array("Hautflügler, Ameisen", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Hautflügler, Ameisen", "101", "Fang");
$methods_artgroup_arr[] = array("Hautflügler, Ameisen", "109", "Foto");
$methods_artgroup_arr[] = array("Hautflügler, Ameisen", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Hautflügler, Ameisen", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Hautflügler, Ameisen", "202", "Nest, Biberburg");
$methods_artgroup_arr[] = array("Hautflügler, Ameisen", "210", "Schlupfloch, Fluchtröhre");
$methods_artgroup_arr[] = array("Hautflügler, Goldwespen", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Hautflügler, Goldwespen", "101", "Fang");
$methods_artgroup_arr[] = array("Hautflügler, Goldwespen", "109", "Foto");
$methods_artgroup_arr[] = array("Hautflügler, Goldwespen", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Hautflügler, Goldwespen", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Hautflügler, Grab-, Weg-, Faltenwespen...", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Hautflügler, Grab-, Weg-, Faltenwespen...", "101", "Fang");
$methods_artgroup_arr[] = array("Hautflügler, Grab-, Weg-, Faltenwespen...", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Hautflügler, Grab-, Weg-, Faltenwespen...", "109", "Foto");
$methods_artgroup_arr[] = array("Hautflügler, Grab-, Weg-, Faltenwespen...", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Hautflügler, Grab-, Weg-, Faltenwespen...", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Hautflügler, Grab-, Weg-, Faltenwespen...", "202", "Nest, Biberburg");
$methods_artgroup_arr[] = array("Hautflügler, Grab-, Weg-, Faltenwespen...", "210", "Schlupfloch, Fluchtröhre");
$methods_artgroup_arr[] = array("Hautflügler, Wildbienen und Spheciden", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Hautflügler, Wildbienen und Spheciden", "101", "Fang");
$methods_artgroup_arr[] = array("Hautflügler, Wildbienen und Spheciden", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Hautflügler, Wildbienen und Spheciden", "109", "Foto");
$methods_artgroup_arr[] = array("Hautflügler, Wildbienen und Spheciden", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Hautflügler, Wildbienen und Spheciden", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Hautflügler, Wildbienen und Spheciden", "202", "Nest, Biberburg");
$methods_artgroup_arr[] = array("Hautflügler, Wildbienen und Spheciden", "203", "Puppenkammer");
$methods_artgroup_arr[] = array("Hautflügler, Wildbienen und Spheciden", "210", "Schlupfloch, Fluchtröhre");
$methods_artgroup_arr[] = array("Heuschrecken", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Heuschrecken", "101", "Fang");
$methods_artgroup_arr[] = array("Heuschrecken", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Heuschrecken", "103", "Ruf, Schrei, Gesang");
$methods_artgroup_arr[] = array("Heuschrecken", "104", "Gesang und Sichtbeobachtung");
$methods_artgroup_arr[] = array("Heuschrecken", "105", "Ultraschall");
$methods_artgroup_arr[] = array("Heuschrecken", "106", "Ruf (Ohr) und Foto");
$methods_artgroup_arr[] = array("Heuschrecken", "107", "Video");
$methods_artgroup_arr[] = array("Heuschrecken", "109", "Foto");
$methods_artgroup_arr[] = array("Heuschrecken", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Heuschrecken", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Heuschrecken", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Käfer, andere Gruppen", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Käfer, andere Gruppen", "101", "Fang");
$methods_artgroup_arr[] = array("Käfer, andere Gruppen", "109", "Foto");
$methods_artgroup_arr[] = array("Käfer, andere Gruppen", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Käfer, andere Gruppen", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Käfer, andere Gruppen", "304", "Knochenreste");
$methods_artgroup_arr[] = array("Käfer, Holzkäfer", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Käfer, Holzkäfer", "101", "Fang");
$methods_artgroup_arr[] = array("Käfer, Holzkäfer", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Käfer, Holzkäfer", "109", "Foto");
$methods_artgroup_arr[] = array("Käfer, Holzkäfer", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Käfer, Holzkäfer", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Käfer, Holzkäfer", "203", "Puppenkammer");
$methods_artgroup_arr[] = array("Käfer, Holzkäfer", "210", "Schlupfloch, Fluchtröhre");
$methods_artgroup_arr[] = array("Käfer, Holzkäfer", "304", "Knochenreste");
$methods_artgroup_arr[] = array("Käfer, Holzkäfer", "405", "Nahrungsreste, Einzelfrassspuren");
$methods_artgroup_arr[] = array("Käfer, Laufkäfer und Kurzflügler", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Käfer, Laufkäfer und Kurzflügler", "101", "Fang");
$methods_artgroup_arr[] = array("Käfer, Laufkäfer und Kurzflügler", "109", "Foto");
$methods_artgroup_arr[] = array("Käfer, Laufkäfer und Kurzflügler", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Käfer, Laufkäfer und Kurzflügler", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Käfer, Laufkäfer und Kurzflügler", "304", "Knochenreste");
$methods_artgroup_arr[] = array("Käfer, pflanzenfressend, Blatt-, Rüssel-, Maikäfer...", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Käfer, pflanzenfressend, Blatt-, Rüssel-, Maikäfer...", "101", "Fang");
$methods_artgroup_arr[] = array("Käfer, pflanzenfressend, Blatt-, Rüssel-, Maikäfer...", "109", "Foto");
$methods_artgroup_arr[] = array("Käfer, pflanzenfressend, Blatt-, Rüssel-, Maikäfer...", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Käfer, pflanzenfressend, Blatt-, Rüssel-, Maikäfer...", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Käfer, pflanzenfressend, Blatt-, Rüssel-, Maikäfer...", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Käfer, pflanzenfressend, Blatt-, Rüssel-, Maikäfer...", "113", "Morphologische Bestimmung");
$methods_artgroup_arr[] = array("Käfer, pflanzenfressend, Blatt-, Rüssel-, Maikäfer...", "304", "Knochenreste");
$methods_artgroup_arr[] = array("Käfer, pflanzenfressend, Blatt-, Rüssel-, Maikäfer...", "306", "Gewölle");
$methods_artgroup_arr[] = array("Käfer, Wasserkäfer", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Käfer, Wasserkäfer", "101", "Fang");
$methods_artgroup_arr[] = array("Käfer, Wasserkäfer", "109", "Foto");
$methods_artgroup_arr[] = array("Käfer, Wasserkäfer", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Käfer, Wasserkäfer", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Köcherfliegen", "101", "Fang");
$methods_artgroup_arr[] = array("Köcherfliegen", "109", "Foto");
$methods_artgroup_arr[] = array("Köcherfliegen", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Köcherfliegen", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Köcherfliegen", "314", "Köcher");
$methods_artgroup_arr[] = array("Krebse, Asseln, Skorpione", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Krebse, Asseln, Skorpione", "101", "Fang");
$methods_artgroup_arr[] = array("Krebse, Asseln, Skorpione", "109", "Foto");
$methods_artgroup_arr[] = array("Krebse, Asseln, Skorpione", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Krebse, Flusskrebse", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Krebse, Flusskrebse", "101", "Fang");
$methods_artgroup_arr[] = array("Krebse, Flusskrebse", "109", "Foto");
$methods_artgroup_arr[] = array("Krebse, Flusskrebse", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Libellen", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Libellen", "101", "Fang");
$methods_artgroup_arr[] = array("Libellen", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Libellen", "107", "Video");
$methods_artgroup_arr[] = array("Libellen", "109", "Foto");
$methods_artgroup_arr[] = array("Libellen", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Libellen", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Netzflügler", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Netzflügler", "101", "Fang");
$methods_artgroup_arr[] = array("Netzflügler", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Netzflügler", "107", "Video");
$methods_artgroup_arr[] = array("Netzflügler", "109", "Foto");
$methods_artgroup_arr[] = array("Netzflügler", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Netzflügler", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Netzflügler", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Reptilien", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Reptilien", "101", "Fang");
$methods_artgroup_arr[] = array("Reptilien", "107", "Video");
$methods_artgroup_arr[] = array("Reptilien", "108", "Radiotelemetriedaten");
$methods_artgroup_arr[] = array("Reptilien", "109", "Foto");
$methods_artgroup_arr[] = array("Reptilien", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Reptilien", "300", "Totfund");
$methods_artgroup_arr[] = array("Reptilien", "301", "Verunfallt (Strasse)");
$methods_artgroup_arr[] = array("Säugetiere", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Säugetiere", "101", "Fang");
$methods_artgroup_arr[] = array("Säugetiere", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Säugetiere", "103", "Ruf, Schrei, Gesang");
$methods_artgroup_arr[] = array("Säugetiere", "104", "Gesang und Sichtbeobachtung");
$methods_artgroup_arr[] = array("Säugetiere", "106", "Ruf (Ohr) und Foto");
$methods_artgroup_arr[] = array("Säugetiere", "107", "Video");
$methods_artgroup_arr[] = array("Säugetiere", "108", "Radiotelemetriedaten");
$methods_artgroup_arr[] = array("Säugetiere", "109", "Foto");
$methods_artgroup_arr[] = array("Säugetiere", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Säugetiere", "201", "Kunstbau Biber");
$methods_artgroup_arr[] = array("Säugetiere", "202", "Nest, Biberburg");
$methods_artgroup_arr[] = array("Säugetiere", "204", "Erdbau, Höhle, Auswurfhügel");
$methods_artgroup_arr[] = array("Säugetiere", "205", "Suhle");
$methods_artgroup_arr[] = array("Säugetiere", "206", "Damm");
$methods_artgroup_arr[] = array("Säugetiere", "207", "Burg eingestürzt");
$methods_artgroup_arr[] = array("Säugetiere", "208", "Schwimmende Burg");
$methods_artgroup_arr[] = array("Säugetiere", "209", "Mittelbau Biber");
$methods_artgroup_arr[] = array("Säugetiere", "300", "Totfund");
$methods_artgroup_arr[] = array("Säugetiere", "301", "Verunfallt (Strasse)");
$methods_artgroup_arr[] = array("Säugetiere", "302", "Abschuss");
$methods_artgroup_arr[] = array("Säugetiere", "303", "Schädel");
$methods_artgroup_arr[] = array("Säugetiere", "304", "Knochenreste");
$methods_artgroup_arr[] = array("Säugetiere", "305", "Haare");
$methods_artgroup_arr[] = array("Säugetiere", "306", "Gewölle");
$methods_artgroup_arr[] = array("Säugetiere", "307", "Reste Körpergewebe, Muskel, Blut");
$methods_artgroup_arr[] = array("Säugetiere", "313", "Kot, Losung");
$methods_artgroup_arr[] = array("Säugetiere", "400", "Ausstieg, Wechsel");
$methods_artgroup_arr[] = array("Säugetiere", "401", "Kanal");
$methods_artgroup_arr[] = array("Säugetiere", "402", "Fällplatz");
$methods_artgroup_arr[] = array("Säugetiere", "403", "Trittsiegel, Spurenfolge");
$methods_artgroup_arr[] = array("Säugetiere", "404", "Schäden");
$methods_artgroup_arr[] = array("Säugetiere", "405", "Nahrungsreste, Einzelfrassspuren");
$methods_artgroup_arr[] = array("Säugetiere", "406", "Wintervorrat");
$methods_artgroup_arr[] = array("Säugetiere", "407", "Frassplatz");
$methods_artgroup_arr[] = array("Säugetiere", "408", "Raubtierriss");
$methods_artgroup_arr[] = array("Säugetiere", "409", "Markierung, Bibergeil");
$methods_artgroup_arr[] = array("Schmetterlinge, Grossschmetterlinge, Glasflügler", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Schmetterlinge, Grossschmetterlinge, Glasflügler", "101", "Fang");
$methods_artgroup_arr[] = array("Schmetterlinge, Grossschmetterlinge, Glasflügler", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Schmetterlinge, Grossschmetterlinge, Glasflügler", "109", "Foto");
$methods_artgroup_arr[] = array("Schmetterlinge, Grossschmetterlinge, Glasflügler", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Schmetterlinge, Grossschmetterlinge, Glasflügler", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Schmetterlinge, Grossschmetterlinge, Glasflügler", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Schmetterlinge, Grossschmetterlinge, Glasflügler", "405", "Nahrungsreste, Einzelfrassspuren");
$methods_artgroup_arr[] = array("Schmetterlinge, Kleinschmetterlinge", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Schmetterlinge, Kleinschmetterlinge", "101", "Fang");
$methods_artgroup_arr[] = array("Schmetterlinge, Kleinschmetterlinge", "109", "Foto");
$methods_artgroup_arr[] = array("Schmetterlinge, Kleinschmetterlinge", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Schmetterlinge, Kleinschmetterlinge", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Schmetterlinge, Kleinschmetterlinge", "210", "Schlupfloch, Fluchtröhre");
$methods_artgroup_arr[] = array("Schmetterlinge, Kleinschmetterlinge", "314", "Köcher");
$methods_artgroup_arr[] = array("Schmetterlinge, Kleinschmetterlinge", "405", "Nahrungsreste, Einzelfrassspuren");
$methods_artgroup_arr[] = array("Schmetterlinge, Tagfalter und Zygeniden", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Schmetterlinge, Tagfalter und Zygeniden", "101", "Fang");
$methods_artgroup_arr[] = array("Schmetterlinge, Tagfalter und Zygeniden", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Schmetterlinge, Tagfalter und Zygeniden", "109", "Foto");
$methods_artgroup_arr[] = array("Schmetterlinge, Tagfalter und Zygeniden", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Schmetterlinge, Tagfalter und Zygeniden", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Schmetterlinge, Tagfalter und Zygeniden", "405", "Nahrungsreste, Einzelfrassspuren");
$methods_artgroup_arr[] = array("Spinnentiere", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Spinnentiere", "101", "Fang");
$methods_artgroup_arr[] = array("Spinnentiere", "109", "Foto");
$methods_artgroup_arr[] = array("Spinnentiere", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Spinnentiere", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Steinfliegen", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Steinfliegen", "101", "Fang");
$methods_artgroup_arr[] = array("Steinfliegen", "109", "Foto");
$methods_artgroup_arr[] = array("Steinfliegen", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Steinfliegen", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Steinfliegen", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Tausendfüssler", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Tausendfüssler", "101", "Fang");
$methods_artgroup_arr[] = array("Tausendfüssler", "109", "Foto");
$methods_artgroup_arr[] = array("Tausendfüssler", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Tausendfüssler", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Tausendfüssler", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Wanzen, Landwanzen", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Wanzen, Landwanzen", "101", "Fang");
$methods_artgroup_arr[] = array("Wanzen, Landwanzen", "102", "Mit Fernglas bestimmt");
$methods_artgroup_arr[] = array("Wanzen, Landwanzen", "109", "Foto");
$methods_artgroup_arr[] = array("Wanzen, Landwanzen", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Wanzen, Landwanzen", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Wanzen, Wasserwanzen", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Wanzen, Wasserwanzen", "101", "Fang");
$methods_artgroup_arr[] = array("Wanzen, Wasserwanzen", "109", "Foto");
$methods_artgroup_arr[] = array("Wanzen, Wasserwanzen", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Wanzen, Wasserwanzen", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Weichtiere", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Weichtiere", "101", "Fang");
$methods_artgroup_arr[] = array("Weichtiere", "109", "Foto");
$methods_artgroup_arr[] = array("Weichtiere", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Weichtiere", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Weichtiere", "308", "Lebendiges Tier, frisches Gehäuse");
$methods_artgroup_arr[] = array("Weichtiere", "309", "Wahrscheinliches Vorkommen");
$methods_artgroup_arr[] = array("Weichtiere", "310", "Status unklar");
$methods_artgroup_arr[] = array("Weichtiere", "311", "Vorkommen vermutlich ehemalig");
$methods_artgroup_arr[] = array("Weichtiere", "312", "Subfossile Gehäuse");
$methods_artgroup_arr[] = array("Zecken", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Zecken", "101", "Fang");
$methods_artgroup_arr[] = array("Zecken", "109", "Foto");
$methods_artgroup_arr[] = array("Zecken", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Zecken", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Zweiflügler, Acalyptera", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Zweiflügler, Acalyptera", "101", "Fang");
$methods_artgroup_arr[] = array("Zweiflügler, Acalyptera", "109", "Foto");
$methods_artgroup_arr[] = array("Zweiflügler, Acalyptera", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Zweiflügler, Acalyptera", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Zweiflügler, Aschiza (Phoriden, Syrphiden)", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Zweiflügler, Aschiza (Phoriden, Syrphiden)", "101", "Fang");
$methods_artgroup_arr[] = array("Zweiflügler, Aschiza (Phoriden, Syrphiden)", "109", "Foto");
$methods_artgroup_arr[] = array("Zweiflügler, Aschiza (Phoriden, Syrphiden)", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Zweiflügler, Aschiza (Phoriden, Syrphiden)", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Zweiflügler, Calyptera", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Zweiflügler, Calyptera", "101", "Fang");
$methods_artgroup_arr[] = array("Zweiflügler, Calyptera", "109", "Foto");
$methods_artgroup_arr[] = array("Zweiflügler, Calyptera", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Zweiflügler, Calyptera", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Culicomorpha", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Culicomorpha", "101", "Fang");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Culicomorpha", "109", "Foto");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Culicomorpha", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Culicomorpha", "111", "genetisch bestimmt");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Culicomorpha", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Culicomorpha", "113", "Morphologische Bestimmung");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Tipulomorpha", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Tipulomorpha", "101", "Fang");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Tipulomorpha", "109", "Foto");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Tipulomorpha", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Zweiflügler, Nematocera, Tipulomorpha", "112", "Genitalbestimmung");
$methods_artgroup_arr[] = array("Zweiflügler, Orthorrhapha", "100", "Sichtbeobachtung");
$methods_artgroup_arr[] = array("Zweiflügler, Orthorrhapha", "101", "Fang");
$methods_artgroup_arr[] = array("Zweiflügler, Orthorrhapha", "109", "Foto");
$methods_artgroup_arr[] = array("Zweiflügler, Orthorrhapha", "110", "Unter Binokular bestimmt");
$methods_artgroup_arr[] = array("Zweiflügler, Orthorrhapha", "112", "Genitalbestimmung");


foreach ($methods_artgroup_arr as $ma){

	$artgroup_check = pg_fetch_array(pg_query($newdb, "select * from organism_artgroup where name='".pg_escape_string($ma[0])."'"));
	if($ma[0] == $artgroup_check['name']){
		$method_check = pg_fetch_array(pg_query($newdb, "select * from organism_artgroup_detmethod where name='".pg_escape_string($ma[2])."'"));
		if($ma[2] == $method_check['name']){

			$insert_subscription = "insert into organism_artgroup_detmethod_subscription (organism_artgroup_id, organism_artgroup_detmethod_id) values ('".$artgroup_check['id']."', '".$method_check['id']."');";
			pg_query($insert_subscription);

		}else{
			echo "Method not found in DB: ".$ma[2]."<br>";
		}



	}else{
		echo "Artgroup not fround in DB: ".$ma[0]."<br>";
	}







}


?>