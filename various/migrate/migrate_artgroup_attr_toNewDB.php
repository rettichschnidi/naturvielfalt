<?php
$olddb = pg_connect("host=localhost port=5432 dbname=swissmon user=postgres password=postgres");
$newdb = pg_connect("host=localhost port=5432 dbname=naturvielfalt_dev user=postgres password=postgres");


// echo pg_last_oid(pg_query($newdb, "insert into organism_artgroup_detmethod_type (name, format) values ('test', 'test')"));
// pg_query($newdb, "update organism_artgroup set name='".pg_escape_string("SŠugetiere")."' where name='".pg_escape_string("SŠugetiere (ohne FledermŠuse)")."'");
// pg_query($newdb, "update organism_artgroup set name='".pg_escape_string('Wirbeltiere')."' where name='".pg_escape_string('andere Wirbeltiere')."'");
// die();


/**
 * Drop existing tables & create it
 */

$sql_create = "


DROP TABLE IF EXISTS organism_artgroup_attr_subscription;
DROP TABLE IF EXISTS organism_artgroup_attr_values;
DROP TABLE IF EXISTS organism_artgroup_attr;
DROP TABLE IF EXISTS organism_artgroup_attr_type;




CREATE TABLE organism_artgroup_attr
(
	id serial NOT NULL UNIQUE,
	name text,
	organism_artgroup_attr_type_id int NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_attr_values
(
	id serial NOT NULL UNIQUE,
	value text,
	organism_artgroup_attr_id int NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_attr_subscription
(
	id serial NOT NULL UNIQUE,
	organism_artgroup_id int NOT NULL,
	organism_artgroup_attr_id int NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_attr_type
(
	id serial NOT NULL UNIQUE,
	name text,
	format text,
	PRIMARY KEY (id)
) WITHOUT OIDS;

ALTER TABLE organism_artgroup_attr_subscription
	ADD FOREIGN KEY (organism_artgroup_id)
	REFERENCES organism_artgroup (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_artgroup_attr_subscription
	ADD FOREIGN KEY (organism_artgroup_attr_id)
	REFERENCES organism_artgroup_attr (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_artgroup_attr_values
	ADD FOREIGN KEY (organism_artgroup_attr_id)
	REFERENCES organism_artgroup_attr (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_artgroup_attr
	ADD FOREIGN KEY (organism_artgroup_attr_type_id)
	REFERENCES organism_artgroup_attr_type (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;






";
pg_query($newdb, $sql_create);

//data for organism_artgroup_detmethod_type
$data_detmet_type = "select * from attribute_format";
$data_detmet_type = pg_fetch_all(pg_query($olddb, $data_detmet_type));
foreach ($data_detmet_type as $tmp){
	pg_query($newdb, "insert into organism_artgroup_attr_type (name, format) values ('".pg_escape_string($tmp['name'])."', '".pg_escape_string($tmp['format'])."')");
}

$tmp = "";
// data for organism_artgroup_detmethod
$before_name_id = "";
$before_name = "";
$before_type_id ="";
$before_type = "";

$data_detmet = "select it.name, ita.name attr_name, af.name attr_type, af.format attr_value
from inventory_type_attribute ita
join attribute_format as af on af.id=ita.attribute_format_id
join inventory_type as it on it.id=ita.inventory_type_id
order by ita.inventory_type_id asc";
$data_detmet = pg_fetch_all(pg_query($olddb, $data_detmet));

foreach ($data_detmet as $tmp){

	$type_check = pg_fetch_array(pg_query($newdb, "select * from organism_artgroup_attr_type where name='".pg_escape_string($tmp['attr_type'])."'"));
	$before_type_id = $type_check['id'];
	$before_type = $type_check['attr_type'];
// 	print_r($type_check);exit();
// 	if($tmp['attr_type'] != $type_check['name']){
// 		pg_query($newdb, "insert into organism_artgroup_detmethod_type (name, format) values ('".pg_escape_string($tmp['attr_type'])."', '".pg_escape_string($tmp['attr_value'])."')");
// 		$before_type_tmp = pg_fetch_array(pg_query($newdb, "select * from organism_artgroup_detmethod_type where name='".pg_escape_string($tmp['attr_type'])."'"));
// 		$before_type_id = $before_type_tmp['id'];
// 		$before_type = $tmp['attr_type'];
// 	}
	$met_check = pg_fetch_array(pg_query($newdb, "select * from organism_artgroup_attr where name='".pg_escape_string($tmp['attr_name'])."'"));
	if($tmp['attr_name'] != $met_check['name']){
		pg_query($newdb,"insert into organism_artgroup_attr (name, organism_artgroup_attr_type_id) values ('".pg_escape_string($tmp['attr_name'])."', '".$before_type_id."')");
		$before_name_tmp = pg_fetch_array(pg_query($newdb, "select * from organism_artgroup_attr where name='".pg_escape_string($tmp['attr_name'])."'"));
		$before_name_id = $before_name_tmp['id'];
		$before_name = $tmp['attr_name'];
	}
	$artgroup = pg_fetch_array(pg_query($newdb, "select * from organism_artgroup where name='".pg_escape_string($tmp['name'])."'"));
	$artgroup_id = $artgroup['id'];
	if(!pg_query($newdb,"insert into organism_artgroup_attr_subscription (organism_artgroup_id, organism_artgroup_attr_id) values ('".$artgroup_id."', '".$before_name_id."')")){
		echo "||||  ".$tmp['name']."   |||";
	}
}


$tmp="";
$before_name ="";
$before_name_id ="";

$data_detmet_val = "select ita.name, itav.value
from inventory_type_attribute_dropdown_value itav
join inventory_type_attribute as ita on ita.id=itav.inventory_type_attribute_id";
$data_detmet_val = pg_fetch_all(pg_query($olddb, $data_detmet_val));

foreach ($data_detmet_val as $tmp){
	if($tmp['name']!=$before_name){
		$before_arr = pg_fetch_array(pg_query($newdb, "select * from organism_artgroup_attr where name='".pg_escape_string($tmp['name'])."'"));
		$before_name_id = $before_arr['id'];
		$before_name = $tmp['name'];
	}
	$value_check = pg_fetch_array(pg_query($newdb, "select * from organism_artgroup_attr_values where value='".pg_escape_string($tmp['value'])."' AND organism_artgroup_attr_id='".$before_name_id."'"));
// 	die(print_r($value_check));
	if(!is_array($value_check)){
		pg_query($newdb, "insert into organism_artgroup_attr_values (value, organism_artgroup_attr_id) values ('".pg_escape_string($tmp['value'])."', '".$before_name_id."')");
	}
}










?>