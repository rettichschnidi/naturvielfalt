<div style="width: 200px; float: left; margin-right: 25px; padding-right: 20px; border-right: 1px solid #eee;">
<form action="find" method="get">

<h6>Suchbegriff: <a href="find" style="float: right; color: #AAA;">Neue Suche</a></h6>

<p><input type="search" name="search" /></p>

<h6>Gebiet:</h6>

<p><img width="200" height="100" src="http://maps.google.com/maps/api/staticmap?center=47.482409,8.212452&amp;zoom=11&amp;size=200x100&amp;sensor=false" alt="" /></p>


<?php

$facets = $result->getFacets();
$filters = array('class' => $class, 'family' => $family, 'genus' => $genus);
function find_render_facet($facets, $filters, $title, $field, $value) {
    $reset = array_merge($filters, array($field => array()));
?>
    <h6 style="margin-top: 10px;"><?php echo $title; ?>: <?php if (count($value) > 0): ?><a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>" style="float: right; font-size: 1.3em; line-height: 12px; color: #860d0d; padding: 0 3px;">×</a><?php endif; ?></h6>

    <ul style="max-height: 200px; overflow: auto; border: 1px solid #eee; border-width: 1px 0 1px 0;">
    <?php foreach ($facets[$field]['terms'] as $term): ?>
        <?php
        if ($active = in_array($term['term'], $value)) {
            $params = array_diff($value, (array) $term['term']); // remove current term for active
        } else {
            $params = array_merge($value, (array) $term['term']);
        }
        $filters = array_merge($filters, array($field => $params));
        ?>
        <li><a href="<?php echo check_url(url($_GET['q'], array('query' => $filters))); ?>" class="<?php echo $active ? 'active' : ''; ?>"><?php echo $term['term'] . ' (' . $term['count'] . ')'; ?></a></li>
    <?php endforeach; ?>
    </ul>
<?php
}

?>

<?php find_render_facet($facets, $filters, 'Klasse', 'class', $class); ?>

<?php find_render_facet($facets, $filters, 'Familie', 'family', $family); ?>

<?php find_render_facet($facets, $filters, 'Gattung', 'genus', $genus); ?>
</form>
</div>

<div style="width: 760px; float: left;">

<ul class="tabs">
<li class="<?php echo 'find/organisms' == $_GET['q'] ? 'active' : ''; ?>"><?php echo l('Tiere und Pflanzen (' . ($organisms ? $organisms->getTotalHits() : 0) . ')', 'find/organisms', array('query' => $filters)); ?></li>
<li class="<?php echo 'find/sightings' == $_GET['q'] ? 'active' : ''; ?>"><?php echo l('Beobachtungen (' . ($sightings ? $sightings->getTotalHits() : 0) . ')', 'find/sightings', array('query' => $filters)); ?></li>
<li class="<?php echo 'find/inventories' == $_GET['q'] ? 'active' : ''; ?>"><?php echo l('Inventare (' . $inventories->getTotalHits() . ')', 'find/inventories', array('query' => $filters)); ?></li>
</ul>

<table style="width: 100%;">
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
