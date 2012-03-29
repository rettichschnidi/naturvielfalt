<?php


function organism_artgroup_get_all_as_option($parent = false, $deep = 0){

	if($parent != false){
		$parent_sql = "'".db_escape_field($parent)."' AND parent!=id";
	}else{
		$parent_sql = "id";
	}

	$top_sql = "SELECT * FROM organism_artgroup WHERE parent=".$parent_sql;
	$top_result= db_query($top_sql);

	$output = "";
	$spacer = "";
	for($i=0; $i < $deep; $i++) $spacer = $spacer."&nbsp;&nbsp;&nbsp;";

	foreach ($top_result as $top){
		$output .= '<option value="'.$top->id.'">'.$spacer.t($top->name).'</option>';
		$output .= organism_artgroup_get_all_as_option($top->id, $deep+1);
	}

	return $output;
}

function organism_artgroup_get_children($artgroup_id, $output = false){

	if(!$artgroup_id) return false;
	$artgroup_id_sql = "'".db_escape_field($artgroup_id)."' AND parent!=id";

	$top_sql = "SELECT * FROM organism_artgroup WHERE parent=".$artgroup_id_sql;
	$top_result= db_query($top_sql);

	if(!$output) $output = array();

	foreach ($top_result as $top){
		$output[] = $top->id;
		$output = organism_artgroup_get_children($top->id, $output);
	}

	return $output;
}

function organism_artgroup_get_sql_from_id($ids_as_array, $col_name, $and_or_or = "OR"){
	$sql_state = " ";
	foreach ($ids_as_array as $id){
		$sql_state .= $col_name."='".$id."' ".$and_or_or." ";
	}
// 	return $sql_state;
	return substr($sql_state, 0, -strlen($and_or_or)-1);
}




?>