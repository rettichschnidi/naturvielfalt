<?php

namespace Naturwerk\Find;

/**
 * Parameter container for search, geo, class, family and genus.
 *
 * @author fabian.vogler
 */
class Parameters {

    /**
     * @var string
     */
    protected $search;

    /**
     * @var array
     */
    protected $geo;

    /**
     * @var array
     */
    protected $class;

    /**
     * @var array
     */
    protected $family;

    /**
     * @var array
     */
    protected $genus;

    /**
     * @param string $search
     * @param array $geo
     * @param array $class
     * @param array $family
     * @param array $genus
     */
    public function __construct($search = '', $geo = array(), $class = array(), $family = array(), $genus = array()) {

        $this->search = $search;
        $this->geo = $geo;
        $this->class = $class;
        $this->family = $family;
        $this->genus = $genus;
    }

    /**
     * @return string
     */
    public function getSearch() {
        return $this->search;
    }

    /**
     * @return array
     */
    public function getGeo() {
        return $this->geo;
    }

    /**
     * @return array
     */
    public function getClass() {
        return $this->class;
    }

    /**
     * @return array
     */
    public function getFamily() {
        return $this->family;
    }

    /**
     * @return array
     */
    public function getGenus() {
        return $this->genus;
    }
}
