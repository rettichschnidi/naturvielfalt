<?php

namespace Naturwerk\Find;

/**
 * Parameter container for search, geo, class, family and genus.
 *
 * @author fabian.vogler
 */
class Parameters {

    /**
     * @var int
     */
    protected $uid;

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
    protected $town;

    /**
     * @var array
     */
    protected $canton;

    /**
     * @var array
     */
    protected $sort;

    /**
     * @var array
     */
    protected $columns;

    /**
     * @param int $uid
     * @param string $search
     * @param array $geo
     * @param array $class
     * @param array $user
     * @param array $family
     * @param array $genus
     * @param array $sort
     * @param array $columns
     */
    public function __construct($uid = 0, $search = '', $geo = array(), $date = array(), $class = array(), $user = array(), $family = array(), $genus = array(), $town = array(), $canton = array(), $sort = array(), $columns = array()) {

        $this->uid = $uid;
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
        $this->town = $town;
        $this->canton = $canton;

        $this->sort = $sort;
        $this->columns = $columns;
    }

    /**
     * @return int
     */
    public function getUid() {
        return $this->uid;
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
    public function getTown() {
        return $this->town;
    }

    /**
     * @return array
     */
    public function getCanton() {
        return $this->canton;
    }

    /**
     * @return array
     */
    public function getSort() {
        return $this->sort;
    }

    /**
     * @return array
     */
    public function getColumns() {
        return $this->columns;
    }

    /**
     * @param array $override
     */
    public function filter($override = array()) {

        $filter = array(
        	'search' => $this->getSearch(),
        	'class' => $this->getClass(),
        	'user' => $this->getUser(),
        	'family' => $this->getFamily(),
        	'genus' => $this->getGenus(),
        	'town' => $this->getTown(),
        	'canton' => $this->getCanton(),
        	'geo' => $this->getGeo(),
        	'date' => $this->getDate(),
        	'sort' => $this->getSort(),
        	'columns' => $this->getColumns(),
        );

        return array_merge($filter, $override);
    }
}
