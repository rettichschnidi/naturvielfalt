<?php

$limit = '';

// $olddb = pg_connect("host=82.220.2.223 port=5432 dbname=swissmon_dev user=swissmon_dev password=gu7ahs9shof6xeekohG4");
$olddb = pg_connect("host=localhost port=5432 dbname=iphone_db user=postgres password=postgres");
$newdb = pg_connect("host=localhost port=5432 dbname=naturvielfalt_dev user=postgres password=postgres");


/**
 * Drop existing tables & create it
 */

$sql_create = "


DROP TABLE IF EXISTS organism_artgroup_subscription;
DROP TABLE IF EXISTS organism_artgroup;


CREATE TABLE organism_artgroup
(
	id serial NOT NULL UNIQUE,
	name text NOT NULL,
	parent int DEFAULT 1,
	pos int,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_subscription
(
	id serial NOT NULL UNIQUE,
	organism_artgroup_id int NOT NULL,
	-- Die eigene Id, wird fortlaufend inkrementiert.
	organism_id int NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;

ALTER TABLE organism_artgroup_subscription
	ADD FOREIGN KEY (organism_artgroup_id)
	REFERENCES organism_artgroup (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;

ALTER TABLE organism_artgroup_subscription
	ADD FOREIGN KEY (organism_id)
	REFERENCES public.organism (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


";
// pg_query($newdb, $sql_create);



$res1 = pg_query($olddb, 'select * from classification');
$data_old = pg_fetch_all($res1);
// die(print_r($data_old));
$insert_artgroup = "";
foreach ($data_old as $temp){
	$insert_artgroup .= "insert into organism_artgroup (id, name, parent, pos) values
	('".pg_escape_string($temp['classification_id'])."', '".pg_escape_string($temp['name_de'])."', '".$temp['parent']."', '".$temp['position']."');
	";
// 	pg_query($newdb, $insert_artgroup);
	echo '<br>Insert artgroup: '.$temp['name_de'];
}
// die();

// pg_query($newdb, "update organism_artgroup set name='".pg_escape_string('S�ugetiere')."' where name='".pg_escape_string('S�ugetiere (ohne Flederm�use)')."'");
// pg_query($newdb, "update organism_artgroup set name='".pg_escape_string('Wirbeltiere')."' where name='".pg_escape_string('andere Wirbeltiere')."'");

pg_query($newdb, $insert_artgroup);




$res2 = pg_query($newdb, "select osn.organism_id, osn.name latnew from organism_scientific_name osn");
$data_temp = pg_fetch_all($res2);
$success = 0;
$failed = 0;
$notfound = array();

foreach ($data_temp as $dtmp){

	$data_org_ip = pg_query($olddb, "select * from organism where name_sc='".pg_escape_string($dtmp['latnew'])."'");
	$data_org_ip = pg_fetch_array($data_org_ip);

	if($data_org_ip){
		$success++;
		$catsql = "select ct.classification_id, c.name_de from classification_taxon ct
		join classification as c on c.classification_id=ct.classification_id
		where ct.taxon_id='".$data_org_ip['id']."' AND c.name_de!='Alle' group by ct.classification_id, c.name_de";
		$catres = pg_fetch_all(pg_query($olddb, $catsql));

		echo "Gefunden:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;".$dtmp['organism_id']." ".$dtmp['latnew']." in Kat: ";
		foreach ($catres as $cat){
			echo $cat['name_de']." ";
			$insert_new_artgroup = "insert into organism_artgroup_subscription
			(organism_artgroup_id, organism_id) values ('".$cat['classification_id']."', '".$dtmp['organism_id']."')";
			pg_query($newdb, $insert_new_artgroup);
		}
		echo "<br>";
	}else{
		$failed++;
		echo "<font color=red>Nicht gefunden:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;".$dtmp['organism_id']." ".$dtmp['latnew']."</font><br>";
		$notfound[] = $dtmp['organism_id'];
	}


// 	if(count($data_org_ip))
// 	print_r(count($data_org_ip));

}
echo "<p>&nbsp;</p>";
$success_after_val = $success;
$failed_after_val = $failed;
$newnotfound = array();

foreach($notfound as $nf){
	$valsql = "select * from organism_artgroup_subscription where organism_id='".$nf."'";
	$valres = pg_fetch_all(pg_query($newdb, $valsql));
// 	print_r($valres);
// 	echo "-- ".count($valres)." ";
// 	if(is_array($valres)) {echo "io"."<br>";}else{echo "<br>";}

	if(is_array($valres)){
		$success_after_val++;
		$failed_after_val--;
	}else{
		$newnotfound [] = $nf;
	}
}

echo "Success: ".$success."<br>Succes after validate: ".$success_after_val."<br>Failed: ".$failed."<br>Failed after validate: ".$failed_after_val."<br>";
// print_r($notfound);
// print_r($data_temp);

print_r($newnotfound);
// foreach ($notfound as $nfr) echo "-- ".$nfr."<br>";

exit();






// echo "<table border=1>";
// echo "<td><b>Artgruppe</b></td>";
// echo "<td><b>Alt DB organsim ID</b></td>";
// echo "<td><b>Alt DB Lat Name</b></td>";
// echo "<td><b>Neu DB Lat Name</b></td>";
// echo "<td><b>Neu DB organsim ID</b></td>";
// echo "</tr>";

// $error_count = 0;
// $succes_count = 0;

// foreach($data_old as $row){
// $res2 = pg_query($newdb, "select osn.organism_id, osn.name from organism_scientific_name osn
// where osn.name = '".pg_escape_string($row['name_sc'])."'");
// $data_temp = pg_fetch_array($res2);


// if(!$data_temp['name']){
// 	$color=' bgcolor=red';
// 	$error_count++;
// }else{
// 	$color=' bgcolor=green';
// 	$succes_count++;
// }
// echo "
// <tr".$color.">";
// echo "<td>".$row['artgruppe']."</td>";
// echo "<td>".$row['id']."</td>";
// echo "<td>".$row['name_sc']."</td>";
// echo "<td>".$data_temp['name']."</td>";
// echo "<td>".$data_temp['organism_id']."</td>";
// echo "</tr>";
// }
// echo "</table>";

// echo "Error Count: ".$error_count;
// echo "<br>Success Count: ".$succes_count;



echo "<hr>";



$res1 = pg_query($newdb, "select osn.organism_id, osn.name from organism_scientific_name osn
		where osn.name like 'Aspicilia%'
		order by osn.organism_id asc
		 ".$limit);

$data_new = pg_fetch_all($res1);

echo "<table border=1>";
echo "<td><b>Artgruppe</b></td>";
echo "<td><b>Alt DB organsim ID</b></td>";
echo "<td><b>Alt DB Lat Name</b></td>";
echo "<td><b>Neu DB Lat Name</b></td>";
echo "<td><b>Neu DB organsim ID</b></td>";
echo "</tr>";

$error_count = 0;
$succes_count = 0;

foreach($data_new as $row){
	$res2 = pg_query($olddb, "select o.name_de, o.id,  o.name_sc, it.name artgruppe from organism as o
			join inventory_type as it on it.id=o.inventory_type_id
			where o.name_sc = '".pg_escape_string($row['name'])."'
			");
	$data_temp = pg_fetch_array($res2);

	if(!$data_temp['name_sc']){
		$color=' bgcolor="#FAA7F8"';
		$error_count++;
	}else{
		$color=' bgcolor="#D4FBCE"';
		$succes_count++;

		pq_query($newdb, "insert");

	}



	echo "
	<tr".$color.">";
	echo "<td>".$data_temp['artgruppe']."</td>";
	echo "<td>".$data_temp['id']."</td>";
	echo "<td>".$data_temp['name_sc']."</td>";
	echo "<td>".$row['name']."</td>";
	echo "<td>".$row['organism_id']."</td>";
	echo "</tr>";
}
echo "</table>";

echo "Error Count: ".$error_count;
echo "<br>Success Count: ".$succes_count;






// print_r($data_zuweisung);

pg_close($olddb);
pg_close($newdb);

?>