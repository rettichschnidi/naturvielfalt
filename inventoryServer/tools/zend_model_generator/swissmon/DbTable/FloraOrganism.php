<?php
require_once('MainDbTable.php');
/**
 * Add your description here
 * 
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Default_Model_DbTable_FloraOrganism extends MainDbTable
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

                   'FloraOrganismIbfk1' => array(
                       'columns' => 'official_flora_orfganism_id',
                       'refTableClass' => 'FloraOrganism',
                       'refColumns' =>  'id'
                           ),
                   'FloraOrganismIbfk2' => array(
                       'columns' => 'status',
                       'refTableClass' => 'FloraOrganismStatus',
                       'refColumns' =>  'status'
                           )          
                );
}


