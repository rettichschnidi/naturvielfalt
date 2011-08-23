<div class="filter-<?php echo $field; ?><?php if (count($value) > 0): ?> active<?php endif; ?>">
    <?php $reset = $parameters->filter(array($field => array())); ?>
    <h6 class="filter">
        <?php echo $title; ?>: 
        <a href="<?php echo check_url(url($_GET['q'], array('query' => $reset))); ?>" class="clear">×</a>
    </h6>
    <?php if ($facets[$field]['total'] > 0): ?>
        <ul class="choices">
        <?php foreach ($facets[$field]['terms'] as $term): ?>
            <?php
            if ($active = in_array($term['term'], $value)) {
                $params = array_diff($value, (array) $term['term']); // remove current term for active
            } else {
                $params = array_merge($value, (array) $term['term']);
            }
            $filters = $parameters->filter(array($field => $params));
            ?>
            <li class="<?php echo $active ? 'active' : ''; ?>"><a href="<?php echo check_url(url($_GET['q'], array('query' => $filters))); ?>" class="<?php echo $active ? 'active' : ''; ?>"><?php echo $term['term'] . ' (' . $term['count'] . ')'; ?></a></li>
        <?php endforeach; ?>
        </ul>
    <?php else: ?>
        <ul class="choices"><li>Keine Werte vorhanden.</li></ul>
    <?php endif; ?>
</div>
