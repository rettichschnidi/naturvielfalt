<?php

$config=array(

    # author name used in documentation
    'docs.author' => 'Janosch Rohdewald',

    # licence for your code
    'docs.license' => 'http://framework.zend.com/license/new-bsd     New BSD License',

    # copyright for your (generated) code
    'docs.copyright' => 'ZF model generator',

    # database type
    'db.type' => 'mysql',

    # database host
    'db.host' => '127.0.0.1',

    # database user
    'db.user' => 'root',

    # database password
    'db.password' => '',

    # if enabled, to add require_once to the model file to include the mapper file,
    # and in the mapper file to include the dbtable file.
    # usful if you don't have class auto-loading.
    # if you're using Zend Framework's MVC you can probably set this to false
    'include.addrequire' => true,

    # default namespace name
    'namespace.default' => 'Application'

);
