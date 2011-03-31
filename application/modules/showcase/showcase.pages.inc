<?php
/**
 * Menu callback. This will be called if someone visits drupal/showcase.
 */
function showcase_page() {
	drupal_set_title(t('Swissmon showcase test page'));
	return "";
}

function showcase_page_no_template() {
	drupal_set_title(t('Swissmon showcase'));

	$render_array['page_example_arguments'] = array(
    '#theme' => 'showcase_page',  // The theme function to apply to the #items
    '#text' => 'my page text',  // The list itself.
	);
	return $render_array;
}
?>