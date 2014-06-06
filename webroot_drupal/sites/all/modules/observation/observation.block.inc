<?php

define('OBSERVATION_BLOCK_PAGESIZE', 12);
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
			'#markup' => '
							<div class="new_observations_container">
								<img id="loadPreviousImages" class="imageSliderNavigation top" src="/' . drupal_get_path('module', 'observation') . '/img/block/up.png" alt="Previous" onclick="moveImages(-3, false);" />
								<div id="previousEntry01">
									<img id="previousImage01" class="imageSliderImage" style="width: 0px;" />
								</div>
								<div id="previousEntry02">
									<img id="previousImage02" class="imageSliderImage" style="width: 0px;" />
								</div>
								<div id="previousEntry03">
									<img id="previousImage03" class="imageSliderImage" style="width: 0px;" />
								</div>
								<div id="currentEntry01">
									<img id="currentImage01" src="/' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="imageSliderImage" alt="Image 01" />
								</div>
								<div id="currentEntry02">
									<img id="currentImage02" src="/' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="imageSliderImage" alt="Image 02" />
								</div>
								<div id="currentEntry03">
									<img id="currentImage03" src="/' . drupal_get_path('module', 'vote') . '/img/loading.gif" class="imageSliderImage" alt="Image 03" />
								</div>
								<div id="futureEntry01">
									<img id="futureImage01" class="imageSliderImage" style="width: 0px;" />
								</div>
								<div id="futureEntry02">
									<img id="futureImage02" class="imageSliderImage" style="width: 0px;" />
								</div>
								<div id="futureEntry03">
									<img id="futureImage03" class="imageSliderImage" style="width: 0px;" />
								</div>
						<img id="loadNextImages" class="imageSliderNavigation bottom" src="/' . drupal_get_path('module', 'observation') . '/img/block/down.png" alt="Next" onclick="moveImages(3, false);" />
						</div>
						<img id="showLoadingImage" src="/' . drupal_get_path('module', 'observation') . '/img/block/loading.gif" />',
			'#attached' => array(
					'css' => array(
							drupal_get_path('module', 'observation') . '/css/observation_block.css',
							drupal_get_path('module', 'gallery') . '/css/gallery.css',
							drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css'
							
					),
					'js' => array(
							drupal_get_path('module', 'observation') . '/js/observation_block.js',
							drupal_get_path('module', 'observation') . '/js/jquery.waitforimages.min.js',
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
	if(!isset($_POST['page']) || !ctype_digit($_POST['page']) || $_POST['page'] < 1)
		return '';
	
	$page = $_POST['page'];
	
	$observations = observation_get_new_observations(($page-1) * OBSERVATION_BLOCK_PAGESIZE, OBSERVATION_BLOCK_PAGESIZE);
	$content = array();
	foreach($observations as $observation) {
		$observation = (array)$observation;
		// get images
		$images = gallery_images(GALLERY_MEDIATYPE_IMAGE, 'observation', $observation['id']);
		
		$img = '';
		if ($images['count'] > 0) {
			$tmp_img = $images['images'][0];
			$url_large = image_style_url('gallery_large', $tmp_img['uri']);
			$url_preview = image_style_url('medium', $tmp_img['uri']);
	
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
	
		global $base_url;
		
		$entry = array(
				'#type' => 'markup',
				'#markup' => 
						$img .'</br>'.
						l($observation['ol_name'], $base_url . '/observation/'.$observation['id'].'/show').'</br>'.
						l(
							'<i>'.$scientific_name.'</i>',
							$base_url . '/organism/' . $observation['organism_id'],
							array(
							'html' => true
						)).
						l(
							$observation['observer'],
							$base_url . '/user/' . $observation['users_uid'] . '/contact',
							array(
							'html' => true
						)).'</br>'.
						$observation['township'].', '.$observation['canton'].'</br>'
		);
		$content[] = drupal_render($entry);
	}
	$output['page'] = $page;
	$output['total'] = observation_get_new_observations_count();
	$output['content'] = $content;
	return drupal_json_output($output);
}
