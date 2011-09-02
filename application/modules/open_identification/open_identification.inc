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
	
	global $user;
	
	$url = drupal_get_destination();
    $url = drupal_parse_url($url['destination']);
    $url = $url['path'];

   $criteria = false;
   if(!empty($_POST)){
   	
   		$filter = array();
   		$type['habitat'] = $_POST['habitat'];
   		$type['organism'] = $_POST['organism'];
   		if($type['organism'] != null){
   			
	   		$result = db_select('inventory_type', 'it')->fields('it', array('id', 'name'))->orderBy('it.id', 'DESC')->execute();
	  		
	   		$filter[] = " ";
	   		if($_POST['unknown'])
	  			$filter[] = $_POST['unknown'];
	  		
	  		foreach($result as $record){
	  			
	  			if($_POST[$record->name])
	  				$filter[] = $_POST[$record->name];
	  		}
	  		$criteria['organismgroup'] = $filter;
   		}
  		$sortby = $_POST['sort'];
  		$criteria['type']   = $type;
  		$criteria['sortby'] = $sortby;
   }	
   $render_array['filter'] = array(
	    '#type' => 'fieldset', 
	    '#title' => t('sort/filter'), 
	    '#weight' => 1, 
	    '#collapsible' => TRUE,
	    '#collapsed' => true,
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
      	  	
   	if(!empty($_POST)){
    	if($_POST['habitat'] == "habitat")
    		$checked_habitat  ='checked="yes"';
    	else 
    		$checked_habitat = "";

    	if($_POST['organism'] == "organism")
    		$checked_organism  ='checked="yes"';
    	else 
    		$checked_organism = "";
    		
    	if($_POST['unknown'] == "unknown")
    		$checked ='checked="yes"';
    	else 
    		$checked = "";
   	}	
    else{
    	$checked_habitat   = 'checked="yes"';
    	$checked_organism  = 'checked="yes"';
    	$checked           = 'checked="yes"';
    }

    $organismgroupoptions = '<input type="checkbox" '.$checked.' name="unknown" value="unknown">'.t('unknown').'<br>';	
  	foreach($result as $record){
    	
  		$options[$record->name] = $record->name;
  		
  		if(!empty($_POST)){
  			
    		if(($_POST[$record->name] == $record->name) && $checked_organism == 'checked="yes"')
    			$checked = 'checked="yes"';
    		else 
  				$checked = "";
  		}
    	else
    		$checked = 'checked="yes"';
    		
    	$organismgroupoptions .='<input type="checkbox" '.$checked.' name="'.$record->name.'" value="'.$record->name.'">'.t($record->name).'<br>';
  	}
    	
    $render_array['filter']['sort'] = array(
      '#markup' => 
        '<form id="sort_filter"  method="post" action="'.url($url).'">'.
    	  '<div class="filtertype" style="float:left; padding-right:100px;"><input type="checkbox" '.$checked_habitat.' name="habitat" value="habitat">'.t('Habitat').'<br>'.
    		'<input type="checkbox" '.$checked_organism.' name="organism" value="organism" >'.t('Organism').'</div><div class="grouptype" style="display:block;float:left;padding-right:100px;">'.
    		$organismgroupoptions.
    		'</div><div class="sort" style="float:left;padding-right:20px;">'.
    		'<input type="radio" checked="no" name="sort" value="bydateold">'.t('sort by oldest date').'<br>'.
    		'<input type="radio" checked="no" name="sort" value="bydatenew">'.t('sort by newest date').'<br>'.
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
function open_identification_show_detail($form, &$form_state,$open_identification_id) {
	global $user;
	
	drupal_add_library('system', 'ui.autocomplete');
	drupal_add_js(drupal_get_path('module', 'open_identification') . '/js/open_identification.js');
	
	$render_array['open_identification_medias'] = gallery_list_renderer('medias','open_identification',$open_identification_id, false, -1,false);
	$open_identification= open_identification_get_entry($open_identification_id);

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
  
  if($open_identification['user_id'] != $user->uid){

  	 $render_array['suggest'] = array(
	    '#type' => 'fieldset', 
	    '#title' => t('Suggest'), 
	    '#weight' => 3, 
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

	  if(trim($open_identification['type']) != 'habitat'){
	  	
	  		if(trim($open_identification['organismgroup']) == 'unknown'){
					
	  			  $result = db_select('inventory_type', 'it')
				    ->fields('it', array('id', 'name'))
				    ->orderBy('it.id', 'DESC')
				    ->execute();
				  
				  	$options = array('' => t('- Please choose -'));
				  	foreach($result as $record)
				    	$options[$record->id] = $record->name;
				    	
				    	
				  $render_array['suggest']['organismgroup'] = array(
				    '#name' => 'organismgroup',
				    '#type' => 'select',
				    '#title' => t('Organismgroup'),
				    '#options' => $options,
				  	'#weight' =>3,
				  	'#required' => false,
				  );
	  		}
	  		else{
	  			  $render_array['suggest']['organismgroup'] = array(
			      '#markup' => '<b>'.t('Organismgroup').'</b>: '.trim($open_identification['organismgroup']),
			      '#weight' => 3,
	    		   );
	  		}
	  }
	  if(trim($open_identification['organismgroup']) != 'unknown'){

	  	   $render_array['suggest']['item_search'] = array(
	          '#type' => 'textfield',
	          '#title' => t('Organism'),
	          '#required' => true,
		  	  '#weight' => 4,
	          '#attributes' => array(
	            'class' => array('search'),
	            'rel' => url('gallery/organism_type_gallery_autocomplete/inventory_type/10'),
		  		//'style' => array('display:none;')
	          )
	        );
	  }
	  else{
	  	   $render_array['suggest']['item_search'] = array(
	          '#type' => 'textfield',
	          '#title' => t('Organism'),
	          '#required' => true,
		  	  '#weight' => 4,
	          '#attributes' => array(
	            'class' => array('search')
	          )
	        );
	  }
      $render_array['suggest']['submit'] = array(
	      '#markup' => '<div style="float:right;margin:0 0 0 0;padding:0.2em 1em;"><input type="submit" name="tip" style="float:right" value="suggest" /></div>',
	      '#weight' => 9,
	    );
  }
  if($open_identification['user_id'] == $user->uid){
	  $render_array['suggestions'] = array(
	    '#type' => 'fieldset', 
	    '#title' => t('Suggestions'), 
	    '#weight' => 3, 
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
	  $col = array();
	  $cols[] = array(array('data' => ' ', 'header' => true),array('data' => t('Organismgroup'), 'header' => true),
	  				  array('data' => t('Organismus/Lebensraum'), 'header' => true),
	  				  array('data' => t('Number of Users'), 'header' => true));
	  $cols[] = array('<input type="radio" name="test"/>','Reptilien','Giftnatter','19 of 33 (57.6%)');
	  $cols[] = array('<input type="radio" name="test"/>','Reptilien','Kapkorallenschlange','13 of 33 (39.4%)');
	  $cols[] = array('<input type="radio" name="test"/>','Reptilien','Rotnatter','1 of 33 (3%)');
	   $render_array['suggestions']['table'] = array(
		    '#theme' => 'table',
		    '#rows' => $cols,
	    );
	   $render_array['suggestions']['submit'] = array(
	      '#markup' => '<div style="float:right;margin:0 0 0 0;padding:0.2em 1em;"><input type="submit" name="submittip" style="float:right" value="Vorschlag annehmen" /></div>',
	      '#weight' => 9,
	    );
  }
	return $render_array;
}

?>