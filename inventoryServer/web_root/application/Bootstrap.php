<?php

class Bootstrap extends Zend_Application_Bootstrap_Bootstrap
{
	protected function _initDocType()
	{
		$this->bootstrap('view');
		$view = $this->getResource('view');
		$loader = new Zend_Loader_Autoloader_Resource
		(
		array (
				'basePath' => APPLICATION_PATH,
				'namespace' => 'Application',
		)
		);

		$loader -> addResourceType ( 'model', 'models', 'Model');

	}


	protected function _initViewHelpers()
	{
		$this->bootstrap('layout');
		$layout = $this->getResource('layout');
		$view = $layout->getView();

		$view->doctype('XHTML1_STRICT');
		$view->setEncoding('UTF-8');
		$view->headMeta()->appendHttpEquiv('Content-Type', 'text/html; charset=utf-8');
		$view->headMeta()->appendName('keywords', 'Swissmon');

		$view->headTitle('Swissmon');
		$view->headTitle()->setSeparator(' - ');
	}

	protected function _initNavigation()
	{
		$this->bootstrap('layout');
		$layout = $this->getResource('layout');
		$view = $layout->getView();
		$view->doctype('XHTML1_STRICT');

		$config = new Zend_Config_Xml(APPLICATION_PATH.'/configs/navigation.xml', 'nav');

		$navigation = new Zend_Navigation($config);
		$view->navigation($navigation);
	}
}

