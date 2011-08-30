<?php

/**
 * Menu callback; View of open identification
 * 
 * @param int $id
 *  The ID of the inventory
 * @return array
 *  Drupal render array
 */
function open_identification_show($mode) {
drupal_add_css(drupal_get_path('module', 'open_identification') . '/css/open_identifications.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
	global $user;
	
	drupal_set_title(t('Open Identification '));
	if($mode == 'mylist')
		$userid = $user->uid;
	else
		$userid = false;
		
	$list = open_identification_get_all_entries($userid);
	$render_array['open_identification_list'] = array(
	    '#theme' => 'open_identification_list',  // The theme function to apply to the #items
	    '#list' => $list, // The list itself.
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
?>