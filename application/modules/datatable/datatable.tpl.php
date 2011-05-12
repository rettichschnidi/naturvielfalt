<?php 
/**
 * Template file for datatable theme
 * Using jQuery plugin DataTables
 */
// add css-file
drupal_add_css(drupal_get_path('module', 'datatable') . '/css/datatable.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
drupal_add_css(drupal_get_path('module', 'datatable') . '/css/jquery-ui-1.8.4.custom.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));
drupal_add_js(drupal_get_path('module', 'datatable') . '/js/lib/jquery.dataTables.js');

$table[$id_table] = array(
    '#theme' => 'table', 
	'#header' => $header,
    '#rows' => $rows,
	'#attributes' => array('id' => $id_table),
);

if($len == NULL) $len = 300;

print drupal_render($table);
?>
<script type="text/javascript" charset="utf-8">
	jQuery(document).ready(function() {
		oTable = jQuery(<?php echo "'#".$id_table."'"; ?>).dataTable({
			"bJQueryUI": true,
			"bPaginate": false,
			"bAutoWidth": false, 
			"bScrollInfinite": true,
			"bScrollCollapse": true,
			"sScrollY": "<?php echo $len."px"; ?>",
			"sDom": '<"ui-toolbar ui-widget-header ui-corner-tl ui-corner-tr ui-helper-clearfix"if>\
					t\
					<"ui-toolbar ui-widget-header ui-corner-bl ui-corner-br ui-helper-clearfix">',
		});
	});
</script>
