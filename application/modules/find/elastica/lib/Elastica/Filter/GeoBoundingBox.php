<?php

/**
 * Geo bounding box filter
 *
 * @uses Elastica_Query_Abstract
 * @category Xodoa
 * @package Elastica
 * @author Fabian Vogler <fabian@equivalence.ch>
 * @link http://www.elasticsearch.com/docs/elasticsearch/rest_api/query_dsl/geo_bounding_box_filter/
 */
class Elastica_Filter_GeoBoundingBox extends Elastica_Filter_Abstract
{
	protected $_key;
	protected $_coordinates;

	/**
	 * @param string $key Key
	 * @param array $coordinates
	 */
	public function __construct($key, $coordinates) {

		if (!isset($coordinates[0]) || !isset($coordinates[1])) {
			throw new Elastica_Exception_Invalid('expected $coordinates to be an array with two elements');
		}

		$this->_key = $key;
		$this->_coordinates = $coordinates;
	}

	/**
	 * Converts filter to array
	 *
	 * @see Elastica_Filter_Abstract::toArray()
	 * @return Elastica_Filter_GeoDistance Current object
	 */
	public function toArray() {
		return array(
			'geo_bounding_box' => array(
				$this->_key => array(
					'top_left' => $this->_coordinates[0],
					'bottom_right' => $this->_coordinates[1]
				),
			)
		);
	}
}
