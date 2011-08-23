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

<h6>
    <label for="search" class="search"><?php echo t('Search term'); ?>:</label>
    <a href="<?php echo check_url(url($_GET['q'])); ?>" class="newsearch"><?php echo t('New search'); ?></a>
</h6>

<p><input class="search" name="search" id="search" value="<?php echo check_plain($parameters->getSearch()); ?>" /></p>

<div class="filter-area">
    <?php $reset = $parameters->filter(array('geo' => array())); ?>
    <h6>
        <?php echo t('Area'); ?>:
        <a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>" class="clear">×</a>
    </h6>

    <p>
        <img
            class="map-select"
            width="200"
            height="130"
            src="http://maps.google.com/maps/api/staticmap?<?php if (count($parameters->getGeo()) > 1): ?>path=color:red|weight:1|fillcolor:red|<?php echo implode('|', $parameters->getGeo()); else: ?>center=CH&amp;maptype=terrain&amp;zoom=5<?php endif; ?>&amp;size=200x130&amp;sensor=false"
            alt=""
        />
    </p>
    <div class="map-overlay">
        <div id="map-canvas"></div>
        <img src="<?php echo base_path() . drupal_get_path('module', 'find') . '/images/close.png'; ?>" width="30" height="30" class="map-close" />
    </div>
</div>

<div class="filter-date">
    <?php $reset = $parameters->filter(array('date' => array())); ?>
    <h6 class="filter">
        <?php echo t('Observation date'); ?>:
        <a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>" class="clear">×</a>
    </h6>
    
    <div class="fieldset">
        <?php $date = $parameters->getDate(); ?>
        <p>
            <label for="date_from"><?php echo t('Date from'); ?>:</label>
            <input name="date[from]" id="date_from" value="<?php echo check_plain(@$date['from']); ?>" />
        </p>
        
        <p>
            <label for="date_to"><?php echo t('Date to'); ?>:</label>
            <input name="date[to]" id="date_to" value="<?php echo check_plain(@$date['to']); ?>" />
        </p>
    </div>
</div>

<?php echo theme('find_facet', array('facets' => $facets, 'parameters' => $parameters, 'title' => t('Class'), 'field' => 'class', 'value' => $parameters->getClass())); ?>

<?php echo theme('find_facet', array('facets' => $facets, 'parameters' => $parameters, 'title' => t('Family'), 'field' => 'family', 'value' => $parameters->getFamily())); ?>

<?php echo theme('find_facet', array('facets' => $facets, 'parameters' => $parameters, 'title' => t('Town'), 'field' => 'town', 'value' => $parameters->getTown())); ?>

<?php echo theme('find_facet', array('facets' => $facets, 'parameters' => $parameters, 'title' => t('Canton'), 'field' => 'canton', 'value' => $parameters->getCanton())); ?>

<?php echo theme('find_facet', array('facets' => $facets, 'parameters' => $parameters, 'title' => t('User'), 'field' => 'user', 'value' => $parameters->getUser())); ?>

<p>
    <select class="filter-selector">
        <option value=""><?php echo t('Add filter...'); ?></option>
        <option value="area"><?php echo t('Area'); ?></option>
        <option value="date"><?php echo t('Observation date'); ?></option>
        <option value="class"><?php echo t('Class'); ?></option>
        <option value="family"><?php echo t('Family'); ?></option>
        <option value="town"><?php echo t('Town'); ?></option>
        <option value="canton"><?php echo t('Canton'); ?></option>
        <option value="user"><?php echo t('User'); ?></option>
    </select>
</p>

<input class="submit" type="submit" />

</div>

<div class="results">

<ul class="tabs">
    <li class="<?php echo 'find/sightings' == $_GET['q'] ? 'active' : ''; ?>">
        <?php echo l(t('Observations') . ' (' . $sightings->count() . ')', 'find/sightings', array('query' => $parameters->filter())); ?>
    </li>
    <li class="<?php echo 'find/inventories' == $_GET['q'] ? 'active' : ''; ?>">
        <?php echo l(t('Inventories') . ' (' . $inventories->count() . ')', 'find/inventories', array('query' => $parameters->filter())); ?>
    </li>
    <li class="<?php echo 'find/areas' == $_GET['q'] ? 'active' : ''; ?>">
        <?php echo l(t('Areas') . ' (' . $areas->count() . ')', 'find/areas', array('query' => $parameters->filter())); ?>
    </li>
    <li class="<?php echo 'find/organisms' == $_GET['q'] ? 'active' : ''; ?>">
        <?php echo l(t('Organisms') . ' (' . $organisms->count() . ')', 'find/organisms', array('query' => $parameters->filter())); ?>
    </li>
</ul>

<div class="toolbar">
<p class="left"><a href="javascript:window.print();"><?php echo t('Print'); ?></a>, <?php echo l(t('Export as CSV'), 'find/' . $key . '/export', array('query' => $parameters->filter())); ?></p>
<p class="right"><a href="#" class="columns-select"><?php echo t('Select columns'); ?></a></p>
<div class="columns-overlay">
    <ul>
    <?php foreach ($current->getColumns() as $column): ?>
        <li>
            <label
                class="option"
                for="columns_<?php echo $column->getName(); ?>"
            >
                <input
                    type="checkbox"
                    name="columns[]"
                    value="<?php echo $column->getName(); ?>"
                    id="columns_<?php echo $column->getName(); ?>"
                    <?php if ($column->isActive()): ?> checked="checked"<?php endif; ?>
                >
                <?php echo $column->getTitle(); ?>
            </label>
        </li>
    <?php endforeach; ?>
    </ul>
    <p>
        <input type="submit" value="<?php echo t('Apply'); ?>" />
        <a href="<?php echo check_url(url($_GET['q'], array('query' => $parameters->filter(array('columns' => array()))))); ?>" class="soft"><?php echo t('Show all'); ?></a>
    </p>
</div>
</div>

<div class="table">
<table>
<thead>
<tr>
    <?php foreach ($current->getActiveColumns() as $column): ?>
        <th>
            <?php $sort = $current->getSort(); ?>
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
</thead>
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

</div>
</form>
