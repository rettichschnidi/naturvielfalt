<?php
require_once('MainDbTable.php');
/**
 * Add your description here
 * 
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Default_Model_DbTable_FloraOrganismHabitat extends MainDbTable
{
        /**
         * $_name - name of database table
         *
         * @var string
         */
	protected $_name='flora_organism_habitat';

        /**
         * $_id - this is the primary key name

         *
         * @var string

         */
	protected $_id='id';

        protected $_referenceMap    = array(

                   'FloraOrganismHabitatIbfk1' => array(
                       'columns' => 'habitat_id',
                       'refTableClass' => 'Habitat',
                       'refColumns' =>  'id'
                           ),
                   'FloraOrganismHabitatIbfk2' => array(
                       'columns' => 'flora_organism_id',
                       'refTableClass' => 'FloraOrganism',
                       'refColumns' =>  'id'
                           )          
                );
}


