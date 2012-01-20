<?php
/**
 * @author Reto Schneider, 2011, github@reto-schneider.ch
 */

$drupalprefix = 'drupal_';

$config['swissmon'] = array(
		'name' => 'olddb',
		'user' => 'olduser',
		'password' => 'password',
		'host' => 'host',
		'driver' => 'pgsql'
);

$config['naturvielfalt_dev'] = array(
		'name' => 'newdb',
		'user' => 'newuser',
		'password' => 'password',
		'host' => 'localhost',
		'driver' => 'pgsql'
);
?>
