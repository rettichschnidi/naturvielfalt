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
$mapid_div = $mapid . '_div';
?>

<div id="<?php echo $mapid_div ?>" class="area-outer-map-container">
	<div id="<?php echo $mapid_canvas ?>" style="min-height: <?php echo $min_height; ?>; height: <?php echo $height; ?>; width: <?php echo $width; ?>;" class="area-canvas">
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
if ($action == 'create' || $action == 'getcoordinate' || $action == 'edit') {
	$libraries .= ',drawing';
}

$area_protocol = isset($_SERVER['HTTPS']) && !empty($_SERVER['HTTPS'])
		? 'https://' : 'http://';
$googlelanguage = isset($user->language) && !empty($user->language)
		? $user->language : $language->language;
commonstuff_add_js_url(
	$area_protocol
			. "maps.google.com/maps/api/js?sensor=false&libraries=$libraries&region=CH&language=$googlelanguage\n");

commonstuff_add_js_url($baseModulJsPath . 'contrib/v3_epoly_sphericalArea.js');
commonstuff_add_js_url($baseModulJsPath . 'area-googlemapsapi-extensions.js');
commonstuff_add_js_url($baseModulJsPath . 'area.js');

commonstuff_add_css_url($baseModulCssPath . 'area-theme.css');

/**
 * Include contributed functions needed by the CH1903 box.
 */
if ($ch1903) {
	commonstuff_add_js_url($baseModulJsPath . 'contrib/wgs84_ch1903.js');
}

/**
 * Show a reticle in the middle of the map.
 */
if ($reticle) {
	$reticle_image_url = base_path() . drupal_get_path('module', 'commonstuff')
			. '/images/reticle.png';
}
?>

<!-- ----------------------------------------------- -->

<script>
	jQuery(document).ready(function() {
		jQuery.ajaxSetup({
				error: function(xhr, status, error) {
					console.error("An AJAX error occured:");
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
				getcoordinate: <?php echo $action == 'getcoordinate' ? 1 : 0 ?>,
				geometryeditid: <?php echo $geometry_edit_id ? $geometry_edit_id
		: 0
								?>,
				reticle: <?php echo $reticle ? 1 : 0 ?>,
				reticleimageurl: '<?php echo $reticle_image_url ?>',
				coordinatestorageid: '<?php echo $coordinate_storage_id ?>',
				geometriesfetchurl: '<?php echo $geometries_fetch_url ?>',
				geometries_autoload: <?php echo (isset($geometries_autoload) && $geometries_autoload) ? 1 : 0 ?>,
				infowindowcontentfetchurl: '<?php echo $infowindow_content_fetch_url ?>',
				infowindowcreateformfetchurl: '<?php echo $infowindow_createform_fetch_url ?>',
				geometryupdateurl: '<?php echo $geometry_update_url ?>',
				infowindowoptions: {
					content: Drupal.t('Loading...'),
				},
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

			// The following code is borrowed from  /misc/textarea.js
			// If you ever need this for any other kind of custom element,
			// please refactor it into something generic.
			jQuery('.area-outer-map-container').each(function () {
				var staticOffset = null;
				var textarea = jQuery(this).addClass('resizable-textarea').find('.area-canvas');
				var grippie = jQuery('<div class="grippie"></div>').mousedown(startDrag);

				grippie.insertAfter(textarea);

				function startDrag(e) {
					staticOffset = textarea.height() - e.pageY;
					textarea.css('opacity', 0.25);
					jQuery(document).mousemove(performDrag).mouseup(endDrag);
					return false;
				}

				function performDrag(e) {
					textarea.height(Math.max(32, staticOffset + e.pageY) + 'px');
					google.maps.event.trigger(<?php echo $mapid ?>.googlemap, 'resize');
					return false;
				}

				function endDrag(e) {
					jQuery(document).unbind('mousemove', performDrag).unbind('mouseup', endDrag);
					textarea.css('opacity', 1);
				}
			});
		} else {
			// display errormessage to console log
			errormsg = "HTML element with id '" + options.canvasid + "' not found. No google maps will be displayed.";
			alert(errormsg);
		}
	});
</script>
