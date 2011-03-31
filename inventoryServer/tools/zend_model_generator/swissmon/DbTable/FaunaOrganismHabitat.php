<?php
require_once('MainDbTable.php');
/**
 * Add your description here
 * 
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Default_Model_DbTable_FaunaOrganismHabitat extends MainDbTable
{
        /**
         * $_name - name of database table
         *
         * @var string
         */
	protected $_name='fauna_organism_habitat';

        /**
         * $_id - this is the primary key name

         *
         * @var string

         */
	protected $_id='id';

        protected $_referenceMap    = array(

                   'FaunaOrganismHabitatIbfk1' => array(
                       'columns' => 'habitat_id',
                       'refTableClass' => 'Habitat',
                       'refColumns' =>  'id'
                           ),
                   'FaunaOrganismHabitatIbfk2' => array(
                       'columns' => 'fauna_organism_id',
                       'refTableClass' => 'FaunaOrganism',
                       'refColumns' =>  'id'
                           )          
                );
}


