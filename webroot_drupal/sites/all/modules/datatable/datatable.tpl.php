<?php
/**
 * Template file for datatable theme
 * Using jQuery plugins DataTables
 *
 * @author Naturwerk, 2011
 * @author Ramon Gamma, 2012
 */

drupal_add_library('system', 'ui.datepicker');

/**
 * add css-file
 */
drupal_add_css(
	drupal_get_path('module', 'datatable') . '/css/flexigrid.css',
	array(
			'group' => CSS_DEFAULT,
			'every_page' => TRUE
	));

/**
 * add javascript files
 */
drupal_add_js(drupal_get_path('module', 'datatable') . '/js/flexigrid.js');
drupal_add_js(
	drupal_get_path('module', 'datatable') . '/js/lib/jquery.cookie.js');

/**
 * Figure out width/height of table or set default values
 */
if (!isset($tableWidth) || !is_int($tableWidth)) {
	$tableWidth = 950;
}

if (!isset($tableHeight) || !is_int($tableHeight)) {
	$tableHeight = 200;
}

/**
 * Render the headers and columns
 */
if ($header) {
	$aoColumns = "colModel : [";
	$headers = array();
	$sortField = isset($header[0]['dbfield']) ? $header[0]['dbfield'] : '';
	$sortOrder = "asc";

	foreach ($header as $head) {
		$aoColumns .= "{ display: '" . $head['name'] . "'";
		if(isset($head['dbfield'])) $aoColumns .= ", name : '" . $head['dbfield'] . "'";
		if (isset($head['noSort']) && $head['noSort'] == true) {
			$aoColumns .= ", sortable : false";
		} else {
			$aoColumns .= ", sortable : true";
		}
		assert($head['width']);
		$aoColumns .= ", width : '" . $head['width'] . "'";

		(isset($head['align']) && $head['align']) ? $aoColumns .= ",  align : '"
						. $head['align'] . "'"
				: $aoColumns .= ",  align : 'left'";

		if (isset($head['hide']) && $head['hide'] == true)
			$aoColumns .= ", hide : true";

		if (isset($options['rowClick']))
			$aoColumns .= ", process : " . $options['rowClick'];
		$aoColumns .= "},";
		$headers[] = $head['name'];
		if (isset($head['initSort']) && $head['initSort'] == true)
			$sortField = $head['dbfield'];
		if (isset($head['sortOrder']) && $head['sortOrder'])
			$sortOrder = $head['sortOrder'];
	}

	// remove trailing comma
	$aoColumns = substr_replace($aoColumns, "", -1);
	$aoColumns .= "],";

// 	if($rows) {
// 		foreach ($header as $head) {
// 			$table_headers[] = $head['name'];
// 		}
// 	}
}
$table[$id_table] = array(
		'#theme' => 'table',
		'#rows' => $rows,
		'#sticky' => false,
		'#attributes' => array('id' => $id_table),
);
print drupal_render($table);
?>

<script type="text/javascript" charset="utf-8">
<!--

jQuery(document).ready(function() {

jQuery("#<?php echo $id_table; ?>").flexigrid
(
{
<?php
if($options['jsonUrl']){
	echo "url: ".$options['jsonUrl'].", dataType: 'json',";
}
echo $aoColumns;
?>

searchitems : [{display: Drupal.t('ALL'), name : '*', isdefault: true}],
sortname: "<?php echo $sortField; ?>",
sortorder: "<?php echo $sortOrder; ?>",
usepager: true,
<?php if ($title)
	echo "title: '" . $title . "',"
?>
useRp: true,
rp: 15,
showTableToggleBtn: true,
width: <?php echo "'$tableWidth'"; ?>,
height: <?php echo "'$tableHeight'"; ?>,
tableId: '<?php echo $id_table; ?>'
});
<?php if (isset($options['rowClick']))
	echo $options['rowClickHandler'];
?>

});

-->
</script>
