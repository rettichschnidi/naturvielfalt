<?php
/**
 * @file area.edit.inc
 * @author Reto Schneider, 2012
 * @copyright 2012 Naturwerk, Brugg
 */

module_load_include('inc', 'area', 'area');
module_load_include('inc', 'area', 'area.ajax');
/**
 * Create a form which lets the user edit the geometry of an area.
 *
 * @param array $form
 *  Drupal form
 * @param $form_state
 *  An associative array containing the current state of the form.
 * @param $areadata
 * 	Array with area data
*/

/**
 * Return a form which allows all textual data of an area to be edited.
 *
 * @param array $areadata
 *  area
 * @param int $inventory_id
 *  id of the current selected inventory or null
 * @param array $areadata
*/
function area_edit($areadata = NULL, $inventory_id = NULL) {
	assert(!empty($areadata));
	//load js for observation table/map
	drupal_add_library('system', 'ui.dialog');
	drupal_add_library('system', 'jquery.form');
	$observation_path = drupal_get_path('module', 'observation');
	drupal_add_js($observation_path . '/js/observation.js');
	
	drupal_set_title($areadata['name']);
	drupal_add_css(drupal_get_path('module', 'area') . '/css/area.css');
	$output['area'] = drupal_get_form('area_edit_form', $areadata, $inventory_id);
	$output['safetynet'] = array(
			'#theme' => 'commonstuff.safetynet'
	);
	return $output;
}

/**
 * Return a form which allows all textual data of an area to be edited.
 *
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 * @param array $areadata
 * 	area
 * @param int $inventory_id 
 * 	id of the current selected inventory or null
 * @param array $areadata
 */
function area_edit_form($form, &$form_state, $areadata = NULL, $inventory_id = NULL) {
	
	global $base_url;
	
	$form['message'] = array(
			'#type'   => 'markup',
			'#markup' => '
			<div id="message" style="display: none; height: auto;">
				<div class="messages status"></div>
			</div>'
	);
	
	/* create a fieldset for the tabular data */
	$form['area_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area details'),
			'#attributes' => array(
					'id' => 'area-edit-details' // required for CSS
			),
	);

	/* add a table with the most imporant information, leave out empty fields */
	$form['area_edit']['table'] = area_get_infotable_of_specific_area($areadata);
	
	/* add a submit button to save the changes */
	$form['area_edit']['save'] = array(
			'#type' => 'submit',
			'#value' => t('Save Now'),
			'#submit' => array(
					'area_edit_save'
			)
	);
	
	/* create a fieldset for the static google maps */
	$form['area_map'] = array(
			'#type' => 'fieldset',
			'#title' => t('Map'),
			'#attributes' => array(
					'id' => 'area-edit-map' // required for CSS
			)
	);
	
	$form['area_map']['helptext'] = array(
			'#type' => 'markup',
			'#markup' => t(
					'<p>Please note: Changes will be saved instantly.</p>')
	);
	
	$iconBaseUrl = '/' . path_to_theme() . '/images/icons/enabled/';
	$form['area_map']['zoom'] = array(
			'#type' => 'markup',
			'#markup' => '<img style="cursor:pointer;" src="' . $iconBaseUrl . 'Zoom in.png" alt="' . t('zoomin') . '" title="' . t('zoomin') . '" onclick="javascript:observationmap.zoomIn(event, ' .$areadata['id'] . ');" /> '.t('zoomin') 
	);
	
	//if area is a child, user is not allowed to add a child
	if(empty($areadata['parent_id'])) {
		$form['area_map']['create_sub_area'] = array(
			  '#type' => 'markup',
			  '#prefix' => '</br>',
			  '#markup' => '<img style="cursor:pointer;" onclick="window.location.href=\''.$base_url . '/area/new/'.$areadata['id'].'\'" src="'.$iconBaseUrl.'Site map.png" style="cursor:pointer;" alt="' .t('Create a sub area') . '" title="' . t('Create a sub area') . '"/> '.  t('Create a sub area')
		);
	}
	
	$form['area_map']['map'] = array(
			'#mapid' => 'observationmap',
			'#theme' => 'area',
			'#showall' => false,
			'#scalecontrol' => true,
			'#action' => 'edit',
			'#defaultzoom' => '15',
			'#infowindow_content_fetch_url_observation' => base_path()
			. 'observation/{ID}/overview/ajaxform',
			'#infowindow_content_fetch_url_area' => base_path()
			. 'area/{ID}/areaoverview/ajaxform',
			'#geometry_edit_id' => (int) $areadata['id'],
			'#geometry_update_url' => base_path() . 'area/'
			. $areadata['id'] . '/updategeometry/json',
			'#geometries_fetch_url' => base_path() . 'area/' . $areadata['id'] . '/json',
	);
	

	/* Create a fieldset for the comment text field */
	$form['area_description_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Description'),
			'#collapsible' => true,
			'#collapsed' => false,
			'#attributes' => array(
					'id' => 'area-edit-comment'
			)
	);
	
	/* add the name of the area as an editable textfield */
	$form['area_description_edit']['areaname'] = array(
			'#type' => 'textfield',
			'#title' => t('Area name'),
			'#size' => 25,
			'#required' => true,
			'#default_value' => $areadata['name']
	);
	
	/* add the comments for the area as an editable textarea */
	$form['area_description_edit']['comment'] = array(
			'#type' => 'textarea',
			'#title' => t('Description'),
			'#default_value' => $areadata['comment']
	);
	
	$form['area_description_edit']['save'] = array(
			'#type' => 'submit',
			'#value' => t('Save'),
			'#submit' => array(
					'area_edit_save'
			)
	);
	
	/* Create a fieldset for the linked habitats */
	$form['area_inventories'] = array(
			'#type' => 'fieldset',
			'#title' => t('inventories').' & '.t('observations'),
			'#collapsible' => true,
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
					'id' => 'area-show-comment' // required for CSS
			)
	);
	
	$iconBaseUrl = '/' . path_to_theme() . '/images/icons/enabled/';
	
	if(area_check_access($areadata, 'ACL_RED_WRITE')  && user_access(CREATE_INVENTORY)) {
		$form['area_inventories']['add_intventory'] = array(
				'#type' => 'markup',
				'#markup' => '<a href="'.$base_url.'/inventory/area/'.$areadata['id'].'/new"> <img src="' . $iconBaseUrl . 'Add.png" alt="' . t('Add Inventory') . '" title="' . t('Add Inventory') . '" />'.t('Add Inventory').'</a>',
		);
	}
	
	$form['area_inventories']['inventories'] = inventory_show_area_inventories($areadata, true);
	$form['area_inventories']['space'] = array(
			'#type' => 'markup',
			'#markup' => '<br><br>'
	);
	
	$form['area_inventories']['observations'] = inventory_show_observations($areadata['id'], $inventory_id, false, true);

	/* create a fieldset for the strategies text fields */
	$form['area_concept_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Area concept'),
			'#collapsible' => true,
			'#collapsed' => true
	);

	/* add the protectiont target textarea */
	$form['area_concept_edit']['protection_target'] = array(
			'#type' => 'textarea',
			'#title' => t('Protection target'),
			'#default_value' => $areadata['protection_target']
	);

	/* add the tending strategies target textarea */
	$form['area_concept_edit']['tending_strategies'] = array(
			'#type' => 'textarea',
			'#title' => t('Tending strategies'),
			'#default_value' => $areadata['tending_strategies']
	);

	/* add the tending strategies target textarea */
	$form['area_concept_edit']['safety_precautions'] = array(
			'#type' => 'textarea',
			'#title' => t('Safety precautions'),
			'#default_value' => $areadata['safety_precautions']
	);

	/* add a save button */
	$form['area_concept_edit']['save'] = array(
			'#type' => 'submit',
			'#value' => t('Save'),
			'#submit' => array(
					'area_edit_save'
			)
	);

	/* create a fieldset for the strategies text fields */
	$form['area_habitats_edit'] = array(
			'#type' => 'fieldset',
			'#title' => t('Linked habitats'),
			'#collapsible' => true,
			'#collapsed' => true,
	);

	/* add the habitats table */
	$form['area_habitats_edit']['table'] = area_edit_habitat($areadata);


	$form['picture'] = array(
			'#type' => 'fieldset',
			'#title' => t('Manage existing media for this area'),
			'#collapsible' => true,
			'#collapsed' => false,
	);
	$form['picture']['media_table'] = gallery_implementation_edit_existing_file_list(
			'area',
			$areadata['id']);
	
	$form['files'] = gallery_implementation_files_table(
			'area',
			$areadata['id'],
			true);
	
	$file_extensions = implode(' ', gallery_get_valid_file_extensions());
	
	$form['media'] = array(
			'#type'=> 'fieldset',
			'#title'=> t('Add media for this area'),
			'#collapsible' => true,
			'#collapsed' => false,
			'#tree' => true
	);
	
	$numbers = array();
	
	//unset session on page_refresh
	if(!isset($form_state['values']['media']['file_number'])) {
		unset($_SESSION['area_files_total']);
	}
	//store number of files to add in session
	$files_total_old = empty($_SESSION['area_files_total']) ? 0 : $_SESSION['area_files_total'];
	$new_to_add=  !empty($form_state['values']['media']['file_number']) ? $form_state['values']['media']['file_number'] : 1;
	isset($_SESSION['area_files_total']) ? $_SESSION['area_files_total'] += $new_to_add : $_SESSION['area_files_total'] = $new_to_add;
	
	for($i=0; $i <= 10; $i++) {
		$numbers[$i] = $i;
	}
	
	$form['media']['file_number'] = array(
			'#title' => t('Number of files to add?'),
			'#description' => t('Allowed files endings are: ') . $file_extensions,
			'#type' => 'select',
			'#options' => $numbers,
			'#default_value' => 0,
			'#ajax' => array(
					'event' => 'change',
					'callback' => 'area_add_uploadslot',
					'wrapper' => 'upload-div',
					'method' => 'append',
					'effect' => 'fade',
			),
			'#tree' => true,
	);
	
	$form['media']['upload_div_start'] = array(
			'#type' => 'markup',
			'#markup'	=>	'<div id="upload-div">'
	);
	
	$form['media']['upload'] = array(
			'#type' => 'fieldset',
			'#tree' => true,
	);
	
	for ($i=$files_total_old; $i < $_SESSION['area_files_total']; $i++) {
		$form['media']['upload'][$i] = array(
				'#name' => 'files['.$i.']',
				'#id' => 'file_input'.$i,
				'#type' => 'file',
				'#prefix' => '<div id="picture_upload__'.$i.'">
						 <input type="text" style="display: none; height: 0px;" id="meta_title" name="meta_title[]" value="">
 						 <input type="text" style="display: none; height: 0px;" id="meta_description" name="meta_description[]" value="">
 					     <input type="text" style="display: none; height: 0px;" id="meta_location" name="meta_location[]" value="">
					     <input type="text" style="display: none; height: 0px;" id="meta_author" name="meta_author[]" value="">
						',
				'#suffix' => ($i+1 < $_SESSION['area_files_total']) ? '<a onclick="observation.galleryMetaDataDialog($(this).parent());return false;" > '	. t('Click here to add details to this file') . '</a><hr></div>'
				: '<a onclick="observation.galleryMetaDataDialog($(this).parent());return false;" > '	. t('Click here to add details to this file') . '</a></div>',
					
				'#title' => t('Choose a file'),
				'#size' => 60,
				'#tree' => true,
		);
	}
	
	$form['media']['button'] = array(
			'#type' => 'submit',
			'#value' => t('Save'),
			'#prefix' => '</div>',
			'#submit' => array(
					'area_edit_save'
			),
			'#tree' => true,
	);
	
// 	if (function_exists('gallery_list_renderer')) {
// 		$form['area_videos'] = gallery_list_renderer(
// 				'videos',
// 				'area',
// 				$areadata['id']);
// 		$output['area_images'] = gallery_list_renderer(
// 				'images',
// 				'area',
// 				$areadata['id']);
// 	}

	$form['actions'] = array(
			'#type' => 'actions'
	);

	if(area_check_access($areadata, 'ACL_ADMIN')) {
		/* create a fieldset for the strategies text fields */
		$form['area_permission'] = array(
				'#type' => 'fieldset',
				'#title' => t('Permission'),
				'#prefix' => '</form>',
				'#collapsible' => true,
				'#collapsed' => true,
				'#prefix' => '</form>',
				'#tree' => true
		);

		$form['area_permission']['acl'] = area_edit_permission($areadata);
	}

	return $form;
}

/**
 * form #ajax callback for adding uploadslots
 * 
 * @param array $form
 * @param array &$form_state
 */
function area_add_uploadslot($form, &$form_state) {
	return $form['media']['upload'];
}

/**
 * Validate the changes to the area
 *
 * @param array $form
 * @param array $form_state
 */
function area_edit_form_validate($form, &$form_state) {
	module_load_include('inc', 'area', 'area.validate');
	
	if (!area_validate_area_name($form_state['values']['areaname'])) {
		form_set_error('areaname', t('You have to enter an area name.'));
	}
	
	if(isset($_FILES['files'])) {
		foreach($_FILES['files']['name'] as $fkey => $filename) {
			if($_FILES['files']['name'][$fkey] != '') {
				$fileextension = array_pop(explode('.', $_FILES['files']['name'][$fkey]));
				if(!area_validate_fileextension($_FILES['files']['name'][$fkey])) {
					form_set_error('media][upload]['.$fkey, t('.@extension is an unallowed fileextension!', array('@extension' => $fileextension)));
				}
			}
		}
	}
}

/**
 * Return a drupal render array with the habitats linked to the given area.
 *
 * @param $areadata
 * 	Array with all data about an area
 */
function area_edit_habitat($areadata = NULL) {
	$results = db_query(
			'SELECT
			h.id AS id,
			h.label AS label,
			h.name AS name,
			CASE
			WHEN ah.id IS NOT NULL
			THEN 1
			ELSE 0
			END
			AS selected
			FROM
			{habitat} h
			LEFT JOIN (
			SELECT
			id,
			habitat_id,
			area_id
			FROM
			{area_habitat}
			WHERE
			area_id = :areaid
	) AS ah ON h.id = ah.habitat_id;',
			array('areaid' => $areadata['id']));

	$habitats = array();
	foreach ($results->fetchAll() as $habitatAsObject) {
		$row = array();
		$id = $habitatAsObject->id;
		$checkText = $habitatAsObject->selected ? "checked='true'" : '';
		$checkbox = "<input type='checkbox' name='selectedhabitats[]' value='"
				. $habitatAsObject->id . "' $checkText />";
		$row[] = $checkbox;
		$row[] = $habitatAsObject->label;
		$row[] = $habitatAsObject->name;
		$habitats[] = $row;
	}

	$form = array(
			'#theme' => 'datatable',
			'#header' => array(
					array(
							'name' => t('Select'),
							'width' => 40
					),
					array(
							'name' => t('Label'),
							'width' => 200
					),
					array(
							'name' => t('Name'),
							'width' => 600
					)
			),
			'#tableWidth' => 900,
			'#rows' => $habitats,
			'#id_table' => 'habitats'
	);
	return $form;
}

/**
 * Save changes to the area texts and files.
 *
 * @param array $form
 *  Drupal form
 * @param array $form_state
 *  An associative array containing the current state of the form.
 */
function area_edit_save($form, &$form_state) {
	global $user;
	
	
	/* Escape values before submitting them to the database */
	$area_id = (int) $form_state['build_info']['args'][0]['id'];
	$area = area_get_full_dataset_of_specific_area($area_id);
	
	$name = filter_xss($form_state['values']['areaname']);
	$comment = filter_xss($form_state['values']['comment']);
	$protection_target = filter_xss($form_state['values']['protection_target']);
	$tending_strategies = filter_xss(
			$form_state['values']['tending_strategies']);
	$safety_precautions = filter_xss(
			$form_state['values']['safety_precautions']);
	$selectedhabitatIds = isset($_POST['selectedhabitats'])
	? $_POST['selectedhabitats'] : array();
	// flush all existing area_habitat records for this area...
	db_delete('area_habitat')->condition('area_id', $area_id)
	->execute();
	// ...and populate them with the current new ones
	$values = array();
	$insert = db_insert('area_habitat')->fields(
			array('area_id',
					'habitat_id'
			));
	foreach ($selectedhabitatIds as $habitatId) {
		$insert->values(
				array('area_id' => $area_id,
						'habitat_id' => $habitatId
				));
	}
	$insert->execute();

	db_update('area')->fields(
	array('name' => $name,
	'comment' => $comment,
	'protection_target' => $protection_target,
	'tending_strategies' => $tending_strategies,
	'safety_precautions' => $safety_precautions,
	'modify_time' => 'NOW()',
	))
	->condition('id', $area_id)
	->execute();
	
	/**
	 * Medias
	 */
	$image = array(
			'item_id' => $area_id,
			'author' => $user->name,
			'location' => $area['township'] . ' ' . $area['canton'],
	);
	
	gallery_implementation_save_manual('area', $image, 'area_id');
	
	if (isset($_POST['file_description'])) {
		gallery_implementation_file_updateComment(
		'area',
		$_POST['file_description']);
	}

	if (isset($_POST['delete'])) {
		gallery_implementation_delete($_POST['delete']);
	}
	
	if (isset($_POST['file_delete'])) {
		gallery_implementation_files_delete($_POST['file_delete'], 'area');
	}
	
	drupal_set_message(t("Area Saved"));
	
	
}

/**
 * Edit the permissions for an area
 * @return render array
 */
function area_edit_permission($areadata) {
	$output = array();
	$defaults = array(
			array(
					'type' => variable_get("ACL_READ"),
					'name' => t('Read'),
					'value' => FALSE,
			),
			array(
					'type' => variable_get("ACL_RED_WRITE"),
					'name' => t('Add Inventory/Subarea'),
					'value' => FALSE,
			),
			array(
					'type' => variable_get("ACL_WRITE"),
					'name' => t('Write'),
					'value' => FALSE,
			),
			array(
					'type' => variable_get("ACL_ADMIN"),
					'name' => t('Administration'),
					'value' => FALSE,
			)
	);

	$output['acl'] = array(
			'#theme' => 'c_acl',
			'#defaults' => $defaults,
			'#acl_id' => $areadata['acl_id'],
			'#add_element' => true,
			'#tree' => true
	);
	return $output['acl'];
}

?>
