<?php

namespace Naturwerk\Find;

/**
 * @author fabian.vogler
 */
class Column {

    /**
     * @var string
     */
    protected $name;

    /**
     * @var string
     */
    protected $title;

    /**
     * @var string
     */
    protected $template;

    /**
     * @var null|function
     */
    protected $condition;

    /**
     * @var boolean
     */
    protected $active = true;

    public function __construct($name, $title, $active = true, $template = 'plain', $condition = null) {
        $this->name = $name;
        $this->title = $title;
        $this->active = $active;
        $this->template = $template;
        $this->condition = $condition;
    }

    /**
     * @return string
     */
    public function getName() {
        return $this->name;
    }

    /**
     * @return string
     */
    public function getTitle() {
        return $this->title;
    }

    /**
     * @param boolean $active
     */
    public function setActive($active) {
        $this->active = $active;
    }

    /**
     * @return boolean
     */
    public function isActive() {
        return $this->active;
    }

    /**
     * @return string
     */
    public function getTemplate() {
        return $this->template;
    }

    /**
     * @param object $object
     * @return bool
     */
    public function condition($object, $parameters) {

        if ($this->condition != null) {
            return call_user_func($this->condition, $object, $parameters);
        }

        return true;
    }
}
