<?php
require_once('MainDbTable.php');
/**
 * Add your description here
 * 
 * @author Janosch Rohdewald
 * @copyright ZF model generator
 * @license http://framework.zend.com/license/new-bsd     New BSD License
 */

class Default_Model_DbTable_AreaHabitat extends MainDbTable
{
        /**
         * $_name - name of database table
         *
         * @var string
         */
	protected $_name='area_habitat';

        /**
         * $_id - this is the primary key name

         *
         * @var string

         */
	protected $_id='id';

        protected $_referenceMap    = array(

                   'AreaHabitatIbfk1' => array(
                       'columns' => 'area_id',
                       'refTableClass' => 'Area',
                       'refColumns' =>  'id'
                           ),
                   'AreaHabitatIbfk2' => array(
                       'columns' => 'habitat_id',
                       'refTableClass' => 'Habitat',
                       'refColumns' =>  'id'
                           )          
                );
}


