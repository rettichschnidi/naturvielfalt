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
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js');
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js');
    drupal_add_js(drupal_get_path('module', 'gallery') . '/js/gallery.upload.js');
    drupal_add_css(drupal_get_path('module', 'open_identification') . '/css/open_identifications.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
	
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
	  $render_array['suggest']['open_identification_id'] = array('#type' => 'hidden', '#value' => $open_identification_id);
	  $render_array['suggest']['type'] = array('#type' => 'hidden', '#value' => $open_identification['type']);
	  if(trim($open_identification['type']) != 'habitat'){
	  	
	  		if(trim($open_identification['organismgroup']) == 'unknown'){
					
	  			  $result = db_select('inventory_type', 'it')
				    ->fields('it', array('id', 'name'))
				    ->orderBy('it.id', 'DESC')
				    ->execute();
				  
				  	$options = array('' => t('- Please choose -'));
				  	foreach($result as $record)
				    	$options[$record->id] = $record->name;
				    	
				    	
				  $render_array['suggest']['organismgroupid'] = array(
				    '#name' => 'organismgroupid',
				    '#type' => 'select',
				    '#title' => t('Organismgroup'),
				    '#options' => $options,
				  	'#weight' =>3,
				  	'#required' => false,
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
	            //'rel' => url('gallery/organism_type_gallery_autocomplete/inventory_type/'.$open_identification['organismgroupid']),
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
		  	   		'style' => array('display:none;')
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
	  	   	    	'rel' => url('open_identification/autocomplete/habitats')
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

	  $tips = open_identification_get_tips($open_identification_id,$open_identification['organismgroupid'],$open_identification['type']);
	  
	  if(!empty($tips)){
		  $cols = array();
		  if($open_identification['type'] == 'organism')
			  $cols[] = array(array('data' => ' ', 'header' => true),array('data' => t('Organismgroup'), 'header' => true),
			  				  array('data' => t($open_identification['type']), 'header' => true),
			  				  array('data' => t('Number of Users'), 'header' => true),
			  				  array('data' => t('Show Users'), 'header' => true));
		  else
		  	  $cols[] = array(array('data' => ' ', 'header' => true),
			  				  array('data' => t($open_identification['type']), 'header' => true),
			  				  array('data' => t('Number of Users'), 'header' => true),
			  				  array('data' => t('Show Users'), 'header' => true));
		  
		  foreach($tips as $tip){
		  	
		  	 if($open_identification['type'] == 'organism'){
		  	 	$cols[] = array('<input type="radio" name="'.$tip['id'].'"/>',''.$tip['organismgroup'].'',''.$tip['item_name'].'',''.$tip['usernum'].' '.t('of').' '.$tip['overallnum'].' ('.$tip['percentage'].'%)','show');
		  	 	$options[$tip['item_id']] = ''.$tip['organismgroup'].': '.''.$tip['item_name'].''.$tip['usernum'].' '.t('of').' '.$tip['overallnum'].' ('.$tip['percentage'].'%)';
		  	 }
		  	 else{
		  	 	$cols[] = array('<input type="radio" name="'.$tip['id'].'"/>',''.$tip['item_name'].'',''.$tip['usernum'].' '.t('of').' '.$tip['overallnum'].' ('.$tip['percentage'].'%)','show');
		  	 	$options[$tip['item_id']] = ''.$tip['item_name'].''.$tip['usernum'].' '.t('of').' '.$tip['overallnum'].' ('.$tip['percentage'].'%)';
		  	 }
		  }
		  
		  $render_array['suggestions']['table'] = array(
			    '#theme' => 'table',
			    '#rows' => $cols,
		  );	  
		  $render_array['suggestions']['submit'] = array(
			      '#type' => 'submit',
		      	  '#value' => t('accept suggestion'),
		      	  '#submit' => array('open_identification_suggestions_submit'),
			      '#weight' => 9,
	      );
	  }
	  else{
	  	
	  	$render_array['suggestions']['nodata'] = array('#markup' => 'no tips available');
	  }

	  $render_array['suggestions']['table1'] = array(
		    '#type' => 'radios',
		    '#title' => t('Suggestions'),
		    '#default_value' => 1,
		    '#options' => $options
	  );
     /* debug($tips,'TIPS:',true);*/

  }
	return $render_array;
}
function open_identification_suggest_submit($form, &$form_state){
	
	//debug($form_state,'form_state',true);
	$open_identification_tip['open_identification_id']  = $form_state['values']['open_identification_id'];
	$open_identification_tip['open_identificatin_type'] = $form_state['values']['type'];
	$open_identification_tip['organismgroupid']			= $form_state['values']['organismgroupid'];
	$item_id											= open_identification_get_item_id($open_identification_tip['open_identificatin_type'],
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
	
	$open_identification_tip['open_identification_id']  = $form_state['values']['open_identification_id'];
	debug($form_state,'FORM_STATE:',true);
	$form_state['redirect'] = 'open_identification/'.$open_identification_tip['open_identification_id'];
}
?>