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

// Active assert and make it quiet
assert_options(ASSERT_ACTIVE, 1);
assert_options(ASSERT_WARNING, 0);
assert_options(ASSERT_QUIET_EVAL, 1);

// Create a handler function
function my_assert_handler($file, $line, $code) {
	echo "Assertion Failed:\n" .
		"\tFile:'$file'\n" .
		"\tLine: '$line'\n" .
		"\tCode: '$code'\n";
	global $errors;
	if (count($errors) > 0) {
		foreach ($errors as $key => $value) {
			print("\tERROR: '$value'\n");
		}
		debug_backtrace();
		exit(-1);
	}
}

// Set up the callback
assert_options(ASSERT_CALLBACK, 'my_assert_handler');
?>