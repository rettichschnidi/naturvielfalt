<?php

define('DRUPAL_ROOT', dirname(__FILE__) . '/../..');

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

$data = array();

function first($name) {
    
    $plain = strtr(utf8_decode(name), utf8_decode('ÄÖÜäöü'), 'AOUaou');
    return strtoupper(substr($plain, 0, 1));
}

// fauna

$select = db_select('inventory_type', 't')->fields('t', array('id', 'name'))->execute();

$classes = array();
foreach ($select->fetchAll() as $class) {
    $classes[$class->id] = $class->name;
}

foreach ($classes as $id => $class) {

    $select = db_select('fauna_organism', 'f');
    $select->condition('fauna_class_id', $id);
    $select->addExpression('name_de', 'de');
    $select->addExpression('ARRAY_TO_STRING(ARRAY[genus, species], \' \')', 'la');
    $select->innerJoin('organism', 'o', 'o.organism_type = 1 AND o.organism_id = f.id');
    $select->fields('o', array('id'));
    $select->orderBy('de');
    $result = $select->execute();

    $organsims = array();
    $progress = 65;
    foreach ($result->fetchAll() as $organism) {

        if ($organism->de || $organism->la) {

            $alpha = first($organism->de);
            $css = array();
            while ($progress < ord($alpha) + 1) {
                $css[] = 'alpha-' . chr($progress++);
            }

            $organsims[$organism->id] = array('de' => $organism->de, 'la' => $organism->la, 'css' => implode(' ', $css));
        }
    }

    $data['organisms'][$id] = $organsims;
}

// add flora
$select = db_select('flora_organism', 'f');
$select->addExpression('name_de', 'de');
$select->addExpression('ARRAY_TO_STRING(ARRAY["Gattung", "Art"], \' \')', 'la');
$select->innerJoin('organism', 'o', 'o.organism_type = 2 AND o.organism_id = f.id');
$select->fields('o', array('id'));
$select->orderBy('de');
$result = $select->execute();

$organsims = array();
$progress = 65;
foreach ($result->fetchAll() as $organism) {

    if ($organism->de) {
        
        $alpha = first($organism->de);
        $css = array();
        while ($progress < ord($alpha) + 1) {
            $css[] = 'alpha-' . chr($progress++);
        }

        $organsims[$organism->id] = array('de' => $organism->de, 'la' => $organism->la, 'css' => implode(' ', $css));
    }
}

$data['organisms']['16'] = $organsims; // 16 = flora

// complete $data
asort($classes);
$data['classes'] = $classes;

header('Content-type: application/json');
echo json_encode($data);
