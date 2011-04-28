<div class='progress_block'>
<?php 
// add css-file
drupal_add_css(drupal_get_path('module', 'progress') . '/css/progress.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));

// initialisation of variables
if($active == NULL) $active = $elements[0];
$cnt = -1;
$class = 'done';

// plot of elements
for ($i=0; $i<count($elements); $i++) {
	if($elements[$i] == $active)
		$cnt = $i;
	if ($i == $cnt)
		$class = 'active';
	else if (($i > $cnt) && ($cnt >= 0))
		$class = 'pending';
?>
	<div class='progress_elem_cont' style='width:<?php echo floor(100/count($elements));?>%'>
	<div class='progress_elem <?php print $class; ?>'><?php print ($i+1).". ".$elements[$i]; ?></div></div>
<?php
}
?>
</div>