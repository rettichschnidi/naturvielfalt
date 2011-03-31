<?php
require_once('Zend/Db/Table/Abstract.php');
require_once('MainDbTable.php');
/**
 * Add your description here
 * 
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Application_Model_DbTable_FloraOrganism extends MainDbTable
{
        /**
         * $_name - name of database table
         *
         * @var string
         */
	protected $_name='flora_organism';

        /**
         * $_id - this is the primary key name

         *
         * @var string

         */
	protected $_id='id';

        protected $_referenceMap    = array(

                   'Application_Model_DbTable_FloraOrganism' => array(
                       'columns' => 'official_flora_orfganism_id',
                       'refTableClass' => 'Application_Model_DbTable_FloraOrganism',
                       'refColumns' =>  'id'
                           ),
                   'Application_Model_DbTable_FloraOrganismStatus' => array(
                       'columns' => 'status',
                       'refTableClass' => 'Application_Model_DbTable_FloraOrganismStatus',
                       'refColumns' =>  'status'
                           )          
                );
}


