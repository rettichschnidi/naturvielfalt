<div class="filters">
<form action="<?php echo check_url(url($_GET['q'])); ?>" method="get">

<?php
$result = $current->search();
$facets = $result->getFacets();
$filters = array('search' => $search, 'class' => $class, 'user' => $user, 'family' => $family, 'genus' => $genus, 'geo' => $geo, 'date' => $date, 'sort' => $sort);
?>

<?php foreach ($filters as $field => $filter): ?>
    <?php if (is_array($filter)): ?>
        <?php foreach ($filter as $i => $v): ?>
            <input type="hidden" name="<?php echo $field . '[' . $i . ']'; ?>" value="<?php echo check_plain($v); ?>" />
        <?php endforeach; ?>
    <?php endif; ?>
<?php endforeach; ?>

<h6><label for="search">Suchbegriff:</label> <a href="<?php echo check_url(url($_GET['q'])); ?>" class="newsearch">Neue Suche</a></h6>

<p><input class="search" name="search" id="search" value="<?php echo check_plain($search); ?>" /></p>

<?php $reset = array_merge($filters, array('geo' => array())); ?>
<h6>Gebiet: <?php if (count($geo) > 0): ?><a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>" class="clear">×</a><?php endif; ?></h6>

<p><img class="map-select" width="200" height="130" src="http://maps.google.com/maps/api/staticmap?<?php if (count($geo) > 1): ?>path=color:red|weight:1|fillcolor:red|<?php echo implode('|', $geo); else: ?>center=CH&amp;maptype=terrain&amp;zoom=5<?php endif; ?>&amp;size=200x130&amp;sensor=false" alt="" /></p>
<div class="map-overlay"><div id="map-canvas"></div><img src="<?php echo base_path() . drupal_get_path('module', 'find') . '/images/close.png'; ?>" width="30" height="30" class="map-close" /></div>

<?php $reset = array_merge($filters, array('date' => array())); ?>
<h6 class="filter">Datum: <?php if (count($date) > 0): ?><a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>" class="clear">×</a><?php endif; ?></h6>

<div class="fieldset">
<p><label for="date_from">Von:</label> <input name="date[from]" id="date_from" value="<?php echo check_plain(@$date['from']); ?>" /></p>

<p><label for="date_to">Bis:</label> <input name="date[to]" id="date_to" value="<?php echo check_plain(@$date['to']); ?>" /></p>
</div>

<?php echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Klasse', 'field' => 'class', 'value' => $class)); ?>

<?php if ($_GET['q'] != 'find/organisms') { echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Benutzer', 'field' => 'user', 'value' => $user)); } ?>

<!-- <?php echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Familie', 'field' => 'family', 'value' => $family)); ?> -->

<!-- <?php echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Gattung', 'field' => 'genus', 'value' => $genus)); ?> -->

<input class="element-hidden" type="submit" />

</form>
</div>

<div class="results">

<ul class="tabs">
    <li class="<?php echo 'find/sightings' == $_GET['q'] ? 'active' : ''; ?>">
        <?php echo l('Beobachtungen (' . $sightings->count() . ')', 'find/sightings', array('query' => $filters)); ?>
    </li>
    <li class="<?php echo 'find/inventories' == $_GET['q'] ? 'active' : ''; ?>">
        <?php echo l('Inventare (' . $inventories->count() . ')', 'find/inventories', array('query' => $filters)); ?>
    </li>
    <li class="<?php echo 'find/organisms' == $_GET['q'] ? 'active' : ''; ?>">
        <?php echo l('Tiere und Pflanzen (' . $organisms->count() . ')', 'find/organisms', array('query' => $filters)); ?>
    </li>
</ul>

<p><a href="javascript:window.print();">Drucken</a>, <?php echo l('Export als CSV', 'find/' . $key . '/export', array('query' => $filters)); ?></p>

<table>
<tr>
    <?php foreach ($current->getColumns() as $column): ?>
        <th>
        <?php $dir = isset($sort[$column->getName()]); ?>
            <?php $asc = $dir ? ('asc' == $sort[$column->getName()]) : false; ?>
            <?php $reset = array_merge($filters, array('sort' => array($column->getName() => $asc ? 'desc' : 'asc'))); ?>
            <a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>">
                <?php echo $column->getTitle(); ?>
                <?php echo $dir ? ($asc ? '↓' : '↑') : ''; ?>
            </a>
        </th>
    <?php endforeach; ?>
</tr>
<?php $i = 0; foreach ($result as $object): ?>
<tr class="<?php echo $i++ % 2 ? 'even' : 'odd'; ?>">
    <?php foreach ($current->getColumns() as $column): ?>
        <?php if ($column->condition($object, $current->getParameters())): ?>
            <?php echo theme('find_render_' . $column->getTemplate(), array('object' => $object, 'name' => $column->getName())); ?>
        <?php else: ?>
            <?php echo theme('find_render_plain', array('object' => $object, 'name' => $column->getName())); ?>
        <?php endif; ?>
    <?php endforeach; ?>
</tr>
<?php endforeach; ?>
</table>

</div>
