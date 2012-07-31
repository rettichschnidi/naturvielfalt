<?php
/**
 * Template file for datatable theme
 * Using jQuery plugins DataTables
 *
 * @author Naturwerk, 2011
 * @author Ramon Gamma, 2012
 */

global $user;
global $language;
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
drupal_add_js(drupal_get_path('module', 'datatable') . '/js/lib/jquery.cookie.js');

/**
 * Figure out width/height of table or set default values
 */
if (!isset($tableWidth) || $tableWidth < 0) {
	$tableWidth = 941;
}

/**
 * calculate the width of the cols
 */
if (!isset($tableHeight) || tableHeight < 0) {
	$tableHeight = 200;
}

$allHeaders = "";
$allWidths = 0;
foreach ($header as $head) {
	$visible = isset($head['hide']) ? !$head['hide'] : true;
	if ($visible) {
		if (isset($head['width'])) {
			$allWidths = $allWidths + $head['width'];
		} else {
			$allHeaders .= $head['name'];
		}
	}
}
$countHeadChars = strlen($allHeaders);
if ($countHeadChars > 0) {
	$charTableSize = ($tableWidth - $allWidths - 62) / $countHeadChars;
} else {
	$charTableSize = 0;
}
$colsized = (7 > $charTableSize) ? 8 : $charTableSize;

/**
 * Render the headers and columns
 */
if ($header) {
	$aoColumns = "colModel : [";
	$searchColumns = "searchitems : [{display: '" . t('ALL') . "', name: '*', isdefault: true},";
	$headers = array();
	$sortField = isset($header[0]['dbfield']) ? $header[0]['dbfield'] : '';
	$sortOrder = "asc";

	foreach ($header as $head) {
		$aoColumns .= "{ display: '" . $head['name'] . "'";
		if (isset($head['dbfield']))
			$aoColumns .= ", name : '" . $head['dbfield'] . "'";

		(isset($head['noSort']) && $head['noSort'] == true)
				? $aoColumns .= ",sortable : false"
				: $aoColumns .= ",sortable : true";
		(isset($head['width']) && $head['width'])
				? $aoColumns .= ",  width : " . $head['width']
				: $aoColumns .= ",  width : "
						. (strlen($head['name']) * $colsized);
		(isset($head['align']) && $head['align'])
				? $aoColumns .= ",  align : '" . $head['align'] . "'"
				: $aoColumns .= ",  align : 'left'";
		if (isset($head['hide']) && $head['hide'] == true)
			$aoColumns .= ", hide : true";
		else
			$aoColumns .= ", hide : false";
		if (isset($options['rowClick']))
			$aoColumns .= ", process : " . $options['rowClick'];
		$aoColumns .= "},";
		$headers[] = $head['name'];
		if (isset($head['initSort']) && $head['initSort'] == true)
			$sortField = $head['dbfield'];
		if (isset($head['sortOrder']) && $head['sortOrder'])
			$sortOrder = $head['sortOrder'];
		
		if(isset($head['dbExactField']))
		  $searchColumns .= "{display: '" . t($head['name']) . "', name: '" . $head['dbExactField'] . "', isdefault: false},";
	}
	
	// remove trailing comma
	$aoColumns = substr_replace($aoColumns, "", -1);
	$aoColumns .= "],";
	$searchColumns = substr_replace($searchColumns, "", -1);
	$searchColumns .= "],";
}
$table[$id_table] = array(
		'#theme' => 'table',
		'#rows' => $rows,
		'#sticky' => false,
		'#attributes' => array(
				'id' => $id_table,
				'class' => $id_table
		),
);
print drupal_render($table);
?>

<script type="text/javascript" charset="utf-8">
<!--

jQuery(document).ready(function() {
	jQuery("#<?php echo $id_table; ?>").flexigrid ({
		url: '<?php echo $options['jsonUrl']; ?>',
		dataType: 'json',
		<?php $aoColumns ? print "$aoColumns\n" : ''; ?>
		<?php $searchColumns ? print "$searchColumns\n" : ''; ?>
		singleSelect: true,
		sortname: "<?php echo $sortField; ?>",
		sortorder: "<?php echo $sortOrder; ?>",
		usepager: true,
		title: '<?php echo $title ?>',
		useRp: true,
		rp: 15,
		showTableToggleBtn: true,
		width: <?php echo $tableWidth; ?>,
		height: <?php echo $tableHeight; ?>,
		tableId: '<?php echo $id_table; ?>',
		errormsg: '<?php echo t('Connection Error'); ?>',
		pagestat: '<?php echo t('Displaying {from} to {to} of {total} items'); ?>',
		pagetext: '<?php echo t('Page'); ?>',
		outof: '<?php echo t('of'); ?>',
		findtext: '<?php echo t('Find'); ?>',
		procmsg: '<?php echo t('Processing, please wait ...'); ?>',
		nomsg: '<?php  echo t('No items'); ?>',
		search : '<?php echo t('Search'); ?>',
		reset : '<?php echo t('Reset'); ?>',
		minmax : '<?php echo t('Minimize/Maximize Table'); ?>',
		hideshow : '<?php echo t('Hide/Show Columns'); ?>',
		dblClickResize: true,
		onToggleCol: true,
		singleSelect: true,
	});
	<?php if (isset($options['rowClick'])) echo $options['rowClickHandler']; ?>
});

-->
</script>
