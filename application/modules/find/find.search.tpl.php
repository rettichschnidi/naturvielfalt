<form action="<?php echo check_url(url($_GET['q'])); ?>" method="get">
<div class="filters">

<?php
$result = $current->search();
$facets = $result->getFacets();
$parameters = $current->getParameters();
?>

<?php foreach ($parameters->filter(array('columns' => array())) as $field => $filter): ?>
    <?php if (is_array($filter)): ?>
        <?php foreach ($filter as $i => $v): ?>
            <input type="hidden" name="<?php echo $field . '[' . $i . ']'; ?>" value="<?php echo check_plain($v); ?>" />
        <?php endforeach; ?>
    <?php endif; ?>
<?php endforeach; ?>

<h6><label for="search">Suchbegriff:</label> <a href="<?php echo check_url(url($_GET['q'])); ?>" class="newsearch">Neue Suche</a></h6>

<p><input class="search" name="search" id="search" value="<?php echo check_plain($parameters->getSearch()); ?>" /></p>

<?php $reset = $parameters->filter(array('geo' => array())); ?>
<h6>Gebiet: <?php if (count($parameters->getGeo()) > 0): ?><a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>" class="clear">×</a><?php endif; ?></h6>

<p><img class="map-select" width="200" height="130" src="http://maps.google.com/maps/api/staticmap?<?php if (count($parameters->getGeo()) > 1): ?>path=color:red|weight:1|fillcolor:red|<?php echo implode('|', $geo); else: ?>center=CH&amp;maptype=terrain&amp;zoom=5<?php endif; ?>&amp;size=200x130&amp;sensor=false" alt="" /></p>
<div class="map-overlay"><div id="map-canvas"></div><img src="<?php echo base_path() . drupal_get_path('module', 'find') . '/images/close.png'; ?>" width="30" height="30" class="map-close" /></div>

<?php $reset = $parameters->filter(array('date' => array())); ?>
<h6 class="filter">Datum: <?php if (count($parameters->getDate()) > 0): ?><a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>" class="clear">×</a><?php endif; ?></h6>

<div class="fieldset">
<p><label for="date_from">Von:</label> <input name="date[from]" id="date_from" value="<?php echo check_plain(@$date['from']); ?>" /></p>

<p><label for="date_to">Bis:</label> <input name="date[to]" id="date_to" value="<?php echo check_plain(@$date['to']); ?>" /></p>
</div>

<?php echo theme('find_facet', array('facets' => $facets, 'parameters' => $parameters, 'title' => 'Klasse', 'field' => 'class', 'value' => $parameters->getClass())); ?>

<?php if ($_GET['q'] != 'find/organisms') { echo theme('find_facet', array('facets' => $facets, 'parameters' => $parameters, 'title' => 'Benutzer', 'field' => 'user', 'value' => $parameters->getUser())); } ?>

<input class="element-hidden" type="submit" />

</div>

<div class="results">

<ul class="tabs">
    <li class="<?php echo 'find/sightings' == $_GET['q'] ? 'active' : ''; ?>">
        <?php echo l('Beobachtungen (' . $sightings->count() . ')', 'find/sightings', array('query' => $parameters->filter())); ?>
    </li>
    <li class="<?php echo 'find/inventories' == $_GET['q'] ? 'active' : ''; ?>">
        <?php echo l('Inventare (' . $inventories->count() . ')', 'find/inventories', array('query' => $parameters->filter())); ?>
    </li>
    <li class="<?php echo 'find/organisms' == $_GET['q'] ? 'active' : ''; ?>">
        <?php echo l('Tiere und Pflanzen (' . $organisms->count() . ')', 'find/organisms', array('query' => $parameters->filter())); ?>
    </li>
</ul>

<div class="toolbar">
<p class="left"><a href="javascript:window.print();">Drucken</a>, <?php echo l('Export als CSV', 'find/' . $key . '/export', array('query' => $parameters->filter())); ?></p>
<p class="right"><a href="#" class="columns-select">Spalten auswählen</a></p>
<div class="columns-overlay">
    <ul>
    <?php foreach ($current->getColumns() as $column): ?>
        <li>
            <input
                type="checkbox"
                name="columns[]"
                value="<?php echo $column->getName(); ?>"
                id="columns_<?php echo $column->getName(); ?>"
                <?php if ($column->isActive()): ?> checked="checked"<?php endif; ?>
            >
            <label
                class="option"
                for="columns_<?php echo $column->getName(); ?>"
            >
                <?php echo $column->getTitle(); ?>
            </label>
        </li>
    <?php endforeach; ?>
    </ul>
    <p>
        <input type="submit" value="Übernehmen" />
        <a href="<?php echo check_url(url($_GET['q'], array('query' => $parameters->filter(array('columns' => array()))))); ?>" class="soft">Alle anzeigen</a>
    </p>
</div>
</div>

<table>
<tr>
    <?php foreach ($current->getActiveColumns() as $column): ?>
        <th>
        <?php $dir = isset($sort[$column->getName()]); ?>
            <?php $asc = $dir ? ('asc' == $sort[$column->getName()]) : false; ?>
            <?php $reset = $parameters->filter(array('sort' => array($column->getName() => $asc ? 'desc' : 'asc'))); ?>
            <a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>">
                <?php echo $column->getTitle(); ?>
                <?php echo $dir ? ($asc ? '↓' : '↑') : ''; ?>
            </a>
        </th>
    <?php endforeach; ?>
</tr>
<?php $i = 0; foreach ($result as $object): ?>
<tr class="<?php echo $i++ % 2 ? 'even' : 'odd'; ?>">
    <?php foreach ($current->getActiveColumns() as $column): ?>
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
</form>
