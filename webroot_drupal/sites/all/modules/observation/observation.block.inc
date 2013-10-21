<?php

define('OBSERVATION_BLOCK_PAGESIZE', 4);
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
	//<img id="loadingImage" src="/' . drupal_get_path('module', 'observation') . '/img/loading.gif" />
	switch($delta) {
		case 'new_observations' :
			$block['subject'] = t('New observations');
			$block['content'] = array(
			'#type' => 'markup',
			'#markup' => '<div class="new_observations_container">
								<div class="new_observations_wrapper">
									
								</div>
						  </div>',
			'#attached' => array(
					'css' => array(
							drupal_get_path('module', 'observation') . '/css/observation.css',
							drupal_get_path('module', 'gallery') . '/css/gallery.css',
							drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css'
							
					),
					'js' => array(
							drupal_get_path('module', 'observation') . '/js/observation_block.js',
							drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js'
					)
			)
	);
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
	$page = $_POST['page'];
	if(!isset($_POST['page']) || !ctype_digit($_POST['page']) || $_POST['page'] < 1)
		return '';
	
	$observations = observation_get_new_observations(($page-1) * OBSERVATION_BLOCK_PAGESIZE, OBSERVATION_BLOCK_PAGESIZE);
	$content = array();
	foreach($observations as $observation) {
		$observation = (array)$observation;
		// get images
		$images = gallery_images(GALLERY_MEDIATYPE_IMAGE, 'observation', $observation['id']);
		
		$img = '';
		if ($images['count'] > 0) {
			$tmp_img = $images['images'][0];
			$url_large = url('gallery/observation/' . $observation['id'] . '/thumb/' . $tmp_img['id'] . '/gallery_large');;
			$url_preview = url('gallery/observation/' .  $observation['id'] . '/thumb/' . $tmp_img['id'] . '/medium');
	
			//prepare caption
			$captionclass = "block_newest_observation_img_caption";
			$caption = '';
			if ($tmp_img['title'])
				$caption .= '<strong>' . $tmp_img['title'] . '</strong>';
			if ($tmp_img['description'])
				$caption .= ' <small>' . $tmp_img['description'] . '</small>';
			if ($caption)
				$caption = '<span class="title">' . $caption . '</span>';
			if ($tmp_img['location'] || $tmp_img['author'])
				$caption .= '<span class="author">&copy; ' . $tmp_img['author']
				. ($tmp_img['location'] ? ', ' . $tmp_img['location'] : '')
				. '</span>';
			
			$imgfooter = '<div class="'
					. $captionclass . '">'
							. ($caption ? '<p class="captionText">' . $caption . '</p>'
									: '').'</div>';
			$img =  '<a class="lightbox" href="'.$url_large.'"><img id="observation_img" class="observation_img'.$observation['id'].'" src="'. $url_preview . '" title="' . $tmp_img['title'] . '" alt="' . $tmp_img['title'] . '" />'.$imgfooter.'</a>';
		} 
		if($observation['organism_id'] == UNKNOWN_SPECIES) {
			$observation['ol_name'] = t('Unknown species');
		}
		if(empty($observation['ol_name'])) {
			$observation['ol_name'] = t("No translation available");
		}
		$scientific_name = isset($observation['scientific_name']) ? $observation['scientific_name'].'</br>' : '';
	
		$content[$observation['id']] = array(
				'#type' => 'markup',
				'#markup' => 
						'<div class="observation_entry">'.
							$img .'</br>'.
							l($observation['ol_name'], 'observation/'.$observation['id'].'/show').'</br>'.
							'<i>'.$scientific_name.'</i>'.
							$observation['observer'].'</br>'.
							$observation['township'].', '.$observation['canton'].'</br>'.
						'</div>'
		);
	}
	$output['total'] = observation_get_new_observations_count();
	$output['content'] = drupal_render($content);
	return drupal_json_output($output);
}
