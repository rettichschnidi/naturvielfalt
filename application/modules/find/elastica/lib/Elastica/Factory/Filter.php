<?php

class Elastica_Factory_Filter
{
    /**
     * @param string $key
     * @param array $coordinates
     */
	public function geo($key, $coordinates) {
		return new Elastica_Filter_GeoBoundingBox($key, $coordinates);
	}

	public function and_() {
		return new Elastica_Filter_And();
	}

	public function terms($key = '', array $terms = array()) {
		return new Elastica_Filter_Terms($key, $terms);
	}
}
