<?php

/**
 * Get artgroups, optionally filtered with the parent id, as html <optio> tag
 * @param int $parent
 * @param int $deep
 * @return string
 */
function organism_artgroup_get_all_as_option($parent = false, $deep = 0, $selected_id = false){

	if($parent != false){
		$parent_sql = "'" . db_escape_field($parent) . "' AND parent != id";
	}else{
		$parent_sql = "id";
	}
	if($selected_id == '') $selected_id = false;

	$top_sql = "SELECT * FROM {organism_artgroup} WHERE parent=" . $parent_sql;
	$top_result= db_query($top_sql);

	$output = "";
	$spacer = "";
	for($i=0; $i < $deep; $i++) $spacer = $spacer."&nbsp;&nbsp;&nbsp;";

	foreach ($top_result as $top){
		$sel = '';
		if($selected_id && $selected_id == $top->id) $sel = ' selected="selected"';
		$output .= '<option value="'.$top->id.'"'.$sel.'>'.$spacer.t($top->name).'</option>';
		$output .= organism_artgroup_get_all_as_option($top->id, $deep+1, $selected_id);
	}

	return $output;
}

/**
 * Get an array of all children id's of a artgroup
 * @param int $artgroup_id
 * @param array $output (internal needed)
 * @return array of int
 */
function organism_artgroup_get_children($artgroup_id = false, $output = false, $organism_id = false){
	$join = '';
	if(!$artgroup_id) $artgroup_id = 3;
	$wooor = array(':parent' => $artgroup_id);

// 	if($organism_id){
		$join = "JOIN {organism_artgroup_subscription} oas "
				."ON oas.organism_artgroup_id = oa.id "
				."AND oas.organism_id=:organism_id ";
		$wooor[':organism_id'] = $organism_id;
// 	}
	$top_sql = "SELECT oa.id id, oas.organism_id organism_id FROM {organism_artgroup} oa ".$join." WHERE oa.parent=:parent AND oa.parent!=oa.id";
// 	die($top_sql);
	$top_result= db_query($top_sql, $wooor);

	if(!$output) $output = array();

	foreach ($top_result as $top){
		if($organism_id){
			if($top->organism_id != "") $output[] = $top->id;
		}else{
			if($top->id == '') continue;
			$output[] = $top->id;
		}
// 		die(print_r($top));
		$output = organism_artgroup_get_children($top->id, $output, $organism_id);
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
 * @param int selected_id
 * @param array visisible_arr id's as value
 * @return object or string
 */
function organism_artgroup_get_determation_methods($artgroup_id = false, $as_html_option = false, $selected_id = false, $visisible_arr = false){
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

	$html = '<option value="0" id="artgroup_detmethod_value_0">'.t('Not definied').'</option>';
	foreach ($results as $result){
		$selected = "";
		if($selected_id && $selected_id == $result->id) $selected = ' selected="selected"';
		$style = 'style="display: none;" ';
		if($visisible_arr && array_search($result->id, $visisible_arr)) $style = 'style="display: block;" ';
		$html .= '<option value="'.$result->id.'" id="artgroup_detmethod_value_'.$result->id.'"  '.$style.$selected.'>'.t($result->name).'</option>';
	}
	return $html;
}

/**
 * Return all or a single artgroup as array or as html tags
 * @param int $artgroup_id
 * @param boolean $as_html
 * @param string $html_before_desc
 * @param string $html_after_desc
 * @param string $html_before_form
 * @param string $html_after_form
 * @return array OR string
 */
function organism_artgroup_get_attributes($artgroup_id = false, $as_html = false, $html_before_desc = false
		, $html_after_desc = false, $html_before_form = false, $html_after_form = false, $attribute_id = false, $attributes_selected = false){
	global $user;
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
		$artgroup_filter = " WHERE oaa.id='".db_escape_field($attribute_id)."'";
	}
	($artgroup_filter == "") ? $artgroup_filter = ' WHERE ' : $artgroup_filter .= ' AND ';
	$artgroup_filter .= "(oaa.users_uid=:users_uid OR oaa.users_uid = '0')";

	$attributes_sql = "SELECT oaa.id, oaa.name, oaat.name type_name, oaat.format type_format, oaa.users_uid FROM {organism_artgroup_attr} oaa "
						."JOIN {organism_artgroup_attr_type} oaat ON oaat.id=oaa.organism_artgroup_attr_type_id".$artgroup_filter;
	$attributes_results = db_query($attributes_sql, array(':users_uid' => $user->uid));

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
				'values' => $tmp_val,
				'custom' => $attributes_result->users_uid,
		);
	}

	if(!$as_html) return $attributes_with_values;
// 	die(print_r($attributes_with_values));

	$attributes_with_values_html = "";
	foreach ($attributes_with_values as $tmp){
		$attributes_with_values_html .= organism_artgroup_parse_html_forms($tmp, $html_before_desc, $html_after_desc, $html_before_form, $html_after_form, $attributes_selected);
	}
	return $attributes_with_values_html;

}

/**
 * Returns a single attribute as array
 * @param int $attribute_id
 * @param bool $just_the_name
 * @return array OR String
 */
function organism_artgroup_get_attribute($attribute_id, $just_the_name = false){
	$attr = organism_artgroup_get_attributes(false, false, false, false, false, false, $attribute_id);
	if($just_the_name) return $attr[0]['name'];
	return $attr;
}

/**
 * parse a given array of attributes to html form elements
 * @param array $form
 * @param string $html_before_desc
 * @param string $html_after_desc
 * @param string $html_before_form
 * @param string $html_after_form
 * @return string
 */
function organism_artgroup_parse_html_forms($form, $html_before_desc = false, $html_after_desc = false,
		$html_before_form = false, $html_after_form = false, $attributes_selected = false){
	$output = "";
	$predefinied_value = '';
	$predefinied_value_id = '';
	$name_trimd = str_replace(' ', '', $form['name']);
	$form_name = 'id="attributes_'.$name_trimd.'" name="attributes['.$form['id'].']"';
	if($html_before_desc) $output .= str_replace('#id#', 'attributes_tr_'.$form['id'], $html_before_desc);
	if($attributes_selected && array_key_exists($form['id'], $attributes_selected)){
		$output = str_replace('display: none;', 'display: table-row;', $output);
		$predefinied_value = $attributes_selected[$form['id']]['value'];
		$predefinied_value_id = $attributes_selected[$form['id']]['value_id'];
	}
	$custom_delete = '';
	if($form['custom'] == '1'){
		$custom_delete = '<a href="" onclick="javascript:customAttributeDelete($(this).parents(\'tr\').attr(\'id\'));return false;" '
				.'title="'.t('Delete this custom attribute').'">&nbsp;<img src="'.base_path().drupal_get_path('module', 'observation').'/img/notfound.gif"></a>';
	}
	$output .= t($form['name']).$custom_delete.':';
	if($html_after_desc) $output .= $html_after_desc;
	if($html_before_form) $output .= $html_before_form;

	switch ($form['type_format']){

		case 'string':
			$output .= '<input type="text" '.$form_name.' value="'.$predefinied_value.'">';
			break;

		case 'dropdown':
			$output .= '
			<select '.$form_name.'><option value="0">'.t('Nicht definiert').'</option>';
			foreach ($form['values'] as $key => $value){
				$selected = '';
				if(isset($predefinied_value_id) && $predefinied_value_id == $key) $selected = ' selected="selected"';
				$output .= '<option value="'.$key.'" '.$selected.'>'.$value.'</option>';
			}
			$output .= '</select>';
			break;

		case 'date':
			$output .= '<input type="date" '.$form_name.' value="'.$predefinied_value.'">';
			break;

		case 'int':
			$output .= '<input type="number" '.$form_name.' value="'.$predefinied_value.'">';
			break;

	}

	if($html_after_form) $output .= $html_after_form;
	return $output;
}

/**
 * Return a array of all attributes id for an artgroup
 * @param int $artgroup_id
 * @return array
 */
function organism_artgroup_get_attributes_ids($artgroup_id){
	return db_query("SELECT organism_artgroup_attr_id id FROM {organism_artgroup_attr_subscription} "
			."WHERE organism_artgroup_id='".db_escape_field($artgroup_id)."' GROUP BY organism_artgroup_attr_id")
			->fetchCol();
}


/**
 * Return the name of a attribute value
 * @param int $value_id
 * @return String
 */
function organism_artgroup_get_attributes_value_name($value_id = false){
	if(!$value_id) return;
	$attr_val=  db_query("SELECT * FROM {organism_artgroup_attr_values} "
				."WHERE id=:value_id", array(':value_id' => $value_id))->fetchAssoc();
	return $attr_val['value'];
}

/**
 * Return a array of all determination methods id for an artgroup
 * @param int $artgroup_id
 * @return array
 */
function organism_artgroup_get_detmethods_ids($artgroup_id){
	return db_query("SELECT organism_artgroup_detmethod_id id FROM {organism_artgroup_detmethod_subscription} "
					."WHERE organism_artgroup_id='".db_escape_field($artgroup_id)."' GROUP BY organism_artgroup_detmethod_id")
					->fetchCol();
}

/**
 * Returns all artgroups id's where an organism is in
 * @param int $organism_id
 * @param bool $just_the_last
 * @return array || int
 */
function organism_artgroup_get_organsim_in_artgroup($organism_id, $just_the_last = false){
	$artgroups = organism_artgroup_get_children(3, false, $organism_id);
	if(!$just_the_last) return $artgroups;
	$output = '';
	foreach ($artgroups as $artgroup) $output = $artgroup;
	if($output == '') return 3;
	return $output;
}

/**
 * Return a artgroup or just the name
 * @param int $artgroup_id
 * @param bool $just_the_name
 * @return array || string
 */
function organism_artgroup_get_artgroup($artgroup_id, $just_the_name = false){
	if(!$artgroup_id) return;
	$artg =  db_query("SELECT * FROM {organism_artgroup} "
			."WHERE id=:id", array(':id' => $artgroup_id))->fetchAssoc();
	if($just_the_name) return $artg['name'];
	return $artg;
}

/**
 * Return a determination method
 * @param int $detmethod_id
 * @param bool $just_the_name
 * @return array|string
 */
function organism_artgroup_get_detmethod($detmethod_id, $just_the_name = false){
	if(!$detmethod_id || $detmethod_id == 0) return '';
	$detmethod = db_query("SELECT * FROM {organism_artgroup_detmethod} "
			."WHERE id=:id", array(':id' => $detmethod_id))->fetchAssoc();
	if($just_the_name) return $detmethod['name'];
	return $detmethod;
}



/**
 * add a custom attribute
 * @param string $name
 * @param int $artgroup_id
 */
function organism_artgroup_add_custom_attribute($name, $artgroup_id){
	global $user;

	if($artgroup_id == null || $artgroup_id == '') return false;

	$attribute['id'] = db_query("SELECT nextval('{organism_artgroup_attr}_id_seq') as nextval")->fetchField();
	$attribute['name'] = check_plain($name);
	$attribute['users_uid'] = $user->uid;
	$attribute['organism_artgroup_attr_type_id'] = 1;

	$attribute_subcr['organism_artgroup_attr_id'] = $attribute['id'];
	$attribute_subcr['organism_artgroup_id'] = $artgroup_id;

	db_insert('organism_artgroup_attr')->fields($attribute)->execute();
	db_insert('organism_artgroup_attr_subscription')->fields($attribute_subcr)->execute();

	return $attribute['id'];


}

/**
 * Checks if a custom attribute exists
 * @param int $attribute_id
 * @return bool | true if exists
 */
function organism_artgroup_custom_attribute_exist($attribute_id){
	global $user;
	$attribute_id = intval($attribute_id);
	$attribute_count = db_query('SELECT * FROM {organism_artgroup_attr} WHERE id=:attr_id AND users_uid=:users_uid',
			array(':users_uid' => $user->uid, ':attr_id' => $attribute_id))->rowCount();
	if($attribute_count < 1) return false;
	return true;
}

/**
 * delete a custom attribute
 * @param int $attribute_id
 */
function organism_artgroup_delete_custom_attribute($attribute_id){
	global $user;
	if(!organism_artgroup_custom_attribute_exist($attribute_id)) return false;
	db_delete('organism_artgroup_attr_subscription')
		->condition('organism_artgroup_attr_id', $attribute_id)->execute();
	db_delete('organism_artgroup_attr')->condition('id', $attribute_id)
		->condition('users_uid', $user->uid)->execute();
	return true;
}















