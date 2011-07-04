<div class="area-search-map"><?php 
  /* Laut Albert nicht mehr benoetigt 
  $output['SearchArea']= drupal_get_form('area_search');
  print drupal_render($output['SearchArea']);
  */
  ?>
  <div id="map_search-element">
    <div><?php echo t('Search with Google Maps')?></div>
    <input type="text" id="map_search" name="map_search" value="" size="60" maxlength="128"/>
    <button id="map_search_button">
    <?php echo t('Search map'); ?>
    </button>
  </div>
  
  <ul class="area-choices">
    <li>
      <div class="controlAreaChoose selected"></div>
      <?php 
        echo t('Choose area:');
      ?>
    </li>
    <li>
      <div class="controlAreaCreate"></div>
      <?php
        echo t('Create new area:');
      ?>
    </li>
    <li>
  </ul>
  
  <div id="map">
    <div id="map_canvas" style="height: 100%"></div>
  </div>

</div>