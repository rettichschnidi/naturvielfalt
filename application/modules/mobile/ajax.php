<?php

define('DRUPAL_ROOT', dirname(__FILE__) . '/../..');

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

function auth() {

    header('WWW-Authenticate: Basic realm="Naturvielfalt"');
    header('HTTP/1.0 401 Unauthorized');
    
    echo 'Beobachtung wurde nicht gespeichert. Bitte auf naturvielfalt.ch registrieren.';
    exit;
}

if (!isset($_SERVER['PHP_AUTH_USER'])) {

    auth();

} else {

    $uid = user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']);

    if ($uid) {

        $organism = @$_POST['organism'];
        $count = @$_POST['count'];
        $date = @$_POST['date'];
        $location = @$_POST['location'];

        echo 'Die Beobachtung wurde erfolgreich gespeichert, vielen Dank!';

    } else {
        auth();
    }
}
