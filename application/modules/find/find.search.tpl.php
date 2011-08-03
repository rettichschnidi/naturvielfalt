<div class="filters">
<form action="find" method="get">

<h6>Suchbegriff: <a href="<?php echo check_url(url($_GET['q'])); ?>" class="newsearch">Neue Suche</a></h6>

<p><input type="search" name="search" /></p>

<?php
$facets = $result->getFacets();
$filters = array('class' => $class, 'family' => $family, 'genus' => $genus, 'geo' => $geo);
$reset = array_merge($filters, array('geo' => array()));
?>
<h6>Gebiet: <?php if (count($geo) > 0): ?><a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>" class="clear">×</a><?php endif; ?></h6>

<p><img class="map-select" width="200" height="130" src="http://maps.google.com/maps/api/staticmap?<?php if (count($box) > 0): ?>path=color:red|weight:1|fillcolor:red|<?php echo implode('|', $box); else: ?>center=CH<?php endif; ?>&amp;size=200x130&amp;sensor=false" alt="" /></p>
<div class="map-overlay"><div id="map-canvas"></div></div>

<?php echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Klasse', 'field' => 'class', 'value' => $class)); ?>

<?php echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Familie', 'field' => 'family', 'value' => $family)); ?>

<?php echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Gattung', 'field' => 'genus', 'value' => $genus)); ?>
</form>
</div>

<div class="results">

<ul class="tabs">
<li class="<?php echo 'find/organisms' == $_GET['q'] ? 'active' : ''; ?>"><?php echo l('Tiere und Pflanzen (' . ($organisms ? $organisms->getTotalHits() : 0) . ')', 'find/organisms', array('query' => $filters)); ?></li>
<li class="<?php echo 'find/sightings' == $_GET['q'] ? 'active' : ''; ?>"><?php echo l('Beobachtungen (' . ($sightings ? $sightings->getTotalHits() : 0) . ')', 'find/sightings', array('query' => $filters)); ?></li>
<li class="<?php echo 'find/inventories' == $_GET['q'] ? 'active' : ''; ?>"><?php echo l('Inventare (' . ($inventories ? $inventories->getTotalHits() : 0) . ')', 'find/inventories', array('query' => $filters)); ?></li>
</ul>

<table>
<tr>
    <th>Name</th>
    <th>Fachbezeichnung</th>
    <th>Benutzer</th>
    <th>Inventar</th>
</tr>
<?php $i = 0; foreach ($result as $object): ?>
<tr class="<?php echo $i++ % 2 ? 'even' : 'odd'; ?>">
    <td><?php echo l($object->name, $object->url); ?></td>
    <td><?php echo l($object->name_la, $object->url); ?></td>
    <td><?php echo $object->user; ?></td>
    <td><?php echo $object->inventory; ?></td>
</tr>
<?php endforeach; ?>
</table>

</div>
