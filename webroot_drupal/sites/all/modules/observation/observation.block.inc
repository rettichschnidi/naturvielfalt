<?php
/**
 * hook_block_info:
 * declares blocks provided by the observation module
 *
 * @return array $blocks
 */
function observation_block_info() {
	//block for a list of new observations
	$blocks['new_observations'] = array(
			'info' => t('New Observations'),
			'cache' => DRUPAL_NO_CACHE,
			'status' => true,
			'region' => 'sidebar_second',
			'visibility' => BLOCK_VISIBILITY_LISTED,
			'pages' => '<front>'
				
	);

	return $blocks;
}

/**
 * hook_block_view:
 * renders the blocks of the observation module
 *
 * @return array $blocks
 */
function observation_block_view($delta = '') {
	switch($delta) {
		case 'new_observations' :
			$block['subject'] = t('New observations');
			$block['content'] = array(
			'#type' => 'markup',
			'#prefix' => '<div class="new_observations_container">',
			'#suffix' => '</div>',
			'#attached' => array(
					'css' => array(
							drupal_get_path('module', 'observation') . '/css/observation.css',
					),
					'js' => array(
							drupal_get_path('module', 'observation') . '/js/observation_block.js',
					)
			)
	);;
			break;
		default :
			$block = array();
	}
	return $block;
}

/**
 * 
 */
function observation_block_new_observations_content_ajax() {
	$offset = $_REQUEST['offset'];
	$limit = $_REQUEST['limit'];
	if((!isset($_REQUEST['offset']) && !ctype_digit($_REQUEST['offset']))
			|| (!isset($_REQUEST['limit']) && !ctype_digit($_REQUEST['limit']))) {
		return '';
	} 
	
	$observations = observation_get_new_observations($offset, $limit);
	$content = array();
	foreach($observations as $observation) {
		$observation = (array)$observation;
		// get images
		$images = gallery_images(GALLERY_MEDIATYPE_IMAGE, 'observation', $observation['id']);
		

		$img = '';
		if ($images['count'] > 0) {
	
			$url_large = url('gallery/observation/' . $observation['id'] . '/thumb/' . $images['images'][0]['id'] . '/gallery_large');;
			$url_preview = url('gallery/observation/' .  $observation['id'] . '/thumb/' . $images['images'][0]['id'] . '/gallery_presentation');
			
			$img =  '<a href="'.$url_large.'"><img class="observation_img'.$observation['id'].'" src="'. $url_preview . '" title="' . $images['images'][0]['title'] . '" alt="' . $images['images'][0]['title'] . '" /></a>';
			
		} 
		$content[$observation['id']] = array(
				'#type' => 'markup',
				'#markup' => 
						$img .'</br>'.
						$observation['ol_name'].'</br>'.
						$observation['scientific_name'].'</br>'.
						$observation['township'].', '.$observation['canton'].'</br>'
		);
	}
	return drupal_json_output($content);
}
