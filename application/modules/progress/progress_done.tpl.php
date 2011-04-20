<div class='progress_block'>
<?php 
// add css-file
drupal_add_css(drupal_get_path('module', 'progress') . '/css/progress.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));

// initialisation of variables
if($active == NULL) $active = $title[0];
$cnt = -1;
$class = 'done';

// plot of elements
for ($i=0; $i<count($title); $i++) {
	if($title[$i] == $active)
		$cnt = $i;
	if ($i == $cnt)
		$class = 'active';
	else if (($i > $cnt) && ($cnt >= 0))
		$class = 'pending';
?>
	<div class='progress_elem_cont'><div class='progress_elem <?php print $class; ?>'><?php print ($i+1).". ".$title[$i]; ?></div></div>
<?php
}
?>
</div>