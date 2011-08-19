<?php

define('DRUPAL_ROOT', dirname(__FILE__) . '/../..');

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

if (!isset($_SERVER['PHP_AUTH_USER'])) {

    header('WWW-Authenticate: Basic realm="Naturvielfalt"');
    header('HTTP/1.0 401 Unauthorized');

    echo 'Text to send if user hits Cancel button';
    exit;

} else {

    $uid = user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_USER']);

    var_dump($uid);
}
