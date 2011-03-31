<?php
class Application_Model_Util {
	public static function resulsToHash($results, $key, $value)
	{
		$arr = array();
		foreach($results as $resKey=>$resValue)
		{
			$arr[$resValue->$key] = $resValue->$value;
		}
		return $arr;
	}
}
