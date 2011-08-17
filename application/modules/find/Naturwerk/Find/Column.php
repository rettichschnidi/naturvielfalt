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

    public function __construct($name, $title, $template = 'plain') {
        $this->name = $name;
        $this->title = $title;
        $this->template = $template;
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
}
