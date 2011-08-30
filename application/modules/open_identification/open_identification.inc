<?php

/**
 * Menu callback; View of open identification
 * 
 * @param int $id
 *  The ID of the inventory
 * @return array
 *  Drupal render array
 */
function open_identification_show($form, &$form_state,$mode) {
	
	drupal_add_css(drupal_get_path('module', 'open_identification') . '/css/open_identifications.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
	drupal_add_js(drupal_get_path('module', 'open_identification') . '/js/open_identification.js', array('weight' => 100));	
	global $user;
	
	$url = drupal_get_destination();
    $url = drupal_parse_url($url['destination']);
    $url = $url['path'];
    //dpm($_POST);
   $criteria = false;
   if(!empty($_POST)){
   	
   		$filter = array();
   		$type['habitat'] = $_POST['habitat'];
   		$type['organism'] = $_POST['organism'];
   		if($type['organism'] != null){
   			
	   		$result = db_select('inventory_type', 'it')->fields('it', array('id', 'name'))->orderBy('it.id', 'DESC')->execute();
	  	
	  		foreach($result as $record){
	  			
	  			if($_POST[$record->name])
	  				$filter[] = $_POST[$record->name];
	  		}
	  		$criteria['organismgroup'] = $filter;
   		}
  		$sortby = $_POST['sort'];
  		$criteria['type']   = $type;
  		$criteria['sortby'] = $sortby;
  		//dpm($criteria);
   }	
   $render_array['filter'] = array(
	    '#type' => 'fieldset', 
	    '#title' => t('sort/filter'), 
	    '#weight' => 1, 
	    '#collapsible' => TRUE,
	    '#collapsed' => false,
	    '#attached' => array (
	      'js' => array (
	        'misc/form.js',
	        'misc/collapse.js',
	      ),
	    ),
	  );
	$result = db_select('inventory_type', 'it')
    ->fields('it', array('id', 'name'))
    ->orderBy('it.id', 'DESC')
    ->execute();
  	$organismgroupoptions = '<input type="checkbox" checked="yes" name="typ" value="unknown">'.t('unknown').'<br>';
  	
  	foreach($result as $record){
    	
  		$options[$record->name] = $record->name;
    	$organismgroupoptions .='<input type="checkbox" checked="yes" name="'.$record->name.'" value="'.$record->name.'">'.t($record->name).'<br>';
  	}
    $render_array['filter']['sort'] = array(
      '#markup' => 
        '<form class="presentation-settings" method="post" action="'.url($url).'">'.
    	  '<div class="filtertype" style="float:left; padding-right:100px;"><input type="checkbox" checked="yes" name="habitat" value="habitat">'.t('Habitat').'<br>'.
    		'<input type="checkbox" checked="yes" name="organism" value="organism">'.t('Organism').'</div><div class="grouptype" style="display:block;float:left;padding-right:100px;">'.
    		$organismgroupoptions.
    		'</div><div class="sort" style="float:left;padding-right:20px;"><input type="radio" checked="no" name="sort" value="bydate">'.t('sort by date').'<br>'.
    		'<input type="radio" checked="no" name="sort" value="byuser">'.t('sort by user').'<br>'.
    		'<input type="radio" checked="no" name="sort" value="byorganism">'.t('sort by organismgroup').'</div>'.
          '<div><input type="submit" name="submit" style="float:right" value="'.t('sort/filter').'" /></div>'.  
        '</form>',
      '#weight' => -1,
   	'#prefix' => '<div>',
    '#suffix' => '</div>',
    );
    
	if($mode == 'mylist')
		$userid = $user->uid;
	else
		$userid = false;
		

	$render_array['open_identification_list'] = array(
	    '#theme' => 'open_identification_list',  // The theme function to apply to the #items
	    '#list' => open_identification_get_all_entries($userid,$criteria), // The list itself.
		'#weight' => 2,
	);
	return $render_array;
}
function open_identification_show_detail($open_identification_id) {

	
	$render_array['open_identification_medias'] = gallery_list_renderer('medias','open_identification',$open_identification_id, false, -1,false);
	$open_identification= open_identification_get_entry($open_identification_id);
	/*$render_array['open_identification_details'] = array(
	    '#theme' => 'open_identification_details',  // The theme function to apply to the #items
	    '#open_identification' => $open_identification, // The list itself.
	);*/
	drupal_set_title(t('Open Identification of ').$open_identification['fullname']);
  $rows = array();
  $rows[] = array(
    array('data' => t('User'), 'header' => true), 
    $open_identification['fullname']
  );
  $rows[] = array(
    array('data' => t('Created on'), 'header' => true), 
    date("j.m.Y H:i:s", strtotime($open_identification['create_date']))
  );
  $rows[] = array(
    array('data' => t('Modified on'), 'header' => true), 
    date("j.m.Y H:i:s", strtotime($open_identification['modified_date']))
  );
  $rows[] = array(
    array('data' => t('Type'), 'header' => true), 
    t($open_identification['type'])
  );
  if($open_identification['organismgroup'])
	  $rows[] = array(
	    array('data' => t('Organismgroup'), 'header' => true), 
	    t($open_identification['organismgroup'])
	  );
  $rows[] = array(
    array('data' => t('Comment'), 'header' => true), 
    t($open_identification['comment'])
  );
  
  $render_array['details'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Open Identification details'), 
    '#weight' => 2, 
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#attached' => array (
      'js' => array (
        'misc/form.js',
        'misc/collapse.js',
      ),
    ),
    '#attributes' => array(
      'class' => array('collapsible'),
      'id' => 'open_identification-details'
    ),
  );
  $render_array['details']['table'] = array(
    '#theme' => 'table',
    '#rows' => $rows,
    '#attributes' => array(
        'class' => array('open_identification_show_type'),
      ),
  );
 $render_array['details']['link'] = array(
      '#markup' => '<div style="float:right;background-color:#F7F6EF;margin:0 0 0 0;padding:0.2em 1em;">'.l(t('Edit details'),'open_identification/'.$open_identification_id.'/edit').'</div>',
      '#weight' => 9,
    );
	return $render_array;
}
function ajax_example_simplest_callback($form, $form_state) {
  
  return $render_array['testajax'];
}
?>