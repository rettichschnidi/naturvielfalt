<div class="open_identifications">
<?php 

$string = "";
foreach($list as $record){

	$medias = $record['medias'];
	$media  = $medias[0];
	
	if($media['filemime'] == 'audio/mpeg')
		$url = '';
	elseif($media['filemime'] == 'video/mp4')
		$url = '';
	else
		$url = url('gallery/open_identification/'.$image['item_id'].'/thumb/'.$image['id'].'/gallery_mini');
		
	$link   = 'open_identification/'.key($list).'/edit';
	$string .= '<a href="'.$link.'">';
	$string .= 	'<div class="open_identification" style=" padding-bottom: 20px;">';
	$string .= 		'<div class="open_identification_image">';
	$string .= 			'<img src="'.$url.'">';
	$string .= 		'</div>';
	$string .= 		'<div class="open_identification_info" >';
	$string .= 			'<div class="info" style="float:left; padding-right:40px;">';	
	$string .= 				'<div class="title">UserId</div>';	
	$string .= 				'<div class="value">';					
	$string .= 					$record['info']['user_id'];
	$string .= 				'</div>';
	$string .= 			'</div>';
	$string .= 			'<div class="info" style="float:left; padding-right:40px;">';
	$string .= 				'<div class="title">create date</div>';	
	$string .= 				'<div class="value">';					
	$string .= 					date("d.m.y H:i:s",strtotime($record['info']['create_date']));
	$string .= 				'</div>';	
	$string .= 			'</div>';
	$string .= 			'<div class="info" style="float:left; padding-right:40px;">';
	$string .= 				'<div class="title">Typ</div>';	
	$string .= 				'<div class="value">';					
	$string .= 					t($record['info']['type']);
	$string .= 				'</div>';	
	$string .= 			'</div>';

	if(isset($record['info']['organismgroup'])){
	
		$string .= 			'<div class="info">';
		$string .= 				'<div class="title">Organismengruppe</div>';	
		$string .= 				'<div class="value">';					
		$string .= 					t($record['info']['organismgroup']);
		$string .= 				'</div>';	
		$string .= 			'</div>';
	}

	$test = "";
	$string .= 		'</div>';
	$string .= 	'</div>';
	$string .= '</a>';
}

?>

<? echo $string; ?>
</div>