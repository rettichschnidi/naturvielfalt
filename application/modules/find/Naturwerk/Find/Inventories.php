<?php

namespace Naturwerk\Find;

/**
 * Find inventories.
 *
 * @author fabian.vogler
 */
class Inventories extends Finder {

    /**
     * @param \Elastica_Index $index
     * @param Parameters $parameter
     */
    public function __construct(\Elastica_Index $index, Parameters $parameters) {
        parent::__construct($index, 'inventory', $parameters);
        
        $this->addColumn('name', 'Name', 'link');
        $this->addColumn('area', 'Gebiet');
        $this->addColumn('user', 'Benutzer');
    }

    /**
     * @inheritDoc
     */
    protected function access(\Elastica_Query_Abstract $query) {

        $access = new \Elastica_Filter_Or();

        $user = new \Elastica_Filter_Term(array('access' => $this->parameters->getUid()));
        $access->addFilter($user);

        $shared = new \Elastica_Filter_Term(array('shared' => true));
        $access->addFilter($shared);

        $query = new \Elastica_Query_Filtered($query, $access);

        return $query;
    }
}
