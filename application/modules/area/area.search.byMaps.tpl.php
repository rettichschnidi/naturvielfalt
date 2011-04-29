<div id="map_search-element">
	<input type="text" id="map_search" name="map_search" value="" />
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