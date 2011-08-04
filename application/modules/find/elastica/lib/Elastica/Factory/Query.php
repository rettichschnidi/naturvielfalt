<?php

class Elastica_Factory_Query
{
	public function filtered(Elastica_Query_Abstract $query, Elastica_Filter_Abstract $filter) {
		return new Elastica_Query_Filtered($query, $filter);
	}

	public function hasChild($query, $type = null) {
		return new Elastica_Query_HasChild($query, $type);
	}

	public function all() {
		return new Elastica_Query_MatchAll();
	}

	public function string($queryString = '') {
		return new Elastica_Query_QueryString($queryString);
	}

	public function flt($likeText = '') {
		$query = new Elastica_Query_FuzzyLikeThis();
		$query->setLikeText($likeText);
		$query->setMinSimilarity(0.8); // require some higher similarity
		return $query;
	}
}
