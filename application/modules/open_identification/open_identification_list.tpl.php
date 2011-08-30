<div class="sort_criteria" ><input type="button" onclick="show_prompt()" value="sortieren/filtern" /></div>
<div class="open_identifications">
<?php 

$string = "";
if($list){
//dpm($list);
		foreach($list as $typerecordkey => $typerecord){
			
			$string .= '<div class="type"><div class="typetitle">'.t($typerecordkey).'</div><div class="typerecords">';
			foreach($typerecord as $grouprecordkey => $grouprecord){
				
				$string .= '<div class="group"><div class="grouptitle">'.t($grouprecordkey).'</div><div class="grouprecords">';
				foreach($grouprecord as $record){
		
					$medias = $record['medias'];
					$media  = $medias[0];
					$url    = "";
					if($media['filemime'] == 'audio/mpeg'){
						$url = url(drupal_get_path('module', 'gallery').'/images/music_icon.png');
					}
					elseif($media['filemime'] == 'video/mp4'){
						$url = url(drupal_get_path('module', 'gallery').'/images/video_icon.png');
					}
					else{
						$url_href  = file_uri_target($media['url']);
						$url 	   = url('sites/default/files/'.$url_href);
					}
		
					$link    = 'open_identification/'.$record['info']['open_identification_id'].'';
					
					$string .= 	'<div class="open_identification">';
					$string .= '<a href="'.url($link).'">';
					$string .= 		'<div class="open_identification_image">';
					$string .= 			'<img src="'.$url.'"/>';
					$string .= 		'</div>';
					$string .= 		'<div class="open_identification_info">';
					$string .= 			'<div class="info">';	
					$string .= 				'<div class="title">'.$record['info']['fullname'].' / '.date("d.m.y",strtotime($record['info']['modified_date'])).'</div>';	
					$string .= 				'<div class="value">';					
					$string .= 				'';
					$string .= 				'</div>';
					$string .= 			'</div>';
					/*$string .= 			'<div class="info">';
					$string .= 				'<span class="title">'.t('Modified on').': </span>';	
					$string .= 				'<span class="value">';					
					$string .= 					date("d.m.y",strtotime($record['info']['modified_date'])); // H:i:s
					$string .= 				'</span>';	
					$string .= 			'</div>';
					$string .= 			'<div class="info">';
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
					
				
					$test = "";*/
					$string .= 		'</div>';
					$string .= 		'</a>';
					$string .= 	'</div>';
					
				}
				$string .= '</div></div>';
			}
			$string .= '</div></div>';
		}
}
else{

	$string = "No open identifications available";
}
?>

<? echo $string; ?>
</div>