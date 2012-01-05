<?php
// include JavaScripts

area_add_gmap_resources();

?>
<p>
<?php echo t('Please search an area either by Google Maps or by it\'s field name.');?>
</p>

<div id='static_image' style="position: absolute; height: 200px; width: 200px; display: none; z-index: 100; border: solid 1px grey; background-color: white"><img style="display:block; position:absolute; left: 84px; top:84px" src="<?php echo base_path() . drupal_get_path('module', 'area') ?>/images/ajax-loader.gif"></div>