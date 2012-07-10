<?php
/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area.tpl.php
 * @note I created may different javascript files for the following reason:
 *  - better structured, easier to manage
 * @note I did not use the drupal drupal_add_css/drupal_add_js because it's easier
 * to use this map in an overlay. Feel free to use the drupal_* functions and it's
 * goodies if you know how to make it working in an overlay.
 */

$mapid_canvas = $mapid . '_canvas';

$style = '';
if ($height || $width) {
	$style = ' style="';
	if ($height)
		$style .= 'height: ' . $height . '; ';
	if ($width)
		$style .= 'width: ' . $width . '; ';
	$style .= '"';
}
?>

<div id="<?php echo $mapid ?>_div">
	<div id="<?php echo $mapid_canvas ?>" <?php echo $style; ?>>
    	<h1><?php print(t('You have to enable Javascript!')) ?></h1>
	</div>
</div>
<?php
global $user;
global $language;

// Set basepaths for usage later on...
$baseModulPath = base_path() . drupal_get_path('module', 'area') . '/';
$baseModulJsPath = $baseModulPath . 'js/';
$baseModulCssPath = $baseModulPath . 'css/';

// Include jQuery.UI for dynamic resizing of the map
drupal_add_library('system', 'ui.resizable');

/**
 * Options set for the google maps api:
 *  - libraries:
 *  	- geometry (computation of geometric data on the surface of the earth)
 *  	- places (search functionality)
 *  	- drawing (tools to create overlays)
 *  - sensor: We do not support GPS devices on the users end - so this is always false
 *  - region: Set this manually to Switzerland (CH). All requests will be biased by swiss «rules».
 *  - language: Localize the google maps service to the desired language (most likely the users language)
 */

$libraries = 'geometry';
if ($search) {
	$libraries .= ',places';
}
if ($action == 'create' || $action == 'getcoordinate') {
	$libraries .= ',drawing';
}

$area_protocol = isset($_SERVER['HTTPS']) && !empty($_SERVER['HTTPS'])
		? 'https://' : 'http://';
$googlelanguage = isset($user->language) && !empty($user->language) ? $user->language : $language->language;
area_add_js_url(
	$area_protocol
			. "maps.google.com/maps/api/js?sensor=false&libraries=$libraries&region=CH&language=$googlelanguage\n");

area_add_js_url($baseModulJsPath . 'contrib/v3_epoly_sphericalArea.js');
area_add_js_url($baseModulJsPath . 'area-googlemapsapi-extensions.js');
area_add_js_url($baseModulJsPath . 'area.js');

area_add_css_url($baseModulCssPath . 'area-theme.css');

/**
 * Include contributed functions needed by the CH1903 box.
 */
if ($ch1903) {
	area_add_js_url($baseModulJsPath . 'contrib/wgs84_ch1903.js');
}

/**
 * Show a reticle in the middle of the map.
 */
if ($reticle) {
	$reticle_image_url = base_path() . drupal_get_path('module', 'commonstuff')
			. '/images/reticle.png';
}

// /**
//  * If existing area(s) should be shown...
//  */
// switch ($show) {
// /**
//  * Display just the ones the user owns.
//  */
// case 'custom-show':
// 	area_add_js_url($baseModulJsPath . 'area-show-geometry.js');
// 	break;
// }

/**
 * Decide which actions should be exectured...
 */
switch ($action) {
/**
 * Create an existing area geometry
 */
case 'custom-edit':
	area_add_js_url($baseModulJsPath . 'area-edit-geometry.js');
	break;
/**
 * Set a marker and update the hidden fields (provided by the user of this theme)
 */
case 'getcoordinate':
	area_add_js_url($baseModulJsPath . 'area-getcoordinate.js');
	break;
}
?>
<!-- ----------------------------------------------- -->
<?php

/**
 * Allow the user of this theme to set a hidden field to store the coordinates
 * (encoded as JSON string)
 */
if ($coordinate_storage_id != false) {
	print 
		"<script>coordinate_storage_id = '$coordinate_storage_id';</script>\n";
}

?>
<script>
	jQuery(document).ready(function() {
		jQuery.ajaxSetup({
			  error: function(xhr, status, error) {
			    alert("An AJAX error occured: " + status + "\nError: " + error);
			    console.error(status);
			    console.error(error);
			  }
			});
		
		var options = {
				canvasid: '<?php echo $mapid_canvas ?>',
				search: <?php echo $search ? 1 : 0 ?>,
				ch1903: <?php echo $ch1903 ? 1 : 0 ?>,
				mapswitch: <?php echo $mapswitch ? 1 : 0 ?>,
				mapswitchlevel: <?php echo $mapswitchlevel ? 1 : 0 ?>,
				loadlastlocation: <?php echo $loadlastlocation ? 1 : 0 ?>,
				savelastlocation: <?php echo $savelastlocation ? 1 : 0 ?>,
				width: '<?php echo $width ?>',
				height: '<?php echo $height ?>',
				drawingmanager: <?php echo $action == 'create' ? 1 : 0 ?>,
				geometryedit: <?php echo $action == 'edit' ? 1 : 0 ?>,
				geometryeditid: <?php echo $geometry_edit_id ? $geometry_edit_id
		: 0
								?>,
				reticle: <?php echo $reticle ? 1 : 0 ?>,
				reticleimageurl: '<?php echo $reticle_image_url ?>',
				coordinatestorageid: <?php echo $coordinate_storage_id
		? $coordinate_storage_id : 0
									 ?>,
				geometriesfetchurl: '<?php echo $geometries_fetch_url ?>',
				infowindowcontentfetchurl: '<?php echo $infowindow_content_fetch_url ?>',
				infowindowcreateformfetchurl: '<?php echo $infowindow_createform_fetch_url ?>',
				googlemapsoptions: {
						zoom: <?php echo $defaultzoom ?>,
						streetViewControl: <?php echo $streetview ? 1 : 0 ?>,
						overviewMapControl: <?php echo $mapcontrol ? 1 : 0 ?>,
						scaleControl: <?php echo $scalecontrol ? 1 : 0 ?>,
						scrollwheel: <?php echo $scrollwheel ? 1 : 0 ?>,
						center: new google.maps.LatLng(46.77373, 8.25073),
						mapTypeId : google.maps.MapTypeId.ROADMAP,
				}
			};

		if( typeof <?php echo $mapid ?> != 'undefined') {
			console.error(<?php echo $mapid ?>);
			alert("'<?php echo $mapid ?>' is already declared! Please use another map id.");
		}
		
		if (jQuery('#' + options.canvasid).length) {
			<?php echo $mapid ?> = new Area(options);

			jQuery( "#<?php echo $mapid_canvas ?>" ).resizable({
				stop: function(event, ui) {
					// Update the size of the google map
					google.maps.event.trigger(<?php echo $mapid ?>.googlemap, "resize");
					console.log(<?php echo $mapid ?>.googlemap);
				},
				maxWidth: jQuery( "#<?php echo $mapid_canvas ?>" ).width(),
				minHeight: 300,
				minWidth: 600,
				// Better alternative, but does not show the cordner:
				// handles: 'n, s',
			});
		} else {
			// display errormessage to console log
			errormsg = "HTML element with id '" + options.canvasid + "' not found. No google maps will be displayed.";
			alert(errormsg);
		}
	});
</script>
