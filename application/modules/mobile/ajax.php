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
    $user = user_load($uid);

    if ($uid) {

        $organism = @$_POST['organism'];
        $count = @$_POST['count'];
        $date = @$_POST['date'];
        $location = @$_POST['location'];
        $type = @$_POST['type'];

        // get head_inventory_id
        $head = _inventory_single_get_id($user);

        // check for existing inventory
        $inventory = db_select('inventory', 'i')->fields('i', array('id'))->condition('inventory_type_id', $type)->condition('head_inventory_id', $head)->execute()->fetchField();

        if (!$inventory) {

            // inventory doesn't exist, create it
            $inventory = db_insert('inventory')->fields(array('inventory_type_id' => $type, 'head_inventory_id' => $head))->execute();
        }

        // insert
        $entry = db_insert('inventory_entry')->fields(array('organism_id' => $organism, 'inventory_id' => $inventory, 'position' => 0))->execute();

        // update location
        $geom = 'ST_GeomFromText(\'POINT(' . $location . ')\', 4326)';
        db_update('inventory_entry')->expression('geom', $geom)->condition('id', $entry)->execute();

        echo 'Die Beobachtung wurde erfolgreich gespeichert, vielen Dank!';

    } else {
        auth();
    }
}
