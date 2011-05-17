<div> <?php 
	/* Laut Albert nicht mehr benoetigt 
	$output['SearchArea']= drupal_get_form('area_search');
	print drupal_render($output['SearchArea']);
	*/
	?>
</div>
<div id="map_search-element">
	<div><?php echo t('Search with Google Maps')?></div>
	<input type="text" id="map_search" name="map_search" value="" size="60" maxlength="128"/>
	<button id="map_search_button">
	<?php echo t('Search map'); ?>
	</button>
</div>

<div>
<div>
<div style="float: left;"><?php echo t('Choose area:')?></div>
<div style="float: left;" id="controlAreaChoose" class="selected"></div>
<div style="float: left;"><?php echo t('Create new area:')?></div>
<div style="float: left;" id="controlAreaCreate"></div>
</div>

<div style="clear: both;" id="map">
	<div id="map_canvas" style="height: 100%"></div>
</div>

</div>