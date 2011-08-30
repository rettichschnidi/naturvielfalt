<div class="sort_criteria" ></div>
<div class="open_identifications">
<?php 

$string = "";
if($list){

		foreach($list as $record){
		
			$medias = $record['medias'];
			$media  = $medias[0];
			$url="";
			if($media['filemime'] == 'audio/mpeg'){
				$url = url(drupal_get_path('module', 'gallery').'/images/music_icon.png');
			}
			elseif($media['filemime'] == 'video/mp4'){
				$url = url(drupal_get_path('module', 'gallery').'/images/video_icon.png');
			}
			else{
				$url_href  = file_uri_target($media['url']);
				$url 	 = url('sites/default/files/'.$url_href);
			}

			$link    = 'open_identification/'.$record['info']['open_identification_id'].'';
			$string .= '<a href="'.url($link).'">';
			$string .= 	'<div class="open_identification" style="float:left;padding-bottom: 40px;padding-right:40px;">';
			$string .= 		'<div class="open_identification_image" style="float:left;">';
			$string .= 			'<img src="'.$url.'" widht="180" height="100" />';
			$string .= 		'</div>';
			$string .= 		'<div class="open_identification_info" style="float:left;">';
			$string .= 			'<div class="info" style="padding-right:40px;">';	
			$string .= 				'<span class="title">'.t('User').': </span>';	
			$string .= 				'<span class="value">';					
			$string .= 					$record['info']['fullname'];
			$string .= 				'</span>';
			$string .= 			'</div>';
			$string .= 			'<div class="info" style="padding-right:40px;">';
			$string .= 				'<span class="title">'.t('Modified on').': </span>';	
			$string .= 				'<span class="value">';					
			$string .= 					date("d.m.y H:i:s",strtotime($record['info']['modified_date']));
			$string .= 				'</span>';	
			$string .= 			'</div>';
			$string .= 			'<div class="info" style="padding-right:40px;">';
			$string .= 				'<span class="title">'.t('Type').': </span>';	
			$string .= 				'<span class="value">';					
			$string .= 					t($record['info']['type']);
			$string .= 				'</span>';	
			$string .= 			'</div>';
		
			if(isset($record['info']['organismgroup'])){
			
				$string .= 			'<div class="info">';
				$string .= 				'<span class="title">'.t('Organismgroup').': </span>';	
				$string .= 				'<span class="value">';					
				$string .= 					t($record['info']['organismgroup']);
				$string .= 				'</span>';	
				$string .= 			'</div>';
			}
			else
			{	$string .= 			'<div class="info">';
				$string .= 				'<div class="title">&nbsp;</div>';	
				$string .= 				'<div class="value">&nbsp;</div>';	
				$string .= 			'</div>';
			}
			
		
			$test = "";
			$string .= 		'</div>';
			$string .= 	'</div>';
			$string .= '</a>';
		}
}
else{

	$string = "No open identifications available";
}
?>

<? echo $string; ?>
</div>