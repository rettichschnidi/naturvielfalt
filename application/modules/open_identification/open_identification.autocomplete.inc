<?php
function organism_type_autocomplete($id) {
  $organisms = inventory_organism_autocomplete($id, $_REQUEST['term'], true);
  $results = array();
  $term = trim($_REQUEST['term']);
  $terms = explode(' ', $term);
  foreach($organisms as $organism) {
    $label = $organism['label'];
    $label_latin = $organism['label_latin'];
    $old_label = $organism['old_label'];
    $old_label_latin = $organism['old_label_latin'];
    if($old_label || $old_label_latin)
      $old_label = preg_replace('/'.$term.'/i', '<span class="ui-state-highlight">$0</span>', $old_label);
    else
      $label = preg_replace('/'.$term.'/i', '<span class="ui-state-highlight">$0</span>', $label);
    foreach($terms as $t) {
      if($old_label || $old_label_latin)
        $old_label_latin = preg_replace('/'.$t.'/i', '<span class="ui-state-highlight">$0</span>', $old_label_latin);
      else
        $label_latin = preg_replace('/'.$t.'/i', '<span class="ui-state-highlight">$0</span>', $label_latin);
    }
    $old = '';
    if($old_label || $old_label_latin)
      $old = '<small>'.$old_label.'<em>'.$old_label_latin.'</em></small>';
    array_push($results, array(
      'item_type' => 'organism',
      'item_id' => $organism['id'],
      'name' => $organism['label'],
      'label' => $label.'<em>'.$label_latin.'</em>'.$old
    ));
  }
    
  return drupal_json_output($results);
}
function habitats_autocomplete() {
	
  $term    = trim($_REQUEST['term']);
  $term    = strtolower($term);
  $termsql = '%'.trim($_REQUEST['term']).'%';
  $terms   = explode(' ', trim($term));
  $terms   = array_filter($terms);
  if($term != ''){
	 
  	 $result  = db_query('SELECT * FROM habitat WHERE name_de ILIKE :term',array(':term' => $termsql));
  }
  else{
  	
  	 $result  = db_query('SELECT * FROM habitat');
  }
  
  $results = array();
  foreach($result as $record){
  	
  	 $org_label = $record->name_de;
  	 $label = preg_replace('/'.$term.'/i', '<span class="ui-state-highlight">$0</span>', $org_label);
  	 $results[] = array('id' => $record->id,'name' => $record->name_de,'label' => $label);
  }

  return drupal_json_output($results);
}


?>