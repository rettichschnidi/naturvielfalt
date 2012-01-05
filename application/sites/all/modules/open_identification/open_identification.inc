<?php

/**
 * Menu callback; View of open identification
 * 
 * @param int $id
 *  The ID of the inventory
 * @return array
 *  Drupal render array
 */
function open_identification_show($form,&$form_state,$mode) {
	global $user;
	drupal_add_css(drupal_get_path('module', 'open_identification') . '/css/open_identifications.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));

	$url = drupal_get_destination();
    $url = drupal_parse_url($url['destination']);
    $url = $url['path'];

   $criteria = false;
   /*if(!empty($_POST)){
   	
   		$filter = array();
   		$type['habitat'] = $_POST['habitat'];
   		$type['organism'] = $_POST['organism'];
   		if($type['organism'] != null){
   			
	   		$result = db_select('inventory_type', 'it')->fields('it', array('id', 'name'))->orderBy('it.id', 'DESC')->execute();
	  		
	   		//$filter[] = 0;
	   		if($_POST['0'])
	  			$filter[] = $_POST['0'];
	  		
	  		foreach($result as $record){
	  			
	  			if($_POST[$record->id])
	  				$filter[] = $_POST[$record->id];
	  		}
	  		$criteria['organismgroupid'] = $filter;
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
   		debug($_POST,'POSTVARIABLE',true);
    	if($_POST['habitat'] == "habitat")
    		$checked_habitat  ='checked="yes"';
    	else 
    		$checked_habitat = "";

    	if($_POST['organism'] == "organism")
    		$checked_organism  ='checked="yes"';
    	else 
    		$checked_organism = "";
    		
    	if(isset($_POST[0]))
    		$checked ='checked="yes"';
    	else 
    		$checked = "";
   	}	
    else{
    	$checked_habitat   = 'checked="yes"';
    	$checked_organism  = 'checked="yes"';
    	$checked           = 'checked="yes"';
    }

    $organismgroupoptions = '<input type="checkbox" '.$checked.' name="0" value="0">'.t('unknown').'<br>';	
  	foreach($result as $record){
    	
  		$options[$record->id] = $record->name;
  		
  		if(!empty($_POST)){
  			
    		if(($_POST[$record->id] == $record->id) && $checked_organism == 'checked="yes"')
    			$checked = 'checked="yes"';
    		else 
  				$checked = "";
  		}
    	else
    		$checked = 'checked="yes"';
    		
    	$organismgroupoptions .='<input type="checkbox" '.$checked.' name="'.$record->id.'" value="'.$record->id.'">'.t($record->name).'<br>';
  	}
    	
    $render_array['filter']['sort'] = array(
      '#markup' => 
        '<form id="sort_filter"  method="post" action="'.url($url).'">'.
    	  '<div class="filtertype" style="float:left; padding-right:100px;"><input type="checkbox" '.$checked_habitat.' name="habitat" value="habitat">'.t('Habitat').'<br>'.
    		'<input type="checkbox" '.$checked_organism.' name="organism" value="organism" >'.t('Organism').'</div><div class="grouptype" style="display:block;float:left;padding-right:100px;">'.
    		$organismgroupoptions.
    		'</div><div class="sort" style="float:left;padding-right:20px;">'.
    		'<input type="radio" name="sort" value="bydateold">'.t('sort by oldest date').'<br>'.
    		'<input type="radio" name="sort" value="bydatenew">'.t('sort by newest date').'<br>'.
    		'<input type="radio" name="sort" value="byuser">'.t('sort by user').'<br>'.
    		'<input type="radio" name="sort" value="byorganism">'.t('sort by organismgroup').'</div>'.
          '<div><input type="submit" name="submit" style="float:right" value="'.t('sort/filter').'" /></div>'.  
        '</form>',
      '#weight' => -1,
   	'#prefix' => '<div>',
    '#suffix' => '</div>',
    );*/
    $solved = false;
	if($mode == 'mylist')
		$userid = $user->uid;
	elseif($mode == 'solvedlist')
		$solved = true;
	else
		$userid = false;
		
		
	$render_array['open_identification_list'] = array(
	    '#theme' => 'open_identification_list',  // The theme function to apply to the #items
	    '#list' => open_identification_get_all_entries($userid,$criteria,$solved), // The list itself.
		'#weight' => 2,
	);
	return $render_array;
}
function open_identification_show_detail($open_identification_id) {

	global $user;
	

 	drupal_add_css(drupal_get_path('module', 'open_identification') . '/css/open_identifications.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
	
    $open_identification= open_identification_get_entry($open_identification_id);
	
  if(!($open_identification['open_identification_id'] > 0)){

	  	$render_array['details']['notfound'] = array('#markup' => 'Open Identification doesn\'t exist');
	  	return $render_array;
  }
  elseif($open_identification['solved'] == 1){
  		
  		$render_array['details']['solved'] = array('#markup' => 'Open Identification has been solved');
	  	return $render_array;
  }
  else{
  	  
  	  drupal_set_title(t('Open Identification of ').$open_identification['fullname']);  
 
	  $render_array['medias'] = gallery_list_renderer('medias','open_identification',$open_identification_id, false,-1,false); 
	  $render_array['details'] = drupal_get_form('open_identification_details',$open_identification);
	  
	  return $render_array;
  }
}
function open_identification_details($form, &$form_state,$open_identification){
	  
	  drupal_add_library('system', 'ui.autocomplete');
	  drupal_add_js(drupal_get_path('module', 'open_identification') . '/js/open_identification.js', array('weight' => 100));
	  
	  global $user;
	  
	  $open_identification_id  = $open_identification['open_identification_id'];
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
	  $count = open_identification_check_user_tip($open_identification_id);
	  if(($open_identification['user_id'] != $user->uid) && !($count > 0)){
	
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
		  $render_array['suggest']['open_identification_id'] = array('#type' => 'hidden', '#value' => $open_identification_id);
		  $render_array['suggest']['type'] = array('#type' => 'hidden', '#value' => $open_identification['type']);
		  if(trim($open_identification['type']) != 'habitat'){
		  	
		  		if(trim($open_identification['organismgroup']) == 'unknown'){
						
		  			  $result = db_select('inventory_type', 'it')
					    ->fields('it', array('id', 'name'))
					    ->orderBy('it.id', 'DESC')
					    ->execute();
					  
					  	$groupoptions = array('' => t('- Please choose -'));
					  	foreach($result as $record)
					    	$groupoptions[$record->id] = $record->name;
					    	
					    	
					  $render_array['suggest']['organismgroupid'] = array(
					    '#name' => 'organismgroupid',
					    '#type' => 'select',
					    '#title' => t('Organismgroup'),
					    '#options' => $groupoptions,
					  	'#weight' =>3,
					  	'#required' => true,
					  );
		  		}
		  		else{
		  			  $render_array['suggest']['organismgroupid'] = array('#type' => 'hidden', '#value' => $open_identification['organismgroupid'] );
		  			  $render_array['suggest']['organismgroup'] = array(
				      '#markup' => '<b>'.t('Organismgroup').'</b>: '.trim($open_identification['organismgroup']),
				      '#weight' => 3,
		    		   );
		  		}
		  }
		  if($open_identification['organismgroupid']){
	
		  	   $render_array['suggest']['item_search'] = array(
		          '#type' => 'textfield',
		          '#title' => t('Organism'),
		          '#required' => true,
			  	  '#weight' => 4,
		          '#attributes' => array(
		            'class' => array('search'),
		  	   		'rel' => url('open_identification/autocomplete/organisms/'.$open_identification['organismgroupid'])
		          )
		        );
		  }
		  else{
		  	   if($open_identification['type'] == 'organism'){
		  	   	
			  	   $render_array['suggest']['item_search'] = array(
			          '#type' => 'textfield',
			          '#title' => t('Organism'),
			          '#required' => true,
				  	  '#weight' => 4,
			          '#attributes' => array(
			            'class' => array('search'),
			  	   		'style' => array('display:none;'),
			  	   
			          )
			        );
		  	   }
		  	   else{
		  	   		
		  	   	    $render_array['suggest']['item_search'] = array(
			          '#type' => 'textfield',
			          '#title' => t('Habitat'),
			          '#required' => true,
				  	  '#weight' => 4,
			          '#attributes' => array(
			            'class' => array('search'),
		  	   	    	'rel' => url('open_identification/autocomplete/habitats'),
		  	   	    	'style' => 'diplay:inline;'
			          )
			        );
		  	   }
		  }
	      $render_array['suggest']['submit'] = array(
		      '#type' => 'submit',
	      	  '#value' => t('suggest'),
	      	  '#submit' => array('open_identification_suggest_submit'),
		      '#weight' => 9,
		    );
	  }
	  
	  
	  if(($open_identification['user_id'] == $user->uid) || $count > 0){
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
	
		  $tips = open_identification_get_tips($open_identification_id,$open_identification['organismgroupid'],$open_identification['type']);
		  debug($tips,'TIPS:',true);
		  if(!empty($tips)){
			  
			  foreach($tips as $tip){
			  	
			  	 if($open_identification['type'] == 'organism'){
			  	 	$options[$tip['item_id']] = array('organismgroup' => $tip['organismgroup'], 
			  	 									  'organism' => $tip['item_name'],
			  	 									  'users' => ''.$tip['usernum'].' '.t('of').' '.$tip['overallnum'].' ('.$tip['percentage'].'%)');
			  	 	$header = array('organismgroup' => t('Organismgroup'),'organism' => t('Organism'),'users' => t('Number of Users'));
			  	 }
			  	 else{
			  	 	$options[$tip['item_id']] = array('habitat' => $tip['item_name'],
			  	 									  'users' => ''.$tip['usernum'].' '.t('of').' '.$tip['overallnum'].' ('.$tip['percentage'].'%)');
			  	 	$header = array('habitat' => t('Habitat'),'users' => t('Number of Users'));
			  	 }
			  }
			  if($open_identification['user_id'] == $user->uid){
				  $render_array['suggestions']['open_identification_id'] = array('#type' => 'hidden', '#value' => $open_identification_id);
				  $render_array['suggestions']['type'] = array('#type' => 'hidden', '#value' => $open_identification['type']);	  
				  $render_array['suggestions']['organismgroupid'] = array('#type' => 'hidden', '#value' => $open_identification['organismgroupid']);	
				  $render_array['suggestions']['suggestion'] = array(
				    '#type' => 'tableselect',
				    '#title' => t('Suggestions'),
				    '#options' => $options,
				  	'#multiple' => false,
				  	'#header' => $header
			  	  );
			  	  
				  $render_array['suggestions']['submit'] = array(
					      '#type' => 'submit',
				      	  '#value' => t('accept suggestion'),
				      	  '#submit' => array('open_identification_suggestions_submit'),
					      '#weight' => 9,
			      );
			  }
			  else{
			  	   $render_array['suggestions']['suggestion'] = array(
				    '#theme' => 'table',
				    '#title' => t('Suggestions'),
				    '#rows' => $options,
				  	'#header' => $header
			  	  );
			  	
			  }
		  }
		  else
		  	$render_array['suggestions']['nodata'] = array('#markup' => 'No suggestions made yet');
	
	  }
		return $render_array;
	
}
function open_identification_suggest_submit($form, &$form_state){
	
	$open_identification_tip['open_identification_id']  = $form_state['values']['open_identification_id'];
	$open_identification_tip['open_identification_type'] = $form_state['values']['type'];
	$open_identification_tip['organismgroupid']			= $form_state['values']['organismgroupid'];
	$item_id											= open_identification_get_item_id($open_identification_tip['open_identification_type'],
																						  $open_identification_tip['organismgroupid'],
																						  $form_state['values']['item_search']);
	if($item_id > 0){
		$open_identification_tip['item_id'] = $item_id;
		open_identification_insert_tip($open_identification_tip);
		$form_state['redirect'] = 'open_identification/'.$open_identification_tip['open_identification_id'];
	}
	else
		form_set_error('suggest_submit','Invalid data');
							
	
}
function open_identification_suggestions_submit($form, &$form_state){
	
	debug($form_state,'Suggestions_submit',true);
	$open_identification_tip['open_identification_id']  = $form_state['values']['open_identification_id'];
	$open_identification_tip['item_id']					= $form_state['values']['suggestion'];
	$open_identification_tip['open_identification_type']= $form_state['values']['type'];
	$open_identification_tip['open_identification_organismgroupid']= $form_state['values']['organismgroupid'];

	open_identification_save_suggestion($open_identification_tip['open_identification_id'],$open_identification_tip['item_id']	,$open_identification_tip['open_identification_type'],$open_identification_tip['open_identification_organismgroupid']);
	
	$form_state['redirect'] = $open_identification_tip['open_identification_type'].'/'.$open_identification_tip['item_id'];
}
?>