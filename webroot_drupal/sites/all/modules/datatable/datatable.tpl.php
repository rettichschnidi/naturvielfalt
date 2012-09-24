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


if(isset($options['gallery_enabled']) && $options['gallery_enabled']){
	// add all libraries needed by the gallery (rating, lightbox...)
	drupal_add_js(drupal_get_path('module', 'datatable') . '/js/datatable_gallery_addon.js');
	drupal_add_css(drupal_get_path('module', 'datatable') . '/css/datatable_gallery_addon.css');
	
	drupal_add_library('system', 'ui.widget');
	drupal_add_css(
			drupal_get_path('module', 'gallery') . '/css/jquery.ui.stars.min.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
	
	drupal_add_css(
	drupal_get_path('module', 'gallery') . '/css/gallery.css',
	array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
	));
	drupal_add_css(
	drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css',
	array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
	));
	drupal_add_js(
			drupal_get_path('module', 'gallery') . '/js/jquery.ui.stars.min.js',
			array(
					'weight' => 101
			));
	drupal_add_js(
			drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js',
			array(
					'weight' => 110
			));
	drupal_add_js(
			drupal_get_path('module', 'gallery') . '/js/gallery.rating.js',
			array(
					'weight' => 111
			));
	drupal_add_js(
	drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js',
	array(
					'weight' => 100
	));
}

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
	$tableHeight = 310;
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

if(isset($options['gallery_enabled']) && $options['gallery_enabled']){
	$table['gallery_buttons'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#attributes' => array(
			'style' => 'margin-bottom: 0px; padding: 0px; border: 0;'
		),
		'#border' => 0,
	);
	
	$table['gallery_buttons']['table_link'] = array(
			'#type' => 'button',
			'#value' => t('Table'),
			'#attributes' => array(
					'id' => $id_table . '_table_link',
					'onClick' => "gallery_addon.toggleGallery('$id_table', false);",
					'disabled' => 'disabled',
			)
	);
	
	$table['gallery_buttons']['gallery_link'] = array(
			'#type' => 'button',
			'#value' => t('Gallery'),
			'#attributes' => array(
				'id' => $id_table . '_gallery_link',
			  	'onClick' => "gallery_addon.toggleGallery('$id_table', true);",
					
			)
	);
	
	if(isset($options['gallery_image_sources'])){
		$image_sources = '';
		foreach($options['gallery_image_sources'] as $key=>$val){
			$selected = '';
			if($key === "selected"){
				$selected = "selected='selected'";
			}
			$image_sources .= "<option $selected value='".$val['value']."'>".$val['option']."</option>\n";
		}
	} else {
		$image_sources = '<option>-</option>';
	}
	
	$table['gallery_buttons']['image_source_select'] = array(
			'#markup' => $image_sources,
			'#prefix' => '<span class="datatable_gallery_imgsource"'
							. ' id="' . $id_table . '_gallery_image_source">' 
							. t('Images') 
							. ': <select class="form-select"'
							. ' onChange="javascript:gallery_addon.sourceChanged(\''.$id_table.'\',this)">',
			'#suffix' => '</select></span>'
	);
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
		autoload: <?php echo (isset($options['autoload']) && !$options['autoload']) ? 0 : 1; ?>,
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
		search: '<?php echo t('Search'); ?>',
		reset: '<?php echo t('Reset'); ?>',
		minmax: '<?php echo t('Minimize/Maximize Table'); ?>',
		hideshow: '<?php echo t('Hide/Show Columns'); ?>',
		dblClickResize: true,
		singleSelect: true,
		<?php if (isset($options['onDragColHandler'])) echo 'onDragCol: ' . $options['onDragColHandler'] . ','; ?>
		<?php if (isset($options['onToggleColHandler'])) echo 'onToggleCol: ' . $options['onToggleColHandler'] . ','; ?>
		<?php if (isset($options['onChangeSortHandler'])) echo 'onChangeSort: ' . $options['onChangeSortHandler'] . ','; ?>
		<?php if (isset($options['onSuccessHandler'])) echo 'onSuccess: ' . $options['onSuccessHandler'] . ','; ?>
		<?php if (isset($options['onErrorHandler'])) echo 'onError: ' . $options['onErrorHandler'] . ','; ?>
		<?php if (isset($options['onSubmitHandler'])) echo 'onSubmit: ' . $options['onSubmitHandler'] . ','; ?>
		<?php if (isset($options['preProcessHandler'])) echo 'preProcess: ' . $options['preProcessHandler'] . ','; ?>
	});
	<?php if (isset($options['rowClick'])) echo $options['rowClickHandler']; ?>
});

<?php if(isset($options['gallery_enabled']) && $options['gallery_enabled']) {?>
jQuery(window).ready(function() {
	// switch to the gallery view if #gallery is in the url
	if (window.location.hash.indexOf('gallery') != -1){
		gallery_addon.toggleGallery('<?=$id_table;?>', true);
	}
});
<?php }?>
-->
</script>
