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

    public function __construct($name, $title, $template = 'plain', $condition = null) {
        $this->name = $name;
        $this->title = $title;
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
