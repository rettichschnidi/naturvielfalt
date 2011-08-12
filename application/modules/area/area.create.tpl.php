<div class="area-search-map"><?php 
  ?>
  <div id="map_search-element">
    <div><?php echo t('Search with Google Maps')?></div>
    <input type="text" id="map_search" name="map_search" value="" size="60" maxlength="128"/>
    <button id="map_search_button">
    <?php echo t('Search map'); ?>
    </button>
  </div>
  
  <div id="map">
    <div id="map_canvas" style="height: 100%"></div>
  </div>

</div>