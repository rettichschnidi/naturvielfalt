<?php

namespace Naturwerk\Find;
/**
 * Parameter container for search, geo, class, family and genus.
 *
 * @author fabian.vogler
 */
class Parameters {

	/**
	 * @var array
	 */
	protected $data;

	/**
	 * @param array $data
	 */
	public function __construct($data) {

		if (isset($data['date']['from'])) {
			if ($data['date']['from']) {
				$data['date']['from'] = date('Y-m-d', strtotime($data['date']['from']));
			} else {
				unset($data['date']['from']);
			}
		}
		if (isset($data['date']['to'])) {
			if ($data['date']['to']) {
				$data['date']['to'] = date('Y-m-d', strtotime($data['date']['to']));
			} else {
				unset($data['date']['to']);
			}
		}

		$this->data = $data;
	}

	/**
	 * @param string $field
	 * @param mixed $default
	 * @return mixed
	 */
	public function get($field, $default = array()) {

		if (isset($this->data[$field])) {

			return $this->data[$field];
		}

		return $default;
	}

	/**
	 * Getter attribute wrapper
	 *
	 * @param string $method
	 * @param array $args
	 * @return mixed
	 */
	public function __call($method, $args) {
		return $this->get(strtolower(substr($method, 3)));
	}

	/**
	 * @param array $override
	 * @return array
	 */
	public function filter($override = array()) {
		return array_merge($this->data, $override);
	}
}
