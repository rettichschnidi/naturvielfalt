<?php

/**
 * Get artgroups, optionally filtered with the parent id, as html <optio> tag
 * @param int $parent
 * @param int $deep
 * @return string
 */
function organism_artgroup_get_all_as_option($parent = false, $deep = 0){

	if($parent != false){
		$parent_sql = "'" . db_escape_field($parent) . "' AND parent != id";
	}else{
		$parent_sql = "id";
	}

	$top_sql = "SELECT * FROM {organism_artgroup} WHERE parent=" . $parent_sql;
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

/**
 * Get an array of all children id's of a artgroup
 * @param int $artgroup_id
 * @param array $output (internal needed)
 * @return array of int
 */
function organism_artgroup_get_children($artgroup_id, $output = false){

	if(!$artgroup_id) return false;
	$artgroup_id_sql = "'".db_escape_field($artgroup_id)."' AND parent!=id";

	$top_sql = "SELECT * FROM {organism_artgroup} WHERE parent=".$artgroup_id_sql;
	$top_result= db_query($top_sql);

	if(!$output) $output = array();

	foreach ($top_result as $top){
		$output[] = $top->id;
		$output = organism_artgroup_get_children($top->id, $output);
	}

	return $output;
}

/**
 * Return an sql formated string from a array of id's
 * @param array $ids_as_array
 * @param string $col_name name of the column which is be used in the query
 * @param string $and_or_or
 * @return string
 */
function organism_artgroup_get_sql_from_id($ids_as_array, $col_name, $and_or_or = "OR"){
	$sql_state = " ";
	foreach ($ids_as_array as $id){
		$sql_state .= $col_name."='".$id."' ".$and_or_or." ";
	}
// 	return $sql_state;
	return substr($sql_state, 0, -strlen($and_or_or)-1);
}

/**
 * Return determation methods of an artgroup
 * optionally as html <option>
 * @param int $artgroup_id
 * @param boolean $as_html_option
 * @return object or string
 */
function organism_artgroup_get_determation_methods($artgroup_id = false, $as_html_option = false, $selected_id = false){
	$artgroup_filter = "";
	if($artgroup_id){
		if($artgroup_id > 0 && $artgroup_id < 1000000){
			$children_ids = organism_artgroup_get_children($artgroup_id);
			$children_ids[] = $artgroup_id;
			$children_sql = organism_artgroup_get_sql_from_id($children_ids, 'oads.organism_artgroup_id');
			$artgroup_filter = "JOIN {organism_artgroup_detmethod_subscription} oads on ".$children_sql
								."WHERE oad.id=oads.organism_artgroup_detmethod_id";
		}
	}

	$determation_sql = "SELECT oad.id, oad.name, oad.cscf_id FROM {organism_artgroup_detmethod} oad ".$artgroup_filter." ORDER BY oad.name asc";
	$results = db_query($determation_sql);

	if(!$as_html_option) return $results;

	$html = '<option value="0">'.t('Not definied').'</option>';
	foreach ($results as $result){
		$selected = "";
		if($selected_id && $selected_id == $result->id) $selected = " selected";
		$html .= '<option value="'.$result->id.'" id="artgroup_detmethod_value_'.$result->id.'" style="display: none;" '.$selected.'>'.t($result->name).'</option>';
	}
	return $html;
}

/**
 *
 * @param int $artgroup_id
 * @param boolean $as_html
 * @param string $html_before_desc
 * @param string $html_after_desc
 * @param string $html_before_form
 * @param string $html_after_form
 * @return
 */
function organism_artgroup_get_attributes($artgroup_id = false, $as_html = false, $html_before_desc = false
		, $html_after_desc = false, $html_before_form = false, $html_after_form = false, $attribute_id = false){
	$artgroup_filter = "";
	if($artgroup_id){
		if($artgroup_id > 0 && $artgroup_id < 1000000){
			$children_ids = organism_artgroup_get_children($artgroup_id);
			$children_ids[] = $artgroup_id;
			$children_sql = organism_artgroup_get_sql_from_id($children_ids, 'oaas.organism_artgroup_id');
			$artgroup_filter = " JOIN {organism_artgroup_attr_subscription} oaas on ".$children_sql
			." WHERE oaa.id=oaas.organism_artgroup_attr_id";
		}
	}
	if($attribute_id > 0 && $attribute_id < 1000000 && $attribute_id != '0'){
		$artgroup_filter .= " WHERE oaa.id='".db_escape_field($attribute_id)."'";
	}

	$attributes_sql = "SELECT oaa.id, oaa.name, oaat.name type_name, oaat.format type_format FROM {organism_artgroup_attr} oaa "
						."JOIN {organism_artgroup_attr_type} oaat ON oaat.id=oaa.organism_artgroup_attr_type_id".$artgroup_filter;
	$attributes_results = db_query($attributes_sql);

// 	if(!$as_html) return $attributes_results;

	$attributes_with_values = array();
	foreach ($attributes_results as $attributes_result){
		$values = db_query("SELECT * FROM {organism_artgroup_attr_values} WHERE organism_artgroup_attr_id='".$attributes_result->id."'");
		$tmp_val = false;
		foreach ($values as $value){
			$tmp_val[$value->id] = $value->value;
		}
		$attributes_with_values[] = array(
				'name' => $attributes_result->name,
				'id' => $attributes_result->id,
				'type_name' => $attributes_result->type_name,
				'type_format' => $attributes_result->type_format,
				'values' => $tmp_val
		);
	}

	if(!$as_html) return $attributes_with_values;

	$attributes_with_values_html = "";
	foreach ($attributes_with_values as $tmp){
		$attributes_with_values_html .= organism_artgroup_parse_html_forms($tmp, $html_before_desc, $html_after_desc, $html_before_form, $html_after_form);
	}
	return $attributes_with_values_html;

}

/**
 *
 * @param int $attribute_id
 */
function organism_artgroup_get_attribute($attribute_id){
	return organism_artgroup_get_attributes(false, false, false, false, false, false, $attribute_id);
}

/**
 *
 * @param array $form
 * @param string $html_before_desc
 * @param string $html_after_desc
 * @param string $html_before_form
 * @param string $html_after_form
 */
function organism_artgroup_parse_html_forms($form, $html_before_desc = false, $html_after_desc = false, $html_before_form = false, $html_after_form = false){
	$output = "";
	$name_trimd = str_replace(' ', '', $form['name']);
	$form_name = 'id="attributes_'.$name_trimd.'" name="attributes['.$form['id'].']"';
	if($html_before_desc) $output .= str_replace('#id#', 'id="attributes_'.$name_trimd.'_bf', $html_before_desc);
	$output .= t($form['name'].':');
	if($html_after_desc) $output .= $html_after_desc;
	if($html_before_form) $output .= $html_before_form;

	switch ($form['type_format']){

		case 'string':
			$output .= '<input type="text" '.$form_name.'>';
			break;

		case 'dropdown':
			$output .= '
			<select '.$form_name.'><option value="0">'.t('Nicht definiert').'</option>';
			foreach ($form['values'] as $key => $value){
				$output .= '<option value="'.$key.'">'.$value.'</option>';
			}
			$output .= '</select>';
			break;

		case 'date':
			$output .= '<input type="date" '.$form_name.'>';
			break;

		case 'int':
			$output .= '<input type="number" '.$form_name.'>';
			break;

	}

	if($html_after_form) $output .= $html_after_form;
	return $output;
}


?>




















