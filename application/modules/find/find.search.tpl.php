<div class="filters">
<form action="<?php echo check_url(url($_GET['q'])); ?>" method="get">

<?php
$facets = $result->getFacets();
$filters = array('search' => $search, 'class' => $class, 'user' => $user, 'family' => $family, 'genus' => $genus, 'geo' => $geo, 'sort' => $sort);
?>

<?php foreach ($filters as $field => $filter): ?>
    <?php if (is_array($filter)): ?>
        <?php foreach ($filter as $i => $v): ?>
            <input type="hidden" name="<?php echo $field . '[' . $i . ']'; ?>" value="<?php echo check_plain($v); ?>" />
        <?php endforeach; ?>
    <?php endif; ?>
<?php endforeach; ?>

<h6>Suchbegriff: <a href="<?php echo check_url(url($_GET['q'])); ?>" class="newsearch">Neue Suche</a></h6>

<p><input name="search" value="<?php echo check_plain($search); ?>" /></p>

<?php $reset = array_merge($filters, array('geo' => array())); ?>
<h6>Gebiet: <?php if (count($geo) > 0): ?><a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>" class="clear">×</a><?php endif; ?></h6>

<p><img class="map-select" width="200" height="130" src="http://maps.google.com/maps/api/staticmap?<?php if (count($box) > 0): ?>path=color:red|weight:1|fillcolor:red|<?php echo implode('|', $box); else: ?>center=CH<?php endif; ?>&amp;size=200x130&amp;sensor=false" alt="" /></p>
<div class="map-overlay"><div id="map-canvas"></div></div>

<?php echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Klasse', 'field' => 'class', 'value' => $class)); ?>

<?php if ($_GET['q'] != 'find/organisms') { echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Benutzer', 'field' => 'user', 'value' => $user)); } ?>

<!-- <?php echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Familie', 'field' => 'family', 'value' => $family)); ?> -->

<!-- <?php echo theme('find_facet', array('facets' => $facets, 'filters' => $filters, 'title' => 'Gattung', 'field' => 'genus', 'value' => $genus)); ?> -->
</form>
</div>

<div class="results">

<ul class="tabs">
<li class="<?php echo 'find/organisms' == $_GET['q'] ? 'active' : ''; ?>"><?php echo l('Tiere und Pflanzen (' . ($organisms ? $organisms->getTotalHits() : 0) . ')', 'find/organisms', array('query' => $filters)); ?></li>
<li class="<?php echo 'find/sightings' == $_GET['q'] ? 'active' : ''; ?>"><?php echo l('Beobachtungen (' . ($sightings ? $sightings->getTotalHits() : 0) . ')', 'find/sightings', array('query' => $filters)); ?></li>
<li class="<?php echo 'find/inventories' == $_GET['q'] ? 'active' : ''; ?>"><?php echo l('Inventare (' . ($inventories ? $inventories->getTotalHits() : 0) . ')', 'find/inventories', array('query' => $filters)); ?></li>
</ul>

<?php
$technical = 'find/inventories' != $_GET['q'];
$user = 'find/organisms' != $_GET['q'];
$inventory = 'find/sightings' == $_GET['q'];
?>
<table>
<tr>
    <?php $asc = ('asc' == @$sort['name']); ?>
    <th><a href="<?php echo check_url(url($_GET['q'], array('query' => array_merge($filters, array('sort' => array('name' => $asc ? 'desc' : 'asc')))))); ?>">Name <?php echo $asc ? '↓' : '↑'; ?></a></th>
    <?php if ($technical): ?><th>Fachbezeichnung</th><?php endif; ?>
    <?php if ($user): ?><th>Benutzer</th><?php endif; ?>
    <?php if ($inventory): ?><th>Inventar</th><?php endif; ?>
</tr>
<?php $i = 0; foreach ($result as $object): ?>
<tr class="<?php echo $i++ % 2 ? 'even' : 'odd'; ?>">
    <td><?php echo l($object->name, $object->url); ?></td>
    <?php if ($technical): ?><td><?php echo l($object->name_la, $object->url); ?></td><?php endif; ?>
    <?php if ($user): ?><td><?php echo $object->user; ?></td><?php endif; ?>
    <?php if ($inventory): ?><td><?php echo $object->inventory; ?></td><?php endif; ?>
</tr>
<?php endforeach; ?>
</table>

</div>
