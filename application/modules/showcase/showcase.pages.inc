<?php
/**
 * Menu callback. This will be called if someone visits drupal/q=swissmon.
 */
function showcase_page() {
	drupal_set_title(t('Swissmon showcase test page'));
	// TODO: What should we return here?
	return ""; 
}
?>