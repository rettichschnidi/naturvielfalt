<?php

define('DRUPAL_ROOT', dirname(__FILE__) . '/../..');

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

$data = array();

// fauna

$select = db_select('fauna_class', 'c')->fields('c', array('id', 'name_de'))->execute();

$classes = array('0' => 'Pflanzen');
foreach ($select->fetchAll() as $class) {
    $classes[$class->id] = $class->name_de;
}

foreach ($classes as $id => $class) {

    $select = db_select('fauna_organism', 'f');
    $select->condition('fauna_class_id', $id);
    $select->addExpression('CASE WHEN name_de IS NULL THEN genus || \' \' || species ELSE name_de END', 'de');
    $select->addExpression('genus || \' \' || species', 'la');
    $select->innerJoin('organism', 'o', 'o.organism_type = 2 AND o.organism_id = f.id');
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

$data['organisms']['0'] = $organsims;


// complete $data
$data['classes'] = $classes;

header('Content-type: application/json');
echo json_encode($data);