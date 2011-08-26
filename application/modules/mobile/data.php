<?php

define('DRUPAL_ROOT', dirname(__FILE__) . '/../..');

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

$data = array();

// fauna

$select = db_select('inventory_type', 't')->fields('t', array('id', 'name'))->execute();

$classes = array();
foreach ($select->fetchAll() as $class) {
    $classes[$class->id] = $class->name;
}

foreach ($classes as $id => $class) {

    $select = db_select('fauna_organism', 'f');
    $select->condition('fauna_class_id', $id);
    $select->addExpression('CASE WHEN name_de IS NULL THEN genus || \' \' || species ELSE name_de END', 'de');
    $select->addExpression('genus || \' \' || species', 'la');
    $select->innerJoin('organism', 'o', 'o.organism_type = 1 AND o.organism_id = f.id');
    $select->fields('o', array('id'));
    $select->orderBy('de');
    $result = $select->execute();

    $organsims = array();
    foreach ($result->fetchAll() as $organism) {
        $organsims[$organism->id] = array('de' => $organism->de, 'la' => $organism->la);
    }

    $data['organisms'][$id] = $organsims;
}

// add flora
$select = db_select('flora_organism', 'f');
$select->addExpression('CASE WHEN name_de IS NULL THEN "Gattung" || \' \' || "Art" ELSE name_de END', 'de');
$select->addExpression('"Gattung" || \' \' || "Art"', 'la');
$select->innerJoin('organism', 'o', 'o.organism_type = 2 AND o.organism_id = f.id');
$select->fields('o', array('id'));
$select->orderBy('de');
$result = $select->execute();

$organsims = array();
foreach ($result->fetchAll() as $organism) {
    $organsims[$organism->id] = array('de' => $organism->de, 'la' => $organism->la);
}

$data['organisms']['16'] = $organsims; // 16 = flora

// complete $data
asort($classes);
$data['classes'] = $classes;

header('Content-type: application/json');
echo json_encode($data);
