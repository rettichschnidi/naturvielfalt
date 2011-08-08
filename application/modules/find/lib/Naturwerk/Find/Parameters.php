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
    protected $date;

    /**
     * @var array
     */
    protected $class;

    /**
     * @var array
     */
    protected $user;

    /**
     * @var array
     */
    protected $family;

    /**
     * @var array
     */
    protected $genus;

    /**
     * @var array
     */
    protected $sort;

    /**
     * @param string $search
     * @param array $geo
     * @param array $class
     * @param array $user
     * @param array $family
     * @param array $genus
     * @param array $sort
     */
    public function __construct($search = '', $geo = array(), $date = array(), $class = array(), $user = array(), $family = array(), $genus = array(), $sort = array()) {

        $this->search = $search;
        $this->geo = $geo;

        if (isset($date['from'])) {
            if ($date['from']) {
                $date['from'] = date('Y-m-d', strtotime($date['from']));
            } else {
                unset($date['from']);
            }
        }
        if (isset($date['to'])) {
            if ($date['to']) {
                $date['to'] = date('Y-m-d', strtotime($date['to']));
            }  else {
                unset($date['to']);
            }
        }
        $this->date = $date;

        $this->class = $class;
        $this->user = $user;
        $this->family = $family;
        $this->genus = $genus;
        $this->sort = $sort;
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
    public function getDate() {
        return $this->date;
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
    public function getUser() {
        return $this->user;
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

    /**
     * @return array
     */
    public function getSort() {
        return $this->sort;
    }
}
