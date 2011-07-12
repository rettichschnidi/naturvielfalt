<?php

/**
 * @file
 * Hooks provided by the Gallery module.
 */

/**
 * Defines the types a gallery is available to and several additional
 * configuration possibilities [all optional] for them.
 * 
 * After registering your type images may be managed by going to
 *  /gallery/YOURTYPE/ID/edit
 * and viewed by going to
 *  /gallery/YOURTYPE/ID
 * where ID is the id of the item of your type to add the images to or 
 * view the images of.
 * 
 * Other usefull functions to include gallery images on a page 
 * are located and documented in gallery.content.inc
 * 
 * @return array
 */
function hook_gallery_types() {
  return array(
    /* The key defines the name of the type and the value holds the configuration
     */
    'YOURTYPE' => array(
    
      /* A function receiving the type, id and image array of the image to be 
       * edited and returns true if access should be granted
       * 
       * If not provided access is granted if the current user is the owner of
       * the image
       */
      'edit_image' => 'YOURMODULE_gallery_image_access_callback',
      
      /* A function receiving the type and id of the gallery to be edited and
       * returns true if access should be granted
       * 
       * Prior to this function beeing called the current user is checked against
       * the "add gallery images" permission
       */
      'edit_gallery' => 'YOURMODULE_gallery_access_callback',
      
      /* A function receiving the id of the item and returning a string used
       * as caption for images of this type and id
       */
      'image_caption' => 'YOURMODULE_gallery_image_caption',
      
      /* A function receiving the id of the item and returning a string used as
       * title for the "manage images" page of this type and id
       */
      'manage_title' => 'YOURMODULE_gallery_manage_title',
      
      /* A function receiving the id of the item and returning a string used as
       * description for the "manage images" page of this type and id
       */
      'manage_description' => 'YOURMODULE_gallery_manage_description',
      
      /* A function receiving the id of the item and returning a string used as
       * default location for images of this type and id
       */
      'location_provider' => 'YOURMODULE_gallery_location_provider',
      
      /* A function defining selectable items when adding images of the specified
       * type. Receives the id of the item and returns an array with the names of
       * the selectable items as value and as key the type and id of the item
       * joined by a colon ":"
       * 
       * NOTE: This function works differently if "item_provider_autocomplete" is
       * set. If so the function receives the type and id of the selected item and
       * returns the text to be set in the autocomplete field.
       */
      'item_provider' => 'YOURMODULE_gallery_item_provider',
      
      /* A function defining the name of the view to be used as a source for the
       * autocompletion. The view has to be available in the context of the gallery
       * e.g.
       *  /gallery/YOURMODULE_type_gallery_autocomplete/YOURTYPE/ID
       * and receives the term to be searched for in the REQUEST. The function
       * returns the found items as an array of arrays having the following
       * keys:
       *  - "item_type": The type of the item
       *  - "item_id": The ID of the item
       *  - "name": The name of the item (To be placed in the input field)
       *  - "label": The label of the item (May be HTML, used in the autocomplete menu)
       */
      'item_provider_autocomplete' => 'YOURMODULE_type_gallery_autocomplete',
      
      /* A function providing an additional SQL condition when searching images of the
       * specified type. Returns a valid SQL condition as a string
       */
      'condition' => 'YOURMODULE_gallery_condition',
      
      /* An array of sub types having the user friendly name as value and the ID as key.
       * Sub types may be selected for images of the specified type and the gallery of that
       * type may afterwards be filter by the sub types.
       */
      'subtypes' => array(
        'presentation' => t('Presentational images')
        // ...
      )
    )
    // ...
  );
}

/**
 * Defines conversions between different gallery types by defining a chain of SQL joins
 * 
 * The example illustrates the usage of this hook in order to display images of inventory
 * entries in the gallery of the inventory types (e.g. Amphibien) which would result in
 * the following abstract SQL JOIN:
 * 
 *  SELECT * FROM inventory_entry e
 *    JOIN organism o ON o.id = e.organism_id
 *    JOIN inventory_type t ON t.id = o.inventory_type_id
 * 
 * @return array
 */
function hook_gallery_type_conversions() {
  return array(
    array(
      array(
        // The source type of this conversion step (has to be a DB table)
        'source' => 'inventory_entry',
        // The source key (the column on which to join the target table)
        'source_key' => 'organism_id',
        // The target type of this conversion step (has to be a DB table)
        'target' => 'organism',
        // The target key (the column on which to join the source table)
        'target_key' => 'id',
        // Optional sub type of images of the source type need in order to be selected
        'subtype' => 'presentation'
      ),
      array(
        'source' => 'organism',
        'source_key' => 'inventory_type_id',
        'target' => 'inventory_type',
        'target_key' => 'id'
      )
      // ...
    )
    // ...
  );
}

/**
 * Defines conversions betwenn different gallery types when editing galleries by defining
 * a chain of SQL joins.
 * 
 * @see hook_gallery_type_conversions
 * 
 * @return array
 */
function hook_gallery_type_edit_conversions() {
}

/**
 * Defines conditions for image categories
 * 
 * The example show some conditions added by the organism module.
 * 
 * @return array
 */
function hook_gallery_category_conditions() {
  return array(
    /* Simple condition by type
     * 
     * The category will only be available for images of type organism
     */
    'organism' => array(
      'name' => t('Organisms'),
      'type' => 'organism'
    ),
    /* Simple lookup condition
     * 
     * The category will be available for images of type organism having
     * the organism_type column set to '2'
     */
    'organism_flora' => array(
      'name' => t('Flora'),
      'type' => 'organism',
      'table' => 'organism',
      'column' => 'organism_type',
      'value' => '2'
    ),
    /* Configurable complex lookup condition
     * 
     * 
     */
    'organism_flora_custom' => array(
      'name' => t('Flora individual'),
      'type' => 'organism',
      /* If the table is an array the first value is taken as the table and 
       * the second value (has to be an array) defines the how to get to this
       * table from the type using SQL joins
       */
      'table' => array('flora_organism', array(
        'organism_id' => 'flora_organism.id',
        'organism_type' => '2'
      )),
      /* The columns property defines the different columns for which the condition
       * may be set.
       */
      'columns' => array(
        'is_neophyte' => t('Neophyte')
      )
    ),
    'organism_fauna_custom' => array(
      'name' => t('Fauna individual'),
      'type' => 'organism',
      'table' => array('fauna_organism', array(
        'organism_id' => 't.id',
        'organism_type' => '1'
      )),
      'columns' => array(
        /* If a columns value is an array the first value is taken as the label and the
         * second one defines the name of the function providing the different values
         * for this column.
         */
        'fauna_class_id' => array(t('Inventory Group'), 'organism_gallery_catgory_invgroup_columns')
      ),
    ),
  );
}
