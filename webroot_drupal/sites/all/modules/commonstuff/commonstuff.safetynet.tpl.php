<?php
/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file commonstuff.safetynet.tpl.php
 */
drupal_add_js(
	drupal_get_path('module', 'commonstuff')
			. '/js/safetynet/lib/jquery.netchanger.min.js');
drupal_add_js(
	drupal_get_path('module', 'commonstuff')
			. '/js/safetynet/jquery.safetynet.js');

drupal_add_js(
	"jQuery(document).ready(function() {
		jQuery('$selector').safetynet({
			message : '$warningmessage'
		});
	});",
	array('type' => 'inline'));
?>
