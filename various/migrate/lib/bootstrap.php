<?php
/**
 * @author Reto Schneider, 2011, github@reto-schneider.ch
 */

$basedir = dirname(__FILE__) . "/../";
$libdir = dirname(__FILE__) . "/";
$configdir = dirname(__FILE__) . "/../config/";

//always chdir to our own directory
chdir($basedir);

// array to hold all error messages which should be presented
// to the user
$errors = array();

// array to hold all messages which should be presented
// to the user
$messages = array();
?>