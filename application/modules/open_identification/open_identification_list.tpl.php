<div class="sort_criteria" ><!--  <input type="button" onclick="show_prompt()" value="sortieren/filtern" />--> </div>
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
					$string .= 		'</div>';
					$string .= 		'</a>';
					$string .= 	'</div>';
					
				}
				$string .= '</div></div><br>';
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