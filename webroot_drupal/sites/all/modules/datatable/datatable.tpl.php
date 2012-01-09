<?php
/**
 * Template file for datatable theme
 * Using jQuery plugin DataTables
 */
// add css-file
drupal_add_library('system', 'ui.datepicker');

drupal_add_css(drupal_get_path('module', 'datatable') . '/css/datatable.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
drupal_add_js(drupal_get_path('module', 'datatable') . '/js/lib/jquery.dataTables.js');
drupal_add_js(drupal_get_path('module', 'datatable') . '/js/lib/jquery.scrollTo-min.js');

$table[$id_table] = array(
		'#theme' => 'table',
		'#header' => $header,
		'#rows' => $rows,
		'#sticky' => false,
		'#attributes' => array(
				'id' => $id_table
		),
);

if ($len == NULL)
	$len = 300;

print drupal_render($table);

$additional_options = '';
if (is_array($options)) {
	foreach ($options as $name => $value) {
		$additional_options .= ', "' . $name . '": ' . $value;
	}
}
?>
<script type="text/javascript" charset="utf-8">
<!--
  jQuery(document).ready(function() {
    jQuery(<?php echo "'#" . $id_table . "'"; ?>).dataTable({
      "bJQueryUI": true,
      "bPaginate": false,
      "bAutoWidth": false, 
      "bScrollInfinite": true,
      "bScrollCollapse": true,
      "sScrollY": "<?php echo $len . "px"; ?>",
      "sDom": '<"ui-toolbar ui-widget-header ui-corner-tl ui-corner-tr ui-helper-clearfix"if>\
          rt\
          <"ui-toolbar ui-widget-header ui-corner-bl ui-corner-br ui-helper-clearfix">'
      <?php echo $additional_options ?>
    });
  });
-->
</script>
