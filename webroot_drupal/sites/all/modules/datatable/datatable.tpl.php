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
drupal_add_css(drupal_get_path('module', 'datatable') . '/css/flexigrid.css',
			array(	'group' => CSS_DEFAULT,
					'every_page' => TRUE));



/**
 * add javascript files
 */
drupal_add_js(drupal_get_path('module', 'datatable') . '/js/flexigrid.js');
drupal_add_js(drupal_get_path('module', 'datatable') . '/js/lib/jquery.cookie.js');

$options['tableWidth'] > 0 ? $tableWidth =  $options['tableWidth']: $tableWidth = 800;
$options['tableHeight'] > 0 ? $tableHeight =  $options['tableHeight']: $tableHeight = 200;

$allHeaders = "";
$allWidths = 0;
foreach ($header as $head){
	if(!$head['hide']){
		if($head['width']){
			$allWidths = $allWidths + $head['width'];
		}else {
			$allHeaders .= $head['name'];
		}
	}
}
$countHeadChars = strlen($allHeaders);
if($countHeadChars > 0) $charTableSize = ($tableWidth- $allWidths - 62) / $countHeadChars;
$colsized = (7 > $charTableSize) ? 8 : $charTableSize;

/**
 * Render the headers and columns
 */
if($header){

	$aoColumns = "colModel : [";
	$headers = array();
	$sortField = $header[0]['dbfield'];
	$sortOrder = "asc";

	foreach($header as $head){
		$aoColumns .= "{ display: '".$head['name']."', name : '".$head['dbfield']."'";
// 		if($head['hide'] == true) $aoColumns .= ", 'bVisible' : false";
// 		if($head['noSort'] == true) $aoColumns .= ", 'bSortable' : false";
		($head['noSort'] == true) ? $aoColumns .= ",sortable : false" : $aoColumns .=  ",sortable : true";
		($head['width']) ? $aoColumns .= ",  width : ".$head['width'] : $aoColumns .= ",  width : ".(strlen($head['name'])*$colsized);
		($head['align']) ? $aoColumns .= ",  align : '".$head['width']."'" : $aoColumns .= ",  align : 'left'";
		if($head['hide'] == true) $aoColumns .= ", hide : true";
		if($options['rowClick'])  $aoColumns .= ", process : ".$options['rowClick'];
		$aoColumns .= "},
";
		$headers[] = $head['name'];
		if($head['initSort'] == true) $sortField = $head['dbfield'];
		if($head['sortOrder']) $sortOrder = $head['sortOrder'];
	}

	// remove trailing comma
	$aoColumns = substr_replace( $aoColumns, "", -1 );
	$aoColumns .= "],";

// 	// we need data, otherwise drupal will render the table without <thead> tag
// 	if(!isset($rows)){
// 		$rows[] = array($head['dbfield'][0] => t('Please wait...'));
// 	}

}


/**
 * setup and render the table
 */
/**$table[$id_table] = array(
		'#theme' => 'table',
		'#header' => $headers,
		'#rows' => $rows,
		'#sticky' => false,
		'#attributes' => array(
				'id' => $id_table,
				'width' => "100%"
		),
	);
print drupal_render($table);
*/
/**
 * set the height of the table
 */
// if ($len == NULL) {
// 	$len = 250;
// }

/**
 * progress the additional options for the table
 */


/**$additional_options = ','.$aoColumns.'';
if (is_array($options)) {
	foreach ($options as $name => $value) {
		$additional_options .= ', "' . $name . '": ' . $value.'';
	}
}
*/
?>



<table id="<?php echo $id_table; ?>" class="<?php echo $id_table; ?>"></table>


<script type="text/javascript" charset="utf-8">
<!--

//alert('1');

jQuery(document).ready(function() {
//$=jQuery;



jQuery("#<?php echo $id_table; ?>").flexigrid
(
{
url: <?php echo $options['jsonUrl'];?>,
dataType: 'json',
<?php echo $aoColumns; ?>
searchitems : [{display: Drupal.t('ALL'), name : '*', isdefault: true}],
sortname: "<?php echo $sortField; ?>",
sortorder: "<?php echo $sortOrder; ?>",
usepager: true,
title: 'Countries',
useRp: true,
rp: 15,
showTableToggleBtn: true,
width: <?php echo $tableWidth; ?>,
height: <?php echo $tableHeight; ?>,
tableId: '<?php echo $id_table; ?>'
});

<?php if($options['rowClick']) echo $options['rowClickHandler']; ?>

});

-->
</script>
